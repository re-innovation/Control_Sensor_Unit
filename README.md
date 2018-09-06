# Control_Sensor_Unit

This is a sensor unit using an Arduino Nano. It is designed to be able to read either a PIR, a TCS3200 colour sensor or a weight sensor. This is for making interactive art projects to control the projection unit.

This was developed to control videos on a LED projector.

It connects via the USB serial line to the projector, where it gets its power and does comms.

The unit uses a TCS3200 RGB colour sensor. The following article and libraries were used:

https://howtomechatronics.com/tutorials/arduino/arduino-color-sensing-tutorial-tcs230-tcs3200-color-sensor/

This is connected to a switch which is triggered when pushed and then reads the RGB value and returns it via the serial line.


