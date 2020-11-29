/* 
 * File:   OSC.h
 * 400HZ xmass light anti-flicker driver.
 * Author: Jarrett Cigainero
 *
 * Created on November 24, 2020, 11:53 AM
 */

#ifndef OSC_H
#define	OSC_H

extern void OSCIntToExt(void);
extern void OSCExtToInt(void);
extern void sysShutdown(void);
extern void StartRecover(void);

#endif	/* OSC_H */

