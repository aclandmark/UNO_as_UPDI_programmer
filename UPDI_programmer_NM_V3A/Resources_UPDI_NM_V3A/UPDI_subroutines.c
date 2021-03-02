
#include "UART_definitions.h"



void send_key_2(const char*, unsigned char*);
void Read_NVM_Reg(int, char);
char txt2bin(char A, char B);
void key2bin(const char*, unsigned char*);




/**********************************************************************************************************/
void Erase_code (void){	
send_key_2(Key_chip_erase, Erase_key);

synch_pulse;
data_byte_Tx = (LCDS | ASI_KEY_STATUS_reg); transmit_byte;
receive_byte; Stop_clock;
if (data_byte_Rx & 0x08);
else sendString("\r\nChip erase failed");

UPDI_reset;

UPDI_timeout = 0;
Timer_T1_sub(T1_delay_250ms);

synch_pulse;
data_byte_Tx = (LCDS | ASI_SYS_STATUS_reg); transmit_byte;
receive_byte; Stop_clock;

if (data_byte_Rx & 0x01){sendString("Still locked");while(1);}

if (UPDI_timeout) 
{sendString("\r\nFailure: Device still locked\t"); while(1);}
else sendString("Device unlocked\t\t\t");}



/**********************************************************************************************************/
char set_up_NVM_prog(void){	

send_key_2(Key_NVM_prog, NVM_prog_key);

synch_pulse;
data_byte_Tx = (LCDS | ASI_KEY_STATUS_reg); transmit_byte;
receive_byte; Stop_clock;
if(data_byte_Rx == 0x10);
else {sendString("\r\nNVM prog failure"); while(1);}

UPDI_reset;

Timer_T1_sub(T1_delay_250ms);
synch_pulse;
data_byte_Tx = (LCDS | ASI_SYS_STATUS_reg); transmit_byte;
receive_byte; Stop_clock;

if (data_byte_Rx & 0x08);
else{sendString("NVM programming not ready");while(1);}

sendString("Ready for NVM programming\t");

newline();
return 1;}




/**********************************************************************************************************/
void read_out_signature_bytes(void){				
int start_add = 0x1100;
for(int m = 0; m<=2; m++){
synch_pulse;
data_byte_Tx = (LDS_from | word_address); transmit_byte;
data_byte_Tx = (start_add); transmit_byte;
data_byte_Tx = (start_add >> 8); transmit_byte;
receive_byte; Stop_clock;
start_add += 1;
sendHex(16, data_byte_Rx);}}




/**********************************************************************************************************/
void write_fuse(int fuse_add, unsigned char value){		

synch_pulse;
data_byte_Tx = (STS | byte_data); transmit_byte;
data_byte_Tx = byte(NVMCTRL_ADDR_reg); transmit_byte;
data_byte_Tx = byte(NVMCTRL_ADDR_reg >> 8); transmit_byte;
receive_byte;  receiveBit; receiveBit;
data_byte_Tx = (fuse_add); transmit_byte;
receive_byte; Stop_clock;

synch_pulse;
data_byte_Tx = (STS | byte_data); transmit_byte;
data_byte_Tx = byte(NVMCTRL_ADDR_reg + 1); transmit_byte;
data_byte_Tx = byte(NVMCTRL_ADDR_reg >> 8); transmit_byte;
receive_byte;  receiveBit; receiveBit;
data_byte_Tx = (fuse_add >> 8); transmit_byte;
receive_byte; Stop_clock;

synch_pulse;
data_byte_Tx = (STS | byte_data); transmit_byte;
data_byte_Tx = byte(NVMCTRL_DATA_reg); transmit_byte;
data_byte_Tx = byte(NVMCTRL_DATA_reg >> 8); transmit_byte;
receive_byte;  receiveBit; receiveBit;
data_byte_Tx = (value); transmit_byte;
receive_byte; Stop_clock;

synch_pulse;
data_byte_Tx = (STS | byte_data); transmit_byte;
data_byte_Tx = byte(NVMCTRL_CTRLA); transmit_byte;
data_byte_Tx = byte(NVMCTRL_CTRLA >> 8); transmit_byte;
receive_byte;  receiveBit; receiveBit;
data_byte_Tx = (cmd_update_fuse); transmit_byte;
receive_byte; Stop_clock;
Timer_T2_sub(T2_delay_5ms);}




/**********************************************************************************************************/
void read_out_fuses(void){								
int start_add = FUSE_base_address - 1;
for(int m = 0; m<=8; m++){
start_add += 1;
  if(m==3)continue;
  if(m==4)continue;

synch_pulse;
data_byte_Tx = (LDS_from | word_address); transmit_byte;
data_byte_Tx = (start_add); transmit_byte;
data_byte_Tx = (start_add >> 8); transmit_byte;
receive_byte; Stop_clock;

sendHex(16, data_byte_Rx);
if(m==5)sendChar('\t');}}



/**********************************************************************************************************/
void inititalise_UPDI_cmd_write(int add_in_flash){		//Setup repeat command
unsigned char block_SZ = 64;

add_in_flash = (add_in_flash * 2) + 0x8000;

cli();

synch_pulse;
data_byte_Tx = (ST | word_pointer); transmit_byte;
data_byte_Tx = (add_in_flash); transmit_byte;
data_byte_Tx = (add_in_flash >> 8); transmit_byte;
receive_byte; Stop_clock;

synch_pulse;
data_byte_Tx = (setup_repeat_op); transmit_byte;
data_byte_Tx = (block_SZ - 1); transmit_byte;
Stop_clock;

synch_pulse;
data_byte_Tx = (ST | inc_byte_ptr); transmit_byte;

Stop_clock;

sei();}



/**********************************************************************************************************/
void UPDI_cmd_to_page_buffer(void){

get_next_hex_cmd();


cli();	
Restart_clock;
data_byte_Tx = (Hex_cmd); transmit_byte;

receive_byte; Stop_clock;

Restart_clock;
data_byte_Tx = (Hex_cmd >> 8); transmit_byte;
receive_byte; Stop_clock;

sei();}



/**********************************************************************************************************/
void Write_page_to_NVM(int PA){							//PA is page address  Interrupts enabled

PA = (PA * 2) + 0x8000;								//Convert cmd address to byte address
cli();

synch_pulse;
data_byte_Tx = (STS | int_data); transmit_byte;
data_byte_Tx = byte(NVMCTRL_ADDR_reg); transmit_byte;
data_byte_Tx = (NVMCTRL_ADDR_reg >> 8); transmit_byte;
receive_byte; Stop_clock;

Restart_clock;
data_byte_Tx = (PA); transmit_byte;
data_byte_Tx = (PA >> 8); transmit_byte;
receive_byte; Stop_clock;

sendChar('*');

synch_pulse;
data_byte_Tx = (STS | byte_data); transmit_byte;
data_byte_Tx = byte(NVMCTRL_CTRLA); transmit_byte;
data_byte_Tx = (NVMCTRL_CTRLA >> 8); transmit_byte;
receive_byte; Stop_clock;

Restart_clock;
data_byte_Tx = (WP_cmd); transmit_byte;
receive_byte; Stop_clock;
sei();
Timer_T2_sub(T2_delay_3ms);}






/**********************************************************************************************************/
void send_key_2(const char *Key, unsigned char array[]){						

key2bin(Key, array);

synch_pulse;
data_byte_Tx = send64bitKey_cmd; transmit_byte;

for(int m = 0; m <= 7; m++){
data_byte_Tx = array[m]; transmit_byte;}Stop_clock;}




/**********************************************************************************************************/
void key2bin(const char *Key, unsigned char array[]){
char key_h, key_l;
for(int m = 0; m <= 7; m++){
key_h = *(Key + 2*(8 - m));
key_l = *(Key + 2*(8 - m) + 1);
array[m] = txt2bin(key_h,key_l);}}



/**********************************************************************************************************/
char txt2bin(char A, char B){
A -= '0';
if (A > 9) A -= 7;
B -= '0';
if (B > 9) B -= 7;
return (A * 0x10 + B);}




