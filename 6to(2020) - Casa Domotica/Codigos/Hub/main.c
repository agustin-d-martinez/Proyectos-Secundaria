#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#define MaxClients 11

// Hardcode WiFi parameters as this isn't going to be moving around.
const char* ssid = "Mansion Wayne 2.4GHZ";
const char* password = "pollito230502";

//Hardcode Server Adress
IPAddress ip(192,168,0,43);     
IPAddress gateway(192,168,0,1);   
IPAddress subnet(255,255,255,0); 
int aux=0;
//Hardcode Server Port (2000)
WiFiServer server(2000);
WiFiClient Clientes[MaxClients];
String  ultimo_msj="";
//Funcion Encargada del Envio de Datos
void Envio(String str) {
  int static i;
  for (i = 0; i < MaxClients; i++) {
    if (NULL != Clientes[i] && Clientes[i].connected()) {
      
        Clientes[i].print(str);
            
    }
    if (NULL != Clientes[i] and not Clientes[i].connected()) {
      Clientes[i].stop();
      
    }
  }
}

//Funcion Encargada del Procesamiento de Datos
void Proceso(String str){
  String static Msj = "";
  char static Val1 = 0, Val2 = 0, Val3 = 0, Reintentos = 0;  
  String static Val;
  int static XOR = 0;
  if(str.substring(3,5).compareTo("ER") == 0 ){
          aux=1;

    Envio(ultimo_msj);
    //Reintentos++;
    Msj=ultimo_msj;

  }
  else{ 
    aux=0; 
    //Si los Datos Provienen de los Dispositivos son Almacenados Para Luego ser Enviados a la App
    if(str.charAt(1) == 'H'){
      Reintentos = 0;
      switch(str.charAt(2)){
        case 'L':
        case 'C':
          XOR = 'A' xor str.charAt(2) xor str.charAt(3) xor str.charAt(4) xor str.charAt(5);
          Msj = "#A" + str.substring(2,6) + String(XOR) + "$";
          Envio(Msj);  
          break;
        
        case 'E':
          XOR = 'A' xor 'E' xor str.charAt(3);
          Msj = "#AE" + str.substring(3,4) + String(XOR) + "$";
          Envio(Msj);
          break; 
  
        case 'B':
          XOR = 'A' xor 'B' xor str.charAt(3) xor str.charAt(4) xor str.charAt(5);
          Msj = "#AB" + str.substring(3,6) + String(XOR) + "$";
          Envio(Msj);             
      }        
    }
  }
  
  //Si los Datos Provienen de la App son Enviados a los Dispositivos
  if(str.charAt(1) == 'R'){
    Reintentos = 0;
    switch(str.charAt(2)){
      case 'L':
        Val =  str.substring(3,6);
        Val1 = str.charAt(3); 
        Val2 = str.charAt(4);
        Val3 = str.charAt(5);
        XOR = 'L' xor Val1 xor Val2 xor Val3;  
        Msj = "#L" + Val + String(XOR) + "$";
        Envio(Msj);
        break;

      case 'C':
        Val = str.substring(3,6);
        Val1 = str.charAt(3); 
        Val2 = str.charAt(4);
        Val3 = str.charAt(5);
        XOR = 'C' xor Val1 xor Val2 xor Val3;  
        Msj = "#C" + Val + String(XOR) + "$";
        Envio(Msj);
        break;
      
      case 'E':
        Val1 = str.charAt(3);
        XOR = 'E' xor Val1; 
        Msj = "#E" + str.substring(3,4) + String(XOR) + "$";
        Envio(Msj);
        break; 

      case 'B':
        Val1 = str.charAt(3);
        XOR = 'B' xor Val1; 
        Msj = "#B" + str.substring(3,4) + String(XOR) + "$";
        Envio(Msj);
        break;
    }
  
  }
  Serial.println(aux);

  ultimo_msj=Msj;
}

//Funcion Encargada de la Recepcion de Datos
void Recibir(){
  String buff = "";
  int static i;
  for (i = 0; i < MaxClients; i++) {
    if (NULL != Clientes[i] and Clientes[i].available() > 0) {
      char charReceived = NULL;    
      while(charReceived != '$'){
        charReceived = Clientes[i].read();
        buff += charReceived;
      }
      Proceso(buff);//Se Procesa Cada Mensaje Recibido
      buff = "";    
   }
  }
}


void setup() {
  //Configuracion

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.println("");
  //Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start the TCP server
  server.begin();
}

void ClientsManager(){
 int static i;
  //Checks if theres a new client:
  WiFiClient client = server.available();
  if (client) {
    for (i = 0; i < MaxClients; i++) {
      if (NULL == Clientes[i]) {
        Clientes[i] = client;
        break;
      }
    }
  }
}

void loop() {
  //Programa
  
  ClientsManager();
  
  Recibir();
  
}