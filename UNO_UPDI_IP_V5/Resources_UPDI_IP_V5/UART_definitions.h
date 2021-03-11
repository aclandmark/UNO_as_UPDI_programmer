

#define Start_Tx_clock_LS				TCCR0B = ((1 << CS00)); OCR0A =  Tx_clock_LS; 
#define Start_Tx_clock					TCCR0B = ((1 << CS00));	OCR0A =  Tx_clock;
#define Start_Rx_clock					TCCR0B = ((1 << CS00));	OCR0A =  Rx_clock;
#define Start_half_Rx_clock				TCCR0B = ((1 << CS00));	OCR0A =  Half_Rx_clock;
#define Stop_clock  					TCCR0B = 0; 
#define Restart_clock					TCNT0 = 0; GTCCR |= (1 << PSRSYNC); Start_Tx_clock;
#define Reset_clock						TCCR0B = 0;	TCNT0 = 0; GTCCR |= (1 << PSRSYNC);

//tx clock ls was 160 but gives poor results 

#define Tx_clock_LS            	140
#define Tx_clock            	60
#define Rx_clock            	60
#define Half_Rx_clock   		30


#define input_h                (PINC & (1 << PINC0))
#define input_l        			(!(PINC & (1 << PINC0)))

#define output_h         		DDRC &= (~(1 << DDC0));
#define output_l          		DDRC |= (1 << DDC0);


#define synch_pulse	\
Reset_clock;\
Start_Rx_clock;\
data_byte_Tx = 0x55; \
transmit_byte;


/****************************************************************************************/
#define transmitBit_LS \
{while (!(TIFR0 & (1 << OCF0A)));\
if(Tx_bit){output_h;}\
else {output_l};\
TIFR0 = 0xFF;\
OCR0A +=  Tx_clock_LS;}


/*****************************************/
#define transmitBit \
{while (!(TIFR0 & (1 << OCF0A)));\
if(Tx_bit){output_h;}\
else {output_l};\
TIFR0 = 0xFF;\
OCR0A +=  Tx_clock;}


/*****************************************/
#define transmit_byte_LS \
{parity_2 = 0;\
Tx_bit = 0;\
 transmitBit_LS;\
\
for(int n = 0; n <= 7; n++){\
(Tx_bit = data_byte_Tx & (1 << n));\
if(Tx_bit)parity_2 += 1;\
transmitBit_LS;}\
 \
if(parity_2%2)Tx_bit = 1;\
else Tx_bit = 0;\
transmitBit_LS;\
\
Tx_bit = 1;\
transmitBit_LS;\
transmitBit_LS;}



/*****************************************/
#define transmit_byte \
{parity_2 = 0;\
Tx_bit = 0;\
 transmitBit;\
\
for(int n = 0; n <= 7; n++){\
(Tx_bit = data_byte_Tx & (1 << n));\
if(Tx_bit)parity_2 += 1;\
transmitBit;}\
 \
if(parity_2%2)Tx_bit = 1;\
else Tx_bit = 0;\
transmitBit;\
\
Tx_bit = 1;\
transmitBit;\
transmitBit;}

/*****************************************/
#define transmit_byte_P \
{parity_2 = 0;\
Tx_bit = 0;\
 transmitBit;\
\
for(int n = 0; n <= 7; n++){\
(Tx_bit = data_byte_Tx & (1 << n));\
if(Tx_bit)parity_2 += 1;\
transmitBit;}\
\
if(parity_2%2)Tx_bit = 0;\
else Tx_bit = 1;\
transmitBit;\
\
Tx_bit = 1;\
transmitBit;\
transmitBit;}


/*****************************************/
#define wait_for_start_bit \
while(input_h);\
Reset_clock;\
Start_half_Rx_clock;\
while (!(TIFR0 & (1 << OCF0A)));\
OCR0A +=  Rx_clock;\
TIFR0 = 0xFF;




/*****************************************/
#define receiveBit \
{while (!(TIFR0 & (1 << OCF0A)));\
if (input_h){Rx_bit = 1;}\
else {Rx_bit = 0;  }\
TIFR0 = 0xFF;\
OCR0A +=  Rx_clock;}




/*****************************************/
#define receive_byte \
data_byte_Rx = 0;\
parity_2 = 0;\
wait_for_start_bit;\
\
for(int n = 0; n <= 7; n++){\
receiveBit;\
if(Rx_bit){data_byte_Rx |= (1 << n);parity_2 += 1;}}\
\
receiveBit;\
if ((Rx_bit) && (parity_2%2));\
else P_counter++;\
receiveBit;\
receiveBit;


/*****************************************/
#define break_page_fill \
\
Reset_clock;\
Start_Rx_clock;\
data_byte_Tx = 0x55;\
transmit_byte_P;\
Stop_clock;\
Timer_T2_sub(T2_delay_750us);\
output_l;\
Timer_T2_sub(T2_delay_500us);\
output_h;\
Reset_clock;\
Start_Tx_clock_LS;\
data_byte_Tx = 0x55; transmit_byte_LS;\
data_byte_Tx = (STCS | 0x09); transmit_byte_LS;\
data_byte_Tx = 0x01; transmit_byte_LS;\
Stop_clock;


/*
750 and 500 are almost OK  BEST so far
Needs Tx_clock_LS     of       	140
750 and 500 are not critical
*/
