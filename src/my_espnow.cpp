#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include "my_espnow.h"

// 接收端的MAC地址
uint8_t broadcastAddress[] = {0x60, 0x55, 0xF9, 0x77, 0x6F, 0xCC};

// 发送结构体类型
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

// 创建一个结构体变量
struct_message myData;

void my_espnow_init() {
  WiFi.mode(WIFI_STA);
  
  // 初始化espnow
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void my_espnow_send(int a)
{
  static int count = 0;

  strcpy(myData.a, "int");
  myData.b = a;
  myData.c = 1.0;
  myData.d = false;
  
  //发送信息到指定ESP32上
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
}
void my_espnow_send(char *a)
{
  static int count = 0;

  strcpy(myData.a, a);
  myData.b = count++;
  myData.c = 1.0;
  myData.d = false;
  
  //发送信息到指定ESP32上
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
}
void my_espnow_test()
{
  strcpy(myData.a, "test char[32]");
  myData.b = random(1,20);
  myData.c = 1.2;
  myData.d = false;
  
  //发送信息到指定ESP32上
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
 //判断是否发送成功
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}