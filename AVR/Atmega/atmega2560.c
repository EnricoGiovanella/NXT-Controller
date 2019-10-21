#include "atmega2560.h"

cli();	//disable interrupt


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


INPUT_ENCODER;	// Set pin as input
PULLUP_ENCODER;	// Activate PULL UP


ISR(INTERRUPTVECT_ENCODER) {	// called by the rising and falling edge of the selected pins
	//TODO
}


/*----------------------------------------------------------------------------------
system timer(timer 4)
This timer acts as a system clock and generates an interrupt every millisecond.
The TCNT timer counter is used by the ecoder to measure the time of a rising
or falling edge of the pins to calculate the motor speed.
Set a flag to activate the callBack () function at a predefined interval (settable).
Perform conversions along with the callBack () function of position, spins, speed (when possible).
TODO callBack () is an inline function in the main loop.
----------------------------------------------------------------------------------*/

SETTIMER_SYSTEMT;
SETINTERRUPT_SYSTEMT;
OCRA_SYSTEMT;

ISR (INTERRUPTVECT_SYSTEMT) {
	//TODO
}

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

OUTPUTDIR_HBRIDGE;
STOP_MOTOR_HBRIDGE;

DIRPWM_HBRIDGE;

SETAPWM_HBRIDGE;
SETBPWM_HBRIDGE;

MAXSTEP_HBRIDGE;
VAR_DUTY_HBRIGE = 0;


/*----------------------------------------------------------------------------------
serial

pin 13 PH1 ATMEGA2560 = Arduino MEGA2560 TX2 digital pin 16 = FT232 USB UART board RXD
pin 12 PH0 ATMEGA2560 = Arduino MEGA2560 RX2 digital pin 17 = FT232 USB UART board TXD
the serial uses two vectors id interrupt one to receive and one to transmit.
binary messages have a first byte that represents the type of message that has a predetermined
size and therefore its length
----------------------------------------------------------------------------------*/

SETRX_SERIAL;
SETTX_SERIAL;

DIVIDER_SERIAL;

CLEARFLAG_SERIAL;
SETBAUDH_SERIAL;
SETBAUDL_SERIAL;
SETOTHER_SERIAL;
ENABLE_SERIAL;

ISR(RX_VECT_SERIAL) {
}

ISR(TX_VECT_SERIAL) {
}

/*
ISR(UDRE_VECT_SERIAL) {
}
*/

//-----------------------------------------------------------------------------------
sei();	//enable interrupt
