#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


int ForgC=15;
int BackC=0;
int o=0;
int proyecto=0;
int num=0;
int num_puerto=0;
int i=0;
char buff[30];

void inicio(void)
{
    gotoxy(26,0);
    printf("\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB");
    gotoxy(26,1);
    puts("\xBA Separador de golosinas:\xBA");
    gotoxy(26,2);
    printf("\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC");
    gotoxy(1,3);
    puts("1. Explicacion.");
    gotoxy(1,4);
    puts("2. Mostrar Temperatura y Peso.");
    gotoxy(1,5);
    puts("3. Integrantes.");
    gotoxy(1,6);
    printf("*Por favor escriba una opcion->");
    scanf("%d",&num);

    if(num==1||num==2||num==3)
    {
        system("cls");
        if(num==1)
            explicacion();
        if(num==2)
            puerto();
        if(num==3)
            integrante();
    }
    if(num!=1&&num!=2&&num!=3)
        error_menu();
}

void error_menu()
{
    gotoxy(1,7);
    printf("Num incorrecto, vuelva a ingresar->");
    scanf("%d",&num);
    if(num==1||num==2||num==3)
    {
        system("cls");
        if(num==1)
            explicacion();
        if(num==2)
            puerto();
        if(num==3)
            integrante();
    }
    else
    {
        system("cls");
        gotoxy(26,0);
    printf("\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB");
    gotoxy(26,1);
    puts("\xBA Separador de golosinas:\xBA");
    gotoxy(26,2);
    printf("\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC");
    gotoxy(1,3);
    puts("1. Explicacion.");
    gotoxy(1,4);
    puts("2. Mostrar Temperatura y Peso.");
    gotoxy(1,5);
    puts("3. Integrantes.");
        error_menu();
    }
}
void explicacion()
{
    system("cls");
    gotoxy(35,0);
    puts("Proyecto 2019");
    gotoxy(35,1);
    printf("\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD");
    gotoxy(0,3);
    puts("*Explicacion:");
    gotoxy(5,5);
    puts("En nuestro proyecto primero se ingresara la cantidad de golosinas ");
    gotoxy(0,6);
    puts("requeridas por color,luego todas las golosinas se introduciran en un embudo ");
    gotoxy(0,7);
    puts("donde los colores requeridos seran seleccionados en un vaso, las golosinas ");
    gotoxy(0,8);
    puts("que no desean, seran separadas en otro recipiente, dando por finalizado");
    gotoxy(0,9);
    puts("el proyecto.");
    puts("\n");
    gotoxy(1,10);
    printf("Escriba 0 para regresar al menu->");
    scanf("%d",&num);
    if(num==0)
    {
        system("cls");
        inicio();
    }
    else
        error();
}
void puerto()
{
    system("cls");
    gotoxy(1,1);
    printf("Ingrese el numero del puerto donde comunicara menor a 100->\n");
    gotoxy(1,2);
    printf("Y que sea distinto de 1");
    gotoxy(60,1);
    scanf("%d",&num_puerto);
    system("cls");
    cargando();
    system("cls");
    if(num_puerto>=0&&num_puerto<=100&&num_puerto!=1)
    {
        FILE *f;

        HANDLE port_serie;

        BYTE numport;
        DWORD baudios;
        BYTE parid;
        BYTE databits;
        BYTE stopbit;

        int timout;
        int numerr = 0;
        int pb = 0;
        unsigned char cr;

        numport = num_puerto;
        baudios = 9600;
        databits = 8;
        parid = NOPARITY;
        stopbit = ONESTOPBIT;
        timout = 100;

        port_serie = abrir_port(numport);

        if (port_serie == INVALID_HANDLE_VALUE)
        {
            system("cls");
            gotoxy(0,0);
            printf("No se pudo abrir puerto.\n");
            printf("Verifique nuevamente.\n");
            Sleep(3000);
            puerto();
        }
        if (configurar_port(port_serie,baudios,databits,parid,stopbit,timout) == 0)
        {
            system("cls");
            gotoxy(0,0);
            printf("Error al config. el puerto.\n");
            printf("Por favor reintente.\n\n");
            Sleep(3000);
            puerto();
        }

        f = fopen("log.txt", "w");
        fprintf(f,"");
        f = fopen("log.txt", "w");

        do
        {
            if (leer_char(port_serie,&cr) != 0)
            {
                if (cr != '>')
                {
                    buff[pb++] = cr;
                }
                else
                {
                    buff[pb] = '\0';
                    pb = 0;
                    variable();
                }
            }
        }
        while( numerr < 10 );

        fclose(f);
        cerrar_port(port_serie);
    }
    else
    {
        gotoxy(1,3);
        printf("Valor incorrecto, ingrese nuevamente.Espere");
        Sleep(2000);
        system("cls");
        puerto();

    }

}
void cargando()
{
    for(i=0; i<=3; i++)
    {
        gotoxy(21,10);
        printf("Estableciendo conexion.");
        Sleep(600);
        gotoxy(44,10);
        printf(".");
        Sleep(600);
        gotoxy(45,10);
        printf(".");
        Sleep(600);
        system("cls");
    }
}

void variable()
{

    if(buff[0]=='T')
    {
        gotoxy(15,0);
        printf(" \x5F\x5F\x5F\x5F\x5F\x5F\x5F\n");
        gotoxy(1,1);
        printf("Temperatura->\n");
        gotoxy(15,1);
        printf("\x7C %c%c,%c %cC \x7C\n", buff[1], buff[2], buff[3],167);
        gotoxy(15,2);
        printf("\x7C\x5F\x5F\x5F\x5F\x5F\x5F\x5F\x5F\x7C \n");

    }
    if(buff[0]=='P')
    {
        gotoxy(15,4);
        printf(" \x5F\x5F\x5F\x5F\x5F\x5F\x5F\n");
        gotoxy(1,5);
        printf("Peso->\n");
        gotoxy(15,5);
        printf("\x7C %c%c%c g \x7C", buff[1], buff[2], buff[3]);
        gotoxy(15,6);
        printf("\x7C\x5F\x5F\x5F\x5F\x5F\x5F\x5F\x7C \n");
        gotoxy(1,10);
        printf("Escriba <ESC> para regresar al menu->");
        if(GetAsyncKeyState(VK_ESCAPE)& 0x8001)
        {
            system("cls");
            inicio();
        }
    }

}
void integrante()
{
    system("cls");
    gotoxy(35,0);
    puts("Integrantes:");
    gotoxy(35,1);
    printf("\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD");
    gotoxy(0,3);
    puts("Martinez, Agustin.");
    puts("Salerno,  Santiago.");
    puts("Spataro,  Guido.");
    puts("Zorrilla, Javier.");
    puts("\n");
    gotoxy(1,10);
    printf("Escriba 0 para regresar al menu->");
    scanf("%d",&num);
    puts("\n");
    if(num==0)
    {
        system("cls");
        inicio();
    }
    else
        error();
}
void error()
{
    system("cls");
    gotoxy(1,11);
    printf("Num incorrecto.");
    printf(" Escriba 0 para regresar al menu->");
    scanf("%d",&num);
    if(num==0)
    {
        system("cls");
        inicio();
    }
    else
        error();
}
int main()
{
    system("cls");
    system("color 2F");
    inicio();
    if(num==1)
        explicacion();
    if(num==2)
        puerto();
    if(num==3)
        integrante();
    return 0;
}

void gotoxy(int x, int y)
{
    HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X=x;
    dwPos.Y=y;
    SetConsoleCursorPosition(hcon,dwPos);
}
