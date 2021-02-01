char txt2bin(char A, char B);
void send_key(const char*);



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
void set_up_NVM_prog(void){
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

if (UPDI_timeout) sendString("\r\nNVMPROG failure: \t");
else sendString("\r\nOK Ready for NVM programming\t");
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
UART_Tx(0x45);								//
UART_Tx(NVMCTRL_ADDR_reg);
UART_Tx(NVMCTRL_ADDR_reg >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);
UART_Tx(PA);
UART_Tx(PA >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);

UART_Tx(0x55);
UART_Tx(0x44);
UART_Tx(NVMCTRL_CTRLA);
UART_Tx(NVMCTRL_CTRLA >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);
UART_Tx(WP);
UART_Rx();
Timer_T0_sub(T0_delay_5ms);}


/**********************************************************************************************************/
void Read_add_of_last_page(void){
int reg_add;

reg_add = NVMCTRL_ADDR_reg;
UART_Tx(0x55);
UART_Tx(LDS_from | word_address);
UART_Tx(reg_add++);
UART_Tx(reg_add >> 8);
sendHex(16, UART_Rx());
Timer_T0_sub(T0_delay_200us);
UART_Tx(0x55);
UART_Tx(LDS_from | word_address);
UART_Tx(reg_add);
UART_Tx(reg_add >> 8);
sendHex(16, UART_Rx());
Timer_T0_sub(T0_delay_200us);
}




/**********************************************************************************************************/
void write_fuse(int fuse_add, unsigned char value){

/**/
/*Write the address of the fuse to the Address register (NVMCTRL.ADDR)L/H 0x1008/9
• Write the data to be written to the fuse to the Data register (NVMCTRL.DATA) L/H 1006/7
• Execute the Fuse Write command.
• After the fuse is written, a Reset is required for the updated value to take effect.
#define LOCKBIT_LOCKBIT  _SFR_MEM8(0x128A)
#define NVMCTRL_CTRLA  _SFR_MEM8(0x1000)
Note: When writing the fuses write all reserved bits to ‘1’.
*/
/*******/


UART_Tx(0x55);                              //Send lock bit address to NVMCTRL.ADD registers
UART_Tx(0x45);                                
UART_Tx(0x08);                                //Address of NVMCTRL.ADD registers                        
UART_Tx(0x10);
//UART_Rx();
sendHex(16,UART_Rx() | 0x01);
Timer_T0_sub(T0_delay_40us);
UART_Tx(fuse_add);                                //Adddress of lock byte
UART_Tx(fuse_add >> 8);
sendHex(16,UART_Rx() | 0x02);
//UART_Rx();
Timer_T0_sub(T0_delay_40us);

UART_Tx(0x55);                                //Send key 0xC5 (device unlocked) to NVMCTRL.DATA register)
UART_Tx(0x44);                                
UART_Tx(0x06);                                //Adress of NVMCTRL.DATA registers                       
UART_Tx(0x10);
sendHex(16,UART_Rx() |0x03);
//UART_Rx();
Timer_T0_sub(T0_delay_40us);
UART_Tx(value); 

UART_Rx();//Data byte C5
//Timer_T0_sub(T0_delay_200us);
Timer_T0_sub(T0_delay_5ms);


UART_Tx(0x55);                                //Enter write to fuse byte command into NVM.CTRLA register
UART_Tx(0x44);                                
UART_Tx(0x00);                                //Register address                         
UART_Tx(0x10);
sendHex(16,UART_Rx() |0x04);
//UART_Rx();
Timer_T0_sub(T0_delay_5us);
UART_Tx(0x07);                                 //Command to update the fuses.  CMD is 0x07
sendHex(16,UART_Rx() | 0x05);
//UART_Rx();
Timer_T0_sub(T0_delay_200us);
UPDI_reset;

sendString("UNO reset\r\n");
set_up_NVM_prog();}



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
Timer_T0_sub(T0_delay_200us);} 
}
