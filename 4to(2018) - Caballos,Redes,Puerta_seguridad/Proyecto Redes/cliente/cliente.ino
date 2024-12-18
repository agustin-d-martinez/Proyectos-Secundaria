// -------------------LIBRERIAS--------------------------

#include <SPI.h>
#include <ESP8266WiFi.h>

//------------------------------------------------------

//----------DATOS DEL SERVIDOR--------------------------

IPAddress server(192,168,0,200);     // IP del servidor.

//------------------------------------------------------

//----------DATOS DEL CLIENTE--------------------------

WiFiClient client;
IPAddress ip(192, 168, 0, 120);      // Fuerzo un IP.
IPAddress gateway(192,168,0,1);     
IPAddress subnet(255,255,255,0);    // Mascara del Router.

//-------------------------------------------------------

//-----------------VARIABLES-----------------------------

char ssid[] = "Fibertel WiFi707 2.4GHz";     //nombre de la Red WiFi.
char pass[] = "0041830982";       //contraseña de la Red WiFi.
byte led_prueba = 2;                // pin de led de prieba de conexion
unsigned long askTimer = 0;         // para que empiece a contar desde tiempo = 0
String answer;

//--------------------------------------------------------



void setup() {
  
  //--------------CONEXION WiFi-----------------------------
  
 Serial.begin(115200);
  WiFi.begin(ssid, pass);                         //se conecta al router WiFi.    
  while (WiFi.status() != WL_CONNECTED) {
  Serial.print(".");
    delay(500);
  Serial.println("WiFi conectado");
  }
  
  //--------------------------------------------------------
   
    pinMode(led_prueba, OUTPUT);          // led

//------------------------PINES RGB---------------------------

pinMode(0, OUTPUT);// led de ROJO.
pinMode(2, OUTPUT);// led de VERDE.
pinMode(4, OUTPUT);// led de AZUL.

//----------------------------------------------------------------


}

void loop () {
  
  if (millis() - askTimer > 2340) {                         // Tiempo de conexion:
    client.connect(server, 80);                             // se conecta al servidor
     digitalWrite(led_prueba, LOW);                         // Mostrar solo la comunicación
    Serial.println(".");
    client.println("Dale servidor, me estoy durmiendo\r");  // envía el mensaje al servidor
    answer = client.readStringUntil('\r');                  // recibe la respuesta del servidor
    Serial.print("La Temperatura es:");
    Serial.print(answer);
    Serial.println("*C");
    client.flush();
    digitalWrite(led_prueba, HIGH);                         // si reciebe el mensaje el led se enciende.
    askTimer = millis();
  }

  
 //---------------VUMETRO-------------------------------

if (answer.toFloat()==0 ) {                                                  //si la temperatura es igual a 0°C...
      Serial.println("Temperatura muy Fria.");                               // indica que la temperatura esta muy fria.
      
      digitalWrite(0,LOW);       // FORMA AZUL.
      digitalWrite(2,LOW);
      digitalWrite(4,HIGH);

    }
    if (answer.toFloat() < 10 && answer.toFloat() > 1) {                    //si la temperatura es entre 1 a  10°C...
      
      Serial.println("Temperatura Fria.");                                  // indica que la temperatura esta Fria.
   
      digitalWrite(0,LOW);      // FORMA CELESTE.
      digitalWrite(2,HIGH);
      digitalWrite(4,HIGH);

    }
    if (answer.toFloat() < 20 && answer.toFloat() > 11) {                   //si la temperatura es entre 11 a  20°C...
     
      Serial.println("Temperatura estable.");                               // indica que la temperatura esta estable.
      
      digitalWrite(0,LOW);     // FORMA VERDE
      digitalWrite(2,HIGH);
      digitalWrite(4,LOW);

    }
    if (answer.toFloat() < 25 && answer.toFloat() > 21) {                   //si la temperatura es entre 21 a  25°C...
      
      Serial.println("Temperatura caliente.");                              // indica que la temperatura esta  caliente.
    
      digitalWrite(0,HIGH);
      digitalWrite(2,HIGH);   // FORMA AMARILLO
      digitalWrite(4,LOW);
    }

    if (answer.toFloat() < 30 && answer.toFloat() > 26) {                   //si la temperatura es entre 26 a  30°C...
      
      Serial.println("Temperatura muy caliente.");                          // indica que la temperatura esta muy caliente.
      
      digitalWrite(0,HIGH);
      digitalWrite(2,LOW);    // FORMA ROJO
      digitalWrite(4,LOW);
    }

     if (answer.toFloat()> 31) {                                            //si la temperatura es mayor a 31°C...
      
      Serial.println("Temperatura hirbinedo.");                             // indica que la temperatura esta hirbinedo.
      
      digitalWrite(0,HIGH);
      digitalWrite(2,LOW);    // FORMA VIOLETA
      digitalWrite(4,HIGH);
    }
   
  //---------------------------------------------------
}
