#include "ESP8266WiFi.h"

//Este programa escanea las redes disponibles y las muestra
void setup() {
  // put your setup code here, to run once:
      Serial.begin(115200);
       WiFi.mode(WIFI_STA);
       WiFi.disconnect();
       delay(100);
       Serial.println("Configuracion completa");
}

void loop() {
  Serial.println("Buscando WIFIs");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("Listo");
  if (n == 0)
    Serial.println("No encuentro redes disponibles");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID y RSSI para cada una
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(50000);

}
