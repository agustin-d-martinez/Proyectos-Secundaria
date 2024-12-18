#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define PERDISTE 4
#define GANASTE  8


int main()
{
    /* Ingreso */
    char respuesta[50], cantidad=0, i=0,letra[50]= {0}, pos_letra=0,bien,intentos=6,terminar=0;
    char respondido[50]= {0};
    printf("ingrese palabra:\n");
    gets(respuesta);
    cantidad=strlen(respuesta);
    //system("cls");

    /* JUEGO */
    do
    {
        system("cls");
        for(i=0; i<cantidad; i++)
        {
            if (respondido[i]==0)
                printf("- ");
            else
                printf("%c ",respondido[i]);
        }
        printf("\n");
        switch(intentos)
        {
        case 1:
            printf("      %c%c%c%c%c%c                                 \n",201,205,205,205,205,187);
            printf("      %c    0                          \n",186);
            printf("      %c   /|\\                          \n",186);
            printf("      %c     \\                         \n",186);
            printf("      %c                                      \n\n",186);
            break;
        case 2:
            printf("      %c%c%c%c%c%c                                 \n",201,205,205,205,205,187);
            printf("      %c    0                          \n",186);
            printf("      %c   /|\\                          \n",186);
            printf("      %c                            \n",186);
            printf("      %c                                      \n\n",186);
            break;
        case 3:
            printf("      %c%c%c%c%c%c                                 \n",201,205,205,205,205,187);
            printf("      %c    0                          \n",186);
            printf("      %c   /|                          \n",186);
            printf("      %c                        \n",186);
            printf("      %c                                      \n\n",186);
            break;
        case 4:
            printf("      %c%c%c%c%c%c                                 \n",201,205,205,205,205,187);
            printf("      %c    0                          \n",186);
            printf("      %c    |                          \n",186);
            printf("      %c                          \n",186);
            printf("      %c                                      \n\n",186);
            break;
        case 5:
            printf("      %c%c%c%c%c%c                                 \n",201,205,205,205,205,187);
            printf("      %c    0                          \n",186);
            printf("      %c                                 \n",186);
            printf("      %c                                   \n",186);
            printf("      %c                                      \n\n",186);
            break;
        case 6:
            printf("      %c%c%c%c%c%c                                 \n",201,205,205,205,205,187);
            printf("      %c                            \n",186);
            printf("      %c                            \n",186);
            printf("      %c                          \n",186);
            printf("      %c                                      \n\n",186);
            break;
        }
        printf("Ingrese una letra: ");
        fflush(stdin);
        gets(letra);
        if(letra[1]!=NULL)
        {
            if (strcmp(letra,respuesta)==0)
                terminar=GANASTE;
            else
                terminar=PERDISTE;
        }
        for (i=0; i<cantidad; i++)
        {
            if (respuesta[i]==letra[0])
            {
                bien=1;
                respondido[i]=respuesta[i];
            }
        }

        if (bien==0)
            intentos--;
        bien=0;

        if (intentos==0)
        {
            terminar=PERDISTE;
        }

        if (strcmp(respondido,respuesta)==0)
        {
            terminar=GANASTE;
        }
    }
    while(terminar!=GANASTE&&terminar!=PERDISTE);


    if(terminar==GANASTE)
    {
        system("cls");
        printf("      %c                                 \n",186,205,205,205,205,187);
        printf("      %c  (^_^)                          \n",186);
        printf("      %c   /|\\                          \n",186);
        printf("      %c   / \\                         \n\n",186);
        printf("\t Ganaste, felicidades!!!!");
    }
    if(terminar==PERDISTE)
    {
        system("cls");
        printf("      %c%c%c%c%c%c                                 \n",201,205,205,205,205,187);
        printf("      %c  (x_x)                          \n",186);
        printf("      %c   /|\\                          \n",186);
        printf("      %c   / \\                         \n",186);
        printf("      %c                                      \n\n",186);
        printf("\t Looser!");
        printf("La palabra era:\n %s\n",respuesta);
    }
    return 0;
}
