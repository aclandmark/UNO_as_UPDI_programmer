

void USART0_init(void)
{PORTB.DIR &= ~PIN3_bm;
	PORTB.DIR |= PIN2_bm;
	USART0.BAUD = (uint16_t)USART0_BAUD_RATE(57600);
	USART0.CTRLB |= USART_TXEN_bm;
USART0.CTRLB |= USART_RXEN_bm;}



/*****************************************************************/
void char_to_PC(char c)
{while (!(USART0.STATUS & USART_DREIF_bm));
USART0.TXDATAL = c;}



/*****************************************************************/
void string_to_PC(char *str)
{for(size_t i = 0; i < strlen(str); i++)
	{char_to_PC(str[i]);}}


/*****************************************************************/
char isCharavailable (int m){int n = 0;
	while (!(USART0.STATUS & USART_RXCIF_bm )){n++;
		if (n>4000) {m--;n = 0;}if (m == 0)return 0;}
	return 1;}

	
/*****************************************************************/
	char waitforkeypress(void){
		while (!(USART0.STATUS & USART_RXCIF_bm ));
	return USART0.RXDATAL;}
	

/*****************************************************************/
	char char_from_PC(void){
	return 	USART0.RXDATAL;	}
	

/*****************************************************************/
void Timer_TCA(char prescaler, unsigned int top_value){
	TCA0.SINGLE.CNT = 0x0;										//Reset counter
	//TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
	TCA0.SINGLE.PER = top_value;								//Set top value
		TCA0_SINGLE_CTRLA = (prescaler*2) +1;					//Start counter
	while (!(TCA0_SINGLE_INTFLAGS & 0x01));						//Wait for it to reach the top value
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;					//Reset OVF flag
	TCA0_SINGLE_CTRLA = 0;}										//Stop Counter

	
	
	
/*	
ISR(TCA0_OVF_vect){
	string_to_PC("ABCDEabcdefghijklFGHIJKL0123456789ABCDEFG\r\n");
TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;}*/




/*void Timer_TCA(char prescaler, unsigned int top_value){//WORKS
	TCA0.SINGLE.CNT = 0x0;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
	TCA0.SINGLE.PER = top_value; //52064;	//13016;	//0x0CB6*4;
	//TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc
	//| TCA_SINGLE_ENABLE_bm;
	TCA0_SINGLE_CTRLA = (prescaler*2) +1;
	while (!(TCA0_SINGLE_INTFLAGS & 0x01));
	//string_to_PC("ABCDEabcdefghijklFGHIJKL0123456789\r\n");
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
TCA0_SINGLE_CTRLA = 0;}*/


	
/*TCA0.SINGLE.CNT = 0x0;	//0xFFFF - start_value + 1;
	TCA0_SINGLE_PER = top_value;
	TCA0_SINGLE_CTRLA = (prescaler*2) +1;
	while (!(TCA0_SINGLE_INTFLAGS & 0x01));
	TCA0_SINGLE_INTFLAGS |= 1;
	TCA0_SINGLE_CTRLA = 0x0;}
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;
	TCA0.SINGLE.EVCTRL &= (~(TCA_SINGLE_CNTEI_bm));
	TCA0.SINGLE.PER = 0x0CB6;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc
	| TCA_SINGLE_ENABLE_bm;}*/	