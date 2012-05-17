* QuadVolucer
=============

QuadVolucer is a Quadcopter project basend on the STM32 Microcontroller. It uses a ITG-3200 3-axis gyroskope.

The aim of this project is not to develop a Copter which is as stable as possible but to leave control to the pilot. Therefore control is HH only.
To archive this no ACCs are needed. There is a footprint for a 3-axis ACC on the PCB though. So angular control could be implemented but I'm propably not going to do so.


* canBLC
========

canBLC is a motor controller for BLDC motors. It is meant to be used with multicopters like the QuadVolucer. As an interface it will support I2C, PWM and of cause high speed CAN.

	status:	- Hardware design is finished 
		- Software basic features working (only CAN support)

	todo:	- extensive testing
		- testing on the QuadVolucer :-)
		- implementation of PWM and I2C interface.
