#include <Wire.h>
#include <AHT20.h>
#include <my_aht20.h>
#include "my_espnow.h"

AHT20 aht20;

void aht20_init()
{
  Wire.begin(0, 1); //Join I2C bus

  //Check if the AHT20 will acknowledge
  if (aht20.begin() == false)
  {
	Serial.println("aht20 false");
  }
  else
  {
	Serial.println("aht20 true");
  }
}

int aht20_read_temp()
{
    int temp = aht20.getTemperature();
    return temp;
}

int aht20_read_humi()
{
    int humi = aht20.getHumidity();
    return humi;
}

void aht20_read()
{
  //if (aht20.available() == true)
  {
    float temperature = aht20.getTemperature();
    float humidity = aht20.getHumidity();

	int temp = temperature;
	int humi = humidity;

#if 0
    char temp_c[10];
    char humi_c[10];

    snprintf(temp_c, sizeof(temp_c), "%d", temp);
    snprintf(humi_c, sizeof(humi_c), "%d", humi);

	//elink_display_int(110, 20, temp);
	Serial.printf("temp=%d\n", temp);
    my_espnow_send(temp_c);
	//elink_display_int(110, 40, humi);
	Serial.printf("humi=%d\n", humi);
    my_espnow_send(humi_c);

    //Print the results
    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.print(" C\t");
    Serial.print("Humidity: ");
    Serial.print(humidity, 2);
    Serial.print("% RH");
    Serial.println();
#endif
  }
}