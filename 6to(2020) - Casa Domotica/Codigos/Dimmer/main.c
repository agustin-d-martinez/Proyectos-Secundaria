#include "derivative.h" /* include peripheral declarations */
#include "string.h" /* include peripheral declarations */
#include "stdio.h"
#include "stdlib.h"

//PORCENTAJES DIMMER
#define porciento_100  	0
#define porciento_90  	20
#define porciento_80 	25
#define porciento_70  	30
#define porciento_60	35
#define porciento_50 	40
#define porciento_40  	45
#define porciento_30	50
#define porciento_20	55
#define porciento_10	60
#define porciento_0 	156

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

enum {
	ECO = 0,
	MODO,
	CONECCION_RED,
	CONECCION_SERVER,
	MODO_COM,
	ESPERA,
	FIN_INICIALIZACION
};
#define comilla    "\""

#define DEBOUNCE_TIMER_BASE  	0.0001 // Para 1 mili segundo.
#define TIEMPO_MANDAR_DATOS 	20 //SEG cambiar a 3
#define TIEMPO_CUENTA 			TIEMPO_MANDAR_DATOS / DEBOUNCE_TIMER_BASE

//Respuesta ESP01
#define NADA    0
#define OK      1
#define ERROR   2

//USUARIO Y CONTRASEÑA
#define SSID        "Mansion Wayne 2.4GHZ" //Nombre de la red WIFI
#define PASSWORD    "pollito230502" //Contraseña de la red WIFI
//Coneccion SERVER
#define PROTOCOL    "TCP" //Protocolo de comunicacion (TCP / UDP)
#define SERVER_IP   "192.168.0.43" // IP del servidor con el que se desea establecer un canal de comunicacion
#define PORT        "2000" // Puerto en el que el servidor "escucha"
//PROTOCOLO
#define PROTOCOL_START  '#'
#define DEVICE          'L'
#define PROTOCOL_END    '$'
#define HUB             'H'

unsigned char aux_msj_error = 0;

char enviar[100]={0};
unsigned int sumar = 0, counter = 0, estado = 0;
unsigned char inicializado = 0, mensaje_enviado = 0, dato_rx, Dato = 0,
		estado_inicializacion = ECO, estado_recepcion_protocolo = INICIO;
int i = 0, aux = 1, respuesta = NADA, estado_anterior = 0, estado_recepcion = 0;
unsigned int valor = 0;

//variables de Recepcion de protocolo
unsigned char Verificacion = 0, v = 0, Disp = 0, DatoA = 0, DatoB = 0,
		DatoC = 0, Ver = 0, nuevo_mensaje = 0, aux_2 = 0;
char s[10];

unsigned int t = 0;
unsigned int dato_mandar = 0, cont = 0, Err = 0, Val = 0, ver_enviada = 0;

//VARIABLES DEL DIMMER
unsigned int timer = 0;
unsigned char valor_adc = 0, valor_dimmer = 0, valor_ant_adc = 0;

unsigned int lista_porcentaje[12] = { porciento_0, porciento_10, porciento_20,
		porciento_30, porciento_40, porciento_50, porciento_60, porciento_70,
		porciento_80, porciento_90, porciento_100 };
unsigned char aux_val_dimmer = 0, val_dimmer = 0;

//funciones-------------------
void mandar(void);
void recepcion(void);
void inicializar(void);
void recepcion_protocolo(void);
//void MANDAR_DATOS(void);
void dimmer(int);

void init_bus_clock() {   // bus clock 23.97 MHz
	/* Switch to FEI Mode */
	/* MCG_C1: CLKS=0,FRDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=0 */
	MCG_C1 = MCG_C1_CLKS(0x00) | MCG_C1_FRDIV(0x00) | MCG_C1_IREFS_MASK
			| MCG_C1_IRCLKEN_MASK;
	/* MCG_C2: LOCRE0=0,??=0,RANGE0=0,HGO0=0,EREFS0=0,LP=0,IRCS=0 */MCG_C2 =
			MCG_C2_RANGE0(0x00);
	/* MCG_C4: DMX32=1,DRST_DRS=1 */MCG_C4 = (uint8_t) ((MCG_C4
			& (uint8_t) ~(uint8_t) (MCG_C4_DRST_DRS(0x02)))| (uint8_t)(
			MCG_C4_DMX32_MASK |
			MCG_C4_DRST_DRS(0x01)
	));
	/* OSC0_CR: ERCLKEN=1,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
	OSC0_CR = OSC_CR_ERCLKEN_MASK;
	/* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=0 */MCG_C5 =
			MCG_C5_PRDIV0(0x00);
	/* MCG_C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=0 */MCG_C6 = MCG_C6_VDIV0(0x00);
	while ((MCG_S & MCG_S_IREFST_MASK) == 0x00U) { /* Check that the source of the FLL reference clock is the internal reference clock. */
	}
	while ((MCG_S & 0x0CU) != 0x00U) { /* Wait until output of the FLL is selected */
	}
}

int main(void) {
	SIM_COPC = (uint32_t) 0x00u;
init_bus_clock();

	// clock para el TPM y UART = 2 MHz 
	MCG_C1 = MCG_C1_IREFS_MASK | MCG_C1_IRCLKEN_MASK;
	MCG_C2 = MCG_C2_IRCS_MASK;

	// pone 1 en bit 26 habilita clock en TPM0
	SIM_SCGC6 |= 1 << 24;   //Enable TPM0 clock
	SIM_SCGC4 |= 1 << 10; // clock UART 0

	// selecciono clock del timer y de UART0
	// ambos MCGIRCLK 
	SIM_SOPT2 |= 0x03000000;  // TPM0
	SIM_SOPT2 |= 0x0C000000;  // UART0

	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; // pata salida
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; // pata pul
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK; // pata CRUCE CERO

	// config modulo TPM
	TPM0_SC |= 0x0000004F;
	TPM0_MOD = 1;

	SIM_SCGC5 |= 1 << 10; // clock de PORTB
	SIM_SCGC5 |= 1 << 9; // clock de PORTA (para UART0)

	PORTA_PCR2 |= 0x00000200; // PTA2 tx (ALT2)
	PORTA_PCR1 |= 0x00000200; // PTA1 rx (ALT2)

	NVIC_BASE_PTR ->ISER = 1 << 17;	// TOF TPM0

	//CONFIG DE PATAS
	PORTC_PCR9 = 0x00000100; 	//LAMPARA
	PORTC_PCR7 = 0x00000100; 	//PULSADOR
	PORTE_PCR29 = 0x00000100; 	//CRUCE CERO

	//CONFIG DE SALIDA/ENTRADA
	GPIOC_PDDR |= (1 << 9);		 //LAMPARA
	GPIOC_PDDR |= (0 << 7); 	//PULSADOR
	GPIOE_PDDR |= (0 << 29); 	//CRUCE CERO
	SIM_SCGC6 |= (1 << 27); 	//adc

	
	
	//adc
	ADC0_CFG1 = 0x00000069; // clock màs lento
	ADC0_SC3 = 1 << 3; // continua
	ADC0_SC1A = 0x0000000F; // clock interno, ADC_SE15

	
	
	
	/* UART 0 */
	UART0_C2 = 0; // apaga Tx y Rx
	UART0_C1 = 0;
	UART0_C4 = 15;

	UART0_BDL = 13; // 9600 baud -->
	UART0_BDH = 0;
	UART0_C2 |= (1 << 3) | (1 << 2);	// Habilitamos Tx y Rx	

	//UART2/

	SIM_SCGC4 |= 1 << 12; // clock UART 2

	SIM_SCGC5 |= 1 << 13; // clock de PORTE (para UART2 E22 tx, E23 rx)
	PORTE_PCR22 |= 0x00000400; // PTE22 tx (ALT4)
	PORTE_PCR23 |= 0x00000400; // PTE23 rx (ALT4)

	UART2_C2 = 0; // apaga Tx y Rx
	UART2_C1 = 0;

	UART2_BDL = 13; // 115200 - busclock = 23.97 MHz
	UART2_BDH = 0;
	UART2_C2 |= (1 << 3) | (1 << 2); // Habilitamos Tx y Rx

	
	
	//enviar[0]='\0';

	for (;;) {

		if (inicializado == 0)
			inicializar();
		if (UART2_S1 & (1 << 5)) {
			Dato = UART2_D;
			if (inicializado == 0)
				recepcion();
			else
				recepcion_protocolo();
		}

		if (GPIOC_PDIR & (1UL << 7)) {
			if (valor_ant_adc != valor_adc) {
				val_dimmer = valor_adc;
				valor_ant_adc = val_dimmer;
			}
		}
		if (nuevo_mensaje == 1) {
			val_dimmer = Val / 10;
			nuevo_mensaje = 0;
		} else {
			if (ADC0_SC1A & (1 << 7)) {
				valor_adc = ADC0_RA/100;
			}
		}
		dimmer(val_dimmer);

		if (Err == 1) {
			if (aux_msj_error == 0) {
				strcpy(enviar, "AT+CIPSEND=8");
				enviar[12] = '\r';
				enviar[13] = '\n';
				enviar[14] = '\0';
				mandar();
			}

			if (mensaje_enviado == 1) {
				mensaje_enviado = 0;
				aux_msj_error = 1;
			}
			if (aux_msj_error == 1) {
				strcpy(enviar, "#HLER19$");
				mandar();
				if (mensaje_enviado == 1) {
					mensaje_enviado = 0;
					aux_msj_error = 0;
					Err = 0;
				}

			}

		}

	}
	return 0;
}

void inicializar() {
	switch (estado_inicializacion) {
	default:
	case ECO:
		strcpy(enviar, "ATE0");

		if (mensaje_enviado == 1) {
			mensaje_enviado = 0;
			estado_inicializacion = ESPERA;
			estado_anterior = ECO;
		}else
		mandar();

		break;

	case ESPERA:

		if (respuesta == OK) {
			if (estado_anterior == MODO_COM) {
				estado_inicializacion = FIN_INICIALIZACION;
				inicializado = 1;
			} else
				estado_inicializacion = estado_anterior + 1;

		}

		if (respuesta == ERROR)
			estado_inicializacion = estado_anterior;
		respuesta = NADA;

		break;

	case MODO:
		strcpy(enviar, "AT+CWMODE_CUR=1");

		if (mensaje_enviado == 1) {
			mensaje_enviado = 0;
			estado_inicializacion = ESPERA;
			estado_anterior = MODO;
		}else
		mandar();

		break;
	case CONECCION_RED:

		strcpy(enviar, "AT+CWJAP_CUR=");
		strcat(enviar, comilla);
		strcat(enviar, SSID);
		strcat(enviar, comilla);
		strcat(enviar, ",");
		strcat(enviar, comilla);
		strcat(enviar, PASSWORD);
		strcat(enviar, comilla);


		if (mensaje_enviado == 1) {
			mensaje_enviado = 0;
			estado_anterior = CONECCION_RED;
			estado_inicializacion = ESPERA;
		}else
		mandar();

		break;
	case CONECCION_SERVER:

		strcpy(enviar, "AT+CIPSTART=");
		strcat(enviar, comilla);
		strcat(enviar, PROTOCOL);
		strcat(enviar, comilla);
		strcat(enviar, ",");
		strcat(enviar, comilla);
		strcat(enviar, SERVER_IP);
		strcat(enviar, comilla);
		strcat(enviar, ",");
		strcat(enviar, PORT);


		if (mensaje_enviado == 1) {
			mensaje_enviado = 0;
			estado_anterior = CONECCION_SERVER;
			estado_inicializacion = ESPERA;
		}else
		mandar();

		break;
	case MODO_COM:
		strcpy(enviar, "AT+CIPMODE=0");
		if (mensaje_enviado == 1) {
			mensaje_enviado = 0;
			estado_anterior = MODO_COM;
			estado_inicializacion = ESPERA;
		}else
		mandar();

		break;
	}
}

void mandar() {
	if (UART2_S1 & (1 << 7)) {
		if (i < strlen(enviar)) {
			UART2_D = enviar[i];
			i++;
		} else {
			if (inicializado == 1) {
				mensaje_enviado = 1;

				i = 0;
			} else {
				if (aux == 2) {
					UART2_D = '\n';
					aux++;
				}

				if (aux == 1) {
					UART2_D = '\r';
					aux++;
				}

				if (aux == 3) {
					mensaje_enviado = 1;
					i = 0;
					aux = 1;
				}
			}
		}
	}
}

void recepcion_protocolo(void) {
	switch (estado_recepcion_protocolo) {
	case INICIO:
		if (Dato == PROTOCOL_START)
			estado_recepcion_protocolo = DISPOSITIVO;
		if (Dato == 'C')
			estado_recepcion_protocolo = CLOSED_L;
		break;

	case DISPOSITIVO:
		Disp = Dato;
		if (Disp != DEVICE) {
			estado_recepcion_protocolo = INICIO;
		} else {

			estado_recepcion_protocolo = DATOA;
			Verificacion = Disp;

		}
		break;

	case DATOA:
		DatoA = Dato;
		Verificacion ^= Dato;
		estado_recepcion_protocolo = DATOB;
		break;

	case DATOB:
		DatoB = Dato;
		Verificacion ^= Dato;
		estado_recepcion_protocolo = DATOC;
		break;

	case DATOC:
		DatoC = Dato;
		Verificacion ^= Dato;
		estado_recepcion_protocolo = VERIFICAR;
		break;

	case VERIFICAR:
		if (Dato != PROTOCOL_END) {
			s[v++] = Dato;
		} else {
			v = 0;
			estado_recepcion_protocolo = INICIO;
			Ver = atoi(s);
			strcpy(s, "");
			Val = ((DatoA - 48) * 100) + ((DatoB - 48) * 10) + (DatoC - 48);
			if (Verificacion != Ver || Val > 100) {
				Err = 1;
			} else
				nuevo_mensaje = 1; //hacer con el valor lo que se tenga que hacer

		}
		break;

	case CLOSED_L:
		if (Dato == 'L') {
			estado_recepcion_protocolo = CLOSED_O;
		} else {
			estado_recepcion_protocolo = INICIO;
		}
		break;

	case CLOSED_O:
		if (Dato == 'O') {
			estado_recepcion_protocolo = CLOSED_S;
		} else {
			estado_recepcion_protocolo = INICIO;
		}
		break;

	case CLOSED_S:
		if (Dato == 'S') {
			estado_recepcion_protocolo = CLOSED_E;
		} else {
			estado_recepcion_protocolo = INICIO;
		}
		break;

	case CLOSED_E:
		if (Dato == 'E') {
			estado_recepcion_protocolo = CLOSED_D;
		} else {
			estado_recepcion_protocolo = INICIO;
		}
		break;

	case CLOSED_D:
		if (Dato == 'D') {
			inicializado = 0;
			estado_inicializacion = CONECCION_RED;
		}
		estado_recepcion_protocolo = INICIO;
		break;
	}
}
/*
 void MANDAR_DATOS(void) {
 if (cont >= TIEMPO_CUENTA && Err == 0 && inicializado == 1) {
 cont = 0;
 aux_2 = 1;
 }
 if (aux_2 == 1) {

 strcpy(enviar, "AT+CIPSEND=9");
 enviar[12] = '\r';
 enviar[13] = '\n';
 enviar[14] = '\0';

 mandar();
 if (mensaje_enviado == 1) {
 mensaje_enviado = 0;
 aux_2 = 2;
 sumar = 0;
 }

 }
 if (aux_2 == 3) {
 strcpy(enviar, "#HL");	//CAMBIAR  #HL- VALOR DE LUZ -XOR $
 //	strcut(valor_luz);
 //strcut(xor);
 //	strcut("$");

 mandar();
 if (mensaje_enviado == 1) {
 mensaje_enviado = 0;
 aux_2 = 0;
 cont = 0;
 }
 }
 }
 */

void recepcion(void) {
	switch (estado_recepcion) {
	case 0:
		if (Dato == 'O')
			estado_recepcion = 10;
		if (Dato == 'E')
			estado_recepcion = 1;
		break;
	case 1:
		if (Dato == 'R')
			estado_recepcion = 2;
		else
			estado_recepcion = 0;
		break;
	case 2:
		if (Dato == 'R')
			estado_recepcion = 3;
		else
			estado_recepcion = 0;
		break;
	case 3:
		if (Dato == 'O')
			estado_recepcion = 4;
		else
			estado_recepcion = 0;
		break;
	case 4:
		if (Dato == 'R')
			estado_recepcion = 5;
		else
			estado_recepcion = 0;
		break;
	case 5:
		if (Dato == '\r')
			estado_recepcion = 6;
		else
			estado_recepcion = 0;
		break;
	case 6:
		if (Dato == '\n')
			respuesta = ERROR;
		estado_recepcion = 0;
		break;
	case 10:
		if (Dato == 'K')
			estado_recepcion = 11;
		else
			estado_recepcion = 0;
		break;
	case 11:
		if (Dato == '\r')
			estado_recepcion = 12;
		else
			estado_recepcion = 0;
		break;
	case 12:
		if (Dato == '\n')
			respuesta = OK;
		estado_recepcion = 0;
		break;

	}
}

void dimmer(int valor_dimmer) {

	if (lista_porcentaje[valor_dimmer] != porciento_100
			&& lista_porcentaje[valor_dimmer] != porciento_0) {
		if (timer >= lista_porcentaje[valor_dimmer] && timer <= 156)
			GPIOC_PSOR |= 1 << 9;
		else
			GPIOC_PCOR |= 1 << 9;
	} else {
		if (lista_porcentaje[valor_dimmer] == porciento_0)
			GPIOC_PCOR |= 1 << 9;

		if (lista_porcentaje[valor_dimmer] == porciento_100)
			GPIOC_PSOR |= 1 << 9;

	}

	if (GPIOE_PDIR & (0UL << 29))
		timer = 0;

}

void FTM0_IRQHandler() {

	if (GPIOE_PDIR & (1UL << 29))
		timer++;
	else
		timer = 0;

	cont++;
	sumar++;

	if (sumar >= 300 && aux_2 == 2) {
		sumar = 0;
		aux_2 = 3;
	}
	TPM0_BASE_PTR ->SC |= TPM_SC_TOF_MASK;

}
