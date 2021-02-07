
#include "Resources_UNO_UPDI_programmer_V2.h"

int main (void){ 
char kepress;
setup_328_HW;                                                     //see "Resources\ATMEGA_Programmer.h"

User_prompt;

sendString("\r\nUPDI development program:  POR required to reset UPDI:  AK to start\r\n");
waitforkeypress();

/********************************Programmer target connection sequence************************************/
  if(PINC & (1 << PINC0))
  DDRC |= (1 << DDC0);                                //Drive reset low
else {sendString ("Device not detected\r\n");
while(1);}
  delay_of_0_25uS;                                    //Wait 0.25uS. None:OK, 0.75uS:OK, 5mS:fails
   DDRC &= (~(1 << DDC0));                             //Release the line: Hi Z
  while(!(PINC & (1 << PINC0)))test += 1;             //estimated delay 8 to 10uS  Spec 10-200uS
   Timer_T0_sub(T0_delay_200us);
Timer_T0_sub(T0_delay_200us);


UART_Tx(0x55);                //Increase UPDI clock
UART_Tx (STCS | 0x09);
UART_Tx (0xFD);             //01try FD

Timer_T0_sub(T0_delay_200us);

UART_Tx(0x55);                //limit guard band
UART_Tx (STCS | 0x02);
UART_Tx (0x46);

Timer_T0_sub(T0_delay_200us);

/****************************Print out the SIB****************************/
do {
 UART_Tx(0x55);
  UART_Tx (download_SIB);
 for(int m = 0; m <= 15; m++){
 SIB_byte[m] = UART_Rx(); }
 sendString("\r\nSIB readout:\t");
for(int m = 0; m <= 15;m++){
  sendChar(SIB_byte[m]);}
    sendString("\tFamily name, NVM version, OCD version, DGB_OSC frequency setting.");
    sendString("\r\n-x- to continue, AOK to repeat.\r\n");
    kepress = waitforkeypress();}while(kepress != 'x');


//while(1){
/*****************************Get ready to program to flash*********************************************/
sendString("\r\nInitialising NVM programming\r\n");
sendString("\r\nErase chip? Y or N");
if (waitforkeypress() == 'Y'){sendString ("\tChip erased");
Erase_code();}
newline();
if (set_up_NVM_prog())
sendString("\r\nSignature byte readout\t\t");
else {sendString("\r\nPOR and Erase to proceed!"); while(1);}
read_out_signature_bytes();
newline();
sendString("\r\nFuse bytes:\t");
read_out_fuses();

/**************************Set fuses to user values*************************************************/
sendString("\r\n\r\nUpdate SYSCFG0 and SYSCFG1 for 16mS SUT and ");
sendString("\r\nEEprom preserved at chip erase");
sendString("\r\nY to proceed, AOK to skip");
if (waitforkeypress() == 'Y'){
write_fuse (0x1285, 0xF7);          //CFG0
write_fuse (0x1286, 0xFD);          //16ms SUT
sendString("\r\n\r\nNew fuse bytes:\t");
read_out_fuses();}



sendString("\r\nProgram flash? Y or N");
if (waitforkeypress() == 'Y')
{Program_Flash_Hex();}
//Verify_Flash_Hex();


 while(1);                                                //Wait for UNO reset
return 1;}





/***************************************************************************************************************************************************/
ISR(USART_RX_vect){upload_hex();}
ISR(TIMER1_OVF_vect) {UPDI_timeout = 1;TCCR1B = 0;}


/***********************************************************************************************/
void UART_Tx(int data_byte_T){                          //starts Hi Z
  unsigned char parity = 0;
  DDRC |= (1 << DDC0);                                  //start bit
  clock_delay_T;

for (int n = 0; n <= 7; n++){
  if (data_byte_T & (1 << n))
  {DDRC &= (~(1 << DDC0)); parity += 1;}
 else {DDRC |= (1 << DDC0);}clock_delay_T;}             //clock data out

if (parity%2){DDRC &= (~(1 << DDC0));} 
else {DDRC |= (1 << DDC0);}
  clock_delay_T;                                          //clock parity bit
  DDRC &= (~(1 << DDC0));                                 //initiate stop bit 
  clock_delay_T;                                          //Stop bit
  clock_delay_T;}




  
/************************************************************************************************/
unsigned char UART_Rx(void){
  unsigned char data_byte_R = 0;
    char parity = 0;
                          
  while (PINC & (1 << PINC0));                                //wait for start bit
  half_clock_delay_R;
  
  for (int n= 0; n <= 7; n++){clock_delay_R;
    if (PINC & (1 << PINC0)){data_byte_R |= (1 << n); parity += 1;}
    }
clock_delay_R;    
 if ((PINC & (1 << PINC0)) && (parity%2)); 
 //else sendChar('P');                                        //Parity often received in error?????
  
 clock_delay_R;                                               //stop bit
 clock_delay_R;                                               //stop bit  
 return data_byte_R;}




/************************************************************************************************************************/
 void UART_Tx_upload(int data_byte_T){                          //starts Hi Z
  unsigned char parity = 0;
  DDRC |= (1 << DDC0);                                  //start bit
  clock_delay_T_upload;

for (int n = 0; n <= 7; n++){
  if (data_byte_T & (1 << n))
  {DDRC &= (~(1 << DDC0)); parity += 1;}
 else {DDRC |= (1 << DDC0);}clock_delay_T_upload;}             //clock data out

if (parity%2){DDRC &= (~(1 << DDC0));} 
else {DDRC |= (1 << DDC0);}
  clock_delay_T_upload;                                          //clock parity bit
  DDRC &= (~(1 << DDC0));                                 //initiate stop bit 
  clock_delay_T_upload;                                          //Stop bit
  clock_delay_T_upload;}




  
/************************************************************************************************/
unsigned char UART_Rx_upload(void){
  unsigned char data_byte_R = 0;
    char parity = 0;
                          
  while (PINC & (1 << PINC0));                                //wait for start bit
  half_clock_delay_R_upload;
  
  for (int n= 0; n <= 7; n++){clock_delay_R_upload;
    if (PINC & (1 << PINC0)){data_byte_R |= (1 << n); parity += 1;}
    }
clock_delay_R_upload;    
 if ((PINC & (1 << PINC0)) && (parity%2)); 
 //else sendChar('P');                                        //Parity often received in error?????
  
 clock_delay_R_upload;                                               //stop bit
 clock_delay_R_upload;                                               //stop bit  
 return data_byte_R;}




 /*****************************************************************************************************/

 
  
