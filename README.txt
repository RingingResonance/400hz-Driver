
Firmware and MPLAB project files located in  '400HZdriver.X'

Schematic located in '400hzDriver'

PDF schematic is also in the driectory '400hzDriver'

The current firmware starts the ciruit off at 60hz for a few seconds to give the user time to reset the GFCI if needed. It will then ramp up to 400hz.
Choose parts based on your (mains voltage * 4) you plan on using.

I've recently upgraded the firmware so that it is easier to change parameters such as output frequency, PWM frequency, and wave type. You must still edit the code and recompile to make these changes. Later I plan on adding dip switches or jumpers for these configurations. However, I may plan on making this a full inverter drive/controll system so for now I'm leaving plenty of I/O for future options.

The IR2104 doesn't have the current driving capability for fast PWM. I have changed the schematic to use asymetric gate driving to help the mosfets turn off before the 500ns deadtime is up on the IR2104. Use appropriate PNP transistors and resistors for this part of the circuit.
Don't oversize your fuses!
Circuit diagram was made AFTER I built the circuit. It may not be exact to what I have built and needs to be thorughly looked at before attemting to build. You have been warned! Twice!
The ENTIRE circuit is live when plugged in, and the output has no neutral, they are both HOT. You have been warned, thrice!

Not for beginners. Build at your own risk!
Enjoy your flickerless LED xmass lights!

I'm still experimenting with different fets and with the asymmetric gate drive. I'm not going to put down any part numbers until I have something that works well.

Also, I have the firmware currently set to produce a sine wave with a PWM frequency of 50khz. This is currently not working presumably due to the low pass filter arrangment.

If you decide to use this in it's current state then I recomend you put it back to square wave mode and exclude the output filter unless you know more about electronics than I do and can spot my mistakes ;)