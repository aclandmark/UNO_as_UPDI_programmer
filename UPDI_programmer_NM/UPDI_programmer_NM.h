
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>



#include "Resources_UPDI_NM\UPDI_Programmer.h"
#include "Resources_UPDI_NM\Basic_IO_and_Timer_subs.c"
#include "Resources_UPDI_NM\ASKII_subroutines.c"
#include "Resources_UPDI_NM\Hex_Programmer_subs.c"
#include "Resources_UPDI_NM\Hex_Programmer.c"
#include "Resources_UPDI_NM\UPDI_subroutines.c"
#include "Resources_UPDI_NM\verify_flash.c"
#include "Resources_UPDI_NM\UART_subroutines.c"

#define delay_of_0_25uS             asm("nop"); asm("nop"); asm("nop"); asm("nop");
#define delay_of_0_75uS             delay_of_0_25uS; delay_of_0_25uS; delay_of_0_25uS;
#define out_h                       PORTC |= (1 << PORTC0)
#define out_l                       PORTC &= (~(1 << PORTC0))


#define clock_delay_T               Timer_T0_sub(T0_delay_18us) 
#define clock_delay_R               Timer_T0_sub(T0_delay_18us) 
#define half_clock_delay_R          Timer_T0_sub(T0_delay_9us)

#define clock_delay_T_upload         Timer_T0_sub(T0_delay_10us) 
#define clock_delay_R_upload         Timer_T0_sub(T0_delay_11us)


#define input_h                     (PINC & (1 << PINC0))
#define input_l                     (!(PINC & (1 << PINC0)))
