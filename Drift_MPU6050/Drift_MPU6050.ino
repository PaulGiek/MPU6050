#include "Wire.h"

//Variables for Orientation
int16_t gyroRawX, gyroRawY, gyroRawZ;
int16_t accelRawX, accelRawY, accelRawZ;

//Variables for temperature
int16_t temperatureRaw;


void begin() {
  Wire.begin();
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(); 
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00010000); //Setting the accel to +/- 8g and activate Selftest
  //Bits :    76543210
  Wire.endTransmission();
  
  //measureVehicleOffset();
}

void readRawData() { 
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,7*2); // request a total of 7*2=14 registers
  while(Wire.available() < 14);
  accelRawX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelRawY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelRawZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accel
  
  temperatureRaw = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)

  gyroRawX = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyroRawY = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyroRawZ = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  begin();
}

void loop() {
  float gyroDriftX = 0.0;
  float gyroDriftY = 0.0;
  float gyroDriftZ = 0.0;
  
  for (int i = 0; i < 10000; i++) {
    readRawData();
    gyroDriftX += (float)gyroRawX / 131.0;
    gyroDriftY += (float)gyroRawY / 131.0;
    gyroDriftZ += (float)gyroRawZ / 131.0;
    
    Serial.println("Drift-X: " + (String)((float)gyroRawX / 131.0) + " Drift-Y: " + (String)((float)gyroRawY / 131.0) + " Drift-Z: " + (String)((float)gyroRawZ / 131.0));
  }

  gyroDriftX /= 10000.0;
  gyroDriftY /= 10000.0;
  gyroDriftZ /= 10000.0;

  //Print Values and wait forever
  Serial.println("===================RESULT==================");
  Serial.println("Drift-X: " + (String)gyroDriftX + " Drift-Y: " + (String)gyroDriftY + " Drift-Z: " + (String)gyroDriftZ);
  while(1);
}
