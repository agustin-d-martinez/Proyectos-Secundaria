/*		Grupo 5:
 * Martinez, Agustin
 * Salerno, Santiago
 * Spataro,Guido
 * Zorrilla, Javier
 */

#include <hidef.h> 		
#include "derivative.h" 
#include "Hardware.h"

int timer = 0;
unsigned int temp = 0, temp_set = 50, peso = 0;
unsigned char pedido = 0;
unsigned long tiempo = 0; 							//variable detector de color
unsigned int i = 0, blanco = 0, azul = 0; 			//variable decir color y auxiliar de tx
unsigned char trama[5] = { 0, 0, 0, 0, '>' }; 		//variables transmision serie		
unsigned char mensaje = 0;							//variables del registro
unsigned char h = 0;

//------Variables para el teclado ------
unsigned int teclado = 0, antirrebote = 0, estado_teclado = 0;
int valor_teclado = 0;
//--------------------------------------

int g = 0;											// Variable de incremento del MotorPP

int RS = 0, E = 0;
int aux = 0;

#define start			123
#define preg_azul		0
#define preg_blanco		1
#define ver_color		3
#define decir_color		4
#define tobogan			5
#define mover_tambor	6
#define giro			7
#define preg_peso		8
#define preg_temp		9
#define comunicar	 	10	
#define comunicar2		11
unsigned char estado = start;

void serie(void);
void LCD_init(void);
void LCD_send(unsigned char, unsigned char);
void delay(unsigned int);
void printstr(char* s);
void registro(void);
void leer_teclado(void);
void pantalla(void);


void Inicializacion(void) {
	SOPT1 = SOPT1 & 0x3F; 			// deshabilito COP
	//"calibra" el osc interno-------
	asm {
		LDA $FFAF;
		STA ICSTRM
	}
	while (!ICSSC_IREFST)
		;

	TPM1MOD = 499; 					
	TPM1SC = 0b01001001;

	TPM2SC = 0b00001101;			//timer del input capture
	TPM2MOD = 65500;
	TPM2C0SC = 0;					//input capture		0b01000100

	SCIBD = 52; 					// divisor = 52 baudrate = 8MHz / 52 / 16 = 9615,38 ? 9600
	SCIC1 = 0b00000000; 			// sin paridad
	SCIC2 = 0b00001000;
	SCIC3 = 0;

	//-------DIRECCIONES----------
	dir_color = 0;
	dir_ntc = 0;
	dir_celda_carga = 0;
	dir_cooler = 1;

	dir_motorPP = 0b1111;

	dir_motorCC = 1;
	dir_FC1 = 0;
	dir_FC2 = 0;

	dir_lcd_hab = 1;
	dir_lcd_data = 1;
	dir_lcd_clk = 1;
	lcd_clk = 0;
	lcd_data = 0;
	lcd_hab = 0;

	dir_teclado = 0;
	//----------------------------

	ADCCFG = 0b01100000; //2MHz
	ADCSC2 = 0;

	ADCSC1 = 0b00000000; // Entrada para ADC para cambiar la pata a ver: ADCSC1_ADCH
	APCTL1_ADPC0 = 1;		//ntc
	APCTL1_ADPC6 = 1;		//peso
	APCTL1_ADPC4 = 1;		//teclado

	EnableInterrupts
	;
}

void main(void) {
	Inicializacion();
	LCD_init();

	for (;;) {
		switch (estado) {

		case start:
			estado_teclado = 1;
			// inicio del LCD
			LCD_send(0b10000000, 0);
			printstr("m&m azul    : 00");
			LCD_send('*', 1);
			LCD_send(0b10111000, 0);
			printstr("m&m blanco  : 00");
			estado = preg_azul;
			break;

		case preg_azul:	//preguntar por color 1--------------------------------------------
			leer_teclado();
			if (estado_teclado == 5) {
				estado_teclado = 1;
				if (valor_teclado == 100) {
					estado = preg_blanco;
					pantalla();
					valor_teclado = 0;
				} else {
					//+1
					if (valor_teclado == 1) {
						azul += 1;
						valor_teclado = 0;
						teclado = 0;
					}
					//-1
					if (valor_teclado == 2) {
						if (azul <= 0)
							azul = 0;
						else
							azul -= 1;
						valor_teclado = 0;
						teclado = 0;
					}
					//+2
					if (valor_teclado == 3) {
						azul += 2;
						valor_teclado = 0;
						teclado = 0;
					}
					//-2
					if (valor_teclado == 4) {
						if (azul < 2)
							azul = 0;
						else
							azul -= 2;
						valor_teclado = 0;
						teclado = 0;
					}
					//+5
					if (valor_teclado == 5) {
						azul += 5;
						valor_teclado = 0;
						teclado = 0;
					}
					//-5
					if (valor_teclado == 6) {
						if (azul < 5)
							azul = 0;
						else
							azul -= 5;
						valor_teclado = 0;
						teclado = 0;
					}
					//+10
					if (valor_teclado == 7) {
						azul += 10;
						valor_teclado = 0;
						teclado = 0;
					}
					//clear
					if (valor_teclado == 40) {
						azul = 0;
						valor_teclado = 0;
						teclado = 0;
					}
					pantalla();
				}
			}

			break;

		case preg_blanco://preguntar por color 2-------------------------------------------
			leer_teclado();
			if (estado_teclado == 5) {
				estado_teclado = 0;
				if (valor_teclado == 100) {
					estado = giro;
					pantalla();
					valor_teclado = 0;
					estado_teclado = 1;

				} else {
					//+1
					if (valor_teclado == 1) {
						blanco += 1;
						valor_teclado = 0;
						teclado = 0;
					}
					//-1
					if (valor_teclado == 2) {
						if (blanco <= 0)
							blanco = 0;
						else
							blanco -= 1;
						valor_teclado = 0;
						teclado = 0;

					}
					//+2
					if (valor_teclado == 3) {
						blanco += 2;
						valor_teclado = 0;
						teclado = 0;
					}
					//-2
					if (valor_teclado == 4) {
						if (blanco < 2)
							blanco = 0;
						else
							blanco -= 2;
						valor_teclado = 0;
						teclado = 0;
					}

					//+5
					if (valor_teclado == 5) {
						blanco += 5;
						valor_teclado = 0;
						teclado = 0;
					}
					//-5
					if (valor_teclado == 6) {
						if (blanco < 5)
							blanco = 0;
						else
							blanco -= 5;
						valor_teclado = 0;
						teclado = 0;
					}
					//+10
					if (valor_teclado == 7) {
						blanco += 10;
						valor_teclado = 0;
						teclado = 0;
					}
					//clear
					if (valor_teclado == 40) {
						blanco = 0;
						valor_teclado = 0;
						teclado = 0;
					}
					estado_teclado = 1;
					pantalla();
				}
			}
			break;

		case giro:
			motorPP = 0b0001;
			delay(2000);
			motorPP = 0b0010;
			delay(2000);
			motorPP = 0b0100;
			delay(2000);
			motorPP = 0b1000;
			delay(2000);
			g++;
			if (g == 130) {				//125 = 90º
				g = 0;
				TPM2C0SC = 0b01000100;
				estado = ver_color;
				motorPP = 0;
			}
			break;

		case ver_color:
			pantalla();
			if (h >= 3) {
				h = 0;
				estado = decir_color;
				TPM2C0SC = 0;
			}
			break;

		case decir_color:
			motorPP = 0;
			if ((tiempo >= 389 && tiempo <= 460)) 	// Rangos del tambor
				pedido = 0;

			if (tiempo >= 367 && tiempo <= 385) {	// Rangos del m&m azul
				if (azul > 0) {
					azul--;
					pedido = 1;
				} else
					pedido = 0;
			}
			if (tiempo >= 345 && tiempo <= 365) {	// Rangos del m&m blanco
				if (blanco > 0) {
					blanco--;
					pedido = 1;
				} else
					pedido = 0;
			}
			//delay para el retraso del tobogan
			delay(20000);
			delay(20000);
			estado = tobogan;
			if (azul == 0 && blanco == 0)
				estado = start;
			break;

		case tobogan:
			delay(10000);
			delay(10000);

			motorCC = 1;
			if (FC1 == 1 && pedido == 1) {
				estado = preg_temp;
				g = 0;
				motorCC = 0;
			}
			if (FC2 == 1 && pedido == 0) {
				estado = preg_temp;
				g = 0;
				motorCC = 0;
			}
			break;

		case preg_temp:				//adc de Sensor de Temp
			ADCSC1 = 0b00000000;
			if (ADCSC1_COCO == 1) {
				temp = 255 - ADCR;
				temp = temp * 196 / 100;
				temp = temp - 110;
				estado = preg_peso;
			}
			break;

		case preg_peso:				//adc de Sensor de peso
			ADCSC1 = 0b00000110;
			if (ADCSC1_COCO == 1) {
				peso = ADCR;
				if (peso <= 196 / 50)
					peso = 0;
				else
					peso = ((ADCR * 196 / 50));
				ADCSC1 = 0b00000110;
				estado = comunicar;
				i = 0;
				TPM2C0SC = 0b01000100;
			}
			break;

		case comunicar:
			trama[0] = 'T';
			trama[1] = temp / 100 + 48;
			trama[2] = (temp / 10 - temp / 100 * 10) + 48;
			trama[3] = temp % 10 + 48;
			serie();
			if (i >= 5) {
				i = 0;
				estado = comunicar2;
			}
			break;

		case comunicar2:
			trama[0] = 'P';
			trama[1] = peso / 100 + 48;
			trama[2] = (peso / 10 - peso / 100 * 10) + 48;
			trama[3] = peso % 10 + 48;
			serie();
			if (i >= 5) {
				i = 0;
				estado = giro;
			}
			break;
		}
	}
}
void pantalla(void) {
	//envio datos del m&m azul
	LCD_send(0b10001110, 0);
	LCD_send(azul / 10 + 48, 1);
	LCD_send(azul % 10 + 48, 1);
	if (estado == preg_azul)
		LCD_send('*', 1);
	else
		LCD_send(' ', 1);

	//envio datos del m&m blanco
	LCD_send(0b11001110, 0);
	LCD_send(blanco / 10 + 48, 1);
	LCD_send(blanco % 10 + 48, 1);
	if (estado == preg_blanco)
		LCD_send('*', 1);
	else
		LCD_send(' ', 1);

	//envio datos de Temp
	LCD_send(0b11010100, 0);
	printstr("Temp:");
	LCD_send(temp / 100 + 48, 1);
	LCD_send(temp / 10 - temp / 100 * 10 + 48, 1);
	printstr(",");
	LCD_send(temp % 10 + 48, 1);
	printstr(" C");

	//envio datos de Peso
	LCD_send(0b10010100, 0);
	printstr("Peso:");
	LCD_send(peso / 100 + 48, 1);
	LCD_send(peso / 10 - peso / 100 * 10 + 48, 1);
	LCD_send(peso% 10 + 48, 1);
	printstr("  gr");

}
//Comunico
void serie(void) {
	if (SCIS1_TDRE == 1) {
		SCID = trama[i];
		i++;
	}
}

/* ------------------------------------------------- inicializacion LCD --- */
void LCD_init(void) {
	delay(50000);
	delay(50000);
	delay(50000);
	LCD_send(0x03, 0);
	LCD_send(0x03, 0);
	LCD_send(0x03, 0);
	/* --- modo 4 bit --- */
	LCD_send(0b0010, 0);
	/* --- modo 4b - 2 lin - car 5x7 --- */
	LCD_send(0x28, 0);
	/* --- apaga display --- */
	LCD_send(0x08, 0);
	/* --- Clear --- */
	LCD_send(0x01, 0);
	delay(50000);
	/* --- disp on - cur off - blink off --- */
	LCD_send(0x0C, 0);
	/* --- inc - no scroll --- */
	LCD_send(0x06, 0);
}

/* ---diferencia entre datos o comandos --- */
void LCD_send(unsigned char data, unsigned char operacion) {//0b1	x							000000
//0b1 	renglon 1/3, 2/4			posicion (mayor a 20 cambia renglon)
	unsigned char half;
	if (operacion == 1)	// si ope es 1, se envia data, si no se envian comandos de posicion
		RS = 1;
	else
		RS = 0;
	aux = mensaje;
	aux = aux & 0xF0;
	mensaje = aux | data;
	half = data & 0xF0;
	half = half >> 4;

	E = 1;
	mensaje = (RS << 5) | (E << 4) | half;
	registro();
	E = 0;
	mensaje = (RS << 5) | (E << 4) | half;
	registro();
	delay(50);

	half = data & 0x0F;
	E = 1;
	mensaje = (RS << 5) | (E << 4) | half;
	registro();
	E = 0;
	mensaje = (RS << 5) | (E << 4) | half;
	registro();
	delay(50);
	RS = 0;
}
void printstr(char* s) {
	int a = 0;
	while (s[a] != '\0') {
		LCD_send(s[a], 1);
		a++;
	}
}

void delay(unsigned int n) {
	asm {
		LDHX n
		_seguir: AIX#-1
		CPHX#0
		BNE _seguir
	}
}

void registro(void) {
	lcd_data = (mensaje & 32) >> 5;
	lcd_clk = 1;
	lcd_clk = 0;
	lcd_data = (mensaje & 16) >> 4;
	lcd_clk = 1;
	lcd_clk = 0;
	lcd_data = (mensaje & 8) >> 3;
	lcd_clk = 1;
	lcd_clk = 0;
	lcd_data = (mensaje & 4) >> 2;
	lcd_clk = 1;
	lcd_clk = 0;
	lcd_data = (mensaje & 2) >> 1;
	lcd_clk = 1;
	lcd_clk = 0;
	lcd_data = mensaje & 1;
	lcd_clk = 1;
	lcd_clk = 0;
	lcd_hab = 1;
	lcd_hab = 0;
}

void leer_teclado() {
	switch (estado_teclado) {
	default:
	case 0:
		break;
	case 1:
		ADCSC1 = 0b00000100;
		if (ADCSC1_COCO == 1) {
			if (ADCR != 0) {
				teclado = ADCR;
				estado_teclado = 2;
			}
		}
		break;
	case 2:
		if (antirrebote >= 600)
			estado_teclado = 3;
		break;
	case 3:
		antirrebote = 0;
		ADCSC1 = 0b00000100;
		estado_teclado = 4;
		break;
	case 4:
		if (ADCSC1_COCO == 1) {

			teclado = ADCR;

			if (teclado >= 234 && teclado <= 238) 			//  +1
				valor_teclado = 1;

			if (teclado >= 63 && teclado <= 67) 			//-1
				valor_teclado = 2;

			if (teclado >= 34 && teclado <= 35) 			//+2
				valor_teclado = 3;

			if (teclado >= 9 && teclado <= 10) 				//-2
				valor_teclado = 4;

			if (teclado >= 4 && teclado <= 5)  				//+5
				valor_teclado = 5;

			if (teclado == 1) 								//-5
				valor_teclado = 6;

			if (teclado >= 2 && teclado <= 3) 				//+10
				valor_teclado = 7;

			if ((teclado == 17 || teclado == 18)) 			//ok
				valor_teclado = 100;

			if (teclado >= 120 && teclado <= 130) 			// clear
				valor_teclado = 40;

			estado_teclado = 5;
		}
		break;

	case 5:
		break;
	}
}

//cooler----------------------------------------------
__interrupt 11 void Timer_Overflow(void) {
	timer++;
	if (estado_teclado == 2)
		antirrebote++;
	if (timer < ((temp - temp_set) / 10) && temp > temp_set)	//anda
		cooler = 1;
	else
		cooler = 0;

	if (timer == 50)
		timer = 0;
	TPM1SC_TOF = 0;
}

__interrupt 12 void tcs3200() { 	//Se detecta un flanco positivo en el DHT
	h++;
	tiempo = TPM2C0V;
	TPM2CNT = 10;
	TPM2C0SC_CH0F = 0;
}
