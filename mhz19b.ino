#include "MHZ19.h"
#include "oursettings.h"

#define RX_PIN 0      // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 26     // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600 // Device to MH-Z19 Serial baudrate (should not be changed)

MHZ19 co2sensor;

void co2Setup()
{
  Serial1.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);
  co2sensor.begin(Serial1);
  co2sensor.autoCalibration();
}

int getCo2Ppm()
{
  return co2sensor.getCO2();
}

int8_t getTemp()
{
  return co2sensor.getTemperature();
}

void sendCO2Data(int ppm, int8_t temperature) {
  char postData[16];
  sprintf(postData, "[%d,%d]", ppm, temperature);
  httpPost(SHEET_URL, postData);
}