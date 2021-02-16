
#include "Resources_UNO_UPDI_programmer_V2E.h"

int main (void){ 
char kepress;
setup_328_HW;                                                     //see "Resources\ATMEGA_Programmer.h"
P_counter = 0;
User_prompt;

sendString("\r\nUPDI development program:  POR required to reset UPDI:  AK to start\r\n");
waitforkeypress();

/********************************Programmer target connection sequence************************************/
contact_target;

set_UPDI_clock_and_guard_band;

Print_out_SIB;


/*****************************Get ready to program to flash*********************************************/
Initialise_NVM_Programming;

write_fuse (WDTCFG, 0x0);           //Default value: WDT under program control
write_fuse (BODCFG,0x0);            //Default value: BOD dissabled
write_fuse (OSCCFG, 0x7E);          //Default value: 20MHz internal clock
write_fuse (SYSCFG0, 0xF7);         //UPDI enabled, EEPROM preserved at chip erase
write_fuse (SYSCFG1, 0xFD);         //16mS SUT
write_fuse (APPEND, 0x0);            //Default value: No flash partitions
write_fuse (BOOTEND, 0x0);          //Default value: No flash partitions




/**************************Set fuses to user values*************************************************/
/*sendString("\r\n\r\nUpdate SYSCFG0 and SYSCFG1 for 16mS SUT and ");
sendString("\r\nEEprom preserved at chip erase");
sendString("\r\nY to proceed, AOK to skip");
if (waitforkeypress() == 'Y'){
write_fuse (0x1285, 0xF7);                                                      //EEPROM preserved
write_fuse (0x1286, 0xFD);                                                      //16ms SUT
sendString("\r\n\r\nNew fuse bytes:\t");
read_out_fuses();}*/

newline();

sendString("\r\nProgram flash? Y or N");
if (waitforkeypress() == 'Y')
{ sendString("\r\nSend file");
  Program_Flash_Hex();}
Verify_Flash_Hex();

sendString("\r\nFuses: WDT, BOD, OSC,SYS0, SYS1, APPEND, BOOTEND\r\n\t");
read_out_fuses();

 while(1);                                                                      //Wait for UNO POR
return 1;}





/***************************************************************************************************************************************************/
ISR(USART_RX_vect){upload_hex();}
ISR(TIMER1_OVF_vect) {UPDI_timeout = 1;TCCR1B = 0;}


/***********************************************************************************************/
void UART_Tx_Ls(unsigned int data_byte_T){                                       //starts Hi Z
  unsigned char parity = 0;
  DDRC |= (1 << DDC0);                                                        //start bit
  clock_delay_T_Ls;

for (int n = 0; n <= 7; n++){
  if (data_byte_T & (1 << n))
  {DDRC &= (~(1 << DDC0)); parity += 1;}
 else {DDRC |= (1 << DDC0);}clock_delay_T_Ls;}                                   //clock data out

if (parity%2){DDRC &= (~(1 << DDC0));} 
else {DDRC |= (1 << DDC0);}
  clock_delay_T_Ls;                                                              //clock parity bit
  DDRC &= (~(1 << DDC0));                                                     //initiate stop bit 
  clock_delay_T_Ls;                                                              //Stop bits
  clock_delay_T_Ls;}




  
/************************************************************************************************/
unsigned char UART_Rx_Ls(void){
  unsigned char data_byte_R = 0;
    char parity = 0;
                          
  while (PINC & (1 << PINC0));                                              //wait for start bit
  clock_delay_R_Ls;                                                            //The full delay is required 
  for (int n= 0; n <= 7; n++){clock_delay_R_Ls;
    if (PINC & (1 << PINC0)){data_byte_R |= (1 << n); parity += 1;}}
    clock_delay_R_Ls;

 if ((PINC & (1 << PINC0)) && (parity%2)); 
 else P_counter++;                                                            //Parity often received in error?????
 
 clock_delay_R_Ls;                                                               //stop bit
 clock_delay_R_Ls;                                                               //stop bit  
 return data_byte_R;}




/************************************************************************************************************************/
 void UART_Tx(unsigned int data_byte_T){                          //starts Hi Z
  unsigned char parity = 0;
  DDRC |= (1 << DDC0);                                                    //start bit
  clock_delay_T;

for (int n = 0; n <= 7; n++){
  if (data_byte_T & (1 << n))
  {DDRC &= (~(1 << DDC0)); parity += 1;}
 else {DDRC |= (1 << DDC0);}clock_delay_T;}                        //clock data out

if (parity%2){DDRC &= (~(1 << DDC0));} 
else {DDRC |= (1 << DDC0);}
  clock_delay_T;                                                   //clock parity bit
  DDRC &= (~(1 << DDC0));                                                 //initiate stop bit 
  clock_delay_T;                                                   //Stop bit
  clock_delay_T;}




  
/************************************************************************************************/
unsigned char UART_Rx(void){
  unsigned char data_byte_R = 0;
    char parity = 0;
                          
  while (PINC & (1 << PINC0));                                        //wait for start bit
 
  clock_delay_R;                                               //The full delay is required
  for (int n= 0; n <= 7; n++){clock_delay_R;
    if (PINC & (1 << PINC0)){data_byte_R |= (1 << n); parity += 1;}}
    
clock_delay_R;                                           
 if ((PINC & (1 << PINC0)) && (parity%2)); 
 else sendChar('P');                                                  //Parity check
  
 clock_delay_R;                                               //stop bit
 clock_delay_R;                                               //stop bit  
 return data_byte_R;}




 /*****************************************************************************************************/
/************************************************************************************************************************/
void UART_break(void){                                          //starts Hi Z
  DDRC |= (1 << DDC0);                                                    //start bit
  for (int n = 0; n <= 15; n++){clock_delay_T;}                    //Hold low for all 12 bits
  DDRC &= (~(1 << DDC0));                                                 //Restore Hi Z
Timer_T0_sub(T0_delay_30us);

sendChar('Y');}
 
  
