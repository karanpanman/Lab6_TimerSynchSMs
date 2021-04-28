/* Author: Karan Bhogal
 * Lab Section: 21
 * Assignment: Lab #6  Exercise #2
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 *
 *  Demo Link:
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;

unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum LA_States { S1, S2, S3, S4, S5, Wait } State;

void LED_latch()
{
	unsigned char tmpA = (~PINA) & 0x01;
  switch(State) {
	case S1:
		if ( tmpA == 0x01){
			State = Wait;
		}
		else{
			State = S2;
		}
		break;

	case S2:
		if ( tmpA == 0x01){
                        State = Wait;
                }
		else{
			State = S3;
		}
		break;

	case S3:
		if ( tmpA == 0x01){
                        State = Wait;
                }
		else{
			State = S4;
		}
		break;

	case S4:
		if ( tmpA == 0x01){
                        State = Wait;
                }
		else{
			State = S5;
		}
		break;

	case S5:
		if ( tmpA == 0x01){
                       State = Wait;
                }
		else {
			State = S2;
		}
		break;
	
	case Wait:
		if ( tmpA == 0x01){
			State = S1;
		}
		else {
			State = Wait;
		}
		break;
	
	default:
		State = S1;
		break;
  }

  switch(State)
  {
	case S1:
		PORTB = 0x01;
		break;
	case S2:
		PORTB = 0x02;
		break;
	case S3:
		PORTB = 0x04;
		break;

	case S4:
		PORTB = 0x02;
		break;

	case S5:
		PORTB = 0x01;
		break;

	case Wait:
		break;

	default:
		break;
  }
}

void main() {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;

	TimerSet(300);
	TimerOn();

    /* Insert your solution below */
	State = S1;

    while (1) {

	while (!TimerFlag);
	TimerFlag = 0;
	LED_latch();
    
    }
}
