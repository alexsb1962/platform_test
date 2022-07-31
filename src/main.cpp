#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <Ticker.h>
#include <PubSubClient.h>
#include "HCSR04.h"
#include "filter.h"

const byte TRIGGER = D7;  
const byte ECHO    = D6;   //D6
const float MAX = 300.0;
const float ALFA_F = 0.01;

const char* ssid = "theflat";
const char* password = "sheludko";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);


Ticker tick;
float distance=0;
char  strVal[10];
UltraSonicDistanceSensor hcr(TRIGGER, ECHO, MAX) ;

void tick_instance(){
    digitalWrite(LED_BUILTIN,LOW);
    distance = hcr.measureDistanceCm();
    Serial.printf("distace = %8.2f  ",distance);
    distance = filter( distance, ALFA_F);
    Serial.printf("filtered distace = %8.2f\n",distance);
    client.publish("dermometter/val","strval");

    digitalWrite(LED_BUILTIN,HIGH);
}
      

void MQTTcallback(char *topic, byte *payload, unsigned int length){
    Serial.print("Message received in topic: ");
    Serial.println(topic);
    Serial.print("Message:");

    String message (( char *) payload );
    Serial.print(message);

    if (message == "on")    {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else if (message == "off")    {
        digitalWrite(LED_BUILTIN, LOW);
    }
    Serial.println();
    Serial.println("-----------------------");
}    

void setup() {
    Serial.begin( 9600 );
    Serial.println("");
    for(int i =0;i<5; i++){
       Serial.printf("Проверка связи с компьютером. Строка № %d \n",i+1);
    }

    pinMode(LED_BUILTIN,OUTPUT);
    digitalWrite(LED_BUILTIN,LOW); delay(1000); digitalWrite(LED_BUILTIN,HIGH);

    // wifi
    Serial.println();
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED){ 
       delay(200);
       Serial.print(".");
    }
    Serial.println("WiFi Connected");
    Serial.print("Local IP = "); Serial.println(WiFi.localIP() );
    Serial.print("MAC =  "); Serial.println( WiFi.macAddress() );

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(MQTTcallback);
     while (!client.connected())     {
         Serial.println("Connecting to MQTT...");
         if (client.connect("ESP8266"))  {
             Serial.println("connected");
         }
         else         {
             Serial.print("failed with state ");
             Serial.println(client.state());
             delay(2000);
         }
     }
     client.subscribe("esp/test");

    tick.attach_ms(5000, tick_instance); // запуск процесса измерений

   // wifi_set_macaddr()

} // setup


void loop(void){

    if (WiFi.status() != WL_CONNECTED)     {
        // перезапуск без разговоров при пропадании сети
        Serial.println("Нет сети. Перезагрузка......");
        ESP.restart();
    }

    client.loop();

    
}
