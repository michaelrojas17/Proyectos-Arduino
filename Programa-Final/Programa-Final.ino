

#include <ESP8266WiFi.h>          //Libreria para control del WIFI del ESP12F 
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer 
#include <EEPROM.h>               //Libreria para Trabajar con la memoria EEPROM integrada en el ESP-12F

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

//#define WLAN_SSID       "vodafone790E"
//#define WLAN_PASS       "BXVISBZUNLQNP4"

///*********Android Wifi**********

//#define WLAN_SSID       "Mike_wifi"
//#define WLAN_PASS       "123456789:"

///*********Projects Labs Wifi**********

//#define WLAN_SSID       "Wifi_Lab_Proyectos"
//#define WLAN_PASS       "R3B00TY0URM1ND"


/************************* LEDS Matrix Control PIN   *********************************/

#define PIN 14 //Placa 


/************************* Configuracion del Broker MQTT *********************************/

//#define BROKER_SERVER      "192.168.0.150"
//#define BROKER_SERVER      "wifi-lights-control-broker.etowns.net"
#define BROKER_SERVER      "wledscontrolbroker.ddns.net"
#define BROKER_SERVERPORT  1883                   
#define BROKER_USERNAME    "mike921217"
#define BROKER_PASS         "921217"

//-------------------VARIABLES GLOBALES--------------------------
int contconexion = 0;
unsigned long previousMillis = 0;

char ssid[50];      
char pass[50];

const char *ssidConf = "ESP-LEDS-WiFi";
const char *passConf = "12345678";

String mensaje = "";



/************ Global State  ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient espClient;
// Create an ESP8266 WebServer
ESP8266WebServer server(80);


// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&espClient, BROKER_SERVER, BROKER_SERVERPORT, BROKER_USERNAME, BROKER_PASS);


//MATRIX DECLARATION:
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(12, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

  

/****************************** Feeds ***************************************/

// subscribing topics

Adafruit_MQTT_Subscribe Modo = Adafruit_MQTT_Subscribe(&mqtt,"Modo");
Adafruit_MQTT_Subscribe Mensaje = Adafruit_MQTT_Subscribe(&mqtt,"Mensaje");
Adafruit_MQTT_Subscribe ColorLEDS = Adafruit_MQTT_Subscribe(&mqtt,"Color");

/*************************** Sketch Code ************************************/



const uint16_t colors[] = {
  matrix.Color(100, 0,0), matrix.Color(0, 100, 0), matrix.Color(0, 0, 100) };


void MQTT_connect();

//------------------------SETUP WIFI-----------------------------
void setup_wifi() {
// Conexión WIFI
  pinMode(2, OUTPUT);
  WiFi.mode(WIFI_STA); //para que no inicie el SoftAP en el modo normal
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED and contconexion <50) { //Cuenta hasta 50 si no se puede conectar lo cancela
    ++contconexion;
    delay(500);
    
  }
  if (contconexion <50) {   
      Serial.println("");
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
      digitalWrite(2, HIGH);
     }
  else { 
      Serial.println("");
      Serial.println("Error de conexion");
      modoconf();
  }

 
}


//--------------------MODO_CONFIGURACION------------------------
void modoconf() {


  pinMode(2, OUTPUT);
   

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssidConf,passConf);

  IPAddress myIP = WiFi.softAPIP(); 
  Serial.print("IP del acces point: ");
  Serial.println(myIP);
  Serial.println("WebServer iniciado...");

  server.on("/guardar_conf", guardar_conf); //Graba en la eeprom la configuracion

  
  server.begin();

  while (true) {
      server.handleClient();
      digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000);                       // wait for a second
      digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);
  }
}

//---------------------Guardar configuracion del WiFi -------------------------
void guardar_conf() {
  
  Serial.println(server.arg("ssid"));//Recibimos los valores que se envian desde la App
  grabar(0,server.arg("ssid"));
  Serial.println(server.arg("pass"));
  grabar(50,server.arg("pass"));

  mensaje = "Configuracion Guardada..";
  server.send(200, "text/html",  mensaje);

  
}

//----------------Función para grabar en la EEPROM-------------------
void grabar(int addr, String a) {
  int tamano = a.length(); 
  char inchar[50]; 
  a.toCharArray(inchar, tamano+1);
  for (int i = 0; i < tamano; i++) {
    EEPROM.write(addr+i, inchar[i]);
  }

  
  for (int i = tamano; i < 50; i++) {
       EEPROM.write(addr+i, 255);  
   }
  //al grabar las credenciales activamos el inicio en Modo Normal
   EEPROM.write(100, 0); 
   
  EEPROM.commit();
 
}

//-----------------Función para leer la EEPROM------------------------
String leer(int addr) {
   byte lectura;
   String strlectura;
   for (int i = addr; i < addr+50; i++) {
      lectura = EEPROM.read(i);
      if (lectura != 255) {
        strlectura += (char)lectura;
      }
   }
   return strlectura;
}


byte modo;

void setup() {

  ///Matrix initialization
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(colors[0]);

  //EEPROM init
  EEPROM.begin(512);  

  // MQTT and ESP initialization 
  Serial.begin(115200);
  delay(10);



  modo = EEPROM.read(100);
  


  //pinMode(14, INPUT_PULLUP);  //Pin de datos como entrada-->si PIN==0 configuracion de WIFI sino-->funcionamiento normal
  if (modo == 255) {
    modoconf();
  }

  pinMode(14, OUTPUT);
  
  

  leer(0).toCharArray(ssid, 50);
  Serial.print(ssid);
  Serial.print(" ");
  
  leer(50).toCharArray(pass, 50);
  Serial.print(pass);

  setup_wifi();


  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&Modo);
  mqtt.subscribe(&Mensaje);
  mqtt.subscribe(&ColorLEDS);
  
}


int x = matrix.width();
int pasar;
String textoRecibido;
String ColorRecibido;
String ModoRecibido;


void loop() {

  
 
  MQTT_connect();

  
  digitalWrite(2, HIGH);
  Adafruit_MQTT_Subscribe *subscription;
  
  while ((subscription = mqtt.readSubscription(50))) {
    if (subscription == &Modo) {

      ModoRecibido =(char *)Modo.lastread;
      if(ModoRecibido=="FF"){

         //si recibimos este mensaje, activamos la opcion de inicion en modo configuracion
           EEPROM.write(100, 255);
           EEPROM.commit();
        
      }else if(M
      odoRecibido=="00"){
         //si recibimos este mensaje, DESACTIVAMOS la opcion de inicion en modo configuracion
           EEPROM.write(100, 0);
           EEPROM.commit();
        
        
      }
      
     

  
    }else if(subscription == &Mensaje){
      
      textoRecibido = (char *)Mensaje.lastread;
      
      int longitud =LongitudCadena(textoRecibido);

      
       Serial.print(F("Mensaje Recibido: "));
       Serial.println(textoRecibido);
       Serial.print(F("Longitud: "));
       Serial.println(longitud);
            
       
      
    }else if(subscription == &ColorLEDS){

       ColorRecibido = (char *)ColorLEDS.lastread;
       Serial.print(F("\nColor de LEDS: "));
       
       if(ColorRecibido=="Rojo"){

        Serial.print(F("texto en Rojo "));
        pasar=0;
        
       }else if(ColorRecibido=="Verde"){

        Serial.print(F("texto en Verde "));
        pasar=1;
        
       }else if(ColorRecibido=="Azul"){

        Serial.print(F("texto en Azul "));
        pasar=2;
        
       }else{

        Serial.print(F("Color invalido.. :/ "));
        
       }
      

      
      
    }
    
  }

 
  mostrarTexto();

  digitalWrite(2, HIGH);

  


  

}// Fin de Loop




void mostrarTexto(){
  
  
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  
  matrix.print(textoRecibido);
  int le = (textoRecibido.length() * 7);
  
  if(--x < -le) {
    x = matrix.width();
    matrix.setTextColor(colors[pasar]);
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
       }
  }
  Serial.println("MQTT Connected!");
   
  
}
