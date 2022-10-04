#include <Arduino.h>

#include "headers.h"
#include "mpu.h"

// The defined MPU-6050 instances.
mpu *mpu_1;
mpu *mpu_2;
mpu *mpu_3;
mpu *mpu_4;
mpu *mpu_5;

void setup()
{
  // Initialising the serial connection.
  Serial.begin(115200);
  while (!Serial)
  {
    delay(1000); // This will wait until the serial console opens.
  }

  // Instantiating the previously defined MPU-6050 objects.
  mpu_1 = new mpu(MPU_1_SCL, MPU_1_SDA);
  mpu_2 = new mpu(MPU_2_SCL, MPU_2_SDA);
  mpu_3 = new mpu(MPU_3_SCL, MPU_3_SDA);
  mpu_4 = new mpu(MPU_4_SCL, MPU_4_SDA);
  mpu_5 = new mpu(MPU_5_SCL, MPU_5_SDA);
}

void loop()
{
  /*
  mpu_1->getValues();
  mpu_2->getValues();
  mpu_3->getValues();
  mpu_4->getValues();
  mpu_5->getValues();

  mpu_1->printRawData();
  mpu_2->printRawData();
  mpu_3->printRawData();
  mpu_4->printRawData();
  mpu_5->printRawData();

  mpu::endOfData();
  */

  mpu_1->getValues();
  mpu_1->printRawData();
  mpu::endOfData();
  mpu_1->printKalmanData();
  mpu::endOfData();
  delay(1000);
}