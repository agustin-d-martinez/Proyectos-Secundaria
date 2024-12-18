#include "mbed.h"

#define ON 1
#define OFF 0
//timer
#define DEBOUNCE_TIMER_BASE  0.001// Para 1 mili segundo.
#define TIEMPO_MANDAR_DATOS 10
#define TIEMPO_CUENTA TIEMPO_MANDAR_DATOS/DEBOUNCE_TIMER_BASE

//motor
#define timer_motor  3

//PROTOCOLO
#define PROTOCOL_START  '#'
#define DEVICE          'C'
#define PROTOCOL_END    '$'


#define HUB    'H'
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

//#define PASOS_MAXIMOS 3000

Serial Esp(PTE22, PTE23), Pc(USBTX, USBRX);

//Estados del Motor PP
enum {
    paso1=100,
    paso2,
    paso3,
    paso4,
    delay,
    horario,
    antihorario,

};

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
    VERIFICAR,
    CLOSED_L,
    CLOSED_O,
    CLOSED_S,
    CLOSED_E,
    CLOSED_D
};

//--Variables utilizadas------------------------
int PASOS_MAXIMOS = 0;
int estado_motor = paso1, aux_motor = 0, sentido = 0, paso_actual = 0, paso = 0;
unsigned int  timer_delay = 5, cont = 0;
bool Inicializado = false, Procesar = false, Err = false, nuevo_mensaje = false, mandar = false;
unsigned char RespuestaAT = NADA, i = 0, v = 0, Val = 0, DatoA = 0, DatoB = 0, DatoC = 0, Disp = 0, Verificacion = 0, Ver = 0, aux = 0, t = 0, dato_mandar = 0, ver_enviada = 0;
char Buffer[50], s[10], Msj[40] = "", EstadoInicializacion = ECO;
Ticker tik;

//--Entradas y Salidas--------------------------
DigitalIn pul_down(PTE5), pul_up(PTE4);
DigitalOut Bobina1A(PTC7), Bobina1B(PTC0), Bobina2A(PTC3), Bobina2B(PTC4);

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
                    aux = 5;
                else
                    aux = 0;
                break;
            case 5:
                if(Dato == '\r')
                    aux = 6;
                else
                    aux = 0;
                break;
            case 6:
                if(Dato == '\n')
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
                if(Dato == 'C')
                    Estado = CLOSED_L;
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
memset( s, 0, 10 );
                    Procesar = true;
                }
                break;
            
            case CLOSED_L:
                if(Dato == 'L'){
                    Estado = CLOSED_O;
                }
                else{
                    Estado = INICIO;
                }
                break;
            
            case CLOSED_O:
                if(Dato == 'O'){
                    Estado = CLOSED_S;
                }
                else{
                    Estado = INICIO;
                }
                break;
            
            case CLOSED_S:
                if(Dato == 'S'){
                    Estado = CLOSED_E;
                }
                else{
                    Estado = INICIO;
                }
                break;
            
            case CLOSED_E:
                if(Dato == 'E'){
                    Estado = CLOSED_D;
                }
                else{
                    Estado = INICIO;
                }
                break;
            
            case CLOSED_D:
                if(Dato == 'D'){
                    Inicializado = false;
                    EstadoInicializacion = CONECCION_RED;
                    Pc.printf("COMUNICACION CERRADA");
                }
                Estado = INICIO;
                break;
                
        }
        Pc.putc(Dato);
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
void control()
{
    while(Pc.readable()) {
        Esp.putc(Pc.getc());
    }
}

//Timer
void fn_timer()
{
    //cont++;
    if(t > 0)
        t--;
    if(not t and Err) {
        Err = false;
        Esp.printf("%c%c%cER28%c", PROTOCOL_START,HUB, DEVICE, PROTOCOL_END);
        
    }
    if(estado_motor==delay)
            timer_delay++;
}

//Funcion Encargada del Movimiento del Motor PP
void movimiento_motor()
{
    switch(estado_motor) {
        case paso1:
            if(sentido == horario) {
                Bobina1A = OFF;
                Bobina1B = OFF;
                Bobina2A = OFF;
                Bobina2B = ON;
            }
            if(sentido == antihorario) {
                Bobina1A = ON;
                Bobina1B = OFF;
                Bobina2A = OFF;
                Bobina2B = OFF;
            }
            aux_motor = 1;
            estado_motor = delay;
            break;

        case paso2:
            if(sentido == horario) {
                Bobina1A = OFF;
                Bobina1B = OFF;
                Bobina2A = ON;
                Bobina2B = OFF;
            }
            if(sentido == antihorario) {
                Bobina1A = OFF;
                Bobina1B = ON;
                Bobina2A = OFF;
                Bobina2B = OFF;
            }
            aux_motor = 2;
            estado_motor = delay;
            break;

        case paso3:
            if(sentido == horario) {
                Bobina1A = OFF;
                Bobina1B = ON;
                Bobina2A = OFF;
                Bobina2B = OFF;
            }
            if(sentido == antihorario) {
                Bobina1A = OFF;
                Bobina1B = OFF;
                Bobina2A = ON;
                Bobina2B = OFF;
            }
            aux_motor = 3;
            estado_motor = delay;
            break;

        case paso4:
            if(sentido == horario) {
                Bobina1A = ON;
                Bobina1B = OFF;
                Bobina2A = OFF;
                Bobina2B = OFF;
            }
            if(sentido == antihorario) {
                Bobina1A = OFF;
                Bobina1B = OFF;
                Bobina2A = OFF;
                Bobina2B = ON;
            }
            aux_motor = 4;
            estado_motor = delay;
            break;

        case delay:
            if(timer_delay >= timer_motor) {
                timer_delay = 0;
                if(aux_motor==1)estado_motor=paso2;
                if(aux_motor==2)estado_motor=paso3;
                if(aux_motor==3)estado_motor=paso4;
                if(aux_motor==4)estado_motor=paso1;
            }
            break;
    }
    if(sentido == horario && estado_motor != delay)
        paso_actual--;
    if(sentido == antihorario && estado_motor != delay){
        paso_actual++;
    }
}

//Funcion para inicializar el ESP01
void Inicializar()
{
    static char  EstadoSiguiente = NADA;// Reintentos = 0;

    switch(EstadoInicializacion) {
        default:
        case ECO:
            Esp.printf("ATE0\r\n");
            Pc.printf("ATE0\r\n");
            EstadoSiguiente = MODO;
            EstadoInicializacion = ESPERA;
            break;

        case MODO:
            Esp.printf("AT+CWMODE_CUR=1\r\n");
            Pc.printf("AT+CWMODE_CUR=1\r\n");
            EstadoSiguiente = CONECCION_RED;
            EstadoInicializacion = ESPERA;
            break;

        case CONECCION_RED:
            Esp.printf("AT+CWJAP_CUR=%c%s%c,%c%s%c\r\n",34,SSID,34,34,PASSWORD,34);
            Pc.printf("AT+CWJAP_CUR=%c%s%c,%c%s%c\r\n",34,SSID,34,34,PASSWORD,34);
            EstadoSiguiente = CONECCION_SERVER;
            EstadoInicializacion = ESPERA;
            break;

        case CONECCION_SERVER:
            Esp.printf("AT+CIPSTART=%c%s%c,%c%s%c,%s\r\n",34,PROTOCOL,34,34,SERVER_IP,34,PORT);
            Pc.printf("AT+CIPSTART=%c%s%c,%c%s%c,%s\r\n",34,PROTOCOL,34,34,SERVER_IP,34,PORT);
            EstadoSiguiente = MODO_COM;
            EstadoInicializacion = ESPERA;
            break;

        case MODO_COM:
            Esp.printf("AT+CIPMODE=0\r\n");
            Pc.printf("AT+CIPMODE=0\r\n");
            EstadoSiguiente = FIN_INICIALIZACION;
            EstadoInicializacion = ESPERA;
            break;

        case ESPERA:
            if(RespuestaAT == OK) {
                if(EstadoSiguiente == FIN_INICIALIZACION)
                    Inicializado = true;
                else {
                    EstadoInicializacion = EstadoSiguiente;
                    RespuestaAT = NADA;
                    //Reintentos = 0;
                }

            }

            if(RespuestaAT == ERROR) {

                EstadoInicializacion = EstadoSiguiente - 1;
            }
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
                Esp.printf("AT+CIPSEND=8\r\n");
                t = 3;
                Err = true;
                return;
            }
            Val = ((DatoA - 48) * 100) + ((DatoB - 48) * 10) + (DatoC - 48);
            if(Val > 100) {
                Esp.printf("AT+CIPSEND=8\r\n");
                t = 3;
                Err = true;
                return;
            } else {
                nuevo_mensaje = true;//hacer con el valor lo que se tenga que hacer
                paso = Val * PASOS_MAXIMOS / 100;
            }
        }
    }
}

//Funcion que Frena el Motor
void frenar()
{
    Bobina1A = OFF;
    Bobina1B = OFF;
    Bobina2A = OFF;
    Bobina2B = OFF;
}

void MANDAR_DATOS()
{
    dato_mandar = paso*100/PASOS_MAXIMOS;
    if(dato_mandar != 100)
        ver_enviada = 'H' ^ 'C' ^ '0' ^ (dato_mandar / 10 + 48) ^ (dato_mandar % 10 + 48) ;
    else
        ver_enviada = 'H' ^ 'C' ^ '1' ^ '0' ^ '0' ;
    Esp.printf("AT+CIPSEND=9\r\n");

    mandar = true;
    t = 3;
}


int main()
{
    Pc.baud(115200);
    Esp.baud(115200);
    Esp.attach(&RxEsp, Serial::RxIrq);
    Pc.attach(&control, Serial::RxIrq);
    tik.attach(&fn_timer,DEBOUNCE_TIMER_BASE);
    
    
    while(not pul_down and not pul_up){
        sentido = antihorario;
        movimiento_motor();
        paso = paso_actual;       
    }    
    
    PASOS_MAXIMOS = paso_actual;
    while(1) {
        if(not Inicializado)
            Inicializar();
        Proceso();
/*
        if(not t and mandar ) {
            mandar = false;
            if(dato_mandar == 100)
                Esp.printf("#HC%d%d$",dato_mandar, ver_enviada);
            else if(dato_mandar >= 10)
                Esp.printf("#HC0%d%d$", dato_mandar, ver_enviada);
            else
                Esp.printf("#HC00%d%d$", dato_mandar, ver_enviada);
                
            
        }

        if(cont >= TIEMPO_CUENTA && Err == false && Inicializado == true) {
            cont = 0;
            MANDAR_DATOS();
        }*/
        //----MODO MANUAL----------
        if(pul_up == 1 && paso_actual > 0) {
            sentido = horario;
            movimiento_motor();
            paso = paso_actual;
            
           
        }
        
        if(pul_down == 1 && pul_up == 0 && paso_actual < PASOS_MAXIMOS) {
            sentido = antihorario;
            movimiento_motor();
            paso = paso_actual;
            
            
        }
        //----MODO AUTOMÁTICO----------
        if(Inicializado == true && nuevo_mensaje == true) {
            if(paso_actual < paso)
                sentido = antihorario;
            if(paso_actual > paso)
                sentido = horario;
            if(paso_actual != paso)
                movimiento_motor();
            else {
                frenar();
                nuevo_mensaje = false;
            }
        }
    }
}