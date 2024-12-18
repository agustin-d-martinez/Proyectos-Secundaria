#include <TimerOne.h>
#include <DHT.h>
#include <Displays.h>

DHT dht(7,DHT22);

void setup() 
{
  pinMode(3,OUTPUT);//LED RGB ROJO
  pinMode(5,OUTPUT);//LED RGB VERDE
  pinMode(6,OUTPUT);//LED RGB AZUL
  dht.begin();
  pinMode(2,OUTPUT); //LAMPARA y BUZZER
  displays.init(8,9,10,11,12,13);
}
void loop() { 

  if(analogRead(1)>600)
  {
    displays.cargarDato(dht.readTemperature());
    if(dht.readTemperature()<20)
    {
      digitalWrite(5,HIGH); //se enciende el led rgb verde
      digitalWrite(6,LOW); //se APAGA el led rgb azul
      digitalWrite(3,LOW); //se apaga el led rgb rojo
    }
    if(dht.readTemperature()>20)
    {
      if(dht.readTemperature()<30)
      {
        digitalWrite(5,LOW); //se apaga el led rgb verde
        digitalWrite(6,HIGH); //se enciende el led rgb azul
        digitalWrite(3,LOW); //se apaga el led rgb rojo
      }
    }
    if(dht.readTemperature()>30)
    {
      if(dht.readTemperature()<40)
      {
        digitalWrite(6,LOW); //se apaga el led rgb AZUL
        digitalWrite(3,HIGH); //se enciende el led rgb ROJO
        digitalWrite(5,LOW); //se apaga el led rgb verde
      }
    }
    if(dht.readTemperature()>40)
    {
      digitalWrite(3,HIGH); //se apaga el led rgb rojo(ROSA)
      digitalWrite(2,HIGH); //se enciende la lampara Y EL BUZZER
      digitalWrite(5,LOW); //se apaga el led rgb verde
      digitalWrite(6,HIGH); //se enciende el led rgb azul(ROSA)
    }
    if(dht.readTemperature()<40)
    {
      digitalWrite(2,LOW); //se APAGA la lampara Y EL BUZZER
    }
  }
  if(analogRead(1)<500)
  {
    displays.cargarDato(dht.readHumidity());
    digitalWrite(3,HIGH); //se enciende el led rgb ROJO (AMARILLO)
    digitalWrite(5,HIGH); //se enciende el led rgb verde(AMARILLO)
    digitalWrite(2,LOW); //se APAGA la lampara Y EL BUZZER
  }

}