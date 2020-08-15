#include "oursettings.h"
const char *INFO_PATH = "/zeroconf/info";
const char *SWITCH_PATH = "/zeroconf/switch";

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

void toggleSwitch()
{
  sendSwitchCommand(getSwitchInfo());
}
