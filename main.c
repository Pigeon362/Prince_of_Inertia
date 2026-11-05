#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gamelib.h"
#include <time.h>


/*Funzionalità extra:
    1)Inserendo 5 nel menù principale viene stampata una piccola introduzione al gioco;
    2)E' presente un nuovo tesoro, un totem curativo in grado di rianimare gli altri giocatori;
*/

int main()
{
    Giocatore** giocatori = malloc(sizeof(Giocatore));
    char** vincitori = malloc(3 * sizeof(char*));
    time_t t;
    int sceltaMenu = 0;
    srand((unsigned)time(&t));
    
    for (int i = 0; i < 3; i++)
    {
        vincitori[i] = realloc(vincitori[i], strlen("Slot Vuoto") + 1);
        strcpy(vincitori[i], "Slot Vuoto");
    }
    
    while (1)
    {
        //con sceltaMenu memorizzo il valore di input del giocatore, ovviamente l'inserimento è sicuro ed evita errori e segmentation fault
        sceltaMenu = input_int("---Prince of Inertia---\nInserisci:\n1)Per impostare il gioco;\n2)Per cominciare il gioco;\n3)Per terminare il gioco;\n4)Per visualizzare i crediti;\n5)Per visualizzare l'introduzione del gioco;\n[Input]: ", 1, 5);
        switch (sceltaMenu)
        {
            case 1:
                imposta_gioco(giocatori);
                break;
            
            case 2:
                gioca(giocatori, vincitori);
                break;

            case 3:
                termina_gioco(giocatori);
                free(vincitori);
                free(giocatori);
                return 1;
                break;

            case 4:
                crediti(giocatori, vincitori);
                break;

            case 5:
                stampa_introduzione();
                break;
            
            default:
                break;
        }
    }
    
    //A fine gioco libero la memoria allocata per salvare gli array contenti la mappa, i vincitori delle ultime 3 partite e i giocatori
    free(vincitori);
    free(giocatori);
    return 1;
}