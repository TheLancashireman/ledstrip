/* daewoo.h - IR remote control codes for an old Daewoo VCR handset
 *
 * (c) 2020 David Haworth
 *
 * daewoo.h is part of ledstrip
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
#ifndef DAEWOO_H
#define DAEWOO_H	1

/* IR codes for Daewoo handset
*/
#define IR_OFF		0x8FC6767C
#define IR_PROG		0xBF57242A
#define IR_1		0x044A3774
#define IR_2		0x47F374A1
#define IR_3		0x8D9D8908
#define IR_4		0x63BC195D
#define IR_5		0x7AC1E53E
#define IR_6		0x9EDDAA2D
#define IR_7		0x36B3537C
#define IR_8		0x97B9DE41
#define IR_9		0x26DD55E2
#define IR_0		0x3F70007B
#define IR_xx30		0xBC09BEA5
#define IR_xx		0x73F78E32
#define IR_TVVCR	0xB883A503
#define IR_AV		0x9DB921EF
#define IR_A		0x5D95D0C6
#define IR_EJECT	0x91C62B02
#define IR_PAUSE	0xEAA43C9E
#define IR_BACK		0x2A6B6F3D
#define IR_PLAY		0x39B909D9
#define IR_FWD		0x9BD7F3D0
#define IR_STOP		0xDFE68592
#define IR_RED		0x61996F77
#define IR_SPLP		0x2DDFF2AA
#define IR_PRu		0x1D0D58A6
#define IR_MENU		0xBA778113
#define IR_PALSEC	0x19873F04
#define IR_LEFT		0x056EBFB2
#define IR_OK		0xCEF6687C
#define IR_RIGHT	0xAA90F990
#define IR_INDEXd	0x3451C218
#define IR_INDEXu	0x8EC21146
#define IR_PRd		0x1A0A930D
#define IR_2ASEL	0xA7B10AFF

#define IR_REPEAT	0xFFFFFFFF

/* Mappings to ledstrip controls
*/
#define BTN_OFF		IR_OFF
#define BTN_M1		IR_1
#define BTN_M2		IR_2
#define BTN_M3		IR_3
#define BTN_M4		IR_4
#define BTN_M5		IR_5
#define BTN_M6		IR_6
#define BTN_M7		IR_7
#define BTN_M8		IR_8
#define BTN_M9		IR_9
#define BTN_M10		IR_0

#define BTN_FASTER	IR_INDEXu
#define BTN_SLOWER	IR_INDEXd

#endif
