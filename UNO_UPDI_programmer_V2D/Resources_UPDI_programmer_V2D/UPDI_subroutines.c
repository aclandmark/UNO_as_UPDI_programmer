void send_key(const char*);
void Read_NVM_Reg(int, char);
char txt2bin(char A, char B);


/**********************************************************************************************************/
void send_key(const char *Key){						//No interrupts enabled here
UART_Tx_Ls(0x55);
UART_Tx_Ls (send64bitKey_cmd);
char key_h, key_l;
for(int m = 0; m <= 7; m++){
key_h = *(Key + 2*(8 - m));
key_l = *(Key + 2*(8 - m) + 1);
UART_Tx_Ls(txt2bin(key_h,key_l));}}




/**********************************************************************************************************/
void Erase_code (void){								//No interrupts enabled here
send_key(Key_chip_erase);


UART_Tx_Ls(0x55);
UART_Tx_Ls(LCDS | ASI_KEY_STATUS_reg);          		/*Test Chip erase bit 3 of ASI_KEY_STATUS register (reg 7)*/
if(UART_Rx_Ls() & 0x08);                       		/*should be 0x08*/
else sendString("\r\nChip erase failed");

UPDI_reset;

UPDI_timeout = 0;
Timer_T1_sub_with_interrupt(T1_delay_1Sec);
sei();
do {                                    			/*wait for chip erase to complete*/
Timer_T0_sub(T0_delay_200us);         				// Estimated to takes just under 5ms
UART_Tx_Ls(0x55);
UART_Tx_Ls(LCDS | ASI_SYS_STATUS_reg);     
Rx_Byte = UART_Rx_Ls();								/*Test lockstatus, bit 0 of SYS_STATUS register (reg B)*/
test += 1;
if(UPDI_timeout)break;
}while(Rx_Byte & 0x01);                 			/*Exit when lock status bit is zero*/
cli();

if (UPDI_timeout) 
sendString("\r\nFailure: Device still locked\t");
else sendString("\r\nOK Device unlocked\t\t\t");
sendHex(16, Rx_Byte);
sendHex(10, test);}



/**********************************************************************************************************/
char set_up_NVM_prog(void){							//No interrupts enabled here
send_key(Key_NVM_prog);
UART_Tx_Ls(0x55);
UART_Tx_Ls(LCDS | ASI_KEY_STATUS_reg);           		//Test NVM_prog bit 4 of ASI_KEY_STATUS register (reg 7)
UART_Rx_Ls();

UPDI_reset;

UPDI_timeout = 0;
Timer_T1_sub_with_interrupt(T1_delay_1Sec);
sei();
do {                                    			//wait for chip erase to complete
Timer_T0_sub(T0_delay_200us);           			//Estimated to takes just under 5ms*/
UART_Tx_Ls(0x55);
UART_Tx_Ls(LCDS | ASI_SYS_STATUS_reg);     			//Test lockstatus, bit 0 of SYS_STATUS register (reg B)
Rx_Byte = UART_Rx_Ls();
test += 1;
if(UPDI_timeout)break;
}while(!(Rx_Byte & 0x08));                 		//Exit when lock status bit is zero
cli();

if (UPDI_timeout) 
{sendString("\r\nNVMPROG failure: \t");return 0;}
else {sendString("\r\nReady for NVM programming\t");
return 1;}
sendHex(16, Rx_Byte);
sendHex(10, test);
newline();}




/**********************************************************************************************************/
void read_out_signature_bytes(void){				//No interrupts enabled here
int start_add = 0x1100;
for(int m = 0; m<=2; m++){
UART_Tx_Ls(0x55);
UART_Tx_Ls(LDS_from | word_address);
UART_Tx_Ls(start_add++);
UART_Tx_Ls(start_add >> 8);
sendHex(16, UART_Rx_Ls());
Timer_T0_sub(T0_delay_200us);}}





/**********************************************************************************************************/
void write_fuse(int fuse_add, unsigned char value){		//No interrupts enabled here

unsigned int Rx_byte;

UART_Tx_Ls(0x55);                              			//Send lock bit address to NVMCTRL.ADD registers
UART_Tx_Ls(STS | byte_data);								                               
UART_Tx_Ls(NVMCTRL_ADDR_reg);                              //Address of NVMCTRL.ADD registers                        
UART_Tx_Ls(NVMCTRL_ADDR_reg >> 8);
UART_Rx_Ls();
Timer_T0_sub(T0_delay_200us);
UART_Tx_Ls(fuse_add);                                		//Adddress of lock byte
UART_Rx_Ls();
Timer_T0_sub(T0_delay_200us);

UART_Tx_Ls(0x55);                              			//Send lock bit address to NVMCTRL.ADD registers
UART_Tx_Ls(STS | byte_data);			                                
UART_Tx_Ls(NVMCTRL_ADDR_reg + 1);                          //Address of NVMCTRL.ADD registers                        
UART_Tx_Ls(NVMCTRL_ADDR_reg >> 8);
UART_Rx_Ls();
Timer_T0_sub(T0_delay_200us);
UART_Tx_Ls(fuse_add >> 8); 
UART_Rx_Ls();                               				//Adddress of lock byte
Timer_T0_sub(T0_delay_200us);

Read_NVM_Reg(NVMCTRL_ADDR_reg, 'I');

UART_Tx_Ls(0x55);                                			//Send key 0xC5 (device unlocked) to NVMCTRL.DATA register)
UART_Tx_Ls(STS | byte_data);								//UART_Tx_Ls(0x44);                                
UART_Tx_Ls(NVMCTRL_DATA_reg);                          	//Adress of NVMCTRL.DATA registers                       
UART_Tx_Ls(NVMCTRL_DATA_reg >> 8);
UART_Rx_Ls();
Timer_T0_sub(T0_delay_200us);
UART_Tx_Ls(value); 
UART_Rx_Ls();
Timer_T0_sub(T0_delay_200us);

Read_NVM_Reg(NVMCTRL_DATA_reg, 'B');

UART_Tx_Ls(0x55);                                			//Enter write to fuse byte command into NVM.CTRLA register
UART_Tx_Ls(STS | byte_data);								//UART_Tx_Ls(0x44);                                
UART_Tx_Ls(NVMCTRL_CTRLA);                       			//Register address                         
UART_Tx_Ls(NVMCTRL_CTRLA >> 8);
UART_Rx_Ls();
Timer_T0_sub(T0_delay_200us);
UART_Tx_Ls(cmd_update_fuse);                          		//Command to update the fuses.  CMD is 0x07
UART_Rx_Ls();
Timer_T0_sub(T0_delay_200us);}



/**********************************************************************************************************/
void read_out_fuses(void){								//No interrupts enabled here
int start_add = FUSE_base_address;
for(int m = 0; m<=10; m++){
  if(m==3)continue;
  if(m==9)continue;
UART_Tx_Ls(0x55);
UART_Tx_Ls(LDS_from | word_address);
UART_Tx_Ls(start_add);
UART_Tx_Ls(start_add >> 8);
start_add += 1;
sendHex(16, UART_Rx_Ls());
Timer_T0_sub(T0_delay_200us);}}



/**********************************************************************************************************/
void inititalise_UPDI_cmd_write(int add_in_flash){		//Setup repeat command
unsigned char block_SZ = 64;

add_in_flash = (add_in_flash * 2) + 0x8000;

cli();
UART_Tx(0x55);
UART_Tx(ST | word_pointer);
UART_Tx(add_in_flash);
UART_Tx(add_in_flash >> 8);
UART_Rx();
sei();
Prog_delay;	

cli();
UART_Tx(0x55);
UART_Tx(setup_repeat_op);
UART_Tx(block_SZ - 1);
sei();
Prog_delay;	

cli();
UART_Tx(0x55);
UART_Tx(ST | inc_byte_ptr);
sei();}



/**********************************************************************************************************/
void UPDI_cmd_to_page_buffer(void){

get_next_hex_cmd();

cli();	
UART_Tx(Hex_cmd);
UART_Rx();
sei();		
Prog_delay_upload;Prog_delay_upload;
cli();
UART_Tx(Hex_cmd >> 8);
UART_Rx();


Prog_delay_upload;Prog_delay_upload;
sei();}



/**********************************************************************************************************/
void Write_page_to_NVM(int PA){							//PA is page address  Interrupts enabled
PA = (PA * 2) + 0x8000;								//Convert cmd address to byte address
cli();
UART_Tx(0x55);
UART_Tx(STS | int_data);				
UART_Tx(NVMCTRL_ADDR_reg);
UART_Tx(NVMCTRL_ADDR_reg >> 8);
UART_Rx();
sei();
Prog_delay_upload;	
cli();
UART_Tx(PA);
UART_Tx(PA >> 8);
UART_Rx();
sei();
Prog_delay_upload;	

sendChar('*');

cli();
UART_Tx(0x55);
UART_Tx(STS | byte_data);	
UART_Tx(NVMCTRL_CTRLA);
UART_Tx(NVMCTRL_CTRLA >> 8);
UART_Rx();
sei();
Prog_delay_upload;	
cli();
UART_Tx(WP_cmd);
UART_Rx();
sei();
//Timer_T0_sub(T0_delay_5ms);
Timer_T0_sub(T0_delay_3ms);}





/**********************************************************************************************************/


void Read_NVM_Reg(int reg_add, char mode){					//No interrupts enabled here
int data = 0;
unsigned int Rx_byte;

if(mode == 'I'){

UART_Tx_Ls(0x55);
UART_Tx_Ls(LDS_from | word_address);
UART_Tx_Ls(reg_add++);
UART_Tx_Ls(reg_add >> 8);
Rx_byte = UART_Rx_Ls();
Timer_T0_sub(T0_delay_200us);}


UART_Tx_Ls(0x55);
UART_Tx_Ls(LDS_from | word_address);
UART_Tx_Ls(reg_add);
UART_Tx_Ls(reg_add >> 8);
data = UART_Rx_Ls();

if(mode == 'I'){
data = (data << 8) | Rx_byte;}

Timer_T0_sub(T0_delay_200us);
sendHex(16, data);}





/**********************************************************************************************************/
char txt2bin(char A, char B){
A -= '0';
if (A > 9) A -= 7;
B -= '0';
if (B > 9) B -= 7;
return (A * 0x10 + B);}




