/*
 * UPDI_programmer.c
 *
 *Uses UNO A0 PCO as programmer output
 */ 

#include "Project_header_file.h"




int main(void)
{ char kepress;
int start_add;
int flash_data;
int page_address;


    setup_328_HW;
  
  User_prompt;                                        //PINC0 initialised Hi Z input
 
  sendString("\r\nUPDI development program:  POR required to reset UPDI:  AK to start\r\n");
 waitforkeypress();
 
  if(PINC & (1 << PINC0))
  DDRC |= (1 << DDC0);                                //Drive reset low
else while(1);
  
  delay_of_0_25uS;                                    //Wait 0.25uS. None:OK, 0.75uS:OK, 5mS:fails
 
  DDRC &= (~(1 << DDC0));                             //Release the line: Hi Z
  
  while(!(PINC & (1 << PINC0)))test += 1;             //estimated delay 8 to 10uS  Spec 10-200uS
  
  Timer_T0_sub(T0_delay_200us);
Timer_T0_sub(T0_delay_200us);


do {
 UART_Tx(0x55);
  UART_Tx (0xE5);
 for(int m = 0; m <= 15; m++){
 SIB_byte[m] = UART_Rx(); }
 sendString("\r\nSIB readout:\t");
for(int m = 0; m <= 15;m++){
  sendChar(SIB_byte[m]);}
    sendString("\tFamily name, NVM version, OCD version, DGB_OSC frequency setting.");
    sendString("\r\n-x- to continue, AOK to repeat.\r\n");
    kepress = waitforkeypress();}while(kepress != 'x');
//sendString("-x- to continue, AOK to repeat.");


sendString("\r\nInitialising NVM programming\r\n");

/***************chip erase************************/
Erase_code();

newline();
set_up_NVM_prog();

sendString("\r\nSignature byte readout\t\t");
start_add = 0x1100;
for(int m = 0; m<=2; m++){
UART_Tx(0x55);
UART_Tx(0x04);
UART_Tx(start_add++);
UART_Tx(start_add >> 8);
sendHex(16, UART_Rx());
Timer_T0_sub(T0_delay_200us);}



sendString("\r\nProgramming flash");
waitforkeypress();
flash_data = 0;
for(int n = 0; n <= 3; n++){
page_address = 0x8000 + n*0x40;
start_add = page_address;
for(int m = 0; m<=63; m++){
UART_Tx(0x55);
UART_Tx(0x44);
UART_Tx(start_add);
UART_Tx(start_add >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);
UART_Tx(flash_data++);
if(UART_Rx() == 0x40); else while(1);
Timer_T0_sub(T0_delay_200us);
start_add += 1;}
 
newline();
State_page_address(page_address);
burn_page;

Read_add_of_last_page();
if (n == 3)sendString("\r\nRead flash\r\n");}

waitforkeypress();
newline();
start_add = 0x8000;
for(int m = 0; m<=255; m++){

UART_Tx(0x55);
UART_Tx(0x04);
UART_Tx(start_add++);
UART_Tx(start_add >> 8);
sendHex(16, UART_Rx());
Timer_T0_sub(T0_delay_200us);}





while(1);
}


ISR(TIMER1_OVF_vect) {Chip_erase_timeout = 1;TCCR1B = 0;}



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
