#include<Wire.h>
#include <MIDI.h>
#include <NewPing.h>
MIDI_CREATE_DEFAULT_INSTANCE();

// VARIABLES

int ccGyroX = 0;
int ccGyroY = 0;
int ccGyroZ = 0;
int ccPot = 0;
int ccUltra = 0;

int ultraSwitch = 0;
int AcXSwitch = 0;
int AcYSwitch = 0;
int AcZSwitch = 0;

int lastUltraCC = 0;
int lastAcXCC = 0;
int lastAcYCC = 0;
int lastAcZCC = 0;

NewPing sonar(12,11,30);  // (Trig pin, Echo ping, Max distance in cm)
const int MPU=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  MIDI.begin(4);
  pinMode(7,INPUT); //Ultrasonic Switch
  pinMode(10,INPUT); // AcX Switch
  pinMode(9,INPUT); //AcY Switch
  pinMode(8,INPUT); //AcZ Switch
}

void loop(){
  
  //GATHERING VALUES FROM GY-521
  
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  //GYROSCOPE/ACCELEROMETER
  
  //X-AXIS  
  AcXSwitch = digitalRead(10);
  ccGyroX = map(AcX, -17000, 17000, 0, 127);
  if (AcXSwitch == HIGH){
    MIDI.sendControlChange(23,ccGyroX,1);
    lastAcXCC = ccGyroX;
  }
  if (ccGyroX == 0){
  MIDI.sendControlChange(23,lastAcXCC,1);
  }
  
  //Y-AXIS
  AcYSwitch = digitalRead(9);
  ccGyroY = map(AcY, -17000, 17000, 0, 127);
  
  if (AcYSwitch == HIGH){
    MIDI.sendControlChange(118,ccGyroY,1);
    lastAcYCC = ccGyroY;
  }
  if (ccGyroY == 0){
  MIDI.sendControlChange(118,lastAcYCC,1);
  }
  
  //Z-AXIS
  AcZSwitch = digitalRead(8);
  ccGyroZ = map(AcZ, -17000, 17000, 0, 127);
  
  if (AcZSwitch == HIGH){
    MIDI.sendControlChange(87,ccGyroZ,1);
    lastAcZCC = ccGyroZ;
  }
  if (ccGyroZ == 0){
    MIDI.sendControlChange(87,lastAcZCC,1);
  }
    
  // ULTRASONIC SENSOR
  
  ultraSwitch = digitalRead(7);
  int uS = sonar.ping();
  ccUltra = map(uS, 0, 1700, 0, 127);
  
  if ((ultraSwitch == LOW) && (lastUltraCC == 0)){
    
  }
  
  if (ultraSwitch == LOW && lastUltraCC >= 1){
    MIDI.sendControlChange(107,lastUltraCC,1);
    
  }
  
  if (ultraSwitch == HIGH){
    MIDI.sendControlChange(107,ccUltra,1);
    lastUltraCC = ccUltra;
  }
}
