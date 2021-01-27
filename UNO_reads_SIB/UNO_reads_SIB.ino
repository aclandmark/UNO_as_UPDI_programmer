/*
 * UPDI_programmer_V1.c
 * UNO establishes comms with the UPDI and reads the SIB
 *
 *Uses UNO A0 PCO as programmer output
 */ 

#include "Project_header_file.h"



void UART_Tx(int);
unsigned char UART_Rx(void);




int main(void)
{ unsigned char SIB_bytes[16];
int test = 0;                                          //Used to estimate time

    setup_328_HW;                                     //PINC0 initialised Hi Z input
    User_prompt;
 
  sendString("\r\nUPDI development program:  POR required to reset UPDI:  AK to start\r\n");
 waitforkeypress();
 
  if(PINC & (1 << PINC0))                             //Shoud be high following POR
  DDRC |= (1 << DDC0);                                //Drive reset low
else while(1);                                         //Stop program if in error
  
  delay_of_0_25uS;                                    //Wait 0.25uS. None:OK, 0.75uS:OK, 5mS:fails
 
  DDRC &= (~(1 << DDC0));                             //Release the line: Hi Z
  
  while(!(PINC & (1 << PINC0)))test += 1;             //estimated delay 8 to 10uS  Spec 10-200uS
  
  Timer_T0_sub(T0_delay_200us);                       //200us is probably always OK
Timer_T0_sub(T0_delay_200us);                         //400uS should be fine


do {
 UART_Tx(0x55);                                     //Synch word
  UART_Tx (0xE5);                                   //Request SIB
 for(int m = 0; m <= 15; m++){
 SIB_bytes[m] = UART_Rx(); }                        //UPDI responds with 16 bytes
 sendString("\r\nSIB readout:\t");
for(int m = 0; m <= 15;m++){                        //Print out the SIB
  sendChar(SIB_bytes[m]);}
  sendString("\tFamily name, NVM version, OCD version, DGB_OSC frequency setting.");}while(waitforkeypress() != 'x');

sendString("\r\n OK");

while(1);
}


/********************************************************************************************/
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


  
/********************************************************************************************/
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
