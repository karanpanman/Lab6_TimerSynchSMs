/* Author: Karan Bhogal
 * Lab Section: 21
 * Assignment: Lab #6  Exercise #3
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/1fhGhJu3tT9tbfwIVooRn9_ExypXcbCnV/view?usp=sharing
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

unsigned char num = 0x09;

enum LA_States { Wait, S1, Inc, S2, Dec, Reset, loop } State;

void LED_latch()
{
unsigned char tmpA = (~PINA) & 0x03;

 switch(State) {
   case Wait:
	if (tmpA == 0x03) {
		State = Reset;
	}
	else if (tmpA == 0x01){
		State = S1;
	}
	else if (tmpA == 0x02){
                State = S2;
        }
	else {
		State = Wait;
	}
	break;
  
   case S1:
	num = 0x09;
	if (tmpA == 0x02){
		State = S2;
	}
	else if (tmpA == 0x00){
		State = Wait;
	}
	else {
		State = Inc;
	}
	break;

   case Inc:
	if (tmpA == 0x03){
                State = Reset;
        }
        else if (tmpA == 0x02){
                State = S2;
        }
	else if (tmpA == 0x01){ //new stuff
		State = loop;
		
	}
        else{
                State = Wait;
        }
	break;

   case loop:

	if ( (tmpA == 0x01) && (num > 0)){
		num = num - 1;
		State = loop;	
	}
	else if ( (tmpA == 0x02) && (num > 0)){
                num = num - 1;
                State = loop;
        }
	else if ( num == 0 ){

		if (tmpA == 0x01){
			State = S1;
		}
		else if (tmpA == 0x02){
			State = S2;
		}
	}
	else{
		State = Wait;
	}

	break;

   case S2:
	num = 0x09;
	if (tmpA == 0x01){
		State = S1;
	}
	else if (tmpA == 0x00){
                State = Wait;
        }
	else {
		State = Dec;
	}
	break;

   case Dec:
        if (tmpA == 0x03){
                State = Reset;
        }
        else if (tmpA == 0x01){
                State = S1;
        }
	else if (tmpA == 0x02){ //new stuff
                State = loop;
        }
        else{
                State = Wait;
        }
	break;

   default:
	State = Wait;
	break;
  } //transitions

  switch(State){
   case Wait:
	break;

   case S1:
	if (PORTB < 0x09){
		PORTB = PORTB + 1;
	}
	break;

   case Inc:
	
	break;
   
   case S2:
	if (PORTB > 0x00){
		PORTB = PORTB - 1;
	}
	break;

   case Dec:
	/*if (PORTB > 0x00){
		while (!TimerFlag);
		PORTB = PORTB - 1;
	}*/
	break;

   case loop:

	break;

   case Reset:
	PORTB = 0x00;
	break;

   default:
	break;
   }
}

void main() {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;

	TimerSet(100);
	TimerOn();

	PORTB = 0x07;

    /* Insert your solution below */
	State = Wait;

    while (1) {

	while (!TimerFlag);
	TimerFlag = 0;
	LED_latch();
    
    }
}
