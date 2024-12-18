#include "mbed.h"
#include "string.h"

//PROTOCOLO
#define PROTOCOL_START  '#'
#define DEVICE          'B'
#define PROTOCOL_END    '$'
#define HUB             'H'

//Respuesta ESP01
#define NADA    0
#define OK      1
#define ERROR   2

//Coneccion WIFI
#define SSID        "Mansion Wayne 2.4GHZ" //Nombre de la red WIFI
#define PASSWORD    "pollito230502" //Contraseña de la red WIFI

//Coneccion SERVER
#define PROTOCOL    "TCP" //Protocolo de comunicacion (TCP / UDP)
#define SERVER_IP   "192.168.0.43" // IP del servidor con el que se desea establecer un canal de comunicacion
#define PORT        "2000" // Puerto en el que el servidor "escucha"



//tiempos TIMER
#define TIEMPO 0.01
#define TIEMPO_MANDAR_DATOS 10
#define TIEMPO_CUENTA TIEMPO_MANDAR_DATOS/TIEMPO
#define REBOTE 0.03f
#define CALCULO_REBOTE REBOTE/TIEMPO
#define TIEMPO_TITILAR 0.5
#define CALCULO_TIEMPO_TITILAR TIEMPO_TITILAR/TIEMPO 
Serial Esp(PTE22, PTE23), Pc(USBTX, USBRX);

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
    DATO,
    VERIFICAR
};

//Estados de antirrebote
enum {
    LIBRE = 0,
    DEMORA,
    PULSADO
};
//Variables utilizadas
bool Inicializado = false, Procesar = false, Err = false, mandar = false, nuevo_mensaje = false;
unsigned char RespuestaAT = NADA,Estado_inicializacion = ECO,EstadoSiguiente = NADA, i = 0, v = 0, Val = 0, DatoA = 0, Disp = 0, Verificacion = 0, Ver = 0, aux = 0, t = 0, fin_carrera1 = 0,fin_carrera2 = 0,bomba_dato = 0,rebote = 0, estado_antirrebote = 0,Epul= 0, Epul_anterior = 0;
unsigned int ver_enviada = 10, cont = 0;
char s[10] = {0};

void MANDAR_DATOS();
void BOMBA_AGUA();
void antirrebote();

//---Entradas y salidas-----------------
DigitalOut bomba(PTC11);
DigitalOut led(PTC7);
DigitalIn pul(PTC12);
DigitalIn low_level(PTC16);
DigitalIn up_level(PTC13);

//Funcion para inicializar el ESP01
void Inicializar()
{
    switch(Estado_inicializacion) {
        default:
        case ECO:
            Esp.printf("ATE0\r\n");
            EstadoSiguiente = MODO;
            Estado_inicializacion = ESPERA;
            break;
        case MODO:
            Esp.printf("AT+CWMODE_CUR=1\r\n");
            EstadoSiguiente = CONECCION_RED;
            Estado_inicializacion = ESPERA;
            break;
        case CONECCION_RED:
            Esp.printf("AT+CWJAP_CUR=%c%s%c,%c%s%c\r\n",34,SSID,34,34,PASSWORD,34);
            EstadoSiguiente = CONECCION_SERVER;
            Estado_inicializacion = ESPERA;
            break;
        case CONECCION_SERVER:
            Esp.printf("AT+CIPSTART=%c%s%c,%c%s%c,%s\r\n",34,PROTOCOL,34,34,SERVER_IP,34,PORT);
            EstadoSiguiente = MODO_COM;
            Estado_inicializacion = ESPERA;
            break;
        case MODO_COM:
            Esp.printf("AT+CIPMODE=0\r\n");
            EstadoSiguiente = FIN_INICIALIZACION;
            Estado_inicializacion = ESPERA;
            break;
        case ESPERA:
            if(RespuestaAT == OK) {
                if(EstadoSiguiente == FIN_INICIALIZACION)
                    Inicializado = true;
                else {
                    Estado_inicializacion = EstadoSiguiente;
                    RespuestaAT = NADA;
                }
            }
            if(RespuestaAT == ERROR) {
                Estado_inicializacion = EstadoSiguiente - 1;
                RespuestaAT = NADA;
            }
    }
}

//Funcion encargada de la recepcion de los datos desde el ESP01
void Datos(char Dato)
{
    static unsigned char Estado = INICIO;
    if(not Inicializado) {
        switch (aux)
        {
        default:
        case 0:
            if (Dato == 'O')
                aux = 10;
            if (Dato == 'E' && EstadoSiguiente != MODO_COM)
                aux = 1;
            if (Dato == 'C')
                aux = 20;
            break;
        case 1:
            if (Dato == 'R')
                aux = 2;
            else
                aux = 0;
            break;
        case 2:
            if (Dato == 'R')
                aux = 3;
            else
                aux = 0;
            break;
        case 3:
            if (Dato == 'O')
                aux = 4;
            else
                aux = 0;
            break;
        case 4:
            if (Dato == 'R')
                RespuestaAT = ERROR;
            aux = 0;
            break;
        case 10:
            if (Dato == 'K')
                aux = 11;
            else
                aux = 0;
            break;
        case 11:
            if (Dato == '\r')
                aux = 12;
            else
                aux = 0;
            break;
        case 12:
            if (Dato == '\n')
                RespuestaAT = OK;
            aux = 0;
        case 20:
            if (Dato == 'L')
                aux = 21;
            else
                aux = 0;
            break;

        case 21:
            if (Dato == 'O')
                aux = 22;
            else
                aux = 0;
            break;

        case 22:
            if (Dato == 'S')
                aux = 23;
            else
                aux = 0;
            break;

        case 23:
            if (Dato == 'E')
                aux = 24;
            else
                aux = 0;
            break;

        case 24:
            if (Dato == 'D')
                aux = 25;
            else
                aux = 0;
            break;

        case 25:
            if (Dato == '\r')
                aux = 26;
            else
                aux = 0;
            break;

        case 26:
            if (Dato == '\n')
            {
                aux = 0;
                RespuestaAT = ERROR;
            }
            break;
        }
    } else {
        switch(Estado) {
            case INICIO:
                if(Dato == 'C')
                    Estado = 20;
                if(Dato == PROTOCOL_START)
                    Estado = DISPOSITIVO;
                break;
            case DISPOSITIVO:
                Estado = DATO;
                Disp = Dato;
                Verificacion = Dato;
                break;

            case DATO:
                DatoA = Dato;
                Verificacion ^= Dato;
                Estado = VERIFICAR;
                break;
            case 20:
                if(Dato == 'L')
                    Estado = 21;
                else
                    Estado = INICIO;
                break;
            case 21:
                if(Dato == 'O')
                    Estado = 22;
                else
                    Estado = INICIO;
                break;
            case 22:
                if(Dato == 'S')
                    Estado = 23;
                else
                    Estado = INICIO;
                break;
            case 23:
                if(Dato == 'E')
                    Estado = 24;
                else
                    Estado = INICIO;
                break;
            case 24:
                if(Dato == 'D')
                    Estado = 25;
                else
                    Estado = INICIO;
                break;
            case 25:
                if(Dato == '\r')
                    Estado = 26;
                else
                    Estado = INICIO;
                break;
            case 26:
                if(Dato == '\n'){
                    Inicializado = false;
                    Estado_inicializacion = ECO;
                    }
                Estado = INICIO;
                break;
            case VERIFICAR:
                if(Dato != PROTOCOL_END) {
                    s[v++] = Dato;
                } else {
                    v = 0;
                    Estado = INICIO;
                    Ver = atoi(s);
                    memset( s, 0, 10 );
                    Procesar = true;
                }
                
        }
        //Pc.putc(Dato);
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
            Val = DatoA - 48;
            if(Val > 1) {
                Esp.printf("AT+CIPSEND=7\r\n");
                t = 3;
                Err = true;
                return;
            }
            nuevo_mensaje = true;
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


//Interrupcion del Timer
void Time()
{
    rebote++;
    cont++;
    if(t > 0)
        t--;
    if(not t and Err) {
        Err = false;
        Esp.printf("%c%cER95%c", PROTOCOL_START, HUB, PROTOCOL_END);
    }
}

void titilar(){
    if(cont >= CALCULO_TIEMPO_TITILAR){
        if(led == 1)
            led = 0;
        else
            led = 1;
        cont = 0; 
        }
    }

int main()
{
    Pc.baud(115200);
    Esp.baud(115200);
    Esp.attach(&RxEsp, Serial::RxIrq);
    Timer1.attach(&Time, TIEMPO);
    while(1) {
        if(not Inicializado)
            Inicializar();
        Proceso();
        
        if(low_level == 0 && up_level == 0 )
            led = 0;
        if(low_level == 1 && up_level == 0 )
            titilar();
        if(low_level == 1 && up_level == 1 )
            led = 1;
            
        BOMBA_AGUA();
        /*if(cont >= TIEMPO_CUENTA && Err == false && Inicializado == true) {
            cont = 0;
            MANDAR_DATOS();
        }*/
        antirrebote();
    }
}

void BOMBA_AGUA()
{
    if(Inicializado == true && nuevo_mensaje == true){
        bomba = Val;
        nuevo_mensaje = false;}

    if(Epul == 1 && Epul_anterior == 0 && low_level == 1){
        Epul_anterior = 1;
        if(bomba == 1)
            bomba = 0;
        else
            bomba = 1;}
}

void MANDAR_DATOS()
{
    if(low_level == 1)
        fin_carrera1 = '1';
    else
        fin_carrera1 = '0';
    if(up_level == 1)
        fin_carrera2 = '1';
    else
        fin_carrera2 = '0';
    if(bomba == 1)
        bomba_dato = '1';
    else
        bomba_dato = '0';
    ver_enviada = 'H' ^ 'B' ^ bomba_dato ^ fin_carrera1 ^ fin_carrera2;     
    Esp.printf("AT+CIPSEND=9\r\n");
    mandar = true;
    t = 3;
}


void antirrebote(){
    switch(estado_antirrebote) {
        case LIBRE:
            Epul = 0;
            if(pul == 1) {
                estado_antirrebote = DEMORA;
                rebote = 0;
            }
            break;
        case DEMORA:
            Epul = 0;
            if(rebote > CALCULO_REBOTE && pul == 1){
                estado_antirrebote = PULSADO;
                Epul_anterior = 0;}
            if(rebote > CALCULO_REBOTE && pul == 0)
                estado_antirrebote = LIBRE;
            break;
        case PULSADO:
            Epul= 1;
            if(pul == 0){
                estado_antirrebote = LIBRE;
                Epul_anterior = 1;
            }
            break;
    }
}