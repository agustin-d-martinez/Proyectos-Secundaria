//----------------LIBRERIAS-------------------------

#include "DHT.h"                    // DHT22
#include <SPI.h>                    // SD
#include <ESP8266WiFi.h>            // NODE MCU 

//-------------------------------------------------


#define DHTPIN 13                   // DHT pin (D7=13)
#define DHTTYPE DHT22


WiFiServer server(80);              // lanzamiento al servidor.


IPAddress ip(192, 168, 0, 200);      // Fuerzo un IP.
IPAddress gateway(192,168,0,1);     // gateway del Router.
IPAddress subnet(255,255,255,0);    // Mascara del Router.

DHT dht(DHTPIN, DHTTYPE);

//-----------------------VARIABLES----------------------
char ssid[] = "Fibertel WiFi707 2.4GHz";     // Nombre de la Red.
char pass[] = "0041830982";       // Contraseña de la Red WiFi.
unsigned long DHTtimer = 0;
float h, t;
unsigned long clientTimer = 0;
//------------------------------------------------------

void setup() {
  
Serial.begin(115200);                 
  dht.begin();
  server_start(0);                           // inicia el servidor WiFi.
  delay(2000);
 
}

void loop() {
  if (millis() > DHTtimer + 2000) 
  {
    h = dht.readHumidity();                   // Lee la Humedad del Sensor.
    t = dht.readTemperature();                // Lee la Temperatura del Sensor.

    if (isnan(h) || isnan(t)) 
    {
      return;
    } 
     else 
    {
      DHTtimer = millis();
    }
  }

  WiFiClient client = server.available();
 
  if (client) 
  {
    if (client.connected()) 
    {
      String request = client.readStringUntil('\r');    // Lee el mesaje del cliente
      client.flush();
      client.println(String(t, 1));                     // envía la temperatura al cliente
    }
    client.stop();                                      // Desconecta al cliente.
    
    clientTimer = millis();

  }
  if (millis() - clientTimer > 25000) 
  {                 
    WiFi.disconnect();                                  // detiene y reinicia el servidor WiFi después de 30 segundos.
    delay(500);                                         // tiempo desconectado.
    server_start(1);                                    //vuelve a resetear
  }
}

void server_start(byte restart) 
{
  WiFi.config(ip, gateway, subnet);                    //obliga a usar el arreglo IP.
  WiFi.begin(ssid, pass);                              //se conecta al router WiFi.

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
  }
  //-----------------ESTADO DE CONEXION--------------------------
  server.begin();
  delay(500);
  clientTimer = millis();
  
  Serial.println("WiFi conectado");                   // al lector le indica que el Wifi esta conectado.
  
  Serial.print("La IP es: ");                         // al lector le indica la IP que tiene.
  Serial.println(WiFi.localIP());
  
  Serial.print("La Mascara es:");                     // al lector le indica la mascara de la Red.
  Serial.println(WiFi.subnetMask());
  
  Serial.print("El Gateway es: ");                    // al lector le indica que el Wifi esta conectado.
  Serial.println(WiFi.gatewayIP());
  
  Serial.print("La Red es: ");                        // al lector le indica en cual Red es la que esta conectado.
  Serial.println(WiFi.SSID());
 
  Serial.print("La MAC del NODE es: ");
  Serial.println(WiFi.macAddress());
  //------------------------------------------------------
  
  Serial.print("La Temperatura es:");
  Serial.print(t);
  Serial.println("*C");

  Serial.print("La Humedad es:");
  Serial.print(h);
  Serial.println("%");
}
