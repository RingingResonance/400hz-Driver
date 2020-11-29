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

#ifndef OSC_C
#define	OSC_C

#include <p30f3011.h>
#include <dsp.h>
#include "IRQs.h"
#include "Init.h"
#include "OSC.h"

//Switch from Internal OSC to External OSC
void OSCIntToExt(void){
    if (OSCCONbits.COSC != 7){
        OSCCONH = 0x78;
        OSCCONH = 0x9A;
        OSCCONbits.NOSC = 7;
        OSCCONL = 0x46;
        OSCCONL = 0x57;
        OSCCONbits.OSWEN = 1;
    }
}

//Switch from External OSC to Internal OSC
//Can be used to attempt to switch to internal fast RC in the event of OSC failure.
void OSCExtToInt(void){
    if (OSCCONbits.COSC != 1){
        OSCCONH = 0x78;
        OSCCONH = 0x9A;
        OSCCONbits.NOSC = 1;
        OSCCONL = 0x46;
        OSCCONL = 0x57;
        OSCCONbits.OSWEN = 1;
    }
}


//Shut everything down, but you must manually clear outputs on the event of an error so that the output is shut off as quickly as possible.
void sysShutdown(void){
    //make sure 400hz gen is OFF
    PDC1 = 0000;            //set output to 0
    PDC2 = 0000;            //set output to 0
    PDC3 = 0000;            //set output to 0
    sftStart = 0;       //Reset soft start.
    T2CONbits.TON = 0;  //stop timer 2
    TMR2 = 0x0000;      //reset timer 2
    IFS0bits.T2IF = 0;  //clear timer 2 IRQ flag.
}

void StartRecover(void){
    //Reset timer vars for wave type.
    //Sine Wave = 250 for 400hz. 1666.6666 for 60hz. 2000 for 50hz. At 20mips.
    //Square Wave = 500 for 400hz. 3,333.3333 for 60hz. 4000 for 50hz. At 20mips.
    if (frequencySet == 50){
        //50hz for running.
        if (waveType == wsine){
            PR2 = 2000;       //2000 is for 50hz in PWM mode. 20MIPS
            slowStart = 2000;
        }
        else{
            PR2 = 4000;       //square wave 50hz
            slowStart = 4000;
        }
    }
    else{
        //60hz for starting, but not for running.
        if (waveType == wsine){
            PR2 = 1666;       //1666 is for 60hz in PWM mode. 20MIPS
            slowStart = 1666;
        }
        else{
            PR2 = 3333;       //square wave starting frequency. 60hz
            slowStart = 3333;
        }
    }
    //If 400hz is chosen then 60hz will be selected here and 400hz will be ramped up to in SquareGen() or SineGen()
    TMR1 = 0x0000;          //Clear Timer 1
    StrtTmr = 0;            //Clear startup timer.
    T1CONbits.TON = 1;      // Start Timer 1 for recovery or startup process.
}

#endif	/* OSC_H */
