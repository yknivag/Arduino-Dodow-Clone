## Dowdow Arduino Clone for sleep meditation. ##
Based on https://github.com/gary909/Dodow-Clone
 
The purpose of the Dowdow is to progessively slow breathing, the user breathes in as the light gets brighter and out as it dims.  Each breath gets slightly longer until the target respiration rate is acheived.

The commercial Dowdow device slows the breathing from 11 breaths per minute to 6 breaths per minute over a period of 8 minutes.

This clone allows the starting frequency and the duration of the slow down to be set with two potentiometers.

Breathing is reduced over "DEFAULT_REDUCING_TIME_PC"% of the selected run time and the remaining time is constant at the target rate.

Default frequencies, run-time and extended-time can be set sing the defines at the head of the sketch.

# Wiring #

- To use the variable start frequency; a potentiomenter should be connected with one side to 5V, the other side to GND and the middle to which ever pin is defined in "INITIAL_FREQUENCY_PIN".  Should this feature not be required, comment out the #define the pin.

- To use the variable target frequency; a potentiomenter should be connected with one side to 5V, the other side to GND and the middle to which ever pin is defined in "TARGET_FREQUENCY_PIN".  Should this feature not be required, comment out the #define the pin.

- To use the variable run-time; a potentiomenter should be connected with one side to 5V, the other side to GND and the middle to which ever pin is defined in "DURATION_PIN".  Should this feature not be required, comment out the #define the pin.

- A switch can be added to pin "NIGHT_MODE_ENABLE_PIN" which will reduce the brightness of the LED by a percentage set in the #define "NIGHT_MODE_PC".  If no switch is to be connected, comment out this #define.

- An LED (with appropriate resistor) should be connected to the pin set in "LED_PIN" (this must be a PWM output, labelled as ~ on most Arduino boards).  If using more than one LED for brightness then connect this pin to a transistor or mosfet and connect the LEDs to that.
