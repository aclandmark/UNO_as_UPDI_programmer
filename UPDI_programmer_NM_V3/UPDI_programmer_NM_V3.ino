//All test files programmed OK at 19.2KHz  Fills 16K flash in 33 seconds
//Text book files only!!!!!!


#include "UPDI_programmer_NM_V3.h"

int main (void){ 
char kepress, Rx_data;

setup_328_HW;                                                     //see "Resources\ATMEGA_Programmer.h"
P_counter = 0;
User_prompt;

sendString("\r\nUPDI development program:  POR required to reset UPDI:  AK to start\r\n\r\n");
waitforkeypress();

/********************************Programmer target connection sequence************************************/
Timer_T1_sub_with_interrupt(T1_delay_100ms);
sei();
contact_target;
configure_updi_LS; 
print_out_SIB_2_LS;

if(UPDI_timeout){sendString("Power Cycle Required\r\n");while(1);}
else{cli();TCCR1B = 0;TIMSK1 &= (~(1 << TOIE1));}

initialise_NVM_programming;

sendString("\r\nProgramming fuses\r\n");
//CAN WE PROGRAM FUSES without erasing flash memory
write_fuse (WDTCFG, 0x0);           //Default value: WDT under program control
write_fuse (BODCFG,0x0);            //Default value: BOD dissabled
write_fuse (OSCCFG, 0x7E);          //Default value: 20MHz internal clock
write_fuse (SYSCFG0, 0xF7);         //UPDI enabled, EEPROM preserved at chip erase
write_fuse (SYSCFG1, 0xFD);         //16mS SUT
write_fuse (APPEND, 0x02);            //Default value: No flash partitions
write_fuse (BOOTEND, 0x03);          //Default value: No flash partitions


sendString("\r\nProgram flash? Y or N");
if (waitforkeypress() == 'Y')
{ sendString("\r\nSend file  ");
  Program_Flash_Hex();
}
Verify_Flash_Hex();

//waitforkeypress();

sendString("\r\nFuses:\tWDT, BOD, OSC,SYS0\tSYS1, APPEND, BOOTEND\r\n\t");
read_out_fuses();

while(1);

sendString("\r\n\r\nUnlocking device");
write_fuse (0x128A, 0xC5);                                        //Device unlocked
sendString("\r\nY to Lock device or AOK");
if (waitforkeypress() == 'Y')
{write_fuse (0x128A, 0x0);
sendString("\r\nDevice locked\r\n");}
else sendString("\r\nDevice unlocked\r\n");

UPDI_reset;

sendString("Sesion terminated\r\n\r\n");
Dissable_UPDI_sesion;

SW_reset;

return 1;}





/***************************************************************************************************************************************************/
ISR(USART_RX_vect){upload_hex();}
ISR(TIMER1_OVF_vect) {UPDI_timeout = 1;TCCR1B = 0;}


/***********************************************************************************************/
void UART_Tx(unsigned int data_byte_T){                                       //starts Hi Z
  unsigned char parity = 0;
  DDRC |= (1 << DDC0);                                                        //start bit
  clock_delay_T;

for (int n = 0; n <= 7; n++){
  if (data_byte_T & (1 << n))
  {DDRC &= (~(1 << DDC0)); parity += 1;}
 else {DDRC |= (1 << DDC0);}clock_delay_T;}                                   //clock data out

if (parity%2){DDRC &= (~(1 << DDC0));} 
else {DDRC |= (1 << DDC0);}
  clock_delay_T;                                                              //clock parity bit
  DDRC &= (~(1 << DDC0));                                                     //initiate stop bit 
  clock_delay_T;                                                              //Stop bits
  clock_delay_T;}




  
/************************************************************************************************/
unsigned char UART_Rx(void){
  unsigned char data_byte_R = 0;
    char parity = 0;
                          
  while (PINC & (1 << PINC0));                                              //wait for start bit
  clock_delay_R;                                                            //The full delay is required 
  for (int n= 0; n <= 7; n++){clock_delay_R;
    if (PINC & (1 << PINC0)){data_byte_R |= (1 << n); parity += 1;}}
    clock_delay_R;

 if ((PINC & (1 << PINC0)) && (parity%2)); 
 else P_counter++;                                                            //Parity often received in error?????
 
 clock_delay_R;                                                               //stop bit
 clock_delay_R;                                                               //stop bit  
 return data_byte_R;}




/************************************************************************************************************************/
 void UART_Tx_upload(unsigned int data_byte_T){                          //starts Hi Z
  unsigned char parity = 0;
  DDRC |= (1 << DDC0);                                                    //start bit
  clock_delay_T_upload;

for (int n = 0; n <= 7; n++){
  if (data_byte_T & (1 << n))
  {DDRC &= (~(1 << DDC0)); parity += 1;}
 else {DDRC |= (1 << DDC0);}clock_delay_T_upload;}                        //clock data out

if (parity%2){DDRC &= (~(1 << DDC0));} 
else {DDRC |= (1 << DDC0);}
  clock_delay_T_upload;                                                   //clock parity bit
  DDRC &= (~(1 << DDC0));                                                 //initiate stop bit 
  clock_delay_T_upload;                                                   //Stop bit
  clock_delay_T_upload;}




  
/************************************************************************************************/
unsigned char UART_Rx_upload(void){
  unsigned char data_byte_R = 0;
    char parity = 0;
                          
  while (PINC & (1 << PINC0));                                        //wait for start bit
 
  clock_delay_R_upload;                                               //The full delay is required
  for (int n= 0; n <= 7; n++){clock_delay_R_upload;
    if (PINC & (1 << PINC0)){data_byte_R |= (1 << n); parity += 1;}}
    
clock_delay_R_upload;                                           
 if ((PINC & (1 << PINC0)) && (parity%2)); 
 else sendChar('P');                                                  //Parity check
  
 clock_delay_R_upload;                                               //stop bit
 clock_delay_R_upload;                                               //stop bit  
 return data_byte_R;}




 /*****************************************************************************************************/
/************************************************************************************************************************/
void UART_Tx_break_upload(void){                                          //starts Hi Z
  DDRC |= (1 << DDC0);                                                    //start bit
  for (int n = 0; n <= 15; n++){clock_delay_T_upload;}                    //Hold low for all 12 bits
  DDRC &= (~(1 << DDC0));                                                 //Restore Hi Z
Timer_T0_sub(T0_delay_30us);
//sendChar('Y');
}
 
  
