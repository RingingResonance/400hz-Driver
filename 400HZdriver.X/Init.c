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

#ifndef INIT_C
#define INIT_C


#include <p30f3011.h>
#include <dsp.h>
#include "Init.h"
#include "OSC.h"

void Init(void){
/*******************************
 * initialization values setup.
*******************************/
    /* Analog inputs and general IO */
    TRISB = 0x0000;              //set portb to mix analog inputs and digital outputs.
    LATB = 0;               //clear portb

    /**************************/
    /* PWM outputs. */
    if (waveType == wsine)
        TRISE = 0xFFFF;              //set porte to all inputs.
    else{
        TRISE = 0x0000;              //set porte to all outputs.
        LATE = 0;               //set all porte outputs to 0
    }
    /**************************/
    /* General IO */
    /* Pins 27 and 28 should be reserved for AUX com on UART2. What was this for again? pins 27/28 are not any kind of serial comm pins??? */
    TRISF = 0xFFBE;              //set portf to all inputs and two output.
    LATF = 0;
    /**************************/
    /* General IO. */
    TRISD = 0xFFF3;              //set portd to all inputs except for RD2 and RD3 which is the KEEPALIVE and mShutdown signals.
    //LATD = 0;
    /**************************/
    /* Brake Light Output */
    TRISC = 0x0000;
    LATC = 0;
/*****************************/
/* CPU and DSP config */
/*****************************/
    CORCONbits.US = 1; //USE UNSIGNED MATH FOR DSP
    CORCONbits.IF = 1; //USE INTERGER MATH FOR DSP
    
    //SATURATION ENABLE
    CORCONbits.SATA = 1;
    CORCONbits.SATB = 1;
    CORCONbits.SATDW = 1;
    
    //ROUND CONVENTIONAL 
    CORCONbits.RND = 1;
    
/*****************************/
/* Configure PWM */
/*****************************/
    PTCON = pwmFreq50K;     //Set the PWM module pre-scale. Init.h has preset values.
    PTMR = 0;
    PTPER = 0xC8;         //set period to 0xC8, 400 steps of resolution when set to 200(0xC8)???
                          //This happens because PTPER is compared to only the upper 15 bits on PDC, effectively making a 15bit MAX PWM resolution.
    SEVTCMP = 0;
    if (waveType == wsine)
        PWMCON1 = 0x00FF;       //Set PWM output for complementary mode. (sine wave))
    else
        PWMCON1 = 0x0000;       //Set PWM output for general IO (square wave))
    PWMCON2 = 0x0000;       //4 for unsync
    DTCON1 = 0;
    FLTACON = 0;
    OVDCON = 0xFF00;
    PDC1 = 0000;            //set output to 0
    PDC2 = 0000;            //set output to 0
    PDC3 = 0000;            //set output to 0

/*****************************/
/* Configure UARTs */
/*****************************/
    //PORT 1 setup
    U1STA = 0;
    U1MODE = 0;
    //U1BRG = BaudCalc(BAUD1, IPS);     //calculate the baud rate.
    U1MODEbits.ALTIO = 1;           //Use alternate IO for UART1.
    //Default power up of UART should be 8n1

    //PORT 2 setup
    U2STA = 0;
    U2STAbits.UTXISEL = 1;
    U2MODE = 0;
    //U2BRG = BaudCalc(BAUD2, IPS);     //calculate the baud rate.
    //Default power up of UART should be 8n1
/*****************************/
/* Configure Timer 1 */
/*****************************/
/* Clock Fail recovery system. */
    PR1 = 7812;                 //7812 should give us just less than 1/10th of a second IRQ at 10mhz
                                //976 should give us around the same time when running on FRC.
    TMR1 = 0x0000;
    T1CON = 0x0000;
    T1CONbits.TCKPS = 3;        //1:256 prescale

/*****************************/
/* Configure Timer 2 */
/* 400hz generation */
/*****************************/
    //with 7.37 mhz clock and 8xPLL 92.125 gives 400 hz (92 gives 400.5435 HZ)
    //with 10 mhz clock and 8xPLL 125 gives exactly 400 hz
    /*if (waveType == wsine){
        PR2 = 833;       //833 is for 60hz. 250 for a 400hz output.
        slowStart = 833;
    }
    else{
        PR2 = 3333;       //square wave starting frequency
        slowStart = 3333;
    } Redundant. we set these vars up in StartRecover() */
    //Set to 494 for 400hz square wave soft start.
    //Will the CPU have enough time for this? Even with optimizations?
    //If not then cut the number of points in lookup table in half and start from there.
    TMR2 = 0x0000;
    T2CON = 0x0000;
//    T2CONbits.TCKPS = 2;        //1:64 prescale

/*****************************/
/* Configure Timer 3 */
/*****************************/
/* For 1 second timing operations. */
    PR3 = 0xE0EA;   //57,578
    //PR3 = 0x7271;     //29,297
    TMR3 = 0x0000;
    T3CON = 0x0000;
    T3CONbits.TCKPS = 3;        //1:256 prescale

/*****************************/
/* Configure analog inputs */
/*****************************/
    //ADCON3upper8 = 0x0F;
    //ADCON3lower8 = 0x0F;
    ADCON1 = 0x02E4;
    ADCON2 = 0x0410;
    ADCON3 = 0x0F0F;    //0x0F0F;
    ADCHS = 0x0000;
    ADPCFG = 0xFF7F;
    ADCSSL = 0x008F;

/*****************************/
/* Configure IRQs. */
/*****************************/
    //Configure Priorities
    IPC1bits.T2IP = 1;
    IPC4bits.INT1IP = 2;
    IPC0bits.T1IP = 3;
    IPC2bits.ADIP = 4;
    IPC0bits.INT0IP = 5;
    IPC2bits.U1RXIP = 6;
    IPC5bits.INT2IP = 7;
    // Clear all interrupts flags
    IFS0 = 0;
    IFS1 = 0;
    IFS2 = 0;

	// enable interrupts
	__asm__ volatile ("DISI #0x3FFF");
    IEC0 = 0;
    IEC1 = 0;
    IEC2 = 0;
	IEC0bits.T1IE = 1;	// Enable interrupts for timer 1
    //IEC0bits.U1RXIE = 1; //Enable interrupts for UART1 Rx.
    //IEC1bits.U2RXIE = 1; //Enable interrupts for UART2 Rx.
    //IEC1bits.U2TXIE = 1; //Enable interrupts for UART2 Tx.
   // if(EnableChIRQ == 1){
    //    IEC0bits.INT0IE = 1;    //Charge Detect IRQ
   // }
    //EnableChIRQ = 1;    //By default, enable charge detect IRQ on init.
    //IEC1bits.INT1IE = 1;    //Wheel rotate IRQ
    IEC1bits.INT2IE = 0;  //Disable irq for INT2, not used.
    IEC0bits.T2IE = 1;	// Enable interrupts for timer 2
    //IEC0bits.T3IE = 0;	// Disable interrupts for timer 3
    //IEC0bits.ADIE = 1;  // Enable ADC IRQs.
    INTCON2bits.INT0EP = 0;
    INTCON2bits.INT2EP = 0;
DISICNT = 0;
/*****************************/
/* Enable our devices. */
/*****************************/
    //ADCON1bits.ADON = 1;    // turn ADC ON
    if (waveType == wsine)
        PTCONbits.PTEN = 1;     // Enable PWM only when using sine wave output.
    //T2CONbits.TON = 1;      // Start Timer 2
    //T1CONbits.TON = 1;      // Start Timer 1
    //T3CONbits.TON = 1;      // Start Timer 3
    //U1MODEbits.UARTEN = 1;  //enable UART1
    //U1STAbits.UTXEN = 1;    //enable UART1 TX
    //U2MODEbits.UARTEN = 1;  //enable UART2
    //U2STAbits.UTXEN = 1;    //enable UART2 TX
/* End Of Initial Config stuff. */
/* Now do some pre-calculations. */

    //Calculate our voltage divider values.
    /*vltg_dvid = R2_resistance / (R1_resistance + R2_resistance);
    //Calculate our charge/discharge rate.
    calc_125 = 0.125 / 3600;
    //We've done Init.
    init_done = 1;
    //We aren't in low power mode
    lw_pwr = 0;
    //Calculate max charge current.
    max_chrg_current = chrg_C_rating * amp_hour_rating;
    //Send Init message.
    //send_string(NLtxtNL, "Initialized.", PORT1);
*/
}

#endif
