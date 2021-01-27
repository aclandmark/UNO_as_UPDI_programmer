/*
 * UPDI_programmer.c
 *
 *Uses UNO A0 PCO as programmer output
 */ 

#include "Project_header_file.h"



void UART_Tx(int);
unsigned char UART_Rx(void);
void Bin_to_PC(unsigned char);



int main(void)
{ unsigned char Sig_byte[24];

char address = 0x80;
  char offset = 0;
int eep_add;
int eep_offset;


int test = 0;
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
 Sig_byte[m] = UART_Rx(); }
 sendString("\r\nSIB readout:\t");
for(int m = 0; m <= 15;m++){
  sendChar(Sig_byte[m]);}
  sendString("\tFamily name, NVM version, OCD version, DGB_OSC frequency setting.");}while(waitforkeypress() != 'x');

sendString("\r\n OK");

while(1);
}



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
