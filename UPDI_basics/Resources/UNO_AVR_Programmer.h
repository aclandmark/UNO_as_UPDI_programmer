
#define NVMCTRL_CTRLA  				0x1000
#define WP 							0x01									//Write page buffer to memory
#define NVMCTRL_ADDR_reg 			0x1008
#define send64bitKey_cmd 			0xE0
#define download_SIB				0xE5
#define LCDS						0x80
#define ASI_KEY_STATUS_reg			0x07
#define ASI_SYS_STATUS_reg			0x0B
#define STCS						0xC0
#define ASI_Reset_Request_reg		0x08
#define Reset_signature				0x59	
#define LDS_from					0x00
#define word_address				0x04

#define STS							0x40

#define FUSE_base_address			0x1280
#define Signature_base_address		0x1100




//8 MHz clock

#define T1_delay_1Sec 4,0


//Based on 16MHz clock
#define T0_delay_2us	1, 224
#define T0_delay_5us	1, 176
#define T0_delay_10us	1, 96
#define T0_delay_20us 	2, 216
#define T0_delay_40us 	2, 176
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
void Write_page_to_NVM(int);
void Read_add_of_last_page(void);


const char *Key_chip_erase = "0x4E564D4572617365";
const char *Key_NVM_prog = "0x4E564D50726F6720";

char User_response;
unsigned char SIB_byte[24], Rx_Byte;
int test = 0;
volatile char UPDI_timeout;
volatile int prog_counter;
int FlashSZ;
int Hex_cmd;
int cmd_counter;
int read_ops;

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
UART_Tx(STCS | ASI_Reset_Request_reg);\
UART_Tx(Reset_signature);\
Timer_T0_sub(T0_delay_200us);\
UART_Tx(0x55);\
UART_Tx(STCS | ASI_Reset_Request_reg);\
UART_Tx(0x0);


//Timer_T0_sub(T0_delay_200us);
//Timer_T0_sub(T0_delay_200us);

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



/*************************************************************************************************/



//Timer_T0_sub(T0_delay_200us);