/*
 * UPDI_programmer.c
 *
 *Uses UNO A0 PCO as programmer output
 */ 

#include "Project_header_file.h"

#define delay_of_0_25uS      asm("nop"); asm("nop"); asm("nop"); asm("nop");
#define delay_of_0_75uS     delay_of_0_25uS; delay_of_0_25uS; delay_of_0_25uS;
#define out_h         PORTC |= (1 << PORTC0)
#define out_l         PORTC &= (~(1 << PORTC0))
#define clock_delay_T     Timer_T0_sub(T0_delay_1ms)
#define input_h         (PINC & (1 << PINC0))
#define clock_delay_R     Timer_T0_sub(T0_delay_1ms)
#define half_clock_delay_R    Timer_T0_sub(T0_delay_500us)

void UART_Tx(unsigned char);
unsigned char UART_Rx(void);
void Bin_to_PC(unsigned char);

int main(void)
{ unsigned char Sig_byte;
    setup_328_HW;
  
  User_prompt;
  
  
  DDRC |= (1 << DDC0);          //Set PC0 to output high
  
  sendString("\r\nUPDI development program: AK to start\r\n");
  
  
  
  PORTC &= (~(1 << PORTC0));              //Issue reset
  Timer_T0_10mS_delay_x_m(1);             //5ms pulse
  PORTC |= (1 << PORTC0);               //Release reset
  
  
  delay_of_0_25uS;                  //Wait 0.25uS for UDPI to respond
  PORTC &= (~(1 << PORTC0));              //Drive reset low
  delay_of_0_75uS;                  //Wait 0.75uS
  PORTC |= (1 << PORTC0);               //Release the line
  Timer_T0_sub(T0_delay_200us);           //Wait 200uS for UDPI to stabilise
  UART_Tx(0x55);                    //Send SYNCH
  UART_Tx (0);                    //LDS command
  UART_Tx (0);                    //Address of signature byte
  Sig_byte = UART_Rx();
  
  //Bin_to_PC(Sig_byte);
  sendHex(16, Sig_byte);
  while(1);
}



void UART_Tx(unsigned char data_byte_T){
  unsigned char parity = 0;
  DDRC |= (1 << DDC0);                    //UART in Tx mode
  out_l;                            //start bit
  clock_delay_T;
for (int n = 0; n <= 7; n++){
  if (data_byte_T & (1 << n))
  {out_h; parity += 1;} else out_l;clock_delay_T;}      //clock data out
  if (parity%2)out_h; else out_l;clock_delay_T;     //clock parity bit
  out_h;                          //initiate stop bit 
  clock_delay_T;                      //Stop bit
  clock_delay_T;
  clock_delay_T;
  
}

unsigned char UART_Rx(void){
  unsigned char data_byte_R = 0;
  DDRC &= (~(1 << DDC0));                 //UART in Receive mode
  PORTC &= (~(1 << PORTC0));
  while input_h;                    //wait for start bit
  half_clock_delay_R;
  
  for (int n= 0; n <= 7; n++){
    if (input_h){data_byte_R |= (1 << n);}
    clock_delay_R;
    }
    return data_byte_R;}
