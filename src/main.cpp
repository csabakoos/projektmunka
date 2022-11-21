#include <Arduino.h>
#include "headers.h"
#include "mpu.h"
#include "NeuralNetwork.h"

// The defined MPU-6050 instances.
mpu *mpu_1;
mpu *mpu_2;
mpu *mpu_3;
mpu *mpu_4;
mpu *mpu_5;

// Buffer for sensory data collection and summarization.
float buffer[15];
float refs[15];
long startMillis;

// Neural Network for prediction
NeuralNetwork *nn;

// Number of labes created of the training set.
const uint8_t labels = 4;

// Array storing the predictable values in training order.
char* actions[labels]  = { "boink", "fel", "nine", "zero" };

void getSensoryData(mpu *mpu_x)
{
  if (mpu_x->checkMotionDetection(5))
  {
    mpu_x->printKalmanData();
  }
  else
  {
    mpu_x->printDefaultData(); /* WARNING! It will possibly overload the Serial Collector! */
  }
}

void readSensors()
{
  mpu_1->getValues();
  mpu_2->getValues();
  mpu_3->getValues();
  mpu_4->getValues();
  mpu_5->getValues();
}

boolean checkMovement(float sens)
{
  readSensors();

  buffer[0] = mpu_1->getKalmanAccelX();
  buffer[1] = mpu_1->getKalmanAccelY();
  buffer[2] = mpu_1->getKalmanAccelZ();
  buffer[3] = mpu_2->getKalmanAccelX();
  buffer[4] = mpu_2->getKalmanAccelY();
  buffer[5] = mpu_2->getKalmanAccelZ();
  buffer[6] = mpu_3->getKalmanAccelX();
  buffer[7] = mpu_3->getKalmanAccelY();
  buffer[8] = mpu_3->getKalmanAccelZ();
  buffer[9] = mpu_4->getKalmanAccelX();
  buffer[10] = mpu_4->getKalmanAccelY();
  buffer[11] = mpu_4->getKalmanAccelZ();
  buffer[12] = mpu_5->getKalmanAccelX();
  buffer[13] = mpu_5->getKalmanAccelY();
  buffer[14] = mpu_5->getKalmanAccelZ();

  float dif;

  for (uint8_t i = 0; i < 15; i++)
  {
    float dif = refs[i] - buffer[i];
    // Serial.println();
    // Serial.printf("Check [%i, refs=%f, buff=%f]: %f > %f (?)", i, refs[i], buffer[i], dif, sens);
    if (abs(dif) > sens)
    {
      for (uint8_t j = 0; j < 15; j++)
      {
        buffer[j] = 0;
      }
      return true;
    }
  }
  for (uint8_t j = 0; j < 15; j++)
  {
    buffer[j] = 0;
  }
  return false;
}

void calibrateReference()
{
  digitalWrite(2, HIGH);
  delay(2500);
  digitalWrite(2, LOW);
  delay(125);
  digitalWrite(2, HIGH);
  delay(125);
  digitalWrite(2, LOW);
  delay(125);
  digitalWrite(2, HIGH);
  delay(125);
  digitalWrite(2, LOW);
  delay(1000);

  // Serial.println();
  // Serial.print("Setting reference signals");
  readSensors();

  for (uint8_t j = 0; j < 15; j++)
  {
    refs[j] = 0;
  }

  startMillis = millis();

  uint8_t count = 0;
  while (millis() - startMillis < 4000)
  {
    // Serial.print(".");
    digitalWrite(2, HIGH);
    delay(50);

    readSensors();

    refs[0] += mpu_1->getKalmanAccelX();
    refs[1] += mpu_1->getKalmanAccelY();
    refs[2] += mpu_1->getKalmanAccelZ();
    refs[3] += mpu_2->getKalmanAccelX();
    refs[4] += mpu_2->getKalmanAccelY();
    refs[5] += mpu_2->getKalmanAccelZ();
    refs[6] += mpu_3->getKalmanAccelX();
    refs[7] += mpu_3->getKalmanAccelY();
    refs[8] += mpu_3->getKalmanAccelZ();
    refs[9] += mpu_4->getKalmanAccelX();
    refs[10] += mpu_4->getKalmanAccelY();
    refs[11] += mpu_4->getKalmanAccelZ();
    refs[12] += mpu_5->getKalmanAccelX();
    refs[13] += mpu_5->getKalmanAccelY();
    refs[14] += mpu_5->getKalmanAccelZ();

    count++;

    digitalWrite(2, LOW);
    delay(50);
  }

  
  // Serial.print("Reference is set: ");
  for (uint8_t j = 0; j < 15; j++)
  {
    refs[j] /= count;
    // (j == 14) ? Serial.printf("%f", refs[j]) : Serial.printf("%f,", refs[j]);
  }
  startMillis = millis();
}

void setup()
{
  // Initialising the serial connection.
  Serial.begin(115200);
  while (!Serial)
  {
    delay(100); // This will wait until the serial console opens.
  }

  nn = new NeuralNetwork();

  // Instantiating the previously defined MPU-6050 objects.
  mpu_1 = new mpu(MPU_1_SCL, MPU_1_SDA);
  mpu_2 = new mpu(MPU_2_SCL, MPU_2_SDA);
  mpu_3 = new mpu(MPU_3_SCL, MPU_3_SDA);
  mpu_4 = new mpu(MPU_4_SCL, MPU_4_SDA);
  mpu_5 = new mpu(MPU_5_SCL, MPU_5_SDA);

  pinMode(2, OUTPUT);

  calibrateReference();
}

void checkSection() {
  digitalWrite(2, HIGH);
  // Serial.print("Waiting for movement...");
  while (!checkMovement(5))
  { /* Nothing should happen... */
  }
  digitalWrite(2, LOW);
}

void collectSection() {
  startMillis = millis();
  while (millis() - startMillis < 5000)
  {
    readSensors();
    buffer[0] += mpu_1->getKalmanAccelX();
    buffer[1] += mpu_1->getKalmanAccelY();
    buffer[2] += mpu_1->getKalmanAccelZ();
    buffer[3] += mpu_2->getKalmanAccelX();
    buffer[4] += mpu_2->getKalmanAccelY();
    buffer[5] += mpu_2->getKalmanAccelZ();
    buffer[6] += mpu_3->getKalmanAccelX();
    buffer[7] += mpu_3->getKalmanAccelY();
    buffer[8] += mpu_3->getKalmanAccelZ();
    buffer[9] += mpu_4->getKalmanAccelX();
    buffer[10] += mpu_4->getKalmanAccelY();
    buffer[11] += mpu_4->getKalmanAccelZ();
    buffer[12] += mpu_5->getKalmanAccelX();
    buffer[13] += mpu_5->getKalmanAccelY();
    buffer[14] += mpu_5->getKalmanAccelZ();
  }

  // Serial.print("Collected movement data: ");
  for (uint8_t j = 0; j < 15; j++)
  {
    (j == 14) ? Serial.printf("%f", buffer[j]) : Serial.printf("%f,", buffer[j]);
  }
  Serial.println();

  digitalWrite(2, HIGH);
  delay(125);
  digitalWrite(2, LOW);
  delay(125);
  digitalWrite(2, HIGH);
  delay(125);
  digitalWrite(2, LOW);
  delay(125);
  digitalWrite(2, HIGH);
  delay(125);
  digitalWrite(2, LOW);
  delay(125);
}

void predictSection() {
  digitalWrite(2, HIGH);
  Serial.print("___NN-PRED: ");

  for (uint8_t i = 0; i < 15; i++)
  {
    nn->getInputBuffer()[i] = buffer[i];
  }

  float* result = nn->predict(labels);

  uint8_t index = 0;
  for (uint8_t i = 1; i < labels; i++)
  {
    if(result[index] < result[i]) {
      index = i;
    }
  }

  char* act = actions[index];
  float pct = result[index];

  Serial.printf("Predicted - %s (%.2f %%)\n", act, pct);
  delay(1000);
  digitalWrite(2, LOW);
}

void loop()
{
  checkSection();
  collectSection();
  predictSection();
}