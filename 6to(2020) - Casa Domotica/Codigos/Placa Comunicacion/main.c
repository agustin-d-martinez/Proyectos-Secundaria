#include "mbed.h"

//PROTOCOLO
#define PROTOCOL_START  '#'
#define DEVICE          'L'
#define PROTOCOL_END    '$'

//Respuesta ESP01
#define NADA    0
#define OK      1
#define ERROR   2

//Coneccion WIFI
#define SSID        "TeleCentro-71d9" //Nombre de la red WIFI
#define PASSWORD    "Q2N2DNGDMHFM" //Contraseña de la red WIFI

//Coneccion SERVER
#define PROTOCOL    "TCP" //Protocolo de comunicacion (TCP / UDP)
#define SERVER_IP   "192.168.0.6" // IP del servidor con el que se desea establecer un canal de comunicacion
#define PORT        "2302" // Puerto en el que el servidor "escucha"

Serial Esp(PTE0, PTE1), Pc(USBTX, USBRX);

Ticker Timer1;

//Estados de Inicializacion
enum {
    ECO = 0,
    MODO,
    CONECCION_RED,
    CONECCION_SERVER,
    MODO_COM,
    ESPERA,
    FIN_INICIALIZACION
};

//Estados de Recepcion de protocolo
enum {
    INICIO = 0,
    DISPOSITIVO,
    DATOA,
    DATOB,
    DATOC,
    VERIFICAR
};

//Variables utilizadas
bool Inicializado = false, Procesar = false, Err = false;
unsigned char RespuestaAT = NADA, i = 0, v = 0, Val = 0, DatoA = 0, DatoB = 0, DatoC = 0, Disp = 0, Verificacion = 0, Ver = 0, cont = 0, aux = 0, t = 0;
char Buffer[50], s[10];

//Funcion para inicializar el ESP01
void Inicializar()
{
    static unsigned char Estado = ECO, EstadoSiguiente = NADA;// Reintentos = 0;

    switch(Estado) {
        default:
        case ECO:
            Esp.printf("ATE0\r\n");
            Pc.printf("ATE0\r\n");
            EstadoSiguiente = MODO;
            Estado = ESPERA;
            break;

        case MODO:
            Esp.printf("AT+CWMODE_CUR=1\r\n");
            Pc.printf("AT+CWMODE_CUR=1\r\n");
            EstadoSiguiente = CONECCION_RED;
            Estado = ESPERA;
            break;

        case CONECCION_RED:
            Esp.printf("AT+CWJAP_CUR=%c%s%c,%c%s%c\r\n",34,SSID,34,34,PASSWORD,34);
            Pc.printf("AT+CWJAP_CUR=%c%s%c,%c%s%c\r\n",34,SSID,34,34,PASSWORD,34);
            EstadoSiguiente = CONECCION_SERVER;
            Estado = ESPERA;
            break;

        case CONECCION_SERVER:
            Esp.printf("AT+CIPSTART=%c%s%c,%c%s%c,%s\r\n",34,PROTOCOL,34,34,SERVER_IP,34,PORT);
            Pc.printf("AT+CIPSTART=%c%s%c,%c%s%c,%s\r\n",34,PROTOCOL,34,34,SERVER_IP,34,PORT);
            EstadoSiguiente = MODO_COM;
            Estado = ESPERA;
            break;

        case MODO_COM:
            Esp.printf("AT+CIPMODE=0\r\n");
            Pc.printf("AT+CIPMODE=0\r\n");
            EstadoSiguiente = FIN_INICIALIZACION;
            Estado = ESPERA;
            break;

        case ESPERA:
            
            if(RespuestaAT == OK) {
                if(EstadoSiguiente == FIN_INICIALIZACION)
                    Inicializado = true;
                else {
                    Estado = EstadoSiguiente;
                    RespuestaAT = NADA;
                    //Reintentos = 0;
                }
                
            }

            if(RespuestaAT == ERROR) {
                       
                    Estado = EstadoSiguiente - 1;
                    
                    
                } 

            
            
    }
}


//Funcion encargada de la recepcion de los datos desde el ESP01
void Datos(char Dato)
{
    static unsigned char Estado = INICIO;
    if(not Inicializado) {
        
        switch (aux) {
            default:
            case 0:
                if(Dato == 'O')
                    aux = 10;
                if(Dato == 'E')
                    aux = 1;
                break;
            case 1:
                if(Dato == 'R')
                    aux = 2;
                else
                    aux = 0;
                break;
            case 2:
                if(Dato == 'R')
                    aux = 3;
                else
                    aux = 0;
                break;
            case 3:
                if(Dato == 'O')
                    aux = 4;
                else
                    aux = 0;
                break;
            case 4:
                if(Dato == 'R')
                    RespuestaAT = ERROR;
                aux = 0;
                break;
            case 10:
                if(Dato == 'K')
                    aux = 11;
                else
                    aux = 0;
                break;
            case 11:
                if(Dato == '\r')
                    aux = 12;
                else
                    aux = 0;
                break;
            case 12:
                if(Dato == '\n')
                    RespuestaAT = OK;
                aux = 0;
        }
        
    } else {

        
        switch(Estado) {
            case INICIO:
                if(Dato == PROTOCOL_START)
                    Estado = DISPOSITIVO;
                break;

            case DISPOSITIVO:
                Estado = DATOA;
                Disp = Dato;
                Verificacion = Dato;
                break;

            case DATOA:
                DatoA = Dato;
                Verificacion ^= Dato;
                Estado = DATOB;
                break;

            case DATOB:
                DatoB = Dato;
                Verificacion ^= Dato;
                Estado = DATOC;
                break;

            case DATOC:
                DatoC = Dato;
                Verificacion ^= Dato;
                Estado = VERIFICAR;
                break;

            case VERIFICAR:

                if(Dato != PROTOCOL_END) {
                    s[v++] = Dato;
                } else {
                    v = 0;
                    Estado = INICIO;
                    Ver = atoi(s);
                    strcpy(s,"");
                    Procesar = true;
                }
        }
        Pc.putc(Dato);
    }
}

//Funcion que procesa los datos una vez inicializado el ESP01
void Proceso()
{
    if(not Procesar) {
        return;
    } else {
        Procesar = false;
        if(Disp != DEVICE) {
            return;
        } else {
            if(Verificacion != Ver) {
                Esp.printf("AT+CIPSEND=7\r\n");
                t = 3;
                Err = true;
                return;
            }
            Val = ((DatoA - 48) * 100) + ((DatoB - 48) * 10) + (DatoC - 48);
            if(Val > 100) {
                Esp.printf("AT+CIPSEND=7\r\n");
                t = 3;
                Err = true;
                return;
            }
            //hacer con el valor lo que se tenga que hacer
        }
    }
}

//Interrupcion del ESP01
void RxEsp()
{
   
        while(Esp.readable()) {
            Datos(Esp.getc());
            //Pc.putc(Esp.getc());
        }
}

//CONTROL POR PC (COMENTAR SI NO SE ESTRA PROBANDO)
void control(){
    while(Pc.readable()){
        Esp.putc(Pc.getc());
    }
}

//Interrupcion del Timer
void Time(){
    if(t > 0)
        t--;
    if(not t and Err){
        Err = false;
        Esp.printf("%c%cER23%c", PROTOCOL_START, DEVICE, PROTOCOL_END);
    }
}

int main()
{
    Pc.baud(115200);
    Esp.baud(115200);
    Esp.attach(&RxEsp, Serial::RxIrq);
    Pc.attach(&control, Serial::RxIrq);
    Timer1.attach(&Time, 0.01);
    while(1) {
        if(not Inicializado)
            Inicializar();
        Proceso();
        
    }
}