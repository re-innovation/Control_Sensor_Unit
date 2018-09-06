# RGBsensor
This is a colour sensor using an Arduino Nano.

This was developed to control videos on a LED projector.

It connects via the USB serial line to the projector, where it gets its power and does comms.

The unit uses a TCS3200 RGB colour sensor. The following article and libraries were used:

https://howtomechatronics.com/tutorials/arduino/arduino-color-sensing-tutorial-tcs230-tcs3200-color-sensor/

This is connected to a switch which is triggered when pushed and then reads the RGB value and returns it via the serial line.


