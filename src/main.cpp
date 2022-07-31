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

WiFiUDP Udp;
const unsigned int localUdpPort   = 54545;  // local port to listen on
const int remouteUdpPort = 54546;  // приемный порт пульта для ответов
char incomingPacket[128];  // buffer for incoming packets
char  replyPacketOk[] = "ok";  // a reply string to send back
char replayPacketName[] = "dermometer";


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

    digitalWrite(LED_BUILTIN,HIGH);
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

    Udp.begin(localUdpPort);
    //udp.beginMulticast(WiFi.localIP(), multicast_ip_addr, localUdpPort)
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

    tick.attach_ms(1000, tick_instance); // запуск процесса измерений

   // wifi_set_macaddr()

}  // setup

void loop(void){

    if (WiFi.status() != WL_CONNECTED)     {
        // перезапуск без разговоров при пропадании сети
        Serial.println("Нет сети. Перезагрузка......");
        ESP.restart();
    }

    int packetSize = Udp.parsePacket();
    if (packetSize)     {
        digitalWrite(LED_BUILTIN, LOW);        delay(10);        digitalWrite(LED_BUILTIN, HIGH);

        IPAddress clientAdr = Udp.remoteIP();
        // receive incoming UDP packets
        int len = Udp.read(incomingPacket, 255);
        if (len > 0) incomingPacket[len] = 0; // конец строки добавил

        // Serial.printf("UDP packet contents: %s\n", incomingPacket);
        if (strcmp("IsSomebodyHere", incomingPacket) == 0) {
            Udp.beginPacket( clientAdr, remouteUdpPort);
            Udp.write(replayPacketName);
            Udp.endPacket();
            return;
        }

        if (strcmp("Restart", incomingPacket) == 0) {
            Udp.beginPacket( clientAdr, remouteUdpPort);
            Udp.write( replyPacketOk );
            Udp.endPacket();
            Serial.println("Restart");
            ESP.restart();
            return;
        }

        if (strcmp("Get distance", incomingPacket) == 0) {
            //      Serial.println("get  resived");
            Udp.beginPacket(Udp.remoteIP(), remouteUdpPort);
            sprintf(strVal,"%8.2f",distance); 

            Udp.write(strVal, sizeof(strVal) );
            Udp.endPacket();
        }
    }
}
