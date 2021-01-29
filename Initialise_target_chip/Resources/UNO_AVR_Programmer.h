



//Based on 16MHz clock
#define T0_delay_100us	2,56
#define T0_delay_200us 3, 206
#define T0_delay_2ms 4,131
#define T0_delay_5ms 5,178
#define T0_delay_10ms 5,100


#define T1_delay_1Sec 4,0



void UART_Tx(int);
unsigned char UART_Rx(void);
void Bin_to_PC(unsigned char);
void Erase_code (void);
void set_up_NVM_prog(void);

const char *Key_chip_erase = "0x4E564D4572617365";
const char *Key_NVM_prog = "0x4E564D50726F6720";

char User_response;
unsigned char SIB_byte[24], Rx_Byte;
int test = 0;
volatile char Chip_erase_timeout;


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




#define UPDI_reset \
Timer_T0_sub(T0_delay_200us);\
UART_Tx(0x55);\
UART_Tx(0xC8);\
UART_Tx(0x59);\
Timer_T0_sub(T0_delay_200us);\
\
UART_Tx(0x55);\
UART_Tx(0xC8);\
UART_Tx(0x0);




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
#define Config_Xtal_port \
ASSR = (1 << AS2);	




/************************************************************************************************************************************/
#define User_prompt \
while(1){\
do{sendString("R?    ");}	 while((isCharavailable(250) == 0));\
User_response = receiveChar();\
if((User_response == 'R') || (User_response == 'r'))break;} sendString("\r\n");


