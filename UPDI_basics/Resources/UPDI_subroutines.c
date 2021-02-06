char txt2bin(char A, char B);
void send_key(const char*);
void Read_NVM_Reg(int, char);


/**********************************************************************************************************/
void Erase_code (void){
send_key(Key_chip_erase);


UART_Tx(0x55);
UART_Tx(LCDS | ASI_KEY_STATUS_reg);          		/*Test Chip erase bit 3 of ASI_KEY_STATUS register (reg 7)*/
if(UART_Rx() & 0x08);                       		/*should be 0x08*/
else sendString("\r\nChip erase failed");

UPDI_reset;

UPDI_timeout = 0;
Timer_T1_sub_with_interrupt(T1_delay_1Sec);
sei();
do {                                    			/*wait for chip erase to complete*/
Timer_T0_sub(T0_delay_200us);         				// Estimated to takes just under 5ms
UART_Tx(0x55);
UART_Tx(LCDS | ASI_SYS_STATUS_reg);     
Rx_Byte = UART_Rx();								/*Test lockstatus, bit 0 of SYS_STATUS register (reg B)*/
test += 1;
if(UPDI_timeout)break;
}while(Rx_Byte & 0x01);                 			/*Exit when lock status bit is zero*/
cli();

if (UPDI_timeout) sendString("\r\nFailure: Device still locked\t");
else sendString("\r\nOK Device unlocked\t\t\t");
sendHex(16, Rx_Byte);
sendHex(10, test);}



/**********************************************************************************************************/
char set_up_NVM_prog(void){
send_key(Key_NVM_prog);
UART_Tx(0x55);
UART_Tx(LCDS | ASI_KEY_STATUS_reg);           //Test NVM_prog bit 4 of ASI_KEY_STATUS register (reg 7)
UART_Rx();

UPDI_reset;


UPDI_timeout = 0;
Timer_T1_sub_with_interrupt(T1_delay_1Sec);
sei();
do {                                    /*wait for chip erase to complete*/
Timer_T0_sub(T0_delay_200us);           //Estimated to takes just under 5ms*/
UART_Tx(0x55);
UART_Tx(LCDS | ASI_SYS_STATUS_reg);     /*Test lockstatus, bit 0 of SYS_STATUS register (reg B)*/
Rx_Byte = UART_Rx();
test += 1;
if(UPDI_timeout)break;
}while(!(Rx_Byte & 0x08));                 /*Exit when lock status bit is zero*/
cli();

if (UPDI_timeout) {sendString("\r\nNVMPROG failure: \t");return 0;}
else {sendString("\r\nReady for NVM programming\t");return 1;}
sendHex(16, Rx_Byte);
sendHex(10, test);
newline();}








/**********************************************************************************************************/
char txt2bin(char A, char B){
A -= '0';
if (A > 9) A -= 7;
B -= '0';
if (B > 9) B -= 7;
return (A * 0x10 + B);}


/**********************************************************************************************************/
void send_key(const char *Key){
UART_Tx(0x55);
UART_Tx (send64bitKey_cmd);
char key_h, key_l;
for(int m = 0; m <= 7; m++){
key_h = *(Key + 2*(8 - m));
key_l = *(Key + 2*(8 - m) + 1);
UART_Tx(txt2bin(key_h,key_l));}}



/**********************************************************************************************************/
void Write_page_to_NVM(int PA){			//PA is page address
UART_Tx(0x55);
UART_Tx(STS | int_data);				//UART_Tx(0x45);	//
UART_Tx(NVMCTRL_ADDR_reg);
UART_Tx(NVMCTRL_ADDR_reg >> 8);
UART_Rx();
Prog_delay;	//Timer_T0_sub(T0_delay_40us);				//200
UART_Tx(PA);
UART_Tx(PA >> 8);
UART_Rx();
Prog_delay;	//Timer_T0_sub(T0_delay_40us);				//200


//Read_NVM_Reg(NVMCTRL_ADDR_reg, 'I');
sendChar('*');

UART_Tx(0x55);
UART_Tx(STS | byte_data);				//UART_Tx(0x44);
UART_Tx(NVMCTRL_CTRLA);
UART_Tx(NVMCTRL_CTRLA >> 8);
UART_Rx();
Prog_delay;	//Timer_T0_sub(T0_delay_40us);				//200
UART_Tx(WP_cmd);
UART_Rx();
Timer_T0_sub(T0_delay_5ms);}


/**********************************************************************************************************/


void Read_NVM_Reg(int reg_add, char mode){
int data = 0;
unsigned int Rx_byte;

if(mode == 'I'){

UART_Tx(0x55);
UART_Tx(LDS_from | word_address);
UART_Tx(reg_add++);
UART_Tx(reg_add >> 8);
Rx_byte = UART_Rx();
Timer_T0_sub(T0_delay_200us);}


UART_Tx(0x55);
UART_Tx(LDS_from | word_address);
UART_Tx(reg_add);
UART_Tx(reg_add >> 8);
data = UART_Rx();

if(mode == 'I'){
data = (data << 8) | Rx_byte;}

Timer_T0_sub(T0_delay_200us);
sendHex(16, data);}







/**********************************************************************************************************/
void write_fuse(int fuse_add, unsigned char value){

unsigned int Rx_byte;

UART_Tx(0x55);                              //Send lock bit address to NVMCTRL.ADD registers
UART_Tx(STS | byte_data);										//UART_Tx(0x45);                                
UART_Tx(NVMCTRL_ADDR_reg);                                //Address of NVMCTRL.ADD registers                        
UART_Tx(NVMCTRL_ADDR_reg >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);
UART_Tx(fuse_add);                                //Adddress of lock byte
UART_Rx();
Timer_T0_sub(T0_delay_200us);

UART_Tx(0x55);                              //Send lock bit address to NVMCTRL.ADD registers
UART_Tx(STS | byte_data);										//UART_Tx(0x45);                                
UART_Tx(NVMCTRL_ADDR_reg + 1);                                //Address of NVMCTRL.ADD registers                        
UART_Tx(NVMCTRL_ADDR_reg >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);
UART_Tx(fuse_add >> 8); 
UART_Rx();                               //Adddress of lock byte
Timer_T0_sub(T0_delay_200us);

Read_NVM_Reg(NVMCTRL_ADDR_reg, 'I');

UART_Tx(0x55);                                //Send key 0xC5 (device unlocked) to NVMCTRL.DATA register)
UART_Tx(STS | byte_data);						//UART_Tx(0x44);                                
UART_Tx(NVMCTRL_DATA_reg);                                //Adress of NVMCTRL.DATA registers                       
UART_Tx(NVMCTRL_DATA_reg >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);
UART_Tx(value); 
UART_Rx();
Timer_T0_sub(T0_delay_200us);

Read_NVM_Reg(NVMCTRL_DATA_reg, 'B');

UART_Tx(0x55);                                //Enter write to fuse byte command into NVM.CTRLA register
UART_Tx(STS | byte_data);						//UART_Tx(0x44);                                
UART_Tx(NVMCTRL_CTRLA);                                //Register address                         
UART_Tx(NVMCTRL_CTRLA >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);
UART_Tx(cmd_update_fuse);                                 //Command to update the fuses.  CMD is 0x07
UART_Rx();
Timer_T0_sub(T0_delay_200us);}



/**********************************************************************************************************/
void read_out_fuses(void){
int start_add = FUSE_base_address;
for(int m = 0; m<=10; m++){
  if(m==3)continue;
  if(m==9)continue;
UART_Tx(0x55);
UART_Tx(LDS_from | word_address);
UART_Tx(start_add);
UART_Tx(start_add >> 8);
start_add += 1;
sendHex(16, UART_Rx());
Timer_T0_sub(T0_delay_200us);}}



/**********************************************************************************************************/
void read_out_signature_bytes(void){
int start_add = 0x1100;
for(int m = 0; m<=2; m++){
UART_Tx(0x55);
UART_Tx(LDS_from | word_address);
UART_Tx(start_add++);
UART_Tx(start_add >> 8);
sendHex(16, UART_Rx());
Timer_T0_sub(T0_delay_200us);}}


/**********************************************************************************************************/
void cmd_to_page_buffer(int flash_data, int add_in_flash){
UART_Tx(0x55);
UART_Tx(STS | int_data);
UART_Tx(add_in_flash);
UART_Tx(add_in_flash >> 8);
UART_Rx();
Prog_delay;	//Timer_T0_sub(T0_delay_40us);
UART_Tx(flash_data >> 8);
UART_Tx(flash_data);
UART_Rx();
Prog_delay;	//Timer_T0_sub(T0_delay_40us);
}



/**********************************************************************************************************/
void fill_page_buffer(int *cmd_buffer, int add_in_flash, unsigned char block_SZ){

char High_byte = 0;

block_SZ -= 1;

UART_Tx_upload(0x55);
UART_Tx_upload(ST | word_pointer);
UART_Tx_upload(add_in_flash);
UART_Tx_upload(add_in_flash >> 8);
UART_Rx_upload();
Prog_delay;	

UART_Tx_upload(0x55);
UART_Tx_upload(setup_repeat_op);
UART_Tx_upload(block_SZ);
Prog_delay;	
Timer_T0_sub(T0_delay_5ms);												//Long delay here is OK

UART_Tx_upload(0x55);
UART_Tx_upload(ST | inc_byte_ptr);
for(int m = 0; m <= block_SZ; m++){
if(!(High_byte)){UART_Tx_upload((*cmd_buffer) >> 8);High_byte = 1;}
else{UART_Tx_upload(*cmd_buffer);cmd_buffer++;High_byte = 0; prog_counter += 1;}
UART_Rx_upload();
Prog_delay_upload;}

}