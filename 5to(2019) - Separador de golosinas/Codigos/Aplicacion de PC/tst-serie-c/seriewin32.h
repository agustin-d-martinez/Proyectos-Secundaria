//Funciones para comunicación serie - W32 - 2011.10.08 - rev 2014.08

HANDLE abrir_port(unsigned char port);
void cerrar_port(HANDLE);
int configurar_port(HANDLE ports, DWORD baudrate, BYTE bytesize, BYTE paridad, BYTE stopbit, unsigned int time_out);

int escribir_char(HANDLE,unsigned char);
int escribir_string(HANDLE ports,unsigned char*);
int escribir_chars(HANDLE ports,unsigned char*, unsigned char num);

int leer_char(HANDLE ports, unsigned char*);
int leer_string(HANDLE ports, unsigned char*);
int leer_chars(HANDLE ports, unsigned char*, unsigned char num);
