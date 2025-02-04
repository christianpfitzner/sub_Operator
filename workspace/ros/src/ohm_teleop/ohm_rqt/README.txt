Package
=======
The package contains 4 nodes:
	- Clock
	- Horizon
	- Horizon2
	- HealthPlugin
and one message:
	- pu.msg

HealthPlugin uses this message. Therefore it is necessary to build the header pu.h before it can be used by Health Plugin.

During the first Compilation process, an error will appear:
	fatal error: ohm_rqt/pu.h: No such file or directory compilation terminated.

When the package is compiled once again, this error shouldn't occur, as the pu.h header has been built in the compilation process before.

Unfortunately we weren't able to solve this problem more elegantly :-(


nodes:
======


Clock
=====
The Clock plugin is a simple clock which supports the driver in his time management during competitions. Before starting the clock, two signals can be set.
When the clock reaches the time set in signal1, the display will be colored yellow.
When reaching signal2, the display will turn orange.
The clock plugin can be run "standalone".


Horizon / Horizon2
==================
The functionality of these two plugins basically is the same. Horizon2 just extends Horizon with some graphical elements. 
The plugins take messages of the type sensor_msgs::Imu, extract the angles Roll and Pitch and visualizes these angles with a virtual horizon.
Tests with simulated Imu messages have been successful, however, we weren't able to test the plugins with real values.
(note: horizon2 uses two images. The path of these images as to be adaptet to other PCs in WidgetHorizon2.cpp in the paintCursor and paintFrame functions.)


Health Plugin
=============
By now, the health plugin has only been included into the ohm_rqt package based on some groundwork.
The functionality has not been changed and the functionality has NOT been verified yet.
