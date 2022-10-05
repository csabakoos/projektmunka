#include <Arduino.h>
#include <Wire.h>
#include <SimpleKalmanFilter.h>

/*
  This class represents a single MPU-6050 multi-aXis sensor.
  These are used in single pattern on the individual fingers of a glove.
  Using this class we are able to extract the necessary sensory data.
*/
class mpu
{
private:
  // The simplified Kalman filters for our acceleration meter and gyroscope needs.
  SimpleKalmanFilter *accelKalman, *gyroKalman;

  // The sensor's I2C slave device address.
  const uint8_t MPU6050SlaveAddress = 0x68;

  // Sensitivity scale factor respective to full scale setting provided in datasheet.
  const uint16_t accelScaleFactor = 16384;
  const uint16_t gyroScaleFactor = 131;

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

  // Constant for the gravitational acceleration multiplier.
  const double g = 9.80665;

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
  uint8_t scl, sda;

  // The variables with the readable sensory data.
  double aX, aY, aZ, gX, gY, gZ;

  // The Kalman variables with the readable sensory data.
  float kaX, kaY, kaZ, kgX, kgY, kgZ;

  // The variables that will contain the sensory data.
  int16_t accelX, accelY, accelZ, gyroX, gyroY, gyroZ;

  /*
    The MPU-6050's constructor that is used to initiate I2C communication for the sensor.
    In it's parameters the desired SCL and SDA pins have to be set accordingly.
  */
  mpu(uint8_t _scl, uint8_t _sda)
  {
    accelKalman = new SimpleKalmanFilter(10, 10, 1);
    gyroKalman = new SimpleKalmanFilter(10, 10, 0.1);
    scl = _scl;
    sda = _sda;
    initSensor();
  }

  /*
    This method gets the raw values from the sensor.
    In it's sensory data variables - accel & gyro - will have the raw readings from the sensors corresponding registers.
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
    accelX = (((int16_t)Wire.read() << 8) | Wire.read());
    accelY = (((int16_t)Wire.read() << 8) | Wire.read());
    accelZ = (((int16_t)Wire.read() << 8) | Wire.read());
    gyroX = (((int16_t)Wire.read() << 16) | Wire.read());
    gyroY = (((int16_t)Wire.read() << 8) | Wire.read());
    gyroZ = (((int16_t)Wire.read() << 8) | Wire.read());

    // Setting the scaled sensory data based on the readings and the previously set scaler values.
    aX = (double)accelX / accelScaleFactor * g;
    aY = (double)accelY / accelScaleFactor * g;
    aZ = g - ((double)accelZ / accelScaleFactor * g);
    gX = (double)gyroX / gyroScaleFactor;
    gY = (double)gyroY / gyroScaleFactor;
    gZ = (double)gyroZ / gyroScaleFactor;

    // Setting the estimated values.
    kaX = accelKalman->updateEstimate(aX);
    kaY = accelKalman->updateEstimate(aY);
    kaZ = accelKalman->updateEstimate(aZ);
    kgX = gyroKalman->updateEstimate(gX);
    kgY = gyroKalman->updateEstimate(gY);
    kgZ = gyroKalman->updateEstimate(gZ);
  }

  /*
    This method returns the raw readings as formatted comma separated values.
    It is needed so that later we can easily get the data for the model training.
  */
  void printRawData()
  {
    Serial.printf("%i,%i,%i,%i,%i,%i,", accelX, accelY, accelZ, gyroX, gyroY, gyroZ);
  }

  /*
    This method returns the readings as formatted comma separated values.
    It is needed so that later we can easily get the data for the model training.
  */
  void printScaledData()
  {
    Serial.printf("%f,%f,%f,%f,%f,%f,", aX, aY, aZ, gX, gY, gZ);
  }

  /*
    This method returns only the accelerometer's readings as formatted comma separated values.
    It is needed so that later we can easily get the data for the model training.
  */
  void printScaledAccelData()
  {
    Serial.printf("%f,%f,%f,", aX, aY, aZ);
  }

  /*
    This method returns only the gyroscope readings as formatted comma separated values.
    It is needed so that later we can easily get the data for the model training.
  */
  void printScaledGyroData()
  {
    Serial.printf("%f,%f,%f,", gX, gY, gZ);
  }

  /*
    This method prints the measued sensory data run through a Kalman filter.
    It is important since this waY we get a clear stream of sensory data that
    can be used to group and identify exact signs for the model later or.
  */
  void printKalmanData()
  {
    Serial.printf("%f,%f,%f,%f,%f,%f,", kaX, kaY, kaZ, kgX, kgY, kgZ);
  }

  /*
    This method prints only the measued accelerometer data run through a Kalman filter.
    It is important since this waY we get a clear stream of sensory data that
    can be used to group and identify exact signs for the model later or.
  */
  void printKalmanAccelData()
  {
    Serial.printf("%f,%f,%f,", kaX, kaY, kaZ);
  }

  /*
    This method prints only the measued gyroscope data run through a Kalman filter.
    It is important since this waY we get a clear stream of sensory data that
    can be used to group and identify exact signs for the model later or.
  */
  void printKalmanGyroData()
  {
    Serial.printf("%f,%f,%f,", kgX, kgY, kgZ);
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