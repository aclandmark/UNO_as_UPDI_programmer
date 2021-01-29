
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>



#include "Resources\UNO_AVR_Programmer.h"
#include "Resources\Basic_IO_and_Timer_subs.c"
#include "Resources\ASKII_subroutines.c"



#define delay_of_0_25uS       asm("nop"); asm("nop"); asm("nop"); asm("nop");
#define delay_of_0_75uS       delay_of_0_25uS; delay_of_0_25uS; delay_of_0_25uS;
#define out_h                 PORTC |= (1 << PORTC0)
#define out_l                 PORTC &= (~(1 << PORTC0))

#define clock_delay_T         Timer_T0_sub(T0_delay_200us)

#define input_h               (PINC & (1 << PINC0))
#define input_l               (!(PINC & (1 << PINC0)))
#define clock_delay_R         Timer_T0_sub(T0_delay_200us)
#define half_clock_delay_R    Timer_T0_sub(T0_delay_100us)