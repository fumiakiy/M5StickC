#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "oursettings.h"

void networkSetup()
{
  WiFi.begin(OUR_SSID, WIFI_PASSWD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
  }
}

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
