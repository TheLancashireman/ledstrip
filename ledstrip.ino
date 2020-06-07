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

char mode =	1;
char speed = 10;	/* Range 0..19, controlled by INDEX+ and INDEX- buttons */

IRrecv irrecv(IR_PIN);

jmp_buf jb;

void all_off(void);
void all_on(void);

void mode_0(void);
void mode_1(void);
void mode_2(void);
void mode_3(void);
void mode_4(void);
void mode_5(void);
void mode_6(void);
void mode_7(void);
void mode_8(void);
void mode_9(void);
void mode_a(void);

void fade_up(int pin);
void fade_down(int pin);
void fade_up_down(int up_pin, int down_pin);
void udelay(unsigned long us);

static inline void mdelay(unsigned long ms)
{
	udelay(ms*100*(20-speed));
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

#if DBG
		Serial.print("Mode ");
		Serial.println(mode, DEC);
#endif

		switch (mode)
		{
		case 0:		mode_0();	break;
		case 1:		mode_1();	break;
		case 2:		mode_2();	break;
		case 3:		mode_3();	break;
		case 4:		mode_4();	break;
		case 5:		mode_5();	break;
		case 6:		mode_6();	break;
		case 7:		mode_7();	break;
		case 8:		mode_8();	break;
		case 9:		mode_9();	break;
		case 10:	mode_a();	break;
		default:	mode_1();	break;		/* Default to ON */
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

/* mode_0() - turn all colours off
*/
void mode_0(void)
{
	all_off();
	for (;;)
	{
		mode_check();
	}
}

/* mode_1() - turn all colours on
*/
void mode_1(void)
{
	all_on();
	for (;;)
	{
		mode_check();
	}
}

/* mode_2() - switch individual colours on and off
*/
void mode_2(void)
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

/* mode_3() - cycle through all on/off states (Gray code)
*/
void mode_3(void)
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

/* mode_4() - cycle through all on/off states with gradual change (Gray code)
*/
void mode_4(void)
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

/* mode_5() - ramp colour intensity up and down simultaneously.
*/
void mode_5(void)
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

/* mode_6() - placeholder
*/
void mode_6(void)
{
	mode_1();
}

/* mode_7() - placeholder
*/
void mode_7(void)
{
	mode_1();
}

/* mode_8() - placeholder
*/
void mode_8(void)
{
	mode_1();
}

/* mode_9() - placeholder
*/
void mode_9(void)
{
	mode_1();
}

/* mode_a() - placeholder
*/
void mode_a(void)
{
	mode_1();
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
		for ( int j = 0; j < (20-speed); j++ )
		{
			digitalWrite(pin, HIGH);
			udelay(i);
			digitalWrite(pin, LOW);
			udelay(10000 - i);
		}
	}
	digitalWrite(pin, HIGH);
}

void fade_down(int pin)
{
	for (unsigned i = 100; i < 10000; i += 100 )
	{
		for ( int j = 0; j < (20-speed); j++ )
		{
			digitalWrite(pin, LOW);
			udelay(i);
			digitalWrite(pin, HIGH);
			udelay(10000 - i);
		}
	}
	digitalWrite(pin, LOW);
}

void fade_up_down(int up_pin, int down_pin)
{
	for (unsigned i = 100; i < 10000; i += 100 )
	{
		for ( int j = 0; j < (20-speed); j++ )
		{
			digitalWrite(up_pin, HIGH);
			digitalWrite(down_pin, LOW);
			udelay(i);
			digitalWrite(up_pin, LOW);
			digitalWrite(down_pin, HIGH);
			udelay(10000 - i);
		}
	}
	digitalWrite(up_pin, HIGH);
	digitalWrite(down_pin, LOW);
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
	char newmode = mode;

	while ( irrecv.decode(&results) )
	{
		switch ( results.value )
		{
		case BTN_OFF:		newmode = 0;	break;
		case BTN_M1:		newmode = 1;	break;
		case BTN_M2:		newmode = 2;	break;
		case BTN_M3:		newmode = 3;	break;
		case BTN_M4:		newmode = 4;	break;
		case BTN_M5:		newmode = 5;	break;
		case BTN_M6:		newmode = 6;	break;
		case BTN_M7:		newmode = 7;	break;
		case BTN_M8:		newmode = 8;	break;
		case BTN_M9:		newmode = 9;	break;
		case BTN_M10:		newmode = 10;	break;

		case BTN_SLOWER:	if ( speed > 0 )	speed--;	break;
		case BTN_FASTER:	if ( speed < 19 )	speed++;	break;

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
		mode = newmode;
		longjmp(jb, 42);
#if DBG
		Serial.println("Oops! longjmp() returned");
#endif
	}
}
