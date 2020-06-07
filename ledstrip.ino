/* ledstrip - arduino-based controller for RGB LED strip
 *
 * (c) 2020 David Haworth
 *
 * ledstrip is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ledstrip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ledstrip.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ledstrip is an Arduino sketch, written for an Arduino Nano
*/
#include <IRremote.h>
#include "daewoo.h"
#include <setjmp.h>

#define DBG	0

/* Pin numbers
*/
#define LED_1	2
#define LED_2	3
#define LED_3	4
#define IR_PIN	7

char mode =	99;
char newmode = 99;

IRrecv irrecv(IR_PIN);

jmp_buf jb;

void all_off(void);
void all_on(void);

void mode_on(void);
void mode_off(void);
void mode_1(void);
void mode_2(void);
void mode_3(void);
void mode_4(void);
void mode_5(void);
void mode_6(void);

void fade_up(int pin);
void fade_down(int pin);
void fade_up_down(int up_pin, int down_pin);
void udelay(unsigned long us);

static inline void mdelay(unsigned long ms)
{
	udelay(ms*1000);
}

void mode_check(void);

/* setup() - standard Arduino "Init Task"
*/
void setup(void)
{
	pinMode(LED_1, OUTPUT);
	pinMode(LED_2, OUTPUT);
	pinMode(LED_3, OUTPUT);
	digitalWrite(LED_1, LOW);
	digitalWrite(LED_2, LOW);
	digitalWrite(LED_3, LOW);

	irrecv.enableIRIn();
	irrecv.blink13(true);

#if DBG
	Serial.begin(115200);
#endif

	for (;;)
	{
		setjmp(jb);
		all_off();
		mode = newmode;

#if DBG
		Serial.print("Mode ");
		Serial.println(mode, DEC);
#endif

		switch (mode)
		{
		case 0:		mode_off();	break;
		case 1:		mode_1();	break;
		case 2:		mode_2();	break;
		case 3:		mode_3();	break;
		case 4:		mode_4();	break;
		case 5:		mode_5();	break;
		case 6:		mode_6();	break;
		default:	mode_on();	break;
		}
	}
}

/* all_off() - turn all colours off
*/
void all_off(void)
{
	digitalWrite(LED_1, LOW);
	digitalWrite(LED_2, LOW);
	digitalWrite(LED_3, LOW);
}

/* all_on() - turn all colours on
*/
void all_on(void)
{
	digitalWrite(LED_1, HIGH);
	digitalWrite(LED_2, HIGH);
	digitalWrite(LED_3, HIGH);
}

/* mode_off() - turn all colours off
*/
void mode_off(void)
{
	all_off();
	for (;;)
	{
		mode_check();
	}
}

/* mode_on() - turn all colours on
*/
void mode_on(void)
{
	all_on();
	for (;;)
	{
		mode_check();
	}
}

/* mode_1() - switch individual colours on and off
*/
void mode_1(void)
{
	for (;;)
	{
		digitalWrite(LED_1, HIGH);
		mdelay(1000);
		digitalWrite(LED_1, LOW);
		digitalWrite(LED_2, HIGH);
		mdelay(1000);
		digitalWrite(LED_2, LOW);
		digitalWrite(LED_3, HIGH);
		mdelay(1000);
		digitalWrite(LED_3, LOW);
	}
}

/* mode_2() - cycle through all on/off states (Gray code)
*/
void mode_2(void)
{
	for (;;)
	{
		mdelay(1000);
		digitalWrite(LED_1, HIGH);
		mdelay(1000);
		digitalWrite(LED_2, HIGH);
		mdelay(1000);
		digitalWrite(LED_1, LOW);
		mdelay(1000);
		digitalWrite(LED_3, HIGH);
		mdelay(1000);
		digitalWrite(LED_1, HIGH);
		mdelay(1000);
		digitalWrite(LED_2, LOW);
		mdelay(1000);
		digitalWrite(LED_1, LOW);
		mdelay(1000);
		digitalWrite(LED_3, LOW);
	}
}

/* mode_3() - cycle through all on/off states with gradual change (Gray code)
*/
void mode_3(void)
{
	for (;;)
	{
		mdelay(1000);
		fade_up(LED_1);
		mdelay(1000);
		fade_up(LED_2);
		mdelay(1000);
		fade_down(LED_1);
		mdelay(1000);
		fade_up(LED_3);
		mdelay(1000);
		fade_up(LED_1);
		mdelay(1000);
		fade_down(LED_2);
		mdelay(1000);
		fade_down(LED_1);
		mdelay(1000);
		fade_down(LED_3);
	}
}

/* mode_4() - ramp colour intensity up and down simultaneously. Approx 10 secs for each fade
*/
void mode_4(void)
{
	fade_up(LED_1);		/* Initial condition */
	for (;;)
	{
		mdelay(1000);
		fade_up_down(LED_2, LED_1);
		mdelay(1000);
		fade_up_down(LED_3, LED_2);
		mdelay(1000);
		fade_up_down(LED_1, LED_3);
	}
}

/* mode_5() - placeholder
*/
void mode_5(void)
{
	mode_on();
}

/* mode_6() - placeholder
*/
void mode_6(void)
{
	mode_on();
}

/* mode_x() - template
*/
void mode_x(void)
{
	for (;;)
	{
	}
}

/* loop() - standard Arduino "Background Task"
*/
void loop(void)
{
	/* Nothing to see here */
}

void fade_up(int pin)
{
	for (unsigned i = 100; i < 10000; i += 100 )
	{
		for ( int j = 0; j < 10; j++ )
		{
			digitalWrite(pin, HIGH);
			udelay(i);
			digitalWrite(pin, LOW);
			udelay(10000 - i);
		}
	}
	if ( mode == newmode )
	{
		digitalWrite(pin, HIGH);
	}
}

void fade_down(int pin)
{
	for (unsigned i = 100; i < 10000; i += 100 )
	{
		for ( int j = 0; j < 10; j++ )
		{
			digitalWrite(pin, LOW);
			udelay(i);
			digitalWrite(pin, HIGH);
			udelay(10000 - i);
		}
	}
	if ( mode == newmode )
	{
		digitalWrite(pin, LOW);
	}
}

void fade_up_down(int up_pin, int down_pin)
{
	for (unsigned i = 100; i < 10000; i += 100 )
	{
		for ( int j = 0; j < 10; j++ )
		{
			digitalWrite(up_pin, HIGH);
			digitalWrite(down_pin, LOW);
			udelay(i);
			digitalWrite(up_pin, LOW);
			digitalWrite(down_pin, HIGH);
			udelay(10000 - i);
		}
	}
	if ( mode == newmode )
	{
		digitalWrite(up_pin, HIGH);
		digitalWrite(down_pin, LOW);
	}
}

void udelay(unsigned long us)
{
	unsigned long start = micros();

	do
	{
		mode_check();
	} while ( (micros() - start) < us );	/* Overflow/underflow doesn't matter */
}

/* mode_check() - check for a mode change
 *
 * If there has been a mode change, longjmp back to the main loop
*/
void mode_check(void)
{
	decode_results results;

	while ( irrecv.decode(&results) )
	{
		switch ( results.value )
		{
		case IR_OFF:	newmode = 0;	break;
		case IR_1:		newmode = 1;	break;
		case IR_2:		newmode = 2;	break;
		case IR_3:		newmode = 3;	break;
		case IR_4:		newmode = 4;	break;
		case IR_5:		newmode = 5;	break;
		case IR_6:		newmode = 6;	break;
		case IR_PROG:	newmode = 99;	break;
		default:						break;	/* No change */
		}
		irrecv.resume();
	}

	if ( mode != newmode )
	{
#if DBG
		Serial.print("mode_check(): mode ");
		Serial.println(newmode, DEC);
#endif
		longjmp(jb, 42);
#if DBG
		Serial.println("Oops! longjmp() returned");
#endif
	}
}
