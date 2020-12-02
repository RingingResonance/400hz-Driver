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

#ifndef IRQS_H
#define	IRQS_H

extern void __attribute__((interrupt, no_auto_psv)) _INT0Interrupt (void);
extern void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt (void);
extern void __attribute__((interrupt, no_auto_psv)) _ADCInterrupt (void);
extern void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt (void);
extern void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void);
extern void __attribute__((interrupt, no_auto_psv)) _T2Interrupt (void);
extern void __attribute__((interrupt, no_auto_psv)) _FLTAInterrupt (void);
extern void __attribute__((interrupt, no_auto_psv)) _OscillatorFail (void);
extern void __attribute__((interrupt, no_auto_psv)) _AddressError (void);
extern void __attribute__((interrupt, no_auto_psv)) _StackError (void);
extern void __attribute__((interrupt, no_auto_psv)) _MathError (void);
extern void __attribute__((interrupt, no_auto_psv)) _ReservedTrap7 (void);

extern void hzSineGen(void);
extern void SquareGen(void);
extern void SineGen(void);

// 400hz generation stuff.
register unsigned int dsp_accA asm("A");
register unsigned int dsp_accB asm("B");

unsigned int wheel = 0;
unsigned int rampDiv = 0;
unsigned int phase = 0;
float FtmpSine = 0;
float FpwmPercent = 0;

//settings
unsigned int waveType = 0;
void waveSet(int, int);
int frequencySet = 0;

//modified sine wave stuff
unsigned int rotation = 0;
unsigned int resetWait = 0;

//Pure sine wave stuff
unsigned int hzdrum = 0;
unsigned int sftStart = 0;
unsigned int slowStart = 0;
unsigned int fracttemp = 0;
unsigned int FRcalcTMP = 0;
unsigned int pwmOutput1 = 0;
unsigned int pwmOutput2 = 0;
void sixtyHZprecision(void);

// Startup and recovery stuff.
unsigned int StrtTmr = 0;
unsigned int clkTimeout = 0;
unsigned int clkReady = 0;
unsigned int exClkFail = 0;

#endif	/* IRQS_H */

