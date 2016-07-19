# GE1111-Arduino-Alarm
Arduino machine code for the intrustion alarm system in Northeastern's GE1111 class

# Description of States
1.	Status: Unarmed: LCD displays “system ready” if all doors are shut and the system is at a standstill (no change in the lighting/motion). LED is off, and verify that alarm is off.  Continuously (while loop) check to see if there is trouble (motion/door opened) and check the temperature is within range.  Trouble is detected using a separate function, checktrouble, which verifies the status of the actual board sensors.

2.	Status: Trouble: This status occurs when unarmed, since homeowner is present and can deal with problem. If the there is a change in the lighting or a door is open, then a solid red LED will turn on, and the LCD will display “TROUBLE”. Continuously (while loop) check if there is still trouble or if temperature is out of range.

3.	Status: Alarm: When armed and any trouble detected (i.e. door open or motion is detected), the alarm will sound and it is accompanied by a solid red light.  Sets an alarm variable to true and calls the checkCode function until the function resets the alarm variable back to false.  The checkCode function requires the secret code to return false.

4.	To arm, the user presses the A button on the keypad. The user has 10 seconds to leave the building while the LCD displays “Arming…Exit within 10s” (eg., Any trouble detected is ignored for 10 seconds).  When armed, the LCD reads “ARMED” and the LED flashes.  The arming action is achieved using an if statement, checking to verify that the system is not armed nor detecting trouble.  Then, it resets all of the secret code variables to false and armed to true after the 10 second delay.

5.	After a door sensor is tripped or motion is detected, the user has 10 seconds to enter the secret code to disarm. If it is not disarmed within 10 seconds, the alarm will go off and the user must enter the code to disable.  This is written into the checkCode function, which uses a while loop with a delay to constantly check another function to see if the code is correct.  Otherwise, the while loop expires and another is entered and cannot be broken until code is correct.  After code is correct, it resets the alarm variable to false and enters the unarmed loop or trouble loop, depending upon whether the trouble has ended.

6.	If the temperature is out of its acceptable range in any status, then the user has 10 seconds to disarm the device or return the temperature back to the acceptable range. If not, then the alarm will go off. The alarm can be silenced automatically if the temperature is moved back into the acceptable range.  This process uses a separate function that again implements the same checkCode function in a different manner.
