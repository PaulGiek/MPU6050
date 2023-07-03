# MPU6050
This is an Arduino libary for the MPU6050 Sensor

You can utilize this libary in two ways:
1. Install it just like a nowmal Arduino libary (not recommended)
2. Drag the MPU6050.h and the MPU6050.cpp file into your project folder (recommended -> configuration is way easyer this way, since there is no runtime configurtation to avoid runtime errors)

This libary has two measuremetn modes:
1. Measuring rotation along the X, Y, Z accxis using the Accelerometer 
    -> The intended use case are static Applications that don't expirence a lot af acceleration. Using this measurement mode on for example a             quadrocopter isn't going to work

2. Measuring using the Gyroscope. Prone to drift over longer times. Works in acccelrated envoirments like Boats, Planes, Drones ...

Before Use:
1. Select the measurement mode by setting bool gyro = 0 (in MPU6050.h) to 1/0 
2. Run the included "Drift_MPU6050.ino" script to measure your sensors gyro drift and copy paste the results into MPU6050.h
3. Configure the libary to your needs by choosing the Accelerometer and Gyroscope Full Scale Range

How to Use:
1. Crate an instance if the "MPU6050" class like this: MPU6050 sensor
2. Call sensor.begin() in the setup() function
3. Call sensor.getMeasurement() and store the Results in a IMUPackage like this:  IMUPackage imuData = imu.getMeasurement();
4. Acces specific values of the Package "imuData" like this: imuData.temperatue;
   
Please direct issues or Questions to erow.wue@gmail.com
