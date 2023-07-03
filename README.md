# MPU6050
This is an Arduino libary for the MPU6050 Sensor

You can utilize this libary in two ways:
1. Install it as standat Arduino libary
2. Drag the MPU6050.h and the MPU6050.cpp file into your project folder (recommended)

Before use please run the included "Drift_MPU6050.ino" script to measure your sensors gyro drift and copy paste the results into MPU6050.h

This libary has two measuremetn modes:
1. Measuring rotation along the X, Y, Z accxis using the Accelerometer 
    -> The intended use case are static Applications that don't expirence a lot af acceleration. Using this measurement mode on for example a             quadrocopter isn't going to work

2. Measuring using the Gyroscope. Prone to drift over longer times. Works in acccelrated envoirments like Boats, Planes, Drones ...
   
Select the measurement mode by setting bool gyro = 0 (in MPU6050.h) to 1/0 
Please direct issues to erow.wue@gmail.com
