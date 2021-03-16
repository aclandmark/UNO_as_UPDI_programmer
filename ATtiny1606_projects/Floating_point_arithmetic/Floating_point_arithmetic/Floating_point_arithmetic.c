/*
 Basic_UART.c
 Uses 20MHz clock
 ATtiny has signature bytes 0x1E, 0x94, 0x24
 Default value of CLK_PER is 3.333MHz
 All CLKCTRL registers are left if their default states
 
 TxD pin is PB2 pin 9
 RxD pin is PB3 pin 8
 TCA 
*/
volatile char cr_keypress;

#include "Floating_point_arithmetic.h"

int main(void)
{char keypress, User_response;
	
	setup_HW; 
	
	User_prompt;
	while(1){
	string_to_PC("\r\nPress R or r to continue\r\n");
	
	while(1){
	while(!(USART0.STATUS & USART_RXCIF_bm ));
	keypress = char_from_PC();
	if((keypress == 'R') || (keypress == 'r'))break;}
	if(keypress == 'R'){
		string_to_PC("Enter Integer number\r\n");
		Num_from_KBD(Int_array);
		string_length = strlen(Int_array);
		reverse(Int_array, string_length);
		Int_num = atol (Int_array);
		string_to_PC(Int_array);
		char_to_PC('\t');
		ltoa(Int_num*2,Int_array,10);
		string_to_PC(Int_array);
		}	
		
	if(keypress == 'r'){
		string_to_PC("Enter floating point number then power (2-9)\r\n");
		Float_from_KBD(Float_array);
		Float_num = atof(Float_array);
				
		Float_from_KBD(power_array);
		power = atof(power_array);
		
		
		while(1){
		if (power){CCP = 0xD8;WDT.CTRLA = 0x07;}
		expt = ftoaL(pow(Float_num, power), Float_array);
		CCP = 0xD8;WDT.CTRLA = 0x0;
		
		itoa(expt, expt_array, 10);
		string_to_PC("\r\n");
		if (Float_array[0] == '.')char_to_PC ('0'); string_to_PC(Float_array);
		if (expt){string_to_PC(" x 10 pow ");
		string_to_PC(expt_array);}
		keypress = waitforkeypress();
		if (keypress == 0x1B)break;
		if (keypress == 'x')power += 1; 
		else power -= 1;
		}}}
}
	
