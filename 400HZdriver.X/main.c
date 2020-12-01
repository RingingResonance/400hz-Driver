
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

#include <p30f3011.h>
#include <dsp.h>
#include <libpic30.h>
#include <xc.h>

/* Include the Source AND Headers HERE or else mplab gets pissy for some reason.
 * Don't bother trying to include the source and header files into the project,
 * it doesn't work. Either I'm doing it wrong or it's broken, but what I have here WORKS SO DON'T TOUCH IT.
 */

#include "chipconfig.h"
#include "IRQs.h"
#include "IRQs.c"
#include "Init.h"
#include "Init.c"
#include "OSC.c"
#include "OSC.h"

/* Program Start */
/***********************************************************
***********************************************************/
int main(void){
    waveType = wsine;
    Init();             //Initialize vars.
    waveSet(waveType, 400);   //Set wave type and frequency standard.
    StartRecover();     //Start systems.
/*****************************/
    // Main Loop.
/*****************************/
    /* Everything is run using IRQs from here on out.
     * Look in the other source files that MPLAB doesn't open automatically.
     */
    for (;;)        //loop forever. or not, idc, we have IRQs for stuff.
    {
        PORTBbits.RB6 = 0;  //CPU ACT light off.
        Idle();                 //Idle Loop, saves power and reduces heat.
    }
    return 0;
}