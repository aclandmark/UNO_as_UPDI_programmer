
unsigned int read_flash(int);
/***************************************************************************************************************************************************/
void Verify_Flash_Hex (void){


int  line_counter = 0, print_line = 0;								//Controls printing of hex file                         
int line_no;														//Refers to the .hex file
signed int phys_address;											//Address in flash memory
signed int prog_counter_mem;										//Initialise with size of .hex file used for programming
unsigned char print_out_mode = 0;									//Print out flash contents as hex or askii characters
char skip_lines[4];													//Enter number to limit the print out

sendHex(16, prog_counter);

if(prog_counter == 0)prog_counter = 256;

sendString("\r\nPress 0 to verify flash or AOK\r\n");
if(waitforkeypress() != '0')return;

phys_address = 0x8000;  read_ops=0; 
line_no = 0; prog_counter_mem = prog_counter; 

sendString("Integer(0-FF)?  ");										//0 prints no lines -1-, every line, -8- prints every eighth line etc... 
skip_lines[0] = '0';												//Acquire integer between 0 and FF
skip_lines[1] = waitforkeypress();
skip_lines[2] = '\0';
Timer_T1_sub(T1_delay_500ms); 
if (isCharavailable(1)){skip_lines[0] = skip_lines[1]; 
skip_lines[1] = receiveChar();} 
binUnwantedChars(); 
print_line = askiX2_to_hex(skip_lines);
sendHex (16,print_line); sendString("   ");



while(1){ if(!(prog_counter_mem))break;								//print out loop starts here, exit when finished
    
while(1) {															//Start reading the flash memory searching for the next hex command

Hex_cmd = read_flash(phys_address);
phys_address += 2;        
if (phys_address == FlashSZ)break;									//No more memory? Quit if yes
if (Hex_cmd != 0xFFFF) break;										//If the hex command is 0xFFFF remain in this loop otherwise exit.

}


if (phys_address == FlashSZ)break;									//Exit when there is no more flash to read

if ((print_line == 0)  && (!(line_no%10)))
sendChar('*');														//Print out of hex file not required
																	//Print a -*- every tenth line of the file
if(print_line && (!(line_no%print_line)))							//Print out required: Print all lines or just a selection     
{sendString("\r"); 
sendHex (16, ((phys_address-2)-0x8000));   
sendString(" "); line_counter++;  

sendHex (16, Hex_cmd);}										//Print first command in askii or hex
read_ops += 1;															//Value to be sent to PC for comparison with the hex filer size
prog_counter_mem -= 1;													//"prog_counter_mem" decrements to zero when the end of the file is reached


for(int m=0; m<7; m++){												//Read the next seven locations in the flash memory   

Hex_cmd = read_flash(phys_address); 
phys_address += 2; 
if(Hex_cmd == 0xFFFF)break;											//Read 0xFFFF: return to start of print out loop
prog_counter_mem -= 1;


if(print_line && (!(line_no%print_line)))

sendHex (16, Hex_cmd);

read_ops += 1;

if(phys_address==FlashSZ)break;}
if ((print_line)&&(!(line_no%print_line)) && (!(line_counter%8)))sendString("\r");
line_no++;
if (phys_address == FlashSZ)break;}


newline(); 

sendString("\r\nHex_file_size:  ");
sendHex(10,prog_counter); sendString(" in:  "); 
sendHex(10,read_ops); sendString(" out\r\n");

newline();}




void Verify_Flash_Hex_basic (void){			//DOES WHOLE LINES ONLY


int  line_counter = 0, print_line = 0;								//Controls printing of hex file                         
int line_no;														//Refers to the .hex file
signed int phys_address;											//Address in flash memory
signed int prog_counter_mem;										//Initialise with size of .hex file used for programming
unsigned char print_out_mode = 0;									//Print out flash contents as hex or askii characters
char skip_lines[4];													//Enter number to limit the print out


//if(prog_counter == 0)prog_counter = 256;
if(prog_counter == 0)prog_counter = 256;
//prog_counter *= 2;

sendString("Press 0 to verify flash or AOK\r\n");
if(waitforkeypress() != '0')return;

phys_address = 0x8000;  read_ops=0; 
line_no = 0; prog_counter_mem = prog_counter; 


print_line = 1;

while(1){ if(!(prog_counter_mem))break;								//print out loop starts here, exit when finished
 Hex_cmd = read_flash(phys_address);
 phys_address += 2;   

if (phys_address == FlashSZ)break;									//Exit when there is no more flash to read

if ((print_line == 0)  && (!(line_no%10)))
sendChar('*');														//Print out of hex file not required
																	//Print a -*- every tenth line of the file
if(print_line && (!(line_no%print_line)))							//Print out required: Print all lines or just a selection     
{newline(); sendHex (16, ((phys_address-2)-0x8000));   
sendString("   "); line_counter++;  

sendHex (16, Hex_cmd);}										//Print first command in askii or hex
read_ops += 1;															//Value to be sent to PC for comparison with the hex filer size
prog_counter_mem -= 1;													//"prog_counter_mem" decrements to zero when the end of the file is reached


for(int m=0; m<7; m++){												//Read the next seven locations in the flash memory   

Hex_cmd = read_flash(phys_address); 
phys_address += 2; 
prog_counter_mem -= 1;
      

if(print_line && (!(line_no%print_line)))

sendHex (16, Hex_cmd);

read_ops += 1;

if(phys_address==FlashSZ)break;}
if ((print_line)&&(!(line_no%print_line)) && (!(line_counter%8)))sendString("\r\n");
line_no++;
if (phys_address == FlashSZ)break;}

newline(); 

sendString("\r\nHex_file_size:  ");
sendHex(10,prog_counter); sendString(" in:  "); 
sendHex(10,read_ops); sendString(" out\r\n");
newline();


}



unsigned int read_flash(int flash_add){							//Read integer from flash
  unsigned int Hexcmd;
UART_Tx(0x55);											//SYNCH pulses
UART_Tx(LDS_from | word_address);											//Send command LDS byte
UART_Tx(flash_add);										//Destination addess LSB
UART_Tx(flash_add >> 8);								//Destination address MSB
 //Hex_cmd = UART_Rx() << 8;								//Receive data byte
 Hexcmd = UART_Rx();
 Hexcmd = Hexcmd << 8;
 
 
 Timer_T0_sub(T0_delay_200us);							//Pause
 flash_add += 1;										//Next address in flash
 UART_Tx(0x55);											//SYNCH pulses
UART_Tx(LDS_from | word_address);											//Send command LDS byte
UART_Tx(flash_add);
UART_Tx(flash_add >> 8);
Hexcmd |= UART_Rx();									//Combine both data bytes
Timer_T0_sub(T0_delay_200us);
return Hexcmd;
}