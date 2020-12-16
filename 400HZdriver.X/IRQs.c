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

#ifndef IRQS_C
#define IRQS_C


#include <p30f3011.h>
#include <dsp.h>
#include "IRQs.h"
#include "Init.h"
#include "OSC.h"

/*****************/
/* IRQs go here. */
/*****************/

//Clock Error recovery process.
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void){
    PORTBbits.RB6 = 1; //CPU ACT light.
    //At 200ms after start/recovery attempt to change clocks to EXTERN.
    if(StrtTmr == 2){
        //Check how many times we have tried to switch clocks. 
        //Make no more than 5 attempts before giving up and using Internal FRC.
        if(clkTimeout <= 5){
            clkTimeout++;   //Log an attempt at changing clocks to EXTERN.
            OSCIntToExt();      //Attempt to switch to external clock.
        }
    }
    //At 500ms after start/recovery check to see if clock switching was successfull.
    if(StrtTmr == 5){
        //check for successfull clock switch to external source.
        if (OSCCONbits.OSWEN == 0 && OSCCONbits.COSC == 7){
            //with 10 mhz clock and 8xPLL 125 gives exactly 400 hz when in PWM mode.
            //PR2 = 125;       //Default is 125 for external 10mhz clock.
            clkReady = 1;
            PR1 = 7812;     //Running on external 10mhz
        }
        else if (OSCCONbits.OSWEN == 0 && OSCCONbits.COSC == 1){
            //Fail the clock source, switch to internal 10mhz clock through relay and try again.
            if (exClkFail == 1){
                //Both clocks have failed. Shutdown all and wait for power cycle.
                OSCCONbits.OSWEN = 0;   //Abandon the clock switch.
                PORTB = 0x0009;      //Clock Fail Light
                T1CONbits.TON = 0;
                PTCONbits.PTEN = 0;
                OSCCONbits.LPOSCEN = 0;
                Sleep();
            }
            exClkFail = 1;
            PORTBbits.RB2 = 1;  //clock select relay.
            clkTimeout = 0;     //Reset clock switch attempts.
            StartRecover();     //Retry, reset timer 1 and StrtTmr
        }
        else {
            //Internal FRC has also failed to switch. Shutdown all and wait for power cycle.
            PORTB = 9;      //Clock Fail Light
            PORTBbits.RB3 = 1;      //Clock Fail Light
            PORTBbits.RB0 = 0;
            OSCCONbits.OSWEN = 0;   //Abandon the clock switch.
            T1CONbits.TON = 0;
            PTCONbits.PTEN = 0;
            OSCCONbits.LPOSCEN = 0;
            Sleep();
        }
    }
    //At one second after start/recovery start the 400hz signal gen.
    if(StrtTmr == 10 && clkReady == 1){
        T2CONbits.TON = 1;      // Start Timer 2 for 400hz gen
        T1CONbits.TON = 0;      // Stop Timer 1, we have started on 10MHZ or 7.37MHZ
        PORTBbits.RB1 = 1;      //Enable output
    }
    StrtTmr++;              //100ms
    IFS0bits.T1IF = 0; 
}
//Used for wave form generation.
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt (void){
    PORTBbits.RB6 = 1;
    if(waveType == wsine){
        SineGen();
    }
    else if (waveType == wsquare){
        SquareGen();
    }
    else{
        //INVALID WAVE TYPE DEFFINED. SHUTDOWN AND DO NOTHING ELSE.
        LATE = 0;               //set all porte outputs to 0
        PORTE = 0;              //probably redundant, but I'm still learning this Micro
        PORTBbits.RB1 = 0; //Disable output as quickly as possible.
        sysShutdown();      //Shut down everything else.
        Sleep();
    }
/****************************************/
    /* End the IRQ. */
	IFS0bits.T2IF = 0;   
}

void SquareGen(void){
    PORTBbits.RB0 = 1;
    // Hold frequency at 60 for a few seconds to let user rest
    // the GFCI if needed. WARNING: A 60hz GFCI will not detect the same amount of fault current at 400hz as it does
    // at 60hz. It will detect less.
    if (resetWait < 20000 && frequencySet == 400){
        //Hold at 60 for a few seconds.
        resetWait++;
        if (r400ramp == 0){
            resetWait = 20000;
        }
    }
    else if (slowStart > 500 && frequencySet == 400){
        if(rampDiv >= 100){
            slowStart--; //Decrease slowStart until it is 500.
            if (r400ramp == 0){
                slowStart = 500;
            }
            PR2 = slowStart; //500 for 400hz. 3,333.3333 for 60hz. 4000 for 50hz. At 20mips.
            rampDiv = 0;
            PORTBbits.RB0 = 1;
        }
        else {
            rampDiv++;
        }
    }
    else if (frequencySet == 60)
        sixtyHZprecision();         //This will add a slight jitter to the wave, but should give a sudo-precise 60hz.
    //100 cycles gives me a full wave.
    //25 cycles per pulse switch.
    //RE1 and RE3 are used.
    if(rotation >= 100){
        rotation = 0;
    }

    if (rotation == 0){
        PORTE = 2;
        PORTBbits.RB0 = 1;
    }
    else if (rotation == 30){
        PORTE = 0;
    }
    else if (rotation == 49){
        PORTE = 8;
        PORTBbits.RB0 = 0;
    }
    else if (rotation == 80){
        PORTE = 0;
    }
    rotation++;
}

void SineGen(void){
    //Increment soft start only on startup. When done, hold frequency at 60 for a few seconds to let user rest
    // the GFCI if needed. WARNING: A 60hz GFCI will not detect the same amount of fault current at 400hz as it does
    // at 60hz. It will detect less.
    if (sftStart < 0xFFFE){
        //Ramp up the voltage scaler from 0
        sftStart += 5;
    }
    else if (resetWait < 40000 && frequencySet == 400){
        //Hold at 60hz for a few seconds if frequencySet is at 400.
        resetWait++;
        if (r400ramp == 0){
            resetWait = 40000;
        }
    }
    else if(slowStart > 250 && frequencySet == 400){
        if(rampDiv >= 100){
            slowStart--;    //Decrease slowStart until it is 250.
            if (r400ramp == 0){
                slowStart = 250;
            }
            PR2 = slowStart;    //250 for 400hz. 1666.6666 for 60hz. 2000 for 50hz. At 20mips.
            rampDiv = 0;
        }
        else {
            rampDiv++;
        }
    }
    else if (frequencySet == 60){
        //for a precision 60hz, we need to constantly rotate our timing variable "PR2" through 1665, 1666, 1669
        //This should average out to 1666.6666 repeating because 1665 + 1666 + 1669 = 5000. 5000 / 3 = 1666.6666 repeating.
        //This will add a slight ripple to the waveform, but should still be acceptable with an appropriate output filter.
        sixtyHZprecision();
    }
    /*Get sine of wheel and multiply it by the soft start number and PWM scaler number.
     * FtmpSine = sine[wheel]; //Convert INT from sine to a float 0 - 65535
     * MAGIC NUMBER is 165 for a scale of 0 - 398 suitable for PWM output due to always needing a negative pulse for the boot strap capacitor.
     * 65535 / 398 is 164.66080402010050251256281407035
     * Eliminate a multiply function by pre-calculating the magic number.
     * Multiplying is generally faster, so instead of dividing by 165 we can multiply by 0.00606060606060606060606060606061
     * or just 0.0060606
     * Calculate the output number for one of two PWM outputs.
     * Floating point looks like the following, but we are going to use fractional math with the DSP engine because it is much faster.
     * FpwmPercent = ((FtmpSine * FsftStart) / 65535) * 398; //Full equation.
     * FpwmPercent = (FtmpSine * FsftStart) * 0.0060606; //This would work in double_floating point, but it's still pretty slow.
     * pwmOutput = FpwmPercent;    // Split up the equation as some weird things happen when you try to calculate a Float directly into an INT.
    
     * Do it with fractional instead to speed things up. Multiplying a number by 0.0060606 is the same as
     * multiplying a fractional by 398 (0x18E) and then shifting the result 16 bits to the right.
     * When the dsPIC reads from the 40 bit accumulator it reads the upper 16 bits.
     * This effectively does our 16 bit shift for us for free.
     */
    dsp_accA = __builtin_mpy(sine[wheel], 0x178, NULL, NULL, 0, NULL, NULL, 0); //(sine of wheel) * 0.0060606
    FRcalcTMP = __builtin_sac(dsp_accA, 0); //store the result into FRcalcTMP
    //Scaled sine data down to something acceptable for PWM. Now just need to scale it down some more for soft starting.
    dsp_accA = __builtin_mpy(sftStart, FRcalcTMP, NULL, NULL, 0, NULL, NULL, 0); //scale FRcalcTMP by the soft start number.
    pwmOutput1 = __builtin_sac(dsp_accA, 0); //Now it's scaled properly for the PWM duty cycle reg.
    //Output 1 is now scaled properly.
    
    //Invert and scale output 2
    pwmOutput2 = 0x0178 - FRcalcTMP;
    dsp_accA = __builtin_mpy(sftStart, pwmOutput2, NULL, NULL, 0, NULL, NULL, 0); //scale pwmOutput2 by the soft start number.
    pwmOutput2 = __builtin_sac(dsp_accA, 0); //Now it's scaled properly for the PWM duty cycle reg.
    //All that math down to 7 instructions.
    
    PDC1 = pwmOutput1;
    PDC2 = pwmOutput2;
    //Blink the LED at the same speed we are running at for debugging and to show that the output is on.
    if (wheel == 51){
        PORTBbits.RB0 = 1;
    }
    else if (wheel == 151){
        PORTBbits.RB0 = 0;
    }
    
    //Rotate the wheel.
    wheel++;
    if (wheel >= 200){
        wheel = 0;
    }
}

//shuts everything down, sets wave type and frequency standards. Square wave or PWM generated Sine wave.
void waveSet(int wv, int freq){
    LATE = 0;               //set all porte outputs to 0
    PORTE = 0;              //probably redundant, but I'm still learning this Micro
    PORTBbits.RB1 = 0; //Disable output as quickly as possible.
    sysShutdown();      //Shut down everything else.
    waveType = wv;
    frequencySet = freq;
}

//for a precision 60hz, we need to constantly rotate our timing variable "PR2" through 1665, 1666, 1669
//This should average out to 1666.6666 repeating because 1665 + 1666 + 1669 = 5000. 5000 / 3 = 1666.6666 repeating.
//This will add a slight ripple to the waveform, but should still be acceptable with an appropriate output filter.
void sixtyHZprecision(void){
    if (hzdrum >= 6)
        hzdrum = 0;
    
    if(waveType == wsine){
        //Sine wave
        if (hzdrum == 0)
            PR2 = 1665;
        else if (hzdrum == 1)
            PR2 = 1666;
        else if (hzdrum == 2)
            PR2 = 1669;
        else if (hzdrum == 3)
            PR2 = 1669;
        else if (hzdrum == 4)
            PR2 = 1666;
        else if (hzdrum == 5)
            PR2 = 1665;
    }
    else {
        //Square wave
        if (hzdrum == 0)
            PR2 = 3332;
        else if (hzdrum == 1)
            PR2 = 3333;
        else if (hzdrum == 2)
            PR2 = 3335;
        else if (hzdrum == 3)
            PR2 = 3335;
        else if (hzdrum == 4)
            PR2 = 3333;
        else if (hzdrum == 5)
            PR2 = 3332;
    }
    hzdrum++;
}

/*******************************************************************************/
/*******************************************************************************/
/* CPU TRAPS, log erros here and shut down if needed.*/
void __attribute__((interrupt, no_auto_psv)) _FLTAInterrupt (void){
    LATE = 0;               //set all porte outputs to 0
    PORTE = 0;              //probably redundant, but I'm still learning this Micro
    PORTBbits.RB1 = 0; //Disable output as quickly as possible.
    PORTBbits.RB6 = 1;//CPU ACT light.
    sysShutdown();      //Shut down everything else.
    StartRecover();     //Attempt a restart.
    INTCON1bits.COVTE = 0;
}
void __attribute__((interrupt, no_auto_psv)) _OscillatorFail (void){
    //Check the PLL lock bit as fast as possible before anything else because it doesn't stay set for long.
    /*if(OSCCONbits.LOCK == 0){
        if(osc_fail_event == 0){
        }*/
    LATE = 0;               //set all porte outputs to 0
    PORTE = 0;              //probably redundant, but I'm still learning this Micro
    PORTBbits.RB1 = 0; //Disable output as quickly as possible.
    PORTBbits.RB6 = 1;//CPU ACT light.
    //make sure 400hz gen is OFF
    sysShutdown();      //Shut down everything else.
    //Attempt a restart.
    StartRecover();
    PR1 = 976;          //running on FRC
    INTCON1bits.OSCFAIL = 0;
}
void __attribute__((interrupt, no_auto_psv)) _AddressError (void){
    LATE = 0;               //set all porte outputs to 0
    PORTE = 0;              //probably redundant, but I'm still learning this Micro
    PORTBbits.RB1 = 0; //Disable output as quickly as possible.
    PORTBbits.RB6 = 1;//CPU ACT light.
    sysShutdown();      //Shut down everything else.
    StartRecover();     //Attempt a restart.
    INTCON1bits.ADDRERR = 0;
}
void __attribute__((interrupt, no_auto_psv)) _StackError (void){
    LATE = 0;               //set all porte outputs to 0
    PORTE = 0;              //probably redundant, but I'm still learning this Micro
    PORTBbits.RB1 = 0; //Disable output as quickly as possible.
    PORTBbits.RB6 = 1;//CPU ACT light.
    sysShutdown();      //Shut down everything else.
    StartRecover();     //Attempt a restart.
    INTCON1bits.STKERR = 0;
}
void __attribute__((interrupt, no_auto_psv)) _MathError (void){
    LATE = 0;               //set all porte outputs to 0
    PORTE = 0;              //probably redundant, but I'm still learning this Micro
    PORTBbits.RB1 = 0; //Disable output as quickly as possible.
    PORTBbits.RB6 = 1;
    sysShutdown();      //Shut down everything else.
    StartRecover();     //Attempt a restart.
    INTCON1bits.MATHERR = 0;
}
void __attribute__((interrupt, no_auto_psv)) _ReservedTrap7 (void){
    LATE = 0;               //set all porte outputs to 0
    PORTE = 0;              //probably redundant, but I'm still learning this Micro
    PORTBbits.RB1 = 0; //Disable output as quickly as possible.
    PORTBbits.RB6 = 1;//CPU ACT light.
    sysShutdown();      //Shut down everything else.
    //StartRecover();     //Attempt a restart.
}
/****************/
/* END IRQs     */
/****************/   

#endif