#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <Ticker.h>
#include "HCSR04.h"
#include "filter.h"

const byte TRIGGER = 1;
const byte ECHO    =1;
const float MAX = 200.0;
const float ALFA_F = 0.1;

const char* ssid = "theflat";
const char* password = "sheludko";
const int  PORT = 23;   // 23 - нативный telnet
#define MAX_SRV_CLIENTS 2         //how many clients should be able to telnet to this ESP8266


WiFiUDP Udp;
unsigned int localUdpPort   = 54545;  // local port to listen on
unsigned int remouteUdpPort = 54546;  // приемный порт пульта для ответов
char incomingPacket[128];  // buffer for incoming packets
char  replyPacketOk[] = "ok";  // a reply string to send back
char replayPacketName[] = "doors";


Ticker tick;
float distance=0;
UltraSonicDistanceSensor hcr(TRIGGER, ECHO, MAX) ;

void tick_instance(){
    digitalWrite(LED_BUILTIN,LOW);
    distance = hcr.measureDistanceCm();
    distance = filter( distance, ALFA_F);
    String val(distance,2);   // Вроде есть перегрузка с float

    digitalWrite(LED_BUILTIN,HIGH);
}
      
void setup() {
    Serial.begin( 9600 );
    Serial.println("");
    for(int i =0;i<5; i++){
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
    Serial.print("Local IP = "); Serial.println(WiFi.localIP() );
    Serial.print("MAC =  "); Serial.println( WiFi.macAddress() );

    server.begin();
    Serial.println("Server started");

    tick.attach_ms(1000, tick_instance); // запуск процесса измерений

}  // setup
 
void loop(void){

    //check if there are any new clients
    if (server.hasClient())    {
        // find free/disconnected spot
        int i;
        for (i = 0; i < MAX_SRV_CLIENTS; i++)
            if (!serverClients[i].connected() )            { // equivalent to !serverClients[i].connected()
                serverClients[i] = server.available();
                Serial.print("New client: index ");
                Serial.print(i);
                break;
            }

        // no free/disconnected spot so reject
        if (i == MAX_SRV_CLIENTS)        {
            server.available().println("busy");
            // hints: server.available() is a WiFiClient with short-term scope
            // when out of scope, a WiFiClient will
            // - flush() - all data will be sent
            // - stop() - automatically too
            Serial.printf("server is busy with %d active connections\n", MAX_SRV_CLIENTS);
        }
    }

    // check TCP clients for data
#if 1
    // Incredibly, this code is faster than the buffered one below - #4620 is needed
    // loopback/3000000baud average 348KB/s
    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
        while (serverClients[i].available() && Serial.availableForWrite() > 0)
        {
            // working char by char is not very efficient
            Serial.write(serverClients[i].read()); 
        }
#else
    // loopback/3000000baud average: 312KB/s
    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
        while (serverClients[i].available() && Serial.availableForWrite() > 0)
        {
            size_t maxToSerial = std::min(serverClients[i].available(), Serial.availableForWrite());
            maxToSerial = std::min(maxToSerial, (size_t)STACK_PROTECTOR);
            uint8_t buf[maxToSerial];
            size_t tcp_got = serverClients[i].read(buf, maxToSerial);
            size_t serial_sent = Serial.write(buf, tcp_got);
            if (serial_sent != maxToSerial)
            {
                logger->printf("len mismatch: available:%zd tcp-read:%zd serial-write:%zd\n", maxToSerial, tcp_got, serial_sent);
            }
        }
#endif
}
