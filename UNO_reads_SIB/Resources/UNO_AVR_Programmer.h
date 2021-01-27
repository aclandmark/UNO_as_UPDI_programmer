


//Based on 16MHz clock

#define T0_delay_100us	2,56
#define T0_delay_200us 3, 206
#define T0_delay_2ms 4,131
#define T0_delay_5ms 5,178
#define T0_delay_10ms 5,100

char User_response;

/************************************************************************************************************************************/
#define setup_328_HW \
setup_watchdog;\
ADMUX |= (1 << REFS0);\
initialise_IO;\
USART_init(0,51);\


/************************************************************************************************************************************/
#define wdr()  __asm__ __volatile__("wdr")



#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;


#define SW_reset {wdt_enable(WDTO_30MS);while(1);}





/************************************************************************************************************************************/
#define initialise_IO \
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFE;\
PORTD = 0xFF;

//PORTC0 stays as Hi Z inputput




/************************************************************************************************************************************/
#define User_prompt \
while(1){\
do{sendString("R?    ");}	 while((isCharavailable(250) == 0));\
User_response = receiveChar();\
if((User_response == 'R') || (User_response == 'r'))break;} sendString("\r\n");


