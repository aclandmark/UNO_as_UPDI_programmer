//All test files programmed OK at 19.2KHz  Fills 16K flash in 33 seconds
//Text book files only!!!!!!


#include "UPDI_programmer_NM_V3A.h"

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
write_fuse (APPEND, 0x0);            //Default value: No flash partitions
write_fuse (BOOTEND, 0x0);          //Default value: No flash partitions


sendString("\r\nProgram flash? Y or N");
if (waitforkeypress() == 'Y')
{ sendString("\r\nSend file  ");
  Program_Flash_Hex();
}
Verify_Flash_Hex();


sendString("\r\nFuses:\tWDT, BOD, OSC,SYS0\tSYS1, APPEND, BOOTEND\r\n\t");
read_out_fuses();



sendString("\r\n\r\nSession terminated.\r\n\r\n");


return 1;}





/***************************************************************************************************************************************************/
ISR(USART_RX_vect){upload_hex();}
ISR(TIMER1_OVF_vect) {UPDI_timeout = 1;TCCR1B = 0;}



/************************************************************************************************************************/

 
  
