# ledstrip

## An arduino-based controller for RGB LED strips

The LED strip used is the common type with four connectors: 12v supply and three
colour connections (marked R, G and B) that cause all the LEDs to glow when connected to 0v.

The controller uses PWM on three output pins to control the intensity of
each colour from 0% to 100% in steps of 1%.

Various modes of operation - constant, switching and cross-fading can be selected.

The arduino is controlled by means of an infra-red remote control handset.

## Circuit

Each output pin is connected to the base of an NPN transistor via a 10k resistor. The collector of each
transistor is connected to one of the R, G and B pins of the LED strip and the emitters are connected
to 0v. I used BC548 transistors, but just about any NPN transistor will work. Watch out for maximum
current ratings though.

You need a two-rail PSU; 12v for the LEDs and 5v for the arduino. You could derive the arduino power
from the 12v supply with a suitable regulator (e.g. 7805).

## Infra-red control

I used a Sharp GP1UV70QS IR sensor. It has three pins: 5v, 0v and output. The output is connected directly
to an arduino pin.

I used a handset that belonged to a long-dead Daewoo VCR. The header file daewoo.h defines the keycodes
and maps them to ledstrip functions.

You can use any old handset, but you will have to determine the coding of the keys and map them to the
mode controls in the ledstrip program. You can create and use your own handset mapping file.

You can find more information at https://www.circuitbasics.com/arduino-ir-remote-receiver-tutorial/

The IRremote library is at https://github.com/z3t0/Arduino-IRremote

If you use the arduino IDE the library seems to work out ot the box. If (like me) you prefer a command-line,
you might have to create a src/ directory under the library directory and move all the .h and .cpp
files into it.

## PWM

The modes that offer variable colour intensities use PWM to control the brightness. The pulse frequency is
approximately  100 Hz (interval 10ms) and the pulse width varies in steps of 100 microseconds. The udelay()
function uses the arduino micros() function for the timing.

Software PWM is used (rather than AnalogWrite) because I want to extend the controller to control several strips
and arduino has a limited number of timer PWM ports.

For longer delays, the mdelay() function provides delays in units of milliseconds, but is implemented in terms
of the udelay() function in order to monitor the IR receiver while the delay is in progress.

The function mode_check() gets called in the delay loops. It reads and acts on keypresses from the handset.
If there is a mode change, the function calls longjmp() to unwind the stack and return (again) from a call to
setjmp() in the outermost loop. Not every keypress causes a mode change. Some just change the intensity or
the speed of change.

I use setjmp/longjmp because the arduino avr libraries don't appear to have the functions to support
C++ exceptions.

## License

(c) 2020 David Haworth

ledstrip is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ledstrip is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ledstrip.  If not, see <http://www.gnu.org/licenses/>.
