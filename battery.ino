#include <M5StickC.h>

// Taken from https://make-muda.net/2019/09/6946/
int8_t getBattery()
{
  uint16_t vbat = M5.Axp.GetVbatData() * 1.1 / 1000;
  int8_t battery = int8_t((vbat - 3.0) / 1.2 * 100);
  if (battery > 100)
  {
    battery = 100;
  }
  else if (battery < 0)
  {
    battery = 0;
  }
  return battery;
}