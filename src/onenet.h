#ifndef _onenet_h_
#define _onenet_h_

//设备上传数据的post主题
#define ONENET_TOPIC_PROP_POST "$sys/" mqtt_pubid "/" mqtt_devid "/dp/post/json"               //$sys/506181/esp1/dp/post/json
//接收下发属性设置主题
#define ONENET_TOPIC_PROP_SET  "$sys/" mqtt_pubid "/" mqtt_devid "/dp/post/json/+"             //带+意味着可以订阅所有的json/* 主题，因此accepted和rejected都可以接收到
//接收下发属性设置成功的回复主题
#define ONENET_TOPIC_PROP_SET_REPLY "$sys/" mqtt_pubid "/" mqtt_devid "/dp/post/json/accepted" //

//接收设备属性获取命令主题
#define ONENET_TOPIC_PROP_GET "$sys/" mqtt_pubid "/" mqtt_devid "/cmd/request/+"               //"$sys/" mqtt_pubid "/" mqtt_devid "/thing/property/get"
//接收设备属性获取命令主题
#define ONENET_TOPIC_PROP_CMD "$sys/" mqtt_pubid "/" mqtt_devid "/cmd/request/"                //cmd命令
//接收设备属性获取命令成功的回复主题
#define ONENET_TOPIC_PROP_GET_REPLY "$sys/" mqtt_pubid "/" mqtt_devid "/cmd/response/"         //"$sys/" mqtt_pubid "/" mqtt_devid "/thing/property/get_reply"

//这是post上传数据使用的模板
#define ONENET_POST_BODY_FORMAT "{\"id\":%d,\"dp\":%s}"

void sendTempAndHumi(int value);
void sendTempAndTemp(int value);
void clientReconnect();
void net_check();
void callback(char *topic, byte *payload, unsigned int length);
void onenet_init();

#endif