#include <dht11.h>
#define DHT11PIN 2
dht11 DHT11;
void setup(){
  // Initialize the serial and set the baud rate
  Serial.begin(9600);
  pinMode(DHT11PIN,OUTPUT);
  // Your code's here

 for(int i=0;i<4;i++){
    delay(4000);
    DHT11.read(DHT11PIN);
    float hum=DHT11.humidity;
    Serial.println("humidity:"+String(hum));
    float tem=DHT11.temperature;
    Serial.println("temperature:"+String(tem));
  }
  //========= End =========

  // print 'end' to complete judge
  Serial.println("end");
}
void loop() {

}
