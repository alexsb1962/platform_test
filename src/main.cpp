#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "HCSR04.h"
#include "filter.h"
#include <ESP8266WebServer.h>

const byte TRIGGER = 1;
const byte ECHO    =1;
const float MAX = 200.0;
const float ALFA_F = 0.1;
const char* ssid = "theflat";
const char* password = "sheludko";

String webPage="";
ESP8266WebServer server(80);

Ticker tick;
float distance=0;
UltraSonicDistanceSensor hcr(TRIGGER, ECHO, MAX) ;

void tick_instance(){
    digitalWrite(LED_BUILTIN,LOW);
    distance = hcr.measureDistanceCm();
    distance = filter( distance, ALFA_F);
    digitalWrite(LED_BUILTIN,HIGH);
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

    webPage += "<h1>ESP8266 Web Server</h1><p>Socket #1 <a href=\"socket1On\"><button>ON</button></a>&nbsp;<a href=\"socket1Off\"><button>OFF</button></a></p>";
    webPage += "<p>Socket #2 <a href=\"socket2On\"><button>ON</button></a>&nbsp;<a href=\"socket2Off\"><button>OFF</button></a></p>";

    server.on("/",[](){
        Serial.println("/");
        server.send(200,"text/html",webPage);
    } );
    server.on("/socet1On",[](){
        Serial.println("//socet1On");
        server.send(200,"text/html",webPage);
    } );

    server.begin(80);
    Serial.println("Server started");

    tick.attach_ms(1000, tick_instance);

}  // setup
 
void loop(void){
    server.handleClient();
}

