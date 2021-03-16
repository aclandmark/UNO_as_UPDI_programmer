
#define F_CPU 3333333
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)


#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>




void USART0_init(void);
void char_to_PC(char);
void string_to_PC(char *str);

char isCharavailable (int);
char char_from_PC(void);
char waitforkeypress(void);

signed char Round_and_Display(char*, char, signed char);
void reverse(char *, int);
long longToStr(long , char *, int );
char ftoaL(float, char *);														//Local version of the float to askii routine


void Timer_TCA(char, unsigned int);


char Int_array[8], Float_array[12];
signed char expt;
	int string_length;
	long  Int_num;
	float Float_num, power;
	char expt_array[4];
	char power_array[12];


#define setup_HW \
USART0_init();

#define SW_reset \
CCP = 0xD8;WDT.CTRLA = 0x03; while(1);




#define TCA_10s		7, 32552
#define TCA_5s		6, 65104	
#define TCA_1s		5, 52083
#define TCA_500ms	5, 26042
#define TCA_100ms	5, 5208

#define User_prompt \
while(1){\
	do{string_to_PC("R?    ");}	 while((isCharavailable(80) == 0));\
	User_response = char_from_PC();\
if((User_response == 'R') || (User_response == 'r'))break;}


