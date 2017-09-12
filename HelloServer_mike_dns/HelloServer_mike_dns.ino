#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

char* ssid ="vodafone790E";
char* password="BXVISBZUNLQNP4";

ESP8266WebServer server(80);
String Argument_Name, Clients_Response;

const int led = 16;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  delay(1000);
  digitalWrite(led, 0);
  delay(1000);
  digitalWrite(led, 1);
  delay(1000);
  digitalWrite(led, 0);
  delay(1000);
  digitalWrite(led, 1);
  delay(1000);
  digitalWrite(led, 0);
}

void HandleClient() {
  String webpage;
  webpage =  "<html>";
   webpage += "<head><title>ESP8266 Input Example</title>";
    webpage += "<style>";
     webpage += "body { background-color: #E6E6FA; font-family: Arial, Helvetica, Sans-Serif; Color: blue;}";
    webpage += "</style>";
   webpage += "</head>";
   webpage += "<body>";
     String IPaddress = WiFi.localIP().toString();
     webpage += "<form action='http://"+IPaddress+"' method='POST'>";
     webpage += "Please enter your details:<input type='text' name='user_input'>&nbsp;<input type='submit' value='Enter'>";
    webpage += "</form>";
   webpage += "</body>";
  webpage += "</html>";
  server.send(200, "text/html", webpage); // Send a response to the client asking for input
  
  if (server.args() > 0 ) { // Arguments were received
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.print(server.argName(i)); // Display the argument
      Argument_Name = server.argName(i);
      if (server.argName(i) == "user_input") {
        Serial.print(" Input received was: ");
        Serial.println(server.arg(i));
        Clients_Response = server.arg(i);
        // e.g. range_maximum = server.arg(i).toInt();   // use string.toInt()   if you wanted to convert the input to an integer number
        // e.g. range_maximum = server.arg(i).toFloat(); // use string.toFloat() if you wanted to convert the input to a floating point number
      }
    }
  }
}






void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA); //Establece el módulo como cliente wifi
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //Se ilumina cuando esta enabled!
  digitalWrite(led, 0);

 ///Mostramos algunos PArametros de la conexion con el Wifi
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  
///Inicializamos el DNS para el servidor
//podemos acceder por IP o en la direccion: Control-LCDMultiLED.local
if (MDNS.begin("Control-LCDMultiLED",WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }


//definimos los paths
  server.on("/", HandleClient); // The client connected with no arguments e.g. http:192.160.0.40/
  server.on("/inline", [](){server.send(200, "text/plain", "this works as well");});
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
