/*  400HZ xmass light anti-flicker driver.>
    Copyright (C) <2020>  <Jarrett R. Cigainero>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>*/

#ifndef INIT_H
#define	INIT_H

#define wsquare 0x0
#define wsine 0x1

#define pwmFreq100K 0x0000
#define pwmFreq50K 0x0002
#define pwmFreq25K 0x0004
#define pwmFreq12500 0x0006
#define pwmFreq6250 0x0008
#define pwmFreq3125 0x000A
#define pwmFreq1562p5 0x000C
#define pwmFreq781p25 0x000E

extern void Init(void);
extern void low_battery_shutdown(void);
extern void low_power_mode(void);

//sine wave lookup. 200 points total.
const unsigned int sine[] = {0x8000,0x8405,0x8809,0x8c0b,0x900a,0x9405,0x97fc,0x9bec,
0x9fd4,0xa3b5,0xa78d,0xab5b,0xaf1e,0xb2d5,0xb67f,0xba1c,
0xbda9,0xc128,0xc495,0xc7f2,0xcb3c,0xce73,0xd196,0xd4a5,
0xd79e,0xda82,0xdd4e,0xe003,0xe29f,0xe523,0xe78d,0xe9dd,
0xec12,0xee2c,0xf02a,0xf20c,0xf3d0,0xf578,0xf702,0xf86e,
0xf9bb,0xfaea,0xfbfa,0xfcea,0xfdbb,0xfe6c,0xfefd,0xff6e,
0xffbe,0xffef,0xffff,0xffef,0xffbe,0xff6e,0xfefd,0xfe6c,
0xfdbb,0xfcea,0xfbfa,0xfaea,0xf9bb,0xf86e,0xf702,0xf578,
0xf3d0,0xf20c,0xf02a,0xee2c,0xec12,0xe9dd,0xe78d,0xe523,
0xe29f,0xe003,0xdd4e,0xda82,0xd79e,0xd4a5,0xd196,0xce73,
0xcb3c,0xc7f2,0xc495,0xc128,0xbda9,0xba1c,0xb67f,0xb2d5,
0xaf1e,0xab5b,0xa78d,0xa3b5,0x9fd4,0x9bec,0x97fc,0x9405,
0x900a,0x8c0b,0x8809,0x8405,0x8000,0x7bfa,0x77f6,0x73f4,
0x6ff5,0x6bfa,0x6803,0x6413,0x602b,0x5c4a,0x5872,0x54a4,
0x50e1,0x4d2a,0x4980,0x45e3,0x4256,0x3ed7,0x3b6a,0x380d,
0x34c3,0x318c,0x2e69,0x2b5a,0x2861,0x257d,0x22b1,0x1ffc,
0x1d60,0x1adc,0x1872,0x1622,0x13ed,0x11d3,0xfd5,0xdf3,
0xc2f,0xa87,0x8fd,0x791,0x644,0x515,0x405,0x315,
0x244,0x193,0x102,0x91,0x41,0x10,0x0,0x10,
0x41,0x91,0x102,0x193,0x244,0x315,0x405,0x515,
0x644,0x791,0x8fd,0xa87,0xc2f,0xdf3,0xfd5,0x11d3,
0x13ed,0x1622,0x1872,0x1adc,0x1d60,0x1ffc,0x22b1,0x257d,
0x2861,0x2b5a,0x2e69,0x318c,0x34c3,0x380d,0x3b6a,0x3ed7,
0x4256,0x45e3,0x4980,0x4d2a,0x50e1,0x54a4,0x5872,0x5c4a,
0x602b,0x6413,0x6803,0x6bfa,0x6ff5,0x73f4,0x77f6,0x7bfa};



#endif	/* INIT_H */
