#include <Arduino.h>
#include <Wire.h>

/*
  This class represents a single MPU-6050 multi-axis sensor.
  These are used in single pattern on the individual fingers of a glove.
  Using this class we are able to extract the necessary sensory data.
*/
class mpu
{
private:
  // The sensor's I2C slave device address.
  const uint8_t MPU6050SlaveAddress = 0x68;

  // Sensitivity scale factor respective to full scale setting provided in datasheet.
  const uint16_t AccelScaleFactor = 16384;
  const uint16_t GyroScaleFactor = 131;

  // Few configuration register addresses. (More on them in the datasheet!)
  const uint8_t MPU6050_REGISTER_SMPLRT_DIV = 0x19;
  const uint8_t MPU6050_REGISTER_USER_CTRL = 0x6A;
  const uint8_t MPU6050_REGISTER_PWR_MGMT_1 = 0x6B;
  const uint8_t MPU6050_REGISTER_PWR_MGMT_2 = 0x6C;
  const uint8_t MPU6050_REGISTER_CONFIG = 0x1A;
  const uint8_t MPU6050_REGISTER_GYRO_CONFIG = 0x1B;
  const uint8_t MPU6050_REGISTER_ACCEL_CONFIG = 0x1C;
  const uint8_t MPU6050_REGISTER_FIFO_EN = 0x23;
  const uint8_t MPU6050_REGISTER_INT_ENABLE = 0x38;
  const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H = 0x3B;
  const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET = 0x68;

  /*
    This method is responsible for writing to the sensor's registers in an encapsulated form.
    It does this so using the previously defined register addressis in it's parameters and by utilizing the I2C protocol.
  */
  void writeRegister(uint8_t deviceAddress, uint8_t regAddress, uint8_t data)
  {
    Wire.beginTransmission(deviceAddress);
    Wire.write(regAddress);
    Wire.write(data);
    Wire.endTransmission();
  }

  /*
    This method initialises the sensor by initially configuring the needed sensor's registers.
    It does this via writing trought the I2C protocol for the given configuration registers.
    (More information can be found in the MPU-6050 manufactor's datasheet about the registers!)
  */
  void initSensor()
  {
    Wire.begin(sda, scl);
    delay(150); // Making sure the I2C connection has been initiated...
    writeRegister(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
    writeRegister(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
    writeRegister(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
    writeRegister(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
    writeRegister(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);  // Currently set +/-250 degree/second full scale
    writeRegister(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00); // Currently set +/- 2g full scale
    writeRegister(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
    writeRegister(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
    writeRegister(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
    writeRegister(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
  }

public:
  // Variables for the SDA and SCL pins used during the I2C communication
  uint8_t scl;
  uint8_t sda;

  // The public variables with the readable sensory data.
  double Ax;
  double Ay;
  double Az;
  double Gx;
  double Gy;
  double Gz;

  // The variables that will contain the sensory data.
  int16_t AccelX;
  int16_t AccelY;
  int16_t AccelZ;
  int16_t GyroX;
  int16_t GyroY;
  int16_t GyroZ;

  /*
    The MPU-6050's constructor that is used to initiate I2C communication for the sensor.
    In it's parameters the desired SCL and SDA pins have to be set accordingly.
  */
  mpu(uint8_t _scl, uint8_t _sda)
  {
    scl = _scl;
    sda = _sda;
    initSensor();
  }

  /*
    This method gets the raw values from the sensor.
    In it's sensory data variables - Accel & Gyro - will have the raw readings from the sensors corresponding registers.
    (It however will still need further formating in order to have a usable and readable sensory data!)
  */
  void getValues()
  {
    // Reading the output register to get the raw sensory data.
    Wire.beginTransmission(MPU6050SlaveAddress);
    Wire.write(MPU6050_REGISTER_ACCEL_XOUT_H);
    Wire.endTransmission();
    Wire.requestFrom(MPU6050SlaveAddress, (uint8_t)14);

    // Reading the raw sensory data into the respective variables.
    AccelX = (((int16_t)Wire.read() << 8) | Wire.read());
    AccelY = (((int16_t)Wire.read() << 8) | Wire.read());
    AccelZ = (((int16_t)Wire.read() << 8) | Wire.read());
    GyroX = (((int16_t)Wire.read() << 16) | Wire.read());
    GyroY = (((int16_t)Wire.read() << 8) | Wire.read());
    GyroZ = (((int16_t)Wire.read() << 8) | Wire.read());
  }

  /*
    This method returns the readings as formatted comma separated values.
    It is needed so that later we can easily get the data for the model training.

    (Note that right now this one is not used because of the planned Kalman filter implementation!)
  */
  void printData()
  {
    Serial.printf("%d,%d,%d,%d,%d,%d,", Ax, Ay, Az, Gx, Gy, Gz);
  }

  /*
    This method returns the raw readings as formatted comma separated values.
    It is needed so that later we can easily get the data for the model training.
  */
  void printRawData()
  {
    Serial.printf("%d,%d,%d,%d,%d,%d,", AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ);
  }

  /*
    This method represents the end of a series of sensory data collection.
    In reality all it does is to write a line break to the serial connection
    so that new data will begin in a new clean line.

    It is important so that we can extract data precisely from the serial monitor.
    (Also later on it will be used to determine when a sign has come to an end.)
  */
  static void endOfData()
  {
    Serial.println();
  }
};