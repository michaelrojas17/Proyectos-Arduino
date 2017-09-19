#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

//-------------------VARIABLES GLOBALES--------------------------
int contconexion = 0;
unsigned long previousMillis = 0;

char ssid[50];      
char pass[50];

const char *ssidConf = "ESP-LEDS-WiFi";
const char *passConf = "12345678";

String mensaje = "";



//------------------------SETUP WIFI-----------------------------
void setup_wifi() {
// Conexión WIFI
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
     }
  else { 
      Serial.println("");
      Serial.println("Error de conexion");
  }
}

//--------------------------------------------------------------
WiFiClient espClient;
ESP8266WebServer server(80);

//--------------------MODO_CONFIGURACION------------------------
void modoconf() {
   

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
  }
}

//---------------------GUARDAR CONFIGURACION-------------------------
void guardar_conf() {
  
  Serial.println(server.arg("ssid"));//Recibimos los valores que envia por GET el formulario web
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


//------------------------SETUP-----------------------------
void setup() {

  pinMode(13, OUTPUT); // D7 
  
  // Inicia Serial
  Serial.begin(115200);
  Serial.println("");

  EEPROM.begin(512);

  pinMode(14, INPUT);  //D5
  if (digitalRead(14) == 0) {
    modoconf();
  }

  pinMode(14, OUTPUT);

  leer(0).toCharArray(ssid, 50);
  leer(50).toCharArray(pass, 50);

  setup_wifi();
}

//--------------------------LOOP--------------------------------
void loop() {


  

  unsigned long currentMillis = millis();
  unsigned int valor;
  
  pinMode(13, INPUT);
  digitalWrite(14, HIGH);

  if (currentMillis - previousMillis >= 5000) { 
    previousMillis = currentMillis;
    Serial.println("Funcionado...");
    valor = digitalRead(13);
    Serial.println("Valor: ");
    Serial.println(valor);
    
  }
}
