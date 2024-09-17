
#include <Arduino.h>
#include "SPIFFS.h"
#include <SPI.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include "elink3.h"
#include "my_espnow.h"
#include "my_aht20.h"
#include "user_wifi.h"
#include "onenet.h"

int temp_i[10];
int humi_i[10];
int cnt __attribute__((section(".rtc.data")));
int last_temp __attribute__((section(".rtc.data")));
int last_humi __attribute__((section(".rtc.data")));

int get_avg(int *a, int n)
{
  int sum_i = 0;
  for(int i = 0; i < n; i++)
    sum_i = sum_i + a[i];
  
  return sum_i / n;
}

void setup() {
	Serial.begin(115200);
  Serial.println("setup start");

  //my_espnow_init();
  //my_espnow_send("setup start");
  //my_espnow_send(cnt);
  cnt += 1;

	//elink3_setup();
	aht20_init();
  setupWifi();
  //my_espnow_send("wifi done");
  onenet_init();

  for(int i = 0; i < 10; i++)
  {
    temp_i[i] = aht20_read_temp();
    humi_i[i] = aht20_read_humi();
    delay(100);
  }

  int temp = get_avg(temp_i, 10);
  int humi = get_avg(humi_i, 10);
  //my_espnow_send(temp);
  //my_espnow_send(humi);

  /*if(last_humi != humi || last_temp != temp)
  {
    elink3_setup();
    elink_set_temp(temp);
    elink_set_humi(humi);
    elink_display_all(); 
  }
  else
  {
    //my_espnow_send("not need flash");
  }
  */


  //my_espnow_send("onenet done");
  sendTempAndHumi(humi);
  sendTempAndTemp(temp);
  //my_espnow_send("upload done");

  //delay(2000);

  WiFi.mode(WIFI_OFF);
  //last_humi = humi;
  //last_temp = temp;

	//Serial.println("setup done");
  //my_espnow_send("setup done");

  //esp_now_deinit();
  // 设置深度睡眠的时间（单位：秒）
  esp_sleep_enable_timer_wakeup(300 * 1000 * 1000); // 60 seconds
  esp_deep_sleep_start();
}

void loop() {
  //my_espnow_test();
}

