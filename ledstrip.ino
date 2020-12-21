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

extern unsigned long lfsr(unsigned long v, unsigned long tap, unsigned long fb);

#define DBG	0

/* Pin numbers
*/
#define LED_B	2
#define LED_R	3
#define LED_G	4
#define IR_PIN	7

/* Hold-off time before key repeat starts (milliseconds)
*/
#define RPT_DELAY	250

char mode =	1;
char speed = 10;					/* Range 0..19, controlled by INDEX+ and INDEX- buttons */
char level[3] = { 5, 5, 5 };		/* Range 0..100, controlled by ??? */
unsigned long lastpress;
unsigned long lastpresstime;

/* Shift registers for pseudo-random modes
*/
unsigned long sr_r = 1;
unsigned long sr_g = 8;
unsigned long sr_b = 64;

IRrecv irrecv(IR_PIN);

jmp_buf jb;

void all_off(void);
void all_on(void);

void mode_0(void);	/* All LEDs off */
void mode_1(void);	/* All LEDs on (white light, or as near as we can get) */
void mode_2(void);	/* Three-colour cycle (exactly one colour on at any time) */
void mode_3(void);	/* Eight-colour cycle (all possible combinations of one, two and three colours) */
void mode_4(void);	/* Eight-colour cycle (like mode_3) but with a gradual change */
void mode_5(void);	/* Three-colour cycle with a gradual change; while one colour decreases another increases. */
void mode_6(void);	/* Fully-adjustable single colour */
void mode_7(void);	/* Red */
void mode_8(void);	/* Green */
void mode_9(void);	/* Blue */
void mode_a(void);	/* Pseudo-random on/off */

void fade_up(int pin);
void fade_down(int pin);
void fade_up_down(int up_pin, int down_pin);
void udelay(unsigned long us);

void mode_check(void);

/* vdelay() - a variable delay - the delay depends on the current speed
 *
 * Higher speed ==> shorter delay
 *
 * The nominal delay for speed 10 is in milliseconds.
 *
 * The arduino delay() function is not used because of the need to monitor the remote control receiver.
*/
static inline void vdelay(unsigned long ms)
{
	udelay(ms*100*(20-speed));
}

/* setup() - standard Arduino "Init Task"
 *
 * Everything happens here.
*/
void setup(void)
{
	pinMode(LED_B, OUTPUT);
	pinMode(LED_R, OUTPUT);
	pinMode(LED_G, OUTPUT);
	digitalWrite(LED_B, LOW);
	digitalWrite(LED_R, LOW);
	digitalWrite(LED_G, LOW);

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
	digitalWrite(LED_B, LOW);
	digitalWrite(LED_R, LOW);
	digitalWrite(LED_G, LOW);
}

/* all_on() - turn all colours on
*/
void all_on(void)
{
	digitalWrite(LED_B, HIGH);
	digitalWrite(LED_R, HIGH);
	digitalWrite(LED_G, HIGH);
}

/* mode_0() - turn all colours off, then wait for a mode change
*/
void mode_0(void)
{
	all_off();
	for (;;)
	{
		mode_check();
	}
}

/* mode_1() - turn all colours on, then wait for a mode change
*/
void mode_1(void)
{
	all_on();
	for (;;)
	{
		mode_check();
	}
}

/* mode_2() - switch the individual colours on and off in a cycle
*/
void mode_2(void)
{
	for (;;)
	{
		digitalWrite(LED_B, HIGH);
		vdelay(1000);
		digitalWrite(LED_B, LOW);
		digitalWrite(LED_R, HIGH);
		vdelay(1000);
		digitalWrite(LED_R, LOW);
		digitalWrite(LED_G, HIGH);
		vdelay(1000);
		digitalWrite(LED_G, LOW);
	}
}

/* mode_3() - cycle through all eight on/off states (Gray code)
*/
void mode_3(void)
{
	for (;;)
	{
		vdelay(1000);
		digitalWrite(LED_B, HIGH);
		vdelay(1000);
		digitalWrite(LED_R, HIGH);
		vdelay(1000);
		digitalWrite(LED_B, LOW);
		vdelay(1000);
		digitalWrite(LED_G, HIGH);
		vdelay(1000);
		digitalWrite(LED_B, HIGH);
		vdelay(1000);
		digitalWrite(LED_R, LOW);
		vdelay(1000);
		digitalWrite(LED_B, LOW);
		vdelay(1000);
		digitalWrite(LED_G, LOW);
	}
}

/* mode_4() - cycle through all eight on/off states with gradual change (Gray code)
*/
void mode_4(void)
{
	for (;;)
	{
		vdelay(1000);
		fade_up(LED_B);
		vdelay(1000);
		fade_up(LED_R);
		vdelay(1000);
		fade_down(LED_B);
		vdelay(1000);
		fade_up(LED_G);
		vdelay(1000);
		fade_up(LED_B);
		vdelay(1000);
		fade_down(LED_R);
		vdelay(1000);
		fade_down(LED_B);
		vdelay(1000);
		fade_down(LED_G);
	}
}

/* mode_5() - ramp colour intensity up and down simultaneously.
*/
void mode_5(void)
{
	fade_up(LED_B);		/* Initial condition */
	for (;;)
	{
		vdelay(1000);
		fade_up_down(LED_R, LED_B);
		vdelay(1000);
		fade_up_down(LED_G, LED_R);
		vdelay(1000);
		fade_up_down(LED_B, LED_G);
	}
}

/* mode_6() - variable intensity with three independent colour intensities.
 *
 * Algorithm:
 *		- In the loop (every 100 us):
 *			LEDs whose level is >= j are on, others are off
*/
void mode_6(void)
{
	for (;;)
	{
		for ( int j = 1; j < 100; j++ )
		{
			digitalWrite(LED_B, ( level[0] >= j ) ? HIGH : LOW);
			digitalWrite(LED_R, ( level[1] >= j ) ? HIGH : LOW);
			digitalWrite(LED_G, ( level[2] >= j ) ? HIGH : LOW);
			udelay(100);
		}
	}
}

/* mode_7() - red
*/
void mode_7(void)
{
	digitalWrite(LED_R, HIGH);
	digitalWrite(LED_G, LOW);
	digitalWrite(LED_B, LOW);
	for (;;)
	{
		mode_check();
	}
}

/* mode_8() - green
*/
void mode_8(void)
{
	digitalWrite(LED_R, LOW);
	digitalWrite(LED_G, HIGH);
	digitalWrite(LED_B, LOW);
	for (;;)
	{
		mode_check();
	}
}

/* mode_9() - blue
*/
void mode_9(void)
{
	digitalWrite(LED_R, LOW);
	digitalWrite(LED_G, LOW);
	digitalWrite(LED_B, HIGH);
	for (;;)
	{
		mode_check();
	}
}

/* mode_a() - pseudo random sequence on each LED
*/
void mode_a(void)
{
	for (;;)
	{
		sr_r = lfsr(sr_r, 0x8, 0x20000);	
		digitalWrite(LED_R, ( ((sr_r & 0x1) == 0) ) ? HIGH : LOW);
		sr_g = lfsr(sr_g, 0x8, 0x100000);
		digitalWrite(LED_G, ( ((sr_g & 0x1) == 0) ) ? HIGH : LOW);
		sr_b = lfsr(sr_b, 0x20, 0x80000);
		digitalWrite(LED_B, ( ((sr_b & 0x1) == 0) ) ? HIGH : LOW);
		vdelay(1000);
	}
}

/* loop() - standard Arduino "Background Task" ... not used
*/
void loop(void)
{
	/* Nothing to see here */
}

/* fade_up() - gradually increase the intensity of a single LED
 *
 * The outer loop ramps up the intensity. The inner loop creates the PWM with a period of 10 ms.
*/
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

/* fade_down() - gradually decrease the intensity of a single LED
 *
 * The outer loop ramps down the intensity. The inner loop creates the PWM with a period of 10 ms.
*/
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

/* fade_down() - gradually change the intensity of two LEDs - one up, one down.
 *
 * The outer loop controls the intensity. The inner loop creates the PWM with a period of 10 ms with
 * the two LEDs working in opposite directions.
*/
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

/* udelay() - delay by the specified number of microseconds
 *
 * While the delay is in progress, the IR receiver is monitored.
*/
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
 * If there has been a mode change, longjmp back to the main loop.
 * Speed and intensity changes just modify the control variables.
*/
void mode_check(void)
{
	decode_results results;
	char newmode = mode;

	while ( irrecv.decode(&results) )
	{
		unsigned long key = results.value;
		irrecv.resume();
		
		if ( key == IR_REPEAT )
		{
			if ( (millis() - lastpresstime) < RPT_DELAY )
			{
				continue;
			}
		}
		else
		{
			lastpress = key;
			lastpresstime = millis();
		}
		switch ( lastpress )
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

		case BTN_L1u:		if ( level[0] < 100 )	level[0]++;		break;
		case BTN_L1d:		if ( level[0] > 0 )		level[0]--;		break;
		case BTN_L2u:		if ( level[1] < 100 )	level[1]++;		break;
		case BTN_L2d:		if ( level[1] > 0 )		level[1]--;		break;
		case BTN_L3u:		if ( level[2] < 100 )	level[2]++;		break;
		case BTN_L3d:		if ( level[2] > 0 )		level[2]--;		break;

		default:						break;	/* No change */
		}
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
