#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

// #include "oursettings.h"
const char *INFO_PATH = "/zeroconf/info";
const char *SWITCH_PATH = "/zeroconf/switch";

char *sonoff_url = NULL;

String httpPostRes(const char *endpoint, int timeout, const char *payload)
{
  HTTPClient http;
  http.begin(endpoint);
  http.setConnectTimeout(timeout);
  http.addHeader("Content-Type", "application/json");
  int status = http.POST((uint8_t *)payload, strlen(payload));
  if (status == 200)
  {
    if (http.connected()) {
      String res = http.getString();
      http.end();
      return res;
    }
  }
  if (http.connected()) http.end();
  return "";
}

void httpPost(const char *endpoint, const char *payload)
{
  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Content-Type", "application/json");
  int status = http.POST((uint8_t *)payload, strlen(payload));
  if (http.connected()) http.end();
  return;
}

void updateBatteryInfo()
{
  float battery = M5.Axp.GetBatVoltage();
  M5.Lcd.setCursor(0, 5);
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("Charge: %.3f", battery);
}

String getSwitchInfoAtUrl(char *url, int timeout)
{
  if (url == NULL) return "";

  char endpoint[strlen(url) + strlen(INFO_PATH) + 1];
  strcpy(endpoint, url);
  strcat(endpoint, INFO_PATH);

  String res = httpPostRes(endpoint, timeout, INFO_JSON);
  return res;
}

bool getSwitchInfo() {
  String res = getSwitchInfoAtUrl(sonoff_url, 5000);
  if (res == NULL || res.length() == 0) return false;
  int offPos = res.indexOf("\"switch\":\"on\",");
  return offPos >= 0;

}

void sendSwitchCommand(bool isOn)
{
  if (sonoff_url == NULL) return;

  char endpoint[strlen(sonoff_url) + strlen(SWITCH_PATH) + 1];
  strcpy(endpoint, sonoff_url);
  strcat(endpoint, SWITCH_PATH);

  httpPost(endpoint, isOn ? OFF_JSON : ON_JSON);
}

void updateURL() {
  if (sonoff_url == NULL) return;
  M5.Lcd.setCursor(0, 28);
  M5.Lcd.setTextSize(1);
  M5.Lcd.print(sonoff_url);
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
  if (sonoff_url == NULL) return;

  sendSwitchCommand(getSwitchInfo());
}

int status = 0;
unsigned long lastStatusUpdateMs = 0;

unsigned long lastMillis = 0;
unsigned long tick = 0;
unsigned long INTERVAL = 1000; // 1sec

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
  status = 1;

  findSonoff();
  delay(1000);
}

void findSonoff() {
  if (sonoff_url != NULL) return;

  String base = "192.168.11.";
  for (int i = 1; i < 20; i++) {
    String _ip = base + String(i);
    String _url = "http://" + _ip + ":8081";
    const char *__url = _url.c_str();
    char ___url[strlen(__url) + 1];
    strcpy(___url, __url);
    String res = getSwitchInfoAtUrl(___url, 1000);
    if (res != NULL && res.length() > 0) {
      sonoff_url = (char *) malloc(strlen(___url));
      strcpy(sonoff_url, ___url);
      break;
    }
  }
}

void preparePowerOff()
{
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(0, 45);
  M5.Lcd.print("BYE");
}

void doPowerOff()
{
  M5.Axp.PowerOff();
}

void statusUpdate(unsigned long tick)
{
  if (tick % 10 != 0)
    return;
  if (status != 1)
    return;

  updateSwitchInfo();
  updateURL();
  updateBatteryInfo();
}

int action(int status)
{
  if (status == 1)
  {
    if (M5.BtnA.wasPressed())
    {
      return status;
    }
    if (M5.BtnA.isPressed())
    {
      toggleSwitch();
      lastStatusUpdateMs = millis();
      return 2;
    }
  }
  unsigned long currentMillis = millis();
  if (status == 2)
  {
    if (currentMillis - lastStatusUpdateMs > INTERVAL * 2)
    {
      preparePowerOff();
      lastStatusUpdateMs = millis();
      return 3;
    }
  }
  if (status == 3)
  {
    if (currentMillis - lastStatusUpdateMs > INTERVAL * 2)
    {
      doPowerOff();
      lastStatusUpdateMs = millis();
      return 4;
    }
  }

  return status;
}

void loop()
{
  if (sonoff_url == NULL) {
    findSonoff();
    delay(1000);
    return;
  }

  M5.update();
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis > INTERVAL)
  {
    statusUpdate(tick++);
  }
  status = action(status);
  lastMillis = millis();
}
