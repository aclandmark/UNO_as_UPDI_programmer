
/*
 * UPDI_programmer.c
 *
 *Uses UNO A0 PCO as programmer output

 Fuse settings: Leave all as default except
 SYSCFG0 0xF7     EEprom preserved during chip erase
 SYSCFG1  0xFD   16ms SUT (For tes purposes really) 
 
 
 */ 

#include "Project_header_file.h"



int main(void)
{ char kepress;
int start_add;
int flash_data;
int page_address, add_in_flash;
unsigned int Hex_cmd, Rx_byte;
char cmd_h;
int cmd_buffer[32];


setup_328_HW;
User_prompt;                                        //PINC0 initialised Hi Z input
 
sendString("\r\nUPDI development program:  POR required to reset UPDI:  AK to start\r\n");
waitforkeypress();

 /********************************Programmer target connection sequence************************************/
  if(PINC & (1 << PINC0))
  DDRC |= (1 << DDC0);                                //Drive reset low
else {sendString ("Device not detected\r\n");
while(1);}
  delay_of_0_25uS;                                    //Wait 0.25uS. None:OK, 0.75uS:OK, 5mS:fails
   DDRC &= (~(1 << DDC0));                             //Release the line: Hi Z
  while(!(PINC & (1 << PINC0)))test += 1;             //estimated delay 8 to 10uS  Spec 10-200uS
   Timer_T0_sub(T0_delay_200us);
Timer_T0_sub(T0_delay_200us);

/****************************Print out the SIB****************************/
do {
 UART_Tx(0x55);
  UART_Tx (download_SIB);
 for(int m = 0; m <= 15; m++){
 SIB_byte[m] = UART_Rx(); }
 sendString("\r\nSIB readout:\t");
for(int m = 0; m <= 15;m++){
  sendChar(SIB_byte[m]);}
    sendString("\tFamily name, NVM version, OCD version, DGB_OSC frequency setting.");
    sendString("\r\n-x- to continue, AOK to repeat.\r\n");
    kepress = waitforkeypress();}while(kepress != 'x');


while(1){
/*****************************Get ready to program to flash*********************************************/
sendString("\r\nInitialising NVM programming\r\n");
sendString("\r\nErase chip? Y or N");
if (waitforkeypress() == 'Y'){sendString ("\tChip erased");
Erase_code();}
newline();
if (set_up_NVM_prog())
sendString("\r\nSignature byte readout\t\t");
else {sendString("\r\nPOR and Erase to proceed!"); while(1);}
read_out_signature_bytes();
newline();
sendString("\r\nFuse bytes:\t");
read_out_fuses();

/**************************Set fuses to user values*************************************************/
sendString("\r\n\r\nUpdate SYSCFG0 and SYSCFG1 for 16mS SUT and ");
sendString("\r\nEEprom preserved at chip erase");
sendString("\r\nY to proceed, AOK to skip");
if (waitforkeypress() == 'Y'){
write_fuse (0x1285, 0xF7);          //CFG0
write_fuse (0x1286, 0xFD);          //16ms SUT
sendString("\r\n\r\nNew fuse bytes:\t");
read_out_fuses();}





/****************************************Program the flash*************************8******************/
sendString("\r\nProgram flash? Y or N");
if (waitforkeypress() == 'Y')
{
FlashSZ = 0xC000;
prog_counter = 0;
sendString("\r\nProgramming flash\t");


page_SZ = 32;
flash_data = 0;
for(int n = 0; n <= 7; n++){
for(int m = 0; m <= 31; m++)cmd_buffer[m] = flash_data++;
page_address = 0x8000 + n*64;
fill_page_buffer(cmd_buffer, page_address, page_SZ*2);
Write_page_to_NVM(page_address);}

for(int m = 0; m <= 31; m++)cmd_buffer[m] = 0;
for(int m = 0; m <= 12; m++)cmd_buffer[m] = flash_data++;
page_address = 0x8000 + 8*64;
fill_page_buffer(cmd_buffer, page_address, 26);
Write_page_to_NVM(page_address);}


/***********************Verify the flash***************************************************/
sendString("\r\nRead flash\r\n");
Verify_Flash_Hex ();


/********************Restore fuse settings if required***************************************/
sendString("\r\nY to Restore default fuse settings or AOK\r\n");
if (waitforkeypress() == 'Y')
{write_fuse (0x1285, 0xF6);                                         //CFG0
write_fuse (0x1286, 0xF7);  }                                     //64ms SUT


/*******************Lock/unlock device*****************************************************/
sendString("\r\nUnlocking device");
write_fuse (0x128A, 0xC5);                                        //Device unlocked
sendString("\r\nY to Lock device or AOK");
if (waitforkeypress() == 'Y')
{write_fuse (0x128A, 0x0);
sendString("\r\nDevice locked\r\n");}
else sendString("\r\nDevice unlocked\r\n");



sendString("\r\nFinnished AK to repeat");
waitforkeypress();}

while(1);}




ISR(TIMER1_OVF_vect) {UPDI_timeout = 1;TCCR1B = 0;}



/***********************************************************************************************/
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




  
/************************************************************************************************/
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
