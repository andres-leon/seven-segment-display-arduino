# seven-segment-display-arduino
seven-segment-display-arduino
this code is a modification from the code found at https://medium.com/@jonah.lefkoff/how-to-hook-up-the-sparkfun-7-segment-display-to-a-raspberry-pi-577591ba94b5 Thank you Jonah for your work on this!

The segment display used is this one https://www.sparkfun.com/products/8530

My code simply blanks out the display when non integers are sent. It also shows single digits without leading zeroes. So when the serial receives the number 8, the display will show only 8 and not 08.

hope this helps someone out there! I'm sure there are better ways of implementing this, but so far this has worked for me :) Enjoy!
