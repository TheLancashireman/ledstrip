/* lfsr.cpp - implementation of linear feedback shift register
 *
 * Generates pseudo-random bit streams
 *
 * (c) 2020 David Haworth
 *
 * This file is part of ledstrip
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

/* A two-tap LFSR. Tap 0 is the LSB as always. Tap 1 is specified.
 * The feed-in is also specified - this defines the length.
*/
unsigned long lfsr(unsigned long v, unsigned long tap, unsigned long fb)
{
	unsigned long x = v;	/* Because directly manipulating v is considered to be Bad Form */

	if ( (x & 0x01) == 0 )
	{
		if ( (x & tap) != 0 )	x |= fb;
	}
	else
	{
		if ( (x & tap) == 0 )	x |= fb;
	}
	return x >> 1;
}
