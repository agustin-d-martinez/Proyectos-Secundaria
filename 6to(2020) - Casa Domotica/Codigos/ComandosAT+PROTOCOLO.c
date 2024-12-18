#include "mbed.h"
#include "string.h"

/*PROTOCOLO:
    [#][PLACA][DATO1][DATO2][SUMA][$]
*/
#define car_INICIO '#'
#define car_PLACA 'L'
#define car_FINAL '$'
#define car_PLACA_HUB 'H'

#define TIEMPO 0.1f
#define TIEMPO_ESPERA 3
#define TIEMPO_ESPERA_CUENTA TIEMPOR_ESPERA/TIEMPO

#define NOMBRE_WIFI "TeleCentro-71d9"
#define CONTRASENA "Q2N2DNGDMHFM"
#define IP_SERVIDOR "192.168.0.6"
#define PUERTO "2302"

#define NADA 0
#define OK 1
#define ERROR 2
//Definicion del puerto serie utilizado
Serial wifi(PTE0, PTE1);
Serial  pc(USBTX, USBRX);

//Estados de la funcion inicializar----------
int estado_arranque;
int reintentos = 0;
enum {
    ECO = 0,
    MODO,
    CONECCION_RED,
    CONECTAR_SERVIDOR,
    MODO_COMUNICACION,
    ESPERANDO_RESPUESTA
};
int proximo_estado=0;
//------------------------------------------

//Estados de la funcion Datos---------------
int estado;
bool mensaje_recibido = false;
char suma= 0;
enum {
    INICIO = 0,
    PLACA,
    DATO1,
    DATO2,
    VALIDACION,
    FIN
};
int respuestaAT = NADA;
int aux = 0;
//------------------------------------------

//temporizador--------------------------
Ticker temporizador;
int cont = 0;
void timer ()
{
    cont++;
}
//--------------------------------------

//Dimmer--------------------------------
int porcentaje_recibido = 0;
int porcentaje = 0;
//--------------------------------------
bool inicializado = false;

void inicializar()
{
    switch (estado_arranque) {
        default:
        case ECO:
            wifi.printf("ATE0\r\n");
            pc.printf("ATE0\r\n");
            proximo_estado = MODO;
            estado_arranque = ESPERANDO_RESPUESTA;
            break;
        case MODO:
            wifi.printf("AT+CWMODE_CUR=1\r\n");
            pc.printf("AT+CWMODE_CUR=1\r\n");
            proximo_estado = CONECCION_RED;
            estado_arranque = ESPERANDO_RESPUESTA;
            break;
        case CONECCION_RED:
            wifi.printf("AT+CWJAP_CUR=%c%s%c,%c%s%c\r\n",34,NOMBRE_WIFI,34,34,CONTRASENA,34);
            pc.printf("AT+CWJAP_CUR=%c%s%c,%c%s%c\r\n",34,NOMBRE_WIFI,34,34,CONTRASENA,34);
            proximo_estado = MODO_COMUNICACION;
            estado_arranque = ESPERANDO_RESPUESTA;
            break;
        case CONECTAR_SERVIDOR:
            wifi.printf("AT+CIPSTART=%cTCP%c,%c%s%c,%s\r\n",34,34,34,IP_SERVIDOR,34,PUERTO);
            pc.printf("AT+CIPSTART=%cTCP%c,%c%s%c,%s\r\n",34,34,34,IP_SERVIDOR,34,PUERTO);            
            proximo_estado = 100;
            estado_arranque = ESPERANDO_RESPUESTA;
            break;
        case MODO_COMUNICACION:
            wifi.printf("AT+CIPMODE=1\r\n");
            pc.printf("AT+CIPMODE=1\r\n");
            proximo_estado = CONECTAR_SERVIDOR;
            estado_arranque = ESPERANDO_RESPUESTA;
            break;
        case ESPERANDO_RESPUESTA:
            if(inicializado == false) {
                if(respuestaAT == OK ) {
                    respuestaAT = NADA;
                    if(proximo_estado == 100) {
                        inicializado = true;
                    } else
                        estado_arranque = proximo_estado;
                }
                if(respuestaAT == ERROR && reintentos < 3) {
                    respuestaAT = NADA;
                    reintentos++;
                    estado_arranque = proximo_estado-1;
                }
                if(respuestaAT == ERROR && reintentos >= 3)
                    inicializado = false;
            }
            break;
    }
}


//Funcion que revisa el mensaje
void Datos(unsigned char byte_recibido)
{
   
    if(inicializado == true) {
        switch (estado) {
            default:
            case INICIO:
                if(byte_recibido == car_INICIO )
                    estado = PLACA;
                cont = 0;
                break;
            case PLACA:
                if(byte_recibido == car_PLACA)
                    estado = DATO1;
                else {
                    estado= INICIO;
                }
                /*if(cont > TIEMPO_ESPERA_CUENTA)
                    estado = INICIO;*/
                break;
            case DATO1:
                if(byte_recibido >= 0x8C && byte_recibido <= 0xF0) {
                    estado = DATO2;
                    porcentaje_recibido = byte_recibido - 140;
                } else {
                    suma = 'E'+'R';
                    wifi.printf("%c%cER%c%c\r\n",car_INICIO,car_PLACA_HUB,suma,car_FINAL); //Mensaje error
                    estado= INICIO;
                }
                /* if(cont > TIEMPO_ESPERA_CUENTA){
                     estado = INICIO;
                     suma = 'E'+'R';
                     wifi.printf("%c%cER%c%c",car_INICIO,car_PLACA_HUB,suma,car_FINAL); //Mensaje error}*/
                break;
            case DATO2:
                if(byte_recibido == '0') {
                    estado = VALIDACION;
                } else {
                    suma = 'E'+'R';
                    wifi.printf("%c%cER%c%c\r\n",car_INICIO,car_PLACA_HUB,suma,car_FINAL); //Mensaje error
                    estado= INICIO;
                }
                /* if(cont > TIEMPO_ESPERA_CUENTA){
                    estado = INICIO;
                    suma = 'E'+'R';
                    wifi.printf("%c%cER%c%c",car_INICIO,car_PLACA_HUB,suma,car_FINAL); //Mensaje error}*/
                break;
            case VALIDACION:
                if(byte_recibido == porcentaje_recibido + 140 + '0') {
                    estado = FIN;
                } else {
                    suma = 'E'+'R';
                    wifi.printf("%c%cER%c%c\r\n",car_INICIO,car_PLACA_HUB,suma,car_FINAL); //Mensaje error
                    estado= INICIO;
                }
                break;
            case FIN:
                if(byte_recibido == car_FINAL) {
                    estado = INICIO;
                    mensaje_recibido = true;
                    suma = 'O'+'K';
                    wifi.printf("%c%cOK%c%c\r\n",car_INICIO,car_PLACA_HUB,suma,car_FINAL); //Mensaje recibido
                } else {
                    suma = 'E'+'R';
                    wifi.printf("%c%cER%c%c\r\n",car_INICIO,car_PLACA_HUB,suma,car_FINAL); //Mensaje error
                    estado= INICIO;
                }
                /* if(cont > TIEMPO_ESPERA_CUENTA){
                    estado = INICIO;
                    suma = 'E'+'R';
                    wifi.printf("%c%cER%c%c",car_INICIO,car_PLACA_HUB,suma,car_FINAL); //Mensaje error}*/
                break;
        }
    } //LECTURA DE DATOS ANTES DE INICIALIZARSE
    else {
        switch (aux) {
            default:
            case 0:
                if(byte_recibido == 'O')
                    aux = 10;
                if(byte_recibido == 'E')
                    aux = 1;
                break;
            case 1:
                if(byte_recibido == 'R')
                    aux = 2;
                else
                    aux = 0;
                break;
            case 2:
                if(byte_recibido == 'R')
                    aux = 3;
                else
                    aux = 0;
                break;
            case 3:
                if(byte_recibido == 'O')
                    aux = 4;
                else
                    aux = 0;
                break;
            case 4:
                if(byte_recibido == 'R')
                    respuestaAT = ERROR;
                aux = 0;
                break;
            case 10:
                if(byte_recibido == 'K')
                    aux = 11;
                else
                    aux = 0;
                break;
            case 11:
                if(byte_recibido == '\r')
                    aux = 12;
                else
                    aux = 0;
                break;
            case 12:
                if(byte_recibido == '\n')
                    respuestaAT = OK;
                aux = 0;
        }
    }
    
}

//Comprueba que se pueda leer
void onCharReceived()
{
    while(wifi.readable())
        
        Datos(wifi.getc());
        
}



int main()
{
    //Ejecutar onCharReceived por cada entrada por puerto
    pc.baud(115200);
    wifi.baud(115200);
    wifi.attach(&onCharReceived, Serial::RxIrq);
    
    temporizador.attach(&timer,TIEMPO);
    while (1) {
        if(inicializado == false)
            inicializar();
        else {
            //acá laburar   <<<<----------------------------------------------------------
        }
    }
}