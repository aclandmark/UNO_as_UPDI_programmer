char txt2bin(char A, char B);
void send_key(const char*);



void Erase_code (void){

send_key(Key_chip_erase);


UART_Tx(0x55);
UART_Tx(0x87);                              /*Test Chip erase bit 3 of ASI_KEY_STATUS register (reg 7)*/
if(UART_Rx() & 0x08);                       /*should be 0x08*/
else sendString("\r\nChip erase failed");

UPDI_reset;

Chip_erase_timeout = 0;
Timer_T1_sub_with_interrupt(T1_delay_1Sec);
sei();
do {                                    /*wait for chip erase to complete*/
Timer_T0_sub(T0_delay_200us);         // Estimated to takes just under 5ms
UART_Tx(0x55);
UART_Tx(0x8B);                          /*Test lockstatus, bit 0 of SYS_STATUS register (reg B)*/
Rx_Byte = UART_Rx();
test += 1;
if(Chip_erase_timeout)break;
}while(Rx_Byte & 0x01);                 /*Exit when lock status bit is zero*/
cli();

if (Chip_erase_timeout) sendString("\r\nFailure: Device still locked\t");
else sendString("\r\nOK Device unlocked\t\t\t");
sendHex(16, Rx_Byte);
sendHex(10, test);}




void set_up_NVM_prog(void){
send_key(Key_NVM_prog);
UART_Tx(0x55);
UART_Tx(0x87);                              //Test Chip erase bit 3 of ASI_KEY_STATUS register (reg 7)
UART_Rx();

UPDI_reset;



Chip_erase_timeout = 0;
Timer_T1_sub_with_interrupt(T1_delay_1Sec);
sei();
do {                                    /*wait for chip erase to complete*/
Timer_T0_sub(T0_delay_200us);           //Estimated to takes just under 5ms*/
UART_Tx(0x55);
UART_Tx(0x8B);                          /*Test lockstatus, bit 0 of SYS_STATUS register (reg B)*/
Rx_Byte = UART_Rx();
test += 1;
if(Chip_erase_timeout)break;
}while(!(Rx_Byte & 0x08));                 /*Exit when lock status bit is zero*/
cli();

if (Chip_erase_timeout) sendString("\r\nNVMPROG failure: \t");
else sendString("\r\nOK Ready for NVM programming\t");
sendHex(16, Rx_Byte);
sendHex(10, test);
newline();}









char txt2bin(char A, char B){
A -= '0';
if (A > 9) A -= 7;
B -= '0';
if (B > 9) B -= 7;
return (A * 0x10 + B);}



void send_key(const char *Key){
UART_Tx(0x55);
UART_Tx (0xE0);
char key_h, key_l;
for(int m = 0; m <= 7; m++){
key_h = *(Key + 2*(8 - m));
key_l = *(Key + 2*(8 - m) + 1);
UART_Tx(txt2bin(key_h,key_l));}}




void State_page_address(int PA){
int reg_add;
reg_add = 0x1008;               //state page address
UART_Tx(0x55);
UART_Tx(0x45);
UART_Tx(reg_add);
UART_Tx(reg_add >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);
UART_Tx(PA);
UART_Tx(PA >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);
}



void Read_add_of_last_page(void){
int reg_add;

reg_add = 0x1008;
UART_Tx(0x55);
UART_Tx(0x04);
UART_Tx(reg_add++);
UART_Tx(reg_add >> 8);
sendHex(16, UART_Rx());
Timer_T0_sub(T0_delay_200us);
UART_Tx(0x55);
UART_Tx(0x04);
UART_Tx(reg_add);
UART_Tx(reg_add >> 8);
sendHex(16, UART_Rx());
Timer_T0_sub(T0_delay_200us);
}



