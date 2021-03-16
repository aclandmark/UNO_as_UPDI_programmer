/*
 Programming AVR devices with a UPDI interface
 Arduino sketch to be downloaded to a UNO
 Open a terminal program before running the shetch.
 Set terminal to 28800Hz, 8 data bits, 1 stop bits, no parity and no handshaking
 User prompt R R R..... is generated
 Press R and then respond to the on-screen instructions
 If the "compiled binary file" is exported (see Shetch drop down menue)
 it can be copied to an Atmega328 using the UNO AVR Programmer.
 The only difference is that the baud rate must be reduced to 19.2K
 (Note: The internal RC clock may also need calibrating but this can usually
 be avoided if several devices are available.)

 This Shetch has only been tested with ATtiny 806/1606 both excellent devices for the
 newcommer: They are available in a SOIC package that can be hand soldered and they have 
 hardware I2C and UART modules rather the the USI that is software driven. 
 Test projects have been written for the ATtiny1606 using Atmel Studio 7.
 
Note:  It is hoped that any newcommer persuing this project will find both Arduino and  
Studio 7 equally invaluable when attempting to build projects in the easiest and most flexible way.
Programmers note pad is also highly recommended for viewing C-files that Arduino can't open easily.  
 */



#include "Resources_UPDI_IP_V8.h"
char mode = 0;
int main (void){ 
char kepress;
setup_328_HW;                                           //see "Resources\ATMEGA_Programmer.h"
P_counter = 0;
User_prompt;

sendString("\r\nUPDI programer: UNO_UPDI_IP_V8 (also works on an Atmega 328 clocked at 8MHz)\r\n\r\n");
sendString ("Press any key to start\r\n\r\n");
waitforkeypress();

/********************************Programmer target connection sequence************************************/
Timer_T1_sub_with_interrupt(T1_delay_100ms);
sei();
contact_target;
configure_updi;                                       //Increases UPDI clock to 16MHz and reduce guard band to 2 cycles
print_out_SIB;
if(UPDI_timeout)
{sendString("Power Cycle Required\r\n");
while(1);}
else{cli();TCCR1B = 0;
TIMSK1 &= (~(1 << TOIE1));}


initialise_NVM_programming;
sendString("Programming fuses\r\n");
write_fuse (WDTCFG, 0x0);                             //Default value: WDT under program control
write_fuse (BODCFG,0x0);                              //Default value: BOD dissabled
write_fuse (OSCCFG, 0x7E);                            //Default value: 20MHz internal clock
write_fuse (SYSCFG0, 0xF7);                           //UPDI enabled, EEPROM preserved at chip erase
write_fuse (SYSCFG1, 0xFD);                           //16mS SUT
write_fuse (APPEND, 0x0);                             //Default value: No flash partitions
write_fuse (BOOTEND, 0x0);                            //Default value: No flash partitions



sendString("\r\nProgram flash with hex? Y or N");
if (waitforkeypress() == 'Y')
{ mode = 'H';
  sendString("\r\nSend file  ");
  Program_Flash_Hex();
}
Verify_Flash_Hex();
sendString("\r\nFuses:\tWDT, BOD, OSC,SYS0\tSYS1, APPEND, BOOTEND\r\n\t");
read_out_fuses();

sendString("\r\n\r\nProgram flash with text? Y or N");
if (waitforkeypress() == 'Y')
{ mode = 'T';
sendString("\r\nSend file  ");
  Program_Flash_Text();}




sendString("\r\n\r\nUnlocking device");
write_fuse (0x128A, 0xC5);                              //Device unlocked
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
ISR(USART_RX_vect){if (mode =='H')upload_hex();
                    if (mode =='T')upload_text();}
                    
ISR(TIMER1_OVF_vect) {UPDI_timeout = 1;TCCR1B = 0;}

void upload_text(void){}
void Program_Flash_Text(void){}
/***********************************************************************************************/

/*
Routines used to drive the UART interface when progamming config bytes and for other non-time
critical operations.  Use of subroutines rather than code definitions make efficient use of program
memory but is not so good on speed.
 */


void UART_Tx(unsigned int data_byte_T){               //starts Hi Z
  unsigned char parity = 0;
  DDRC |= (1 << DDC0);                                //start bit
  clock_delay_T;

for (int n = 0; n <= 7; n++){
  if (data_byte_T & (1 << n))
  {DDRC &= (~(1 << DDC0)); parity += 1;}
 else {DDRC |= (1 << DDC0);}clock_delay_T;}           //clock data out

if (parity%2){DDRC &= (~(1 << DDC0));} 
else {DDRC |= (1 << DDC0);}
  clock_delay_T;                                      //clock parity bit
  DDRC &= (~(1 << DDC0));                             //initiate stop bit 
  clock_delay_T;                                      //Stop bits
  clock_delay_T;}




  
/************************************************************************************************/
unsigned char UART_Rx(void){
  unsigned char data_byte_R = 0;
    char parity = 0;
                          
  while (PINC & (1 << PINC0));                        //wait for start bit
  clock_delay_R;                                      //The full delay is required 
  for (int n= 0; n <= 7; n++){clock_delay_R;
    if (PINC & (1 << PINC0))
    {data_byte_R |= (1 << n); parity += 1;}}
    clock_delay_R;

 if ((PINC & (1 << PINC0)) && (parity%2)); 
 else P_counter++;                                      //Parity often received in error?????
 
 clock_delay_R;                                         //stop bit
 clock_delay_R;                                         //stop bit  
 return data_byte_R;}
