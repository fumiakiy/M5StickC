#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#include "oursettings.h"
const char *INFO_PATH = "/zeroconf/info";
const char *SWITCH_PATH = "/zeroconf/switch";

String httpPost(const char *endpoint, const char *payload)
{
  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Content-Type", "application/json");
  int status = http.POST((uint8_t *)payload, strlen(payload));
  if (status == 200)
  {
    String res = http.getString();
    http.end();
    return res;
  }
  http.end();
  return "";
}

// Taken from https://make-muda.net/2019/09/6946/
void updateBatteryInfo()
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
  M5.Lcd.setCursor(0, 5);
  M5.Lcd.printf("Charge: %3d%%", battery);
}

bool getSwitchInfo()
{
  char endpoint[strlen(SONOFF_URL) + strlen(INFO_PATH) + 1];
  strcpy(endpoint, SONOFF_URL);
  strcat(endpoint, INFO_PATH);

  String res = httpPost(endpoint, INFO_JSON);
  int offPos = res.indexOf("\"switch\":\"on\",");
  return offPos >= 0;
}

void sendSwitchCommand(bool isOn)
{
  char endpoint[strlen(SONOFF_URL) + strlen(SWITCH_PATH) + 1];
  strcpy(endpoint, SONOFF_URL);
  strcat(endpoint, SWITCH_PATH);

  httpPost(endpoint, isOn ? OFF_JSON : ON_JSON);
}

void updateSwitchInfo()
{
  if (getSwitchInfo())
  {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 45);
    M5.Lcd.print("ON");
  }
  else
  {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 45);
    M5.Lcd.print("OFF");
  }
}

void toggleSwitch()
{
  sendSwitchCommand(getSwitchInfo());
}

void setup()
{
  M5.begin();

  M5.Axp.ScreenBreath(9);
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillScreen(WHITE);

  WiFi.begin(OUR_SSID, WIFI_PASSWD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
  }

  M5.Lcd.fillScreen(BLACK);
}

void powerOff()
{
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(0, 45);
  M5.Lcd.print("BYE");
  delay(1000);
  M5.Axp.PowerOff();
}

void statusUpdate(unsigned long tick)
{
  if (tick % 10 != 0)
    return;

  updateSwitchInfo();
  updateBatteryInfo();
}

void action() {
  if (M5.BtnA.isPressed())
  {
    toggleSwitch();
    powerOff();
  }
}

unsigned long lastMillis = 0;
unsigned long tick = 0;
unsigned long INTERVAL = 1000; // 1sec

void loop()
{
  M5.update();
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis > INTERVAL)
  {
    statusUpdate(tick++);
  }
  action();
  lastMillis = millis();
}
