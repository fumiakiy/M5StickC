#include <M5StickC.h>

void powerOff()
{
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(20, 45);
  M5.Lcd.print("BYE");
  delay(1000);
  M5.Axp.PowerOff();
}

void statusUpdate(unsigned long tick)
{
  if (tick % 60 == 0) {
    int8_t battery = getBattery();
    M5.Lcd.fillRect(0, 0, 240, 20, BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("Battery: %4d%%", battery);
  }

  if (tick % 60 == 0) {
    M5.Lcd.fillRect(0, 20, 240, 40, BLACK);
    int ppm = getCo2Ppm();
    M5.Lcd.setCursor(0, 20);
    M5.Lcd.printf("CO2    : %4d", ppm);
    int temp = getTemp();
    M5.Lcd.setCursor(0, 40);
    M5.Lcd.printf("Temp   : %3dc", temp);
  }

  if (tick % 180 == 0) {
    M5.Lcd.fillRect(0, 60, 240, 20, BLACK);
    M5.Lcd.setCursor(0, 60);
    if (getSwitchInfo())
    {
      M5.Lcd.print("Switch :  ON");
    }
    else
    {
      M5.Lcd.print("Switch :  OFF");
    }
  }
}

void setup()
{
  M5.begin();

  M5.Axp.ScreenBreath(9);
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillScreen(WHITE);

  networkSetup();
  co2Setup();

  M5.Lcd.fillScreen(BLACK);
  statusUpdate(0);
}

unsigned long lastMillis = 0;
unsigned long tick = 0;
unsigned long INTERVAL = 10000; // 10sec

void loop()
{
  M5.update();
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis > INTERVAL)
  {
    statusUpdate(tick++);
    lastMillis = millis();
  }
  if (M5.BtnA.isPressed())
  {
    toggleSwitch();
    powerOff();
  }
}
