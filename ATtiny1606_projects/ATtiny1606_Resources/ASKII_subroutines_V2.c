

long Num_from_KBD(char*);
char decimal_digit (char);
char wait_for_return_key(void);
float Float_from_KBD(char *);

//void int_string_to_display(void);
void float_string_to_display(void);



/**********************************************************************************************/



/***************************************************************************************************************************************/
char decimal_digit (char data){
if (((data > '9') || (data < '0')) )return 0;
else return 1;}


/***************************************************************************************************************************************/
char wait_for_return_key(void){  									//Returns keypress
char keypress;														//but converts \r\n, \r or \n to \r
while(!(USART0.STATUS & USART_RXCIF_bm )); 
keypress = char_from_PC();
if((keypress == '\r') || (keypress == '\n')){
if (isCharavailable(1)){char_from_PC();}keypress = '\r';}
return keypress;}





/***************************************************************************************************************************************/
long Num_from_KBD(char digits[]){                                   //Acquires an integer string from the keyboard and returns the binary equivalent
char keypress;
//long I_number;
cr_keypress = 0;                                                    //Set to one when carriage return keypress terminates the string
for(int n = 0; n<=8; n++) digits[n] = 0;            				//Clear the buffer used for the string
do
{while(!(USART0.STATUS & USART_RXCIF_bm ));						//Wait for first keypress
keypress = char_from_PC();} 


while ((!(decimal_digit(keypress)))
&& (keypress != '-'));												//Ignore keypress if it is not OK
digits[0] = keypress;
//int_string_to_display();                                           //Update display with the first key press
while(1){
if ((keypress = wait_for_return_key())  =='\r')break;              //Detect return key press (i.e \r or\r\n)
if ((decimal_digit(keypress)) || (keypress == '\b')\
 || (keypress == '-'))

{if (keypress == '\b'){											//Backspace key
for (int n = 0; n <= 7; n++)
digits[n] = digits[n + 1];}

else
{for(int n = 8; n>=1; n--)                                         //Shift display for each new keypress
digits[n] = digits[n-1];
digits[0] = keypress;  }                                           //Add new keypress           


 }}                                                                //Update display includes "cr_keypress"                                                 
cr_keypress = 1;                                                    //End of string; return key press detected
cr_keypress = 0;

return 1;}




/***************************************************************************************************************************************/
float Float_from_KBD(char digits[]){                    					//Acquires an integer string from the keyboard and returns the binary equivalent
char keypress;

cr_keypress = 0;                                                    		//Set to one when carriage return keypress terminates the string
for(int n = 0; n<=11; n++) digits[n] = 0;                           		//Clear the buffer used to the string
do
{while(!(USART0.STATUS & USART_RXCIF_bm )); 
keypress = char_from_PC();} 
while ((!(decimal_digit(keypress)))
&& (keypress != '-')
&& (keypress != '.'));


digits[0] = keypress;

while(1){
if ((keypress = wait_for_return_key())  =='\r')break;               		//Detect return key press (i.e \r or\r\n)

if ((decimal_digit(keypress)) || (keypress == '.')
|| (keypress == '\b')|| (keypress == '-')
|| (keypress == 'E') || (keypress == 'e'))

{if(keypress == '\b'){for (int n = 0; n <= 7; n++)
digits[n] = digits[n + 1];}

else


{for(int n = 11; n>=1; n--)                                          		//Shift display for each new keypress except '.'
digits[n] = digits[n-1];
digits[0] = keypress;}                                              		//Add new keypress           
													//Intregrate decimal point and its parent digit

}}                                                                  		//Update display includes "cr_keypress"                                                 
//cr_keypress = 1;                                                     		//End of string; return key pressed
//cr_keypress = 0;


string_length = strlen(digits);
reverse(digits, string_length);
string_to_PC(digits);
char_to_PC('\t');


return 1;}







