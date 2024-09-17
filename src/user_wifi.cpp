#include <Arduino.h>
#include "WiFi.h"
#include "user_wifi.h"

//const char *ssid = "CMCC-UC7S";                 //wifi名
//const char *password = "csuvy7wa";              //wifi密码
const char *ssid = "WIFI_20BA";                 //wifi名
const char *password = "1234567890";              //wifi密码

//连接WIFI相关函数
void setupWifi()
{
  delay(10);
  Serial.println("connect WIFI");
  WiFi.begin(ssid, password);
  while (!WiFi.isConnected())
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("OK");
  Serial.println("Wifi connected!");
}

void wifi_check()
{
  if (!WiFi.isConnected())  //先看WIFI是否还在连接
  {
    setupWifi();
  }
}