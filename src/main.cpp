#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "HCSR04.h"
#include "filter.h"

const byte TRIGGER = 1;
const byte ECHO    =1;
const float MAX = 200.0;
const float ALFA_F = 0.1;
const char* ssid = "theflat";
const char* password = "sheludko";



Ticker tick;
float distance=0;
UltraSonicDistanceSensor hcr(TRIGGER, ECHO, MAX) ;

void tick_instance(){
   digitalWrite(LED_BUILTIN,LOW);
   distance = hcr.measureDistanceCm();
   Serial.printf("Distance before filter:  %f ",distance );
   distance = filter( distance, ALFA_F);
   digitalWrite(LED_BUILTIN,HIGH);
   Serial.printf("Distance after filter:   %f \n",distance );
}
        
void setup() {
    Serial.begin( 9600 );
    Serial.println("");
    for(int i =0;i<20; i++){
       Serial.printf("Проверка связи с компьютером. Строка № %d \n",i+1);
    }

    pinMode(LED_BUILTIN,OUTPUT);
    digitalWrite(LED_BUILTIN,LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN,HIGH);

    // wifi
    Serial.println();
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED){ 
       delay(200);
       Serial.print(".");
    }
    Serial.println("WiFi Connected");
    Serial.printf("Local IP:   "); Serial.println(WiFi.localIP() );
    

    tick.attach_ms(1000, tick_instance);

}  // setup
 
void loop(void){
  
}

