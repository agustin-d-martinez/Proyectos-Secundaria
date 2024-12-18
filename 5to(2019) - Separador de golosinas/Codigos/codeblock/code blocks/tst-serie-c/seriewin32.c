//Funciones para comunicación serie - W32 - 2011.10.08 - rev 2014.08
#include <windows.h>

COMMTIMEOUTS tempComTimeouts;      // timeout
COMMTIMEOUTS savedComTimeouts;     // timeout originales

/* ---------------------------------------------------------------------------
 Abre puerto
 - recibe entero con nro. de port serie a abrir (nro. de "COM")
--------------------------------------------------------------------------- */
HANDLE abrir_port(unsigned char port)
	{
    char nombreport[] = "\\\\.\\COM--";

    if (port > 9) {
        nombreport[7] = 0x30 + (port / 10);
        nombreport[8] = 0x30 + (port % 10);
    } else {
        nombreport[7] = 0x30 + port;
        nombreport[8] = '\0';
    }

    return CreateFile(nombreport,  //Nombre del puerto ("COM2","COM3"...)
	                    GENERIC_READ | GENERIC_WRITE,
	                    0,                            // acceso exclusivo
	                    NULL,                         // sin seguridad
	                    OPEN_EXISTING,
	                    FILE_ATTRIBUTE_NORMAL,
	                    NULL);
	}

/* ---------------------------------------------------------------------------
 Cierra port
 - recibe HANDLE de port serie abierto y configurado
--------------------------------------------------------------------------- */
void cerrar_port(HANDLE ports) {
    SetCommTimeouts(ports,&savedComTimeouts);
    CloseHandle(ports);
}

/* ---------------------------------------------------------------------------
 Configura puerto
 - devuelve 0 si hubo errores
 - recibe HANDLE de port serie abierto y configurado
 - recibe:
        baudrate (9600 default)
        bits de datos (8 default)
        paridad (SIN paridad default)
        bits de stop (1 bit de stop default)
        timeout (en ms) (250 ms default)
--------------------------------------------------------------------------- */
int configurar_port(HANDLE ports, DWORD baudrate, BYTE bytesize, BYTE paridad, BYTE stopbit, unsigned int time_out) {
    DCB          port_conf;          // (Device Control Block)

    GetCommTimeouts(ports,&savedComTimeouts);

    /* configura timeouts nuevos */
    tempComTimeouts.ReadIntervalTimeout         = 0;
    tempComTimeouts.ReadTotalTimeoutMultiplier  = 0;
    tempComTimeouts.ReadTotalTimeoutConstant    = time_out;
    tempComTimeouts.WriteTotalTimeoutMultiplier = 0;
    tempComTimeouts.WriteTotalTimeoutConstant   = 100;

    SetCommTimeouts(ports,&tempComTimeouts);

    /* Configura puerto */

    /* Llena estructura dcb con valores actuales */
    GetCommState(ports, &port_conf);

    /* Fija estructura dcb con valores nuevos */
    port_conf.BaudRate = baudrate;
    port_conf.StopBits = stopbit;
    port_conf.ByteSize = bytesize;
    port_conf.Parity   = paridad;

    if (paridad == NOPARITY)
        port_conf.fParity  = FALSE;
    else
        port_conf.fParity  = TRUE;

    /* Configura port con valores nuevos */
    return SetCommState(ports, &port_conf);
}

/* ---------------------------------------------------------------------------
 Escribe char
 - devuelve cantidad de caracteres escritos
 - recibe:
        HANDLE de port serie abierto y configurado
        el byte a transmitir (unsigned char )
--------------------------------------------------------------------------- */
int escribir_char(HANDLE ports,unsigned char c) {
	DWORD bytes;
    return WriteFile(ports,&c,1,&bytes,NULL);
}

/* ---------------------------------------------------------------------------
 Escribe n bytes
 - devuelve 0 si hubo errores (timeout)
 - recibe:
        HANDLE de port serie abierto y configurado
        vector unsigned char a escribir
        cantidad de bytes a escribir
--------------------------------------------------------------------------- */
int escribir_chars(HANDLE ports,unsigned char *c, unsigned char num) {
	DWORD bytes;

    return WriteFile(ports,c,num,&bytes,NULL);
}


/* ---------------------------------------------------------------------------
 Escribe string
 - devuelve 0 si hubo errores (timeout)
 - recibe:
        HANDLE de port serie abierto y configurado
        vector de unsigned char a escribir
--------------------------------------------------------------------------- */
int escribir_string(HANDLE ports,unsigned char *c) {
	DWORD bytes;
	unsigned int largo = 0;

    while( *(c) != '\0') {
        WriteFile(ports,(c++),1,&bytes,NULL);
        largo += bytes;
    }

    WriteFile(ports,c,1,&bytes,NULL);

    return largo;
}

/* ---------------------------------------------------------------------------
 Lee byte
 - devuelve 0 si hubo errores (timeout)
 - recibe:
        HANDLE de port serie abierto y configurado
        referencia a una variable unsigned char donde entrega byte recibido
--------------------------------------------------------------------------- */
int leer_char(HANDLE ports, unsigned char *c) {
	DWORD by_read;

	ReadFile(ports,c,1,&by_read,NULL);

    return (by_read);
}

/* ---------------------------------------------------------------------------
 Lee n bytes
 - devuelve 0 si hubo errores (timeout)
 - recibe:
        HANDLE de port serie abierto y configurado
        vector unsigned char donde entrega bytes recibidos
        cantidad de bytes a leer
--------------------------------------------------------------------------- */
int leer_chars(HANDLE ports, unsigned char *c, unsigned char num) {
	DWORD by_read;

	ReadFile(ports,c,num,&by_read,NULL);

    return (by_read);
}

/* ---------------------------------------------------------------------------
 Lee string
 - largo de string  (incluyendo \0) o 0 si hubo errores (timeout)
 - recibe:
        HANDLE de port serie abierto y configurado
        vector unsigned char donde entrega string recibido
--------------------------------------------------------------------------- */
int leer_string(HANDLE ports, unsigned char *c) {
	DWORD by_read;
	unsigned int largo = 0;

	ReadFile(ports,c,1,&by_read,NULL);

	while( (by_read > 0) && ( (*c) != '\0') ) {
        largo ++;
	    ReadFile(ports,(++c),1,&by_read,NULL);
	}

    if ( (by_read == 0) || ( (*c) != '\0') )
        return 0;
    else
        return ( largo );
}


