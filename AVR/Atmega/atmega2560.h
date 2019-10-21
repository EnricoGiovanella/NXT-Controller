#ifndef ATMEGA2560_H
#define ATMEGA2560_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>


/*----------------------------------------------------------------------------------
work in progress
these files contain the hardware settings of the atmega2560
and the necessary data structures.
in the .h there are the macros that define the settings of each individual hardware
so as to be able to modify them without modifying the code in .c.
uart, interrupts and timers used:

high priority		PCINT2_vect			encoder pin change interrupt
			(timer 4) TIMER_COMPA_SYSTEMT	system timer
			(timer 5) no interrupt		PWM encoder
			USART2_RX_vect 
			USART2_UDRE_vect 
			USART2_TX_vect			serial UART
low priority

the variables modified in the interrupt routines have the volatile tag.
The macro ATOMIC_BLOCK is used to have an atomic reading or writing of the variables
involved outside the interrupt routines.
ATOMIC_BLOCK(ATOMIC_FORCEON) {} in include <util/atomic.h>
in interrupt routines the other possible interrupts are suspended by default.
----------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
Eencoder uses the interrupt pin change and the vector PCINT2_vect PCIINT[16-23]
the interrupt is activated on the rising and falling edges of both encoder pins
the input pins are:
encoder input 1 --> input Atmega2560 pin 89 PK1 PCINT17 = arduino A8
encoder input 2 --> input Atmega2560 pin 88 PK0 PCINT16 = arduino A9
TODO in the routine the position on a uint16_t variable is calculated from 0 to 65535,
the direction of rotation is calculated and the value of the system timer counter is stored.
Moreover if enabled and if the position is equal to the target position the motor is stopped.
The conversion of the position into revolutions and position 0-720 is done in the interrupt system timer
or in the callback function
----------------------------------------------------------------------------------*/

// input pin
#define INPUT_ENCODER DDRK &= ~((1<<PK1) | (1<<PK0))  // Set PK0 PK1 as input

#define PULLUP_ENCODER PORTK |= ((1<<PK1) | (1<<PK0)) // Activate PULL UP

#define INTERRUPTVECT_ENCODER PCINT2_vect


/*----------------------------------------------------------------------------------
system timer(timer 4)
This timer acts as a system clock and generates an interrupt every millisecond.
The TCNT timer counter is used by the ecoder to measure the time of a rising
or falling edge of the pins to calculate the motor speed.
Set a flag to activate the callBack () function at a predefined interval (settable).
Perform conversions along with the callBack () function of position, spins, speed (when possible).
TODO callBack () is an inline function in the main loop.
----------------------------------------------------------------------------------*/

#define PRESCALER_SYSTEMT (1 << CS40)	// timer 4 clk/1

#define OCRA_SYSTEMT OCR4A = 16000	// Output Compare Register A 16000000/16000 = 1000 interrupt/s

#define MODEB_SYSTEMT (1 << WG42)	// TCCR4B register mode 4

#define SETTIMER_SYSTEMT TCCR4B |= MODEB_SYSTEMT | PRESCALER_SYSTEMT // TCCR4B register mode 4 prescaler /1

#define SETINTERRUPT_SYSTEMT TIMSK4 | = (1 << OCIE4A)	// set interrupt set the interrupt when TCNT4 == OCR4A

#define INTERRUPTVECT_SYSTEMT TIMER4_COMPA_vect

/*----------------------------------------------------------------------------------
HBridge timer 5

pin 53 ATMEGA2560 PC0 = Arduino MEGA2560 digital pin 37 = motor direction to Hbridge in 
pin 54 ATMEGA2560 PC1 = Arduino MEGA2560 digital pin 36 = motor direction to Hbridge in 
pin 39 ATMEGA2560 PL4 = Arduino MEGA2560 digital pin 45 = motor pwm speed to Hbridge in
We use timer 5 as a pwm generator in mode 9 (PWM phase / frequency correct).
The TCNT register counts forward when the counter is equal to the OCR5A register
starts counting backwards to zero.
Based on the value of the OCR5B register the square wave is generated in phase.
the OCR5A register is set to the fixed value 1750 (so as to generate steps that represent
a voltage that controls 1/10 revolutions per minute of the motor) so the frequency correct is not used.
No interrupt routine is needed as the hardware connects directly to pin PL4 (port L)
by setting the correct square wave value.
----------------------------------------------------------------------------------*/

#define OUTPUTDIR_HBRIDGE DDRC |= (1<<PC1) | (1<<PC0) // Set PC0 PC1 as output


///////////////////////////////////////////////////////////////////////////////////////////////////////

// use macros to change direction of the engine or stop it

#define STOP_MOTOR_HBRIDGE PORTC &= 0xFC	// 0b11111100 set to 0 PC0 PC1
#define SET_FORWARD_HBRIDGE PORTC |= 1		// 0b00000001 set to 1 PCO
#define SET_REVERSE_HBRIDGE PORTC = 2		// 0b00000010 set to 1 PC1

// use  STOP_MOTOR_HBRIDGE before SET_FORWARD_HBRIDGE or SET_REVERSE_HBRIDGE
///////////////////////////////////////////////////////////////////////////////////////////////////////

#define PRESCALER_HBRIDGE (1 << CS50) //clk  /1 (No prescaling) clk = 16000000 Hz


#define DIRPWM_HBRIDGE DDRL |= (1 << DDL4)
#define OUTPWM_HBRIDGE (1 << COM5B1)	// non-inverting output pin associated with the OCR5B register

// (1 << WG53) (1 << WG50) 0b1001
// mode 9 0b1001 PWM phase/frequency correct use TCNT5 = OCR5A max counter update BOTTOM

#define SETAPWM_HBRIDGE TCCR5A |= OUTPWM_HBRIDGE | (1 << WG50)	
#define SETBPWM_HBRIDGE TCCR5B |= (1 << WG53) |  PRESCALER_HBRIDGE 
/*
OCR5A
register OCR5B contains the maximum number of PWM steps
1750 step the maximum number of engine laps is 175 rpm so there is a minimum resolution of 0.1 rpm
				16000000
the frequency of the PWM = ------------------ = 4571,4 Hz
				1750 X 2
*/

#define MAXSTEP_HBRIDGE OCR5A = 1750

#define VAR_DUTY_HBRIGE OCR5B

/*----------------------------------------------------------------------------------
serial

pin 13 PH1 ATMEGA2560 = Arduino MEGA2560 TX2 digital pin 16 = FT232 USB UART board RXD
pin 12 PH0 ATMEGA2560 = Arduino MEGA2560 RX2 digital pin 17 = FT232 USB UART board TXD
the serial uses two vectors id interrupt one to receive and one to transmit.
binary messages have a first byte that represents the type of message that has a predetermined
size and therefore its length
----------------------------------------------------------------------------------*/

#define SETRX_SERIAL DDRH |= (1 << PH0); PORTH |= ((1<<PH0)
#define SETTX_SERIAL DDRH &= (~(1 << PH1))

#define F_CPU 16000000
#define USART_BAUDRATE 115200

#define UBRR_VALUE_SERIAL (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define DIVIDER_SERIAL UCSR2A |= (1 << U2X2)

//#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 8UL))) - 1)
//#define DIVIDER_SERIAL UCSR2A &= ~(1 << U2X2)

// Set baud rate
#define CLEARFLAG_SERIAL UCSR2A = 0x00	//Clear send and receive flags
#define SETBAUDH_SERIAL UBRR2H = (uint8_t) (UBRR_VALUE_SERIAL >> 8)
#define SETBAUDL_SERIAL UBRR2L = (uint8_t) UBRR_VALUE_SERIAL
  // Set frame format to 8 data bits, no parity, 1 stop bit
#define SETOTHER_SERIAL UCSR2C |= (1 << UCSZ20) | (1 << UCSZ21)
  //enable transmission, reception and routine interrupt
//#define ENABLE_SERIAL UCSR2B |= (1<<RXEN2) | (1<<TXEN2) | (1<<RXCIE2) | (1<<TXCIE2) | (1<<UDRIE2)
#define ENABLE_SERIAL UCSR2B |= (1<<RXEN2) | (1<<TXEN2) | (1<<RXCIE2) | (1<<TXCIE2)

#define DATARXTX_SERIAL UDR2

#define RX_VECT_SERIAL USART2_RX_vect

#define TX_VECT_SERIAL USART2_TX_vect

#define UDRE_VECT_SERIAL USART2_UDRE_vect

#endif
