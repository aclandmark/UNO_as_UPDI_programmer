


/*********************************************************************************************************************************/
/*signed char Format_for_Display(char* array, char sign, signed char expt){		//Receives FP_strings with MSB or sign in location 0 (RH end of display)
																				//Takes positive numbers ONLY but adds a negative sign bit if necessary
	int array_ptr, LSB_ptr;
	char E_space;																//Space required on display for exponential notation
		
	if (array[0] == '9'){														//If array zero is 9 shift it one place to the right
		for(int m = 0; m <= 14; m++)array[15-m] = array[14-m];					//to allow for the possibility of rounding
	array[0] = '0'; expt += 1;}													//Exponent must be incremented
	
	
	for (int m = 15; m; m--)													//Convert trailing zeros to null characters 
	{if (array[m] == 0) continue;
		if (array [m] == '0') array[m] = 0;
	else break;}

	for (int m = 15; m; m--)													//Identify location of the least significant digit
	{LSB_ptr = m;
		if (array[m] == 0) continue;
	else  break;}
	
if (array[LSB_ptr] == '.') array [LSB_ptr + 1] = '0';							//Rounding is not required
	
else
{
	if (LSB_ptr >= 5){

	if (array[LSB_ptr] >= '5')													//Round least significant digit
	{array[LSB_ptr--] = 0; if(array[LSB_ptr] == '.')LSB_ptr -= 1;				//Round additional digits as necessary
		array[LSB_ptr] += 1;
		
		while (array[LSB_ptr] == ':'){array[LSB_ptr--] = 0;						//If a 9 is incremented the next digit must also be incremented  
			if (array[LSB_ptr] == '.')LSB_ptr -= 1;								//Note '9' incremented becomes ':'
		array[LSB_ptr] += 1;}}
	}
}
		
		
		array_ptr = 0;
		if (flt_array[0] == '.')flt_array[0] = '0' | 0x80;						//Convert a decimal point in location zero to "0."
		else
		{for(int m = 0; m <= 15; m++)											//Locate decimal point
			{if (array[m] != '.' )continue;
			else array_ptr = m; break;}											//Combine it with the preceding digit
			array[array_ptr-1] |= 0x80;
		for (int m = array_ptr; m <=14; m++)array[m] = array[m+1];}				//Shift array to fill the space left by the decimal point
		
		if (array[0] == '0'){expt -= 1;											//Location zero still empty:  Shift array one place to the right
		for (int m = 0; m <= 14; m++) array [m] = array[m+1];}					//Restore exponent to its original value
				
		if(array[1] == 0x80){array[0] |= 0x80; array[1] = '0';}					//Special case:  09.9999 E22 (say)
		if((array[0] & 0x80) && (!(array[1]))) array[1] = '0';					//Special case 1.9999, 2.9999999, 3.9999999 etc.
		
		if(sign == '-')																		
		{for(int m = 0; m <= 14; m++)array[15-m] = array[14-m];					//For negative numbers shift the array once place to the right
		array[0] = '-';}														//and add the minus sign
				
		if (!(expt)){															//If there is no exponent right justify string on display
			for(int m = 0; m <= 15; m++)										//First get the length of the string
			{array_ptr = m; if (array[m]  == 0)break;}							
			array_ptr = 8 - array_ptr;											//Then shift it.
			while(array_ptr){for (int m = 15; m; m--)
				{array[m] = array[m-1];} array[0] = 0; array_ptr -= 1;}}
			
			else{																//If there is an exponent overwrite the RH display digits
				E_space = 2;														//with the Exponential notation
				if (expt >= 10) E_space = 3;										//and leave the number at the left hand end of the display
				if (expt <= -10)E_space = 4;
				if ((expt < 0) && (expt > (-10)))E_space = 3;
				
				for(int m = 0; m <= 15; m++)Non_exp_array[m] = array[m];			//Save array before overwriting with exponent
							
				switch (E_space){
					case 2:	array[7] = expt + '0';array[6] = 'E';break;			//E1 to E9
					case 3:	if (expt > 0)										//E10, E11, E12......etc
					{array[7] = (expt%10) + '0'; 
					array[6] = ((expt/10)%10) + '0';array[5] = 'E';}
					if (expt < 0)												//E-1 to E-9
					{array[7] = expt*(-1) + '0';
					array[6] = '-';array[5] = 'E';}
					break;
					case 4:	array[7] = ((expt*(-1))%10) + '0';					//E-10, E-11, E-12....etc
					array[6] = (((expt*(-1))/10)%10) + '0';
					array[5] = '-';array[4] = 'E';
					break;
					}}
			
		Display_mode = 1;
		
		return expt;}

*/


/***************************************************************************************************************************************/
char ftoaL(float Fnum, char FP_string[]){
	int afterpoint = 0;
	long ipart, Fnum_int;
	char sign = '+';
	signed char expt;
	
	if (Fnum < 0){sign = '-'; Fnum *= (-1);}									//Convert negative numbers to positive ones and set the sign character
	
	for(int m = 0; m <= 11; m++) FP_string[m] = 0;								//Clear the floating point array
	
	Fnum_int = (long)Fnum;														//Obtain integer part of the number
	
	if (Fnum_int < 10)afterpoint = 5;											//Number of decimal places is matched to display length
	if ((Fnum_int >= 10) && (Fnum_int < 100))afterpoint = 4;
	if ((Fnum_int >= 100) && (Fnum_int < 1000))afterpoint = 3;
	if ((Fnum_int >= 1000) && (Fnum_int < 10000))afterpoint = 2;
	
	expt = 0;																	//Convert very large and small numbers to scientific form
	if (Fnum  >= 10000) {while (Fnum >= 10)
	{Fnum /= 10; expt += 1;}afterpoint = 5;}
	
	if(Fnum < 0.01) {while (Fnum < 1){Fnum *= 10; expt -= 1;}}
	
																				//FP to askii routines taken from "https://www.geeksforgeeks.org/convert-floating-point-number-string/"
	ipart = (long)Fnum;															//Obtain integer part of FP number
	float fpart = Fnum - (float)ipart;											//Obtain floating part
	long i = longToStr(ipart, FP_string, 0);									//Convert integer part to string

	if (afterpoint != 0){														//Add Decimal part to the string
		FP_string[i] = '.';
		fpart = fpart * pow(10,afterpoint);
	longToStr((long)fpart, FP_string + i + 1, afterpoint);}
	
	if(sign == '-')
	{for(int m = 0; m <= 10; m++)FP_string[11-m] = FP_string[10-m]; 
	FP_string[0] = '-';}
	return expt;
	//expt = Format_for_Display(FP_string, sign, expt);
}



/***************************************************************************************************************************************/
long longToStr(long x, char str[], int d)
{
	int i = 0;
	while (x)
	{   str[i++] = (x%10) + '0';
	x = x/10; }
	
	while (i < d)
	str[i++] = '0';
	reverse(str, i);
	str[i] = '\0';
return i; }


/***************************************************************************************************************************************/
void reverse(char *str, int len)
{
	int i=0, j=len-1, temp;
	while (i<j)
	{   temp = str[i];
		str[i] = str[j];
		str[j] = temp;
	i++; j--; }}

	