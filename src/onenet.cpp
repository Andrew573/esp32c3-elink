#include <Arduino.h>
#include "WiFi.h"
#include <PubSubClient.h>
#include "ArduinoJson.h"
#include "onenet.h"

char motor = 0;

extern const char *ssid;                //wifi名
extern const char *password;            //wifi密码

const char *mqtt_server = "mqtts.heclouds.com"; //onenet 的 IP地址
const int port = 1883;                          //端口号

#define mqtt_pubid "L9v795OfUr"                     //产品ID
#define mqtt_devid "esp32c3_1"                       //设备名称
//鉴权信息,  1652971258 = 2022年5月19日
//#define mqtt_password "version=2018-10-31&res=products%2F506181%2Fdevices%2Fesp1&et=1652971258&method=md5&sign=plDsytZzyH3e66MowbbPug%3D%3D" //鉴权信息
#define mqtt_password "version=2018-10-31&res=products%2FL9v795OfUr%2Fdevices%2Fesp32c3_1&et=2031749601&method=md5&sign=Fcg7uonnb%2FllXGCMwxGq3A%3D%3D" //鉴权信息

                       
WiFiClient espClient;           //创建一个WIFI连接客户端
PubSubClient client(espClient); // 创建一个PubSub客户端, 传入创建的WIFI客户端
int postMsgId = 0; //记录已经post了多少条

//向主题发送模拟的温湿度数据
void sendTempAndHumi(int value)
{
  if (client.connected())
  {
    //先拼接出json字符串
    char param[82];
    char jsonBuf[178];
    sprintf(param, "{ \"Humi\":[{\"v\":%d}] }", value); //我们把要上传的数据写在param里
    postMsgId += 1;
    sprintf(jsonBuf, ONENET_POST_BODY_FORMAT, postMsgId, param);
    //再从mqtt客户端中发布post消息
    if (client.publish(ONENET_TOPIC_PROP_POST, jsonBuf))
    {
      Serial.print("Post message to cloud: ");
      Serial.println(jsonBuf);
    }
    else
    {
      Serial.println("Publish message to cloud failed!");
    }
  }
}
 
void sendTempAndTemp(int value)
{
  if (client.connected())
  {
    //先拼接出json字符串
    char param[82];
    char jsonBuf[178];
    sprintf(param, "{ \"Temp\":[{\"v\":%d}] }", value); //我们把要上传的数据写在param里
    postMsgId += 1;
    sprintf(jsonBuf, ONENET_POST_BODY_FORMAT, postMsgId, param);
    //再从mqtt客户端中发布post消息
    if (client.publish(ONENET_TOPIC_PROP_POST, jsonBuf))
    {
      Serial.print("Post message to cloud: ");
      Serial.println(jsonBuf);
    }
    else
    {
      Serial.println("Publish message to cloud failed!");
    }
  }
}

//重连函数, 如果客户端断线,可以通过此函数重连
void clientReconnect()
{
  while (!client.connected()) //再重连客户端
  {
    Serial.println("reconnect MQTT...");
    if (client.connect(mqtt_devid, mqtt_pubid, mqtt_password))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.println("failed");
      Serial.println(client.state());
      Serial.println("try again in 5 sec");
      delay(5000);
    }
  }
}

void net_check()
{
  if (!client.connected())  //如果客户端没连接ONENET, 重新连接
  {
    clientReconnect();
    delay(100);
  }
  client.loop();            //客户端循环检测
}

//订阅的主题，一般是第一行
//具体的内容，一般是第二行
//具体内容的长度，一般是第二行的长度
void callback(char *topic, byte *payload, unsigned int length)
{
  char resp[5];

  Serial.println("message rev:");
  Serial.println(topic);
  for (size_t i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println("");
 
  if (strstr(topic, ONENET_TOPIC_PROP_SET_REPLY))    //这里是发送数据成功的确认
  {
    DynamicJsonDocument doc(100);
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.println("parse json failed");
      return;
    }
    JsonObject setAlinkMsgObj = doc.as<JsonObject>();
    String str = setAlinkMsgObj["id"];
    char sendbuf[100];
    sprintf(sendbuf, "{\"id\": \"%s\",\"code\":200,\"msg\":\"success\"}", str.c_str());
    Serial.println(sendbuf);
  }
  if (strstr(topic, ONENET_TOPIC_PROP_CMD))
  {
    Serial.println("recv cmd");

    DynamicJsonDocument doc(100);
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.println("parse json failed");
      return;
    }
    JsonObject setAlinkMsgObj = doc.as<JsonObject>();
    //{"motor":12},这里解析的是motor后面携带的值
    String str = setAlinkMsgObj["motor"];
    if(!strcmp(str.c_str(), "1"))
    {
      Serial.println("recv motor on");
      //digitalWrite(2, HIGH);
      //delay(1000);
      //digitalWrite(2, LOW);
      motor = 1;
      strcpy(resp, "ok");
    }
    else
    {
      strcpy(resp, "fail");
    }
    char sendbuf[100];
    //回复的topic，主要要在response后面携带上cmd id，服务器通过这个判断是那条cmd被接收到的
    sprintf(sendbuf, "%s%s", ONENET_TOPIC_PROP_GET_REPLY, (strstr(topic, ONENET_TOPIC_PROP_CMD) + strlen(ONENET_TOPIC_PROP_CMD)));    
    Serial.println(sendbuf);
    //回复的数据，用户可自定义应答数据内容，这里回复ok即可
    client.publish(sendbuf, resp);   
  }
}

void onenet_init()
{
  client.setServer(mqtt_server, port);                   //设置客户端连接的服务器,连接Onenet服务器, 使用6002端口
  client.connect(mqtt_devid, mqtt_pubid, mqtt_password); //客户端连接到指定的产品的指定设备.同时输入鉴权信息
  if (client.connected())
  {
    Serial.println("OneNet is connected!");              //判断以下是不是连好了.
  }
  client.subscribe(ONENET_TOPIC_PROP_SET);
  client.subscribe(ONENET_TOPIC_PROP_GET);
  client.setCallback(callback);

  //pinMode(2, OUTPUT);
}
