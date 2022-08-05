#include "DHT.h"
#include "systime.h"
 
#define DHTPIN 23   //DHT11 DATA 数据引脚
 
#define DHTTYPE DHT11  //选择的类型
 
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("DHT22 sensor!");
  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  delay(500);
}
