
#include <Arduino.h>
#include "SPIFFS.h"
#include <SPI.h>
#include "elink3.h"
#include <Wire.h>
#include <AHT20.h>

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

void aht20_display()
{
  //if (aht20.available() == true)
  {
    //Get the new temperature and humidity value
    float temperature = aht20.getTemperature();
    float humidity = aht20.getHumidity();

	int temp = temperature;
	int humi = humidity;

	elink_display_int(110, 20, temp);
	Serial.printf("temp=%d\n", temp);
	elink_display_int(110, 40, humi);
	Serial.printf("humi=%d\n", humi);

#if 0
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

void setup() {
	Serial.begin(115200);
  Serial.println("setup start");

	elink3_setup();
	aht20_init();
	aht20_display();

	Serial.println("setup done");
}

void loop() {
	
	//delay(1000);
	//Serial.println("loop");
}

