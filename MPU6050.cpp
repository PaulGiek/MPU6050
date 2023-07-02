#include "MPU6050.h"

#include "Arduino.h"
#include "Wire.h"

struct IMUPackage {
  float temperature;
  
  float accelX;
  float accelY;
  float accelZ;

  float totalAccelVector;
  
  float rotX;
  float rotY;
  float rotZ;

  bool gyroValues;
};

bool MPU6050::begin() {
  Wire.begin();

  Wire.beginTransmission(mpuAdress); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
  Wire.beginTransmission(mpuAdress); //I2C address of the MPU
  Wire.write(gyroConfig); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(GYRO_MIN_FSR_250); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(); 
  Wire.beginTransmission(mpuAdress); //I2C address of the MPU
  Wire.write(accelConfig); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(ACCEL_NORMAL_FSR_8G); //Setting the accel to +/- 8g and activate Selftest
  //Bits :     76543210
  Wire.endTransmission();

  Wire.beginTransmission(mpuAdress); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x75); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.endTransmission();
  
  Wire.requestFrom(mpuAdress, 1);
  uint8_t res = Wire.read();
  if(res == mpuAdress) {
    IMUInitSC = true;
  }
  else {
    IMUInitSC = false;
  }
  
  //measureVehicleOffset();
  
  return IMUInitSC;
}

void MPU6050::readRawData() { 
  Wire.beginTransmission(mpuAdress); //I2C address of the MPU
  Wire.write(0x3B); //Starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission();
  Wire.requestFrom(mpuAdress,7*2); // request a total of 7(Values)*2(two bites per register)=14 registers
  while(Wire.available() < 14); //Wait for all the data to arive
  accelRawX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelRawY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelRawZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accel
  
  temperatureRaw = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)

  gyroRawX = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyroRawY = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyroRawZ = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
}

void MPU6050::setMeasurementMode(bool gyroNew) {
  gyro = gyroNew;
}

IMUPackage MPU6050::getMeasurement() {
  float dt = (millis() - millisOld)/1000.0;
 
  //Get the raw data
  readRawData();

  //Convert raw values to filtered values (currently no filtering implemented)
  //Devide by these values acording to the resolution mode (->check datasheet)
  float accelX = ((float)accelRawX /  ACCEL_NORMAL_FSR_8G_BPU) * -1;
  float accelY = (float)accelRawY / ACCEL_NORMAL_FSR_8G_BPU;
  float accelZ = (float)accelRawZ / ACCEL_NORMAL_FSR_8G_BPU;
  
  float gyroX = (float)gyroRawX / GYRO_MIN_FSR_250_BPU;
  float gyroY = (float)gyroRawY / GYRO_MIN_FSR_250_BPU;
  float gyroZ = (float)gyroRawZ / GYRO_MIN_FSR_250_BPU;

  //Drift is given in deg/s
  gyroX -= gyroDriftX;
  gyroY -= gyroDriftY;
  gyroZ -= gyroDriftZ;

  //Calculate temperature
  //Formula was taken from the Datasheet
  float temperature = temperatureRaw / 340.00 + 36.53;

  //Calculate the magnitude of the acceleration Vector
  totalAccelVector = sqrtf((accelX*accelX) + (accelY * accelY) + (accelZ * accelZ));

  //Get the Sign of the acceleration Vector
  int sign = 1;
  if (accelX + accelY + accelZ < 0) {
    sign = -1;
    totalAccelVector *= sign;
  }
  else ; //Nothing to do

  //Crate the package that will hold and return all values
  IMUPackage package;

  if (gyro == 1) {
    //Integrate the gyro measurements on to the rotation measurements
    rotX += gyroX * dt;
    rotY += gyroY * dt;
    rotZ += gyroZ * dt;
     
    package = {
      temperature,
  
      accelX,
      accelY,
      accelZ,
      totalAccelVector,
  
      rotX,
      rotY,
      rotZ,
      1
    };
  }

  else {
    //rotX is just Integrated like the other measurements because the rotation of the axcis the vehicle stands in can't be calculated that way    
    rotX += gyroX * dt;
    
    //Limit the accel measurements 
    accelY = constrain(accelY, -1, 1);
    accelZ = constrain(accelZ, -1, 1);

    //Shift the buffers
    for (int i = accelBufferLength - 2; i >= 0; i--) {
      bufY[i + 1] = bufY[i];
      bufZ[i + 1] = bufZ[i];
    }

    //Caculate the other Y/Z rotation using the accelerometer:
    float newRotY = asin(accelY);
    float newRotZ = asin(accelZ);

    //Insert new Values
    bufY[0] = newRotY;
    bufZ[0] = newRotZ;

    float ySum = 0.0;
    float zSum = 0.0;

    //High Pass filter
    for (int i = 0;i < accelBufferLength;i++) {
      ySum += bufY[i];
      zSum += bufZ[i];
    }

    rotY = ySum/accelBufferLength;
    rotZ = zSum/accelBufferLength;  

    //Calculated angles from rad to degrees
    rotY *= rad2deg;
    rotZ *= rad2deg;

    //Subtract the sensor offset
    //Not subtracting the vehicle offset b.c. the accelerometer measures this so its already included
    rotY -= sensorOffsetY;
    rotZ -= sensorOffsetZ;

    package = {
      temperature,
  
      accelX,
      accelY,
      accelZ,
      totalAccelVector,
  
      rotX,
      rotY,
      rotZ,
      0
    };
  }

  if (debug) {
    Serial.print(package.temperature);
    Serial.print(",");
    Serial.print(package.accelX);
    Serial.print(",");
    Serial.print(package.accelY);
    Serial.print(",");
    Serial.print(package.accelZ);
    Serial.print(",");
    Serial.print(package.rotX);
    Serial.print(",");
    Serial.print(package.rotY);
    Serial.print(",");
    Serial.print(package.rotZ);
    Serial.println(",");
    //Serial.println(package.gyroValues);
  }

  millisOld = millis();  

  return package;
}

void MPU6050::measureVehicleOffset() {
  float calCount = 2000;
  
  float offsetX = 0;
  float offsetY = 0;
  float offsetZ = 0;
  
  for (int i = 0; i < calCount; i++) {
    IMUPackage package = getMeasurement();
    offsetX += package.rotX;
    offsetY += package.rotY;
    offsetZ += package.rotZ;
  }

  offsetX /= calCount;
  offsetY /= calCount;
  offsetZ /= calCount;

  vehicleOffsetX = offsetX;
  vehicleOffsetY = offsetY;
  vehicleOffsetX = offsetZ;
}
