#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server;

uint8_t pin_led = 16;
char* ssid ="vodafone790E";
char* password="BXVISBZUNLQNP4";


void setup() {
  // put your setup code here, to run once:
  pinMode(pin_led, OUTPUT);
  WiFi.begin(ssid,password);
  Serial.begin(115200);

  
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());

  server.on("/",[](){server.send(200,"text/plain","Hello World! Este es el modo solo STATION");});
  server.on("/toggle",toggleLED);
  server.begin();
  WiFi.mode(WIFI_STA);
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}

void toggleLED()
{
  digitalWrite(pin_led,!digitalRead(pin_led));
  server.send(204,"");
}
