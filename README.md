MPU6050 Arduino Library
This is an Arduino library for the MPU6050 sensor.

Usage
You can use this library in two ways:

(Not recommended) Install it like a standard Arduino library.

(Recommended) Copy MPU6050.h and MPU6050.cpp directly into your project folder.
→ This makes configuration easier, as there's no runtime configuration (avoiding potential runtime errors).

Measurement Modes
This library supports two measurement modes:

Accelerometer-based orientation (rotation along the X, Y, Z axes):
Best suited for static applications that don’t experience much acceleration.
⚠️ Not suitable for dynamic applications like quadcopters, as external forces affect readings.

Gyroscope-based orientation:
Works well in moving or accelerated environments such as drones, planes, or boats.
⚠️ Prone to drift over time, so periodic calibration may be necessary.

Setup Instructions
Before using the library:

Select the measurement mode by setting bool gyro = 0 in MPU6050.h to either 1 (gyro) or 0 (accelerometer).

Run the included Drift_MPU6050.ino script to measure your sensor’s gyro drift.
Copy the resulting values into MPU6050.h.

Configure the accelerometer and gyroscope full-scale range according to your needs.

How to Use
Create an instance of the class like this:

cpp
Copy
Edit
MPU6050 sensor;
Call sensor.begin() inside your setup() function.

Get sensor data:

cpp
Copy
Edit
IMUPackage imuData = sensor.getMeasurement();
Access individual values, e.g.:

cpp
Copy
Edit
imuData.temperature;
Change measurement mode at runtime (optional):

cpp
Copy
Edit
sensor.gyro = 1; // or 0
For questions or issues, please contact: erow.wue@gmail.com
