

#include <ESP8266WiFi.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic


////Para la matrix de LEDS


#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

///*********home Wifi**********

#define WLAN_SSID       "vodafone790E"
#define WLAN_PASS       "BXVISBZUNLQNP4"

///*********Android Wifi**********

//#define WLAN_SSID       "Mike_wifi"
//#define WLAN_PASS       "123456789:"

///*********Projects Labs Wifi**********

//#define WLAN_SSID       "Wifi_Lab_Proyectos"
//#define WLAN_PASS       "R3B00TY0URM1ND"

/************************* LEDS Matrix   *********************************/

#define PIN 14 //Placa Mia
//#define PIN 19 //NodeMCU

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "mike921217"
#define AIO_KEY         "62d2cbedd8c346cc8bfb4c029b26fcc3"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);


//MATRIX DECLARATION:
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(12, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

  

/****************************** Feeds ***************************************/


// subscribing topics

Adafruit_MQTT_Subscribe Light = Adafruit_MQTT_Subscribe(&mqtt,AIO_USERNAME "/feeds/Light");
Adafruit_MQTT_Subscribe Mensaje = Adafruit_MQTT_Subscribe(&mqtt,AIO_USERNAME "/feeds/Mensaje");
Adafruit_MQTT_Subscribe ColorLEDS = Adafruit_MQTT_Subscribe(&mqtt,AIO_USERNAME "/feeds/Color");

/*************************** Sketch Code ************************************/



const uint16_t colors[] = {
  matrix.Color(100, 0,0), matrix.Color(0, 100, 0), matrix.Color(0, 0, 100) };


void MQTT_connect();

void setup() {

  ///Matrix initialization
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(colors[0]);

  // MQTT and ESP initialization 
  Serial.begin(115200);
  delay(10);
  pinMode(16,OUTPUT);

  Serial.println(F("\nAdafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&Light);
  mqtt.subscribe(&Mensaje);
  mqtt.subscribe(&ColorLEDS);
}


int x    = matrix.width();
int pass;
String textoRecibido;
String ColorRecibido;

void loop() {

  
 
  MQTT_connect();

  

  Adafruit_MQTT_Subscribe *subscription;
  
  while ((subscription = mqtt.readSubscription(50))) {
    if (subscription == &Light) {
      Serial.print(F("Got_Light: "));
      Serial.println((char *)Light.lastread);
      uint16_t num = atoi((char *)Light.lastread);


      if(num){        
        digitalWrite(16,0);         
      }else{
        digitalWrite(16,1);         
      }      
           
    }else if(subscription == &Mensaje){
      
      textoRecibido = (char *)Mensaje.lastread;
      
      int longitud =LongitudCadena(textoRecibido);

       //matrix.fillScreen(0);
      
       Serial.print(F("Mensaje Recibido: "));
       Serial.println(textoRecibido);
       Serial.print(F("Longitud: "));
       Serial.println(longitud);
            
       
      
    }else if(subscription == &ColorLEDS){

       ColorRecibido = (char *)ColorLEDS.lastread;
       Serial.print(F("\nColor de LEDS: "));
       
       if(ColorRecibido=="Rojo"){

        Serial.print(F("texto en Rojo "));
        pass=0;
        
       }else if(ColorRecibido=="Verde"){

        Serial.print(F("texto en Verde "));
        pass=1;
        
       }else if(ColorRecibido=="Azul"){

        Serial.print(F("texto en Azul "));
        pass=2;
        
       }else{

        Serial.print(F("Color invalido.. :/ "));
        
       }
      

      
      
    }
    
  }

 
  mostrarTexto();



  


  

}// Fin de Loop




void mostrarTexto(){
  
  
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  
  matrix.print(textoRecibido);
  int le = (textoRecibido.length() * 7);
  
  if(--x < -le) {
    x = matrix.width();
    //if(++pass >= 3) pass = 0;
    matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  delay(50);
}


//Funcion para contar longitud de cadenas

int LongitudCadena(String cadena){
  int i=0, contador=0;

  while(cadena[i]!='\0'){
    i++;
  }
  return i;
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
