# Binary Clock
This sketch programs an Arduino to become a digital binary clock! Seconds are represented by
toggling an LED every (you guessed it) second. Minutes and hours are represented as series of LEDs.

This sketch uses pins as follows:
 0 - seconds (overloaded with Rx when the serial pins are set for Debug).
 1 - lowest minute bit
 2 - minute bit
 3 - minute bit
 4 - minute bit
 5 - minute bit
 6 - highest minute bit
 7 - lowest hour bit
 8 - hour bit
 9 - hour bit
 10 - highest hour bit
 11 - button input pin

## The button
The button in this sketch has two modes:
 - short press; which toggles between 11 brightnesses (0 - 100%)
 - long press (and hold); which starts adding a minute to the time for every second after the first
   second that the button is held.
