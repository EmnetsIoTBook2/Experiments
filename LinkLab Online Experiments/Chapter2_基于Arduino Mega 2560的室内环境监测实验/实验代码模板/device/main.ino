#include <dht11.h>
#define DHT11PIN 2
dht11 DHT11;
void setup(){
  // Initialize the serial and set the baud rate
  Serial.begin(9600);
  pinMode(DHT11PIN,OUTPUT);
  // Your code's here


  //========= End =========

  // print 'end' to complete judge
  Serial.println("end");
}
void loop() {

}