
volatile int P_counter;



/********************************************************/

#define NVMCTRL_CTRLA  				0x1000
#define Signature_base_address		0x1100
#define NVMCTRL_ADDR_reg 			0x1008
#define NVMCTRL_DATA_reg			0x1006
#define FUSE_base_address			0x1280

#define ASI_KEY_STATUS_reg			0x07
#define ASI_Reset_Request_reg		0x08
#define ASI_SYS_STATUS_reg			0x0B

#define UPDI_CTRLB					0x03

#define Reset_signature				0x59

#define WP_cmd 						0x01			//Write page buffer to memory
#define send64bitKey_cmd 			0xE0
#define download_SIB				0xE5
#define cmd_update_fuse				0x07

#define LCDS						0x80
#define STCS						0xC0

#define LDS_from					0x00
#define word_address				0x04

#define STS							0x40
#define byte_data					0x04
#define int_data					0x05


#define ST							0x60
#define byte_pointer				0x08
#define word_pointer				0x09
#define inc_byte_ptr				0x04
#define inc_word_ptr				0x05
#define setup_repeat_op				0xA0


#define Prog_delay            		Timer_T2_sub(T2_delay_30us)	
#define Prog_delay_upload			Timer_T2_sub(T2_delay_30us)	


#define WDTCFG  	0x1280
#define BODCFG  	0x1281
#define	OSCCFG  	0x1282
#define TCD0CFG 	0x1284
#define SYSCFG0		0x1285
#define SYSCFG1  	0x1286
#define APPEND  	0x1287
#define BOOTEND  	0x1288

#define LOCKBIT_LOCKBIT  0x128A







void UART_Tx(unsigned int);
unsigned char UART_Rx(void);
void UART_Tx_upload(unsigned int);
unsigned char UART_Rx_upload(void);
void UART_Tx_break_upload(void);

void Bin_to_PC(unsigned char);
void Erase_code (void);
char set_up_NVM_prog(void);
void Write_page_to_NVM(int);
void Read_add_of_last_page(void);


unsigned int read_flash(int);
void write_fuse(int, unsigned char);
void read_out_fuses(void);
void read_out_signature_bytes(void);
void Verify_Flash_Hex (void);
void Verify_Flash_Hex_basic (void);
void Read_NVM_Reg(int, char);

void cmd_to_page_buffer(int, int);
void fill_page_buffer(unsigned char *, int, unsigned char);


const char *Key_chip_erase = "0x4E564D4572617365";
const char *Key_NVM_prog = "0x4E564D50726F6720";

unsigned char SIB_byte[24], Rx_Byte;
int test = 0;
volatile char UPDI_timeout = 0;





unsigned char  data_byte_Tx, data_byte_Rx;
unsigned char Rx_bit, Tx_bit;
unsigned char parity_2;

unsigned char Erase_key[8], NVM_prog_key[8];
volatile unsigned char SREG_BKP;



/*************************************************************************/







void upload_hex(void);
void Program_Flash_Hex (void);

#define T0_delay_200us 3, 206
#define T0_delay_400us 3, 156

#define T1_delay_100ms 4,58982
#define T1_delay_250ms 4,49151
#define T1_delay_500ms 4,32768


#define T2_delay_7_5us 	1,136
#define T2_delay_30us 	2,196
#define T2_delay_45us 	2,166
#define T2_delay_56_25us 	2,144
#define T2_delay_100us	2,56
#define T2_delay_200us	3,156
#define T2_delay_500us	4, 131

#define T2_delay_1ms 	4,6
#define T2_delay_3ms 	6,68
#define T2_delay_5ms 	7,178


	
unsigned int  cmd_counter;									//Counts commands as they are downloaded from the PC
volatile unsigned int prog_counter;							//Counts commands burned to flash
unsigned int  read_ops=0;									//Total number of commands read from flash
volatile int counter;										//Counts characters in a record as they are downloded from the PC
volatile int char_count;									//The number of askii character in a single record
volatile unsigned char Count_down;							//Counts commands as record is programmed
volatile int   tempInt1, tempInt2;							//Used to assemble commands and addresses as the are downloaded
int store[64];												//Used to store commands and address ready for the programmer
volatile unsigned char w_pointer,r_pointer;					//Read/write pointers to "store" to which hex file is saved
unsigned int Hex_cmd;										//Command read from flash during verification

unsigned int Hex_address;									//Address read from the hex file
unsigned int HW_address;									//Hard ware address (usually tracks Hex_address)
unsigned int page_address;									//Address of first location on a page of flash 
volatile unsigned int write_address;						//Address on page_buffer to which next command will be written

signed char short_record;									//Record  containing less that eight 16 bit commands
unsigned char page_offset;									//Address of first location on page buffer to be used
unsigned char space_on_page;									//Keeps a track of the space remaining on a page buffer
unsigned char Flash_flag;									//Indicates that the page buffer contains commands

signed char record_length;									//Num commands one one line of hex file (i.e. on one record)
signed char record_length_old;								//If record length changes, length of the previous one is important
signed char orphan;											//Indicates that the contents of a record span two flash pages
signed char section_break;									//Set to 1 if at least one page of flash memory will be unused.
signed char page_break;										//Page only partialy filled before programming next one starts
volatile signed char line_offset;							//LSB of address of first command in record (usually zero)
unsigned int prog_led_control;								//Used to control Leds as hex file is downloaded

unsigned char cal_factor=0; 								//Either default or user supplied

volatile char endoftext, text_started;
volatile char Rx_askii_char_old;                             //Required to check for a \r\n string
int address_in_flash;                                       //Address in flash at which text data is to be programmed
unsigned char  op_code;

char User_response;

signed int PageSZ = 0x20;									//Size of a page of flash in 16 bit words
signed int PAmask = 0x1FE0;									//Used to obtain the flash page address from the hex address
unsigned int FlashSZ = 0x2000;								//Amount of flash memory in 16 bit words supplied on target device


volatile char record_type, record_type_old;



/************************************************************************************************************************************/
#define setup_328_HW \
\
setup_watchdog;\
ADMUX |= (1 << REFS0);\
initialise_IO;\
\
USART_init(0,52);



/************************************************************************************************************************************/
#define wdr()  __asm__ __volatile__("wdr")


#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;


#define SW_reset {wdt_enable(WDTO_30MS);while(1);}



/************************************************************************************************************************************/
#define UPDI_reset \
\
Reset_clock;\
Start_Tx_clock;\
data_byte_Tx = 0x55; transmit_byte;\
data_byte_Tx = (STCS | ASI_Reset_Request_reg); transmit_byte;\
data_byte_Tx = (Reset_signature); transmit_byte; Stop_clock;\
Timer_T2_sub(T2_delay_3ms);\
Reset_clock;\
Start_Tx_clock;\
data_byte_Tx = 0x55; transmit_byte;\
data_byte_Tx = (STCS | ASI_Reset_Request_reg); transmit_byte;\
data_byte_Tx = 0x0; transmit_byte;Stop_clock;




/************************************************************************************************************************************/
#define contact_target \
if(PINC & (1 << PINC0))\
DDRC |= (1 << DDC0);\
else {sendString ("Device not detected\r\n");\
while(1);}\
delay_of_0_25uS;\
DDRC &= (~(1 << DDC0));\
while(!(PINC & (1 << PINC0)));\
Timer_T0_sub(T0_delay_400us);




/************************************************************************************************************************************/

#define initialise_NVM_programming \
sendString("\r\nInitialising NVM programming");\
sendString("\r\nErase chip? Y or N");\
if (waitforkeypress() == 'Y'){sendString ("\tErasing chip\r\n");\
Erase_code();} else sendString ("\tChip not erased\r\n");\
newline();\
if (set_up_NVM_prog())\
sendString("\r\nSignature byte readout\t\t");\
else {sendString("\r\nPOR to proceed!"); while(1);}\
read_out_signature_bytes();\
newline();



/************************************************************************************************************************************/
#define Dissable_UPDI_sesion \
synch_pulse;\
data_byte_Tx = (STCS | UPDI_CTRLB); transmit_byte;\
data_byte_Tx = (0xE7); transmit_byte;




/************************************************************************************************************************************/
#define Initialise_variables_for_programming_flash \
prog_counter=0;  prog_led_control = 0; cmd_counter = 0; record_length_old = 0x10;\
Flash_flag = 0;  HW_address = 0;  section_break = 0; orphan = 0;\
w_pointer = 0; r_pointer = 0; short_record=0;\
counter = 0;




/*************************Atmega programming commands (see data sheet)******************************/
#define inc_w_pointer \
w_pointer++;\
w_pointer = w_pointer & 0x3F;


#define inc_r_pointer \
r_pointer++;\
r_pointer = r_pointer & 0b00111111;



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


