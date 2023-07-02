#ifndef MPU6050_h
#define MPU6050_h

#include "Package.h"

class MPU6050 {
  public:  
    bool begin();
    IMUPackage getMeasurement();

    //1 Means Gyro Measurements, 0 Accelerometer
    void setMeasurementMode(bool gyroNew);

    bool IMUInitSC;

  private:   
    void readRawData();
    
    //Gyro and Accelerometer Config
    uint8_t GYRO_MIN_FSR_250 = 0x11100000;
    uint8_t GYRO_LOW_FSR_500 = 0x11101000;
    uint8_t GYRO_NORMAL_FSR_1000 = 0x11110000;
    uint8_t GYRO_HIGH_FSR_2000 = 0x11111000;

    float GYRO_MIN_FSR_250_BPU = 131.0; 
    float GYRO_LOW_FSR_500_BPU = 65.5;
    float GYRO_NORMAL_FSR_1000_BPU = 32.8;
    float GYRO_HIGH_FSR_2000_BPU = 16.4;

    uint8_t ACCEL_MIN_FSR_2G = 0b00000000; 
    uint8_t ACCEL_LOW_FSR_4G = 0b00001000;
    uint8_t ACCEL_NORMAL_FSR_8G = 0b00010000;
                                  //76543210
    uint8_t ACCEL_HIGH_FSR_16G = 0b00011000;
    
    float ACCEL_MIN_FSR_2G_BPU = 16384; 
    float ACCEL_LOW_FSR_4G_BPU = 8192;
    float ACCEL_NORMAL_FSR_8G_BPU = 4096;
    float ACCEL_HIGH_FSR_16G_BPU = 2048;

    bool debug = 0;

    bool gyro = 0;

    uint8_t mpuAdress = 0x68;
    uint8_t gyroConfig = 0x1B;
    uint8_t accelConfig = 0x1C;

    //variables for Raw-Orientation
    int16_t gyroRawX, gyroRawY, gyroRawZ;
    int16_t accelRawX, accelRawY, accelRawZ;

    //Output variables
    float rotX, rotY,rotZ;

    //Buffers for accel based orientation
    static const int accelBufferLength = 50;
    float bufY[accelBufferLength];
    float bufZ[accelBufferLength];

    //Drift in deg/sec
    float gyroDriftX = -7.50;
    float gyroDriftY = -0.29;
    float gyroDriftZ = 0.12;

    //Variables for temperature
    int16_t temperatureRaw;

    //Timing Variables
    unsigned long millisOld;

    float totalAccelVector = -1.0;

    //Offset Variables (Sensor Offset needs to be calibrated before flight)
    float sensorOffsetX = 0;
    float sensorOffsetY = 0;
    float sensorOffsetZ = 0;

    float vehicleOffsetX = 0;
    float vehicleOffsetY = 0;
    float vehicleOffsetZ = 0;
};

#endif
