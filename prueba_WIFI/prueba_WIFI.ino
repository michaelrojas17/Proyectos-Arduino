#include <ESP8266WiFi.h>


char* ssid ="Mike_1";
char* password="12345678";

void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
