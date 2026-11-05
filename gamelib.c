#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gamelib.h"

Stanza* pfirst;
static int numeroStanze = 0, numGiocatori = 0, gioco_finito = 0;

//funzioni da usare solo in gamelib.c

static void ins_stanza();
static void posiziona_stanza(Stanza* stanza, int direzione);
static void canc_stanza();
static void stampa_stanze();
static void resetta_mappa();
static char* tipoStanza(Tipo_stanza stanza);
static char* tipoTrabocchetto(Tipo_trabocchetto trabocchetto);
static char* tipoTesoro(Tipo_tesoro tesoro);
static void genera_random();
static void avanza(Giocatore* giocatore, char* tipoNemico);
static void combatti(Giocatore* giocatore, char* nemico, char** vincitori);
static void scappa(Giocatore* giocatore, char* nemico[10]);
static void stampa_giocatore(Giocatore* giocatore);
static void stampa_zona(Stanza* stanza);
static void prendi_tesoro(Giocatore* giocatore, Stanza* stanza);
static void cerca_porta_segreta(Giocatore* giocatore);
static void passa();
static void genera_stanza_segreta(Giocatore* giocatore, int sceltaMenu, char stanze[3][2][20]);
static void stanza_segreta(Giocatore* giocatore, Stanza* stanza);
static void effetto_trabocchetto(Giocatore* giocatore, Stanza* stanza);
static void turno_combattimento_giocatore(Giocatore* giocatore, Nemico* nemico_attivo);
static void turno_combattimento_nemico(Giocatore* giocatore, Nemico* nemico_attivo);
static int stanzaCercataCheck(Giocatore* giocatori_misch);
static int tesoroCercatoCheck(Giocatore* giocatori_misch);
static void resetta_partita(Giocatore** giocatori);
static void inserisci_vincitori(char nome[21], char** vincitori);
static void rianima_giocatore(Giocatore** giocatori);

void imposta_gioco(Giocatore** giocatori)
{   
    numGiocatori=0;
    int sceltaClasse, principeScelto = 0, sceltaMenu = 0;
    numGiocatori = input_int("Inserisci il numero di giocatori (da 1 a 3).\n [Input]: ", 1, 3);
    *giocatori = realloc(*giocatori, numGiocatori * sizeof(Giocatore)); //Rialloco la memoria per giocatori, in modo che avere un array di dimensioni perfette per il numero di giocatori presenti
    for (int i = 0; i < numGiocatori; i++)
    {
        strcpy((*giocatori)[i].nome_giocatore, input_string("\nInserisci il nome del giocatore: ", 21));
        do
        {
            sceltaClasse = input_int("\nScegli la tua classe:\n1) Principe\n2) Doppleganger\n[Input]: ", 1, 2);
            printf("\n");
            //Il principe può essere scelto solo una volta
            if(sceltaClasse==1 && principeScelto) 
            {
                printf("il principe è già stato scelto! riprova.\n");
            }
        } while (sceltaClasse==1 && principeScelto);

        (*giocatori)[i].p_vita = 3;
        (*giocatori)[i].p_vita_max = 3;
        (*giocatori)[i].dadi_attacco = 2;
        (*giocatori)[i].dadi_difesa = 2;
        (*giocatori)[i].segretaContr = malloc(sizeof(Stanza));
        (*giocatori)[i].tesoroContr = malloc(sizeof(Stanza));
        (*giocatori)[i].num_segretaContr = 0;
        (*giocatori)[i].num_tesoroContr = 0;
        (*giocatori)[i].totem_curativo = 0;
        (*giocatori)[i].vecchiaStanza = NULL;
        (*giocatori)[i].stanzaFuga = NULL;

        if (sceltaClasse == 1) {
            (*giocatori)[i].classe_giocatore = principe;
            (*giocatori)[i].pos_fuga = 1;
            principeScelto = 1;
        } else if (sceltaClasse == 2) {
            (*giocatori)[i].classe_giocatore = doppleganger;
            (*giocatori)[i].pos_fuga = 2;
        }
    }
    
    //Come scritto sul file, dopo aver inserito i player, si passa alla creazione della mappa
    do
    {

        sceltaMenu = input_int("---Menù creazione mappa---\nInserisci:\n1)Per creare una nuova stanza;\n2)Per cancellare l'ultima stanza inserita;\n3)Per stampare tutte le stanze;\n4)Per generare randomicamente la mappa;\n5)Per chiudere il menù;\n[Input]: ", 1, 5);

        switch (sceltaMenu)
        {
        case 1:
            ins_stanza();
            break;

        case 2:
            canc_stanza();
            break;

        case 3:
            if(numeroStanze!=0)
            {
                stampa_stanze();
            }else{
                printf("Non hai ancora creato nessuna stanza!\n");
            }
            break;

        case 4:
            genera_random();
            break;
        
        case 5:
            if(numeroStanze>14)
            {
                printf("Ritorno al menù principale!\n");
                for (int i = 0; i < numGiocatori; i++)
                {
                    (*giocatori)[i].posizione = pfirst;
                }
                return;
            }else
            {
                //I livelli devono essere più di 15 ma non hanno limite massimo
                printf("Inserisci almeno 15 livelli per uscire!\n");
                continue;
            }
            break;
        
        default:
            break;
        }
    } while (1);
    
    
    
    
}

static void ins_stanza()
{
    int sceltaMenu = 0, sceltaDirezione = 0;
    Stanza* stanza = malloc(sizeof(Stanza)); // Assegno al puntatore 'stanza' l'indirizzo della 'Stanza' in 'mappa[numeroStanze]', in questo modo risulta più semplice lavorare sul valore dell'array.

    stanza->stanza_destra = NULL;
    stanza->stanza_sinistra = NULL;
    stanza->stanza_sopra = NULL;
    stanza->stanza_sotto = NULL;

    if (numeroStanze > 0) {
        
        sceltaDirezione = input_int("Dove vuoi inserire la nuova stanza rispetto all'ultima?\n1)A destra;\n2)A Sinistra;\n3)Sopra;\n4)Sotto;\n[Input]: ", 1, 4);
        posiziona_stanza(stanza, sceltaDirezione);

    } else {
        pfirst = stanza;
    }

    //Quando assegno al campo di stanza il valore, devo decrementare di uno il valore in modo da rientrare nel range dell'enum.
    sceltaMenu = input_int("Quale tipo di stanza vuoi inserire?\n1)Corridoio;\n2)Scala;\n3)Sala banchetto;\n4)Magazzino;\n5)Posto di Guardia;\n6)Prigione;\n7)Armeria;\n8)Moschea;\n9)Torre;\n10)Bagni;\n[Input]: ", 1, 10);
    printf("\n");
    stanza->tipo_stanza = sceltaMenu - 1;

    sceltaMenu = input_int("Quale tipo di trabocchetto vuoi utilizzare?\n1)Nessuno;\n2)Tegola;\n3)Lame;\n4)Caduta;\n5)Burrone;\n[Input]: ", 1, 5);
    printf("\n");
    stanza->tipo_trabocchetto = sceltaMenu - 1;

    sceltaMenu = input_int("Quale tipo di tesoro vuoi inserire?\n1)Nessuno;\n2)Verde veleno;\n3)Blu guarigione;\n4)Rosso aumenta vita;\n5)Spada tagliente;\n6)Scudo;\n7)Totem curativo;\n[Input]: ", 1, 7);
    printf("\n");
    stanza->tipo_tesoro = sceltaMenu - 1;
    numeroStanze++;
}

static void posiziona_stanza(Stanza* stanza, int direzione)
{
    Stanza* temp = pfirst;
    
    while (1)
    {
        if(temp->stanza_destra != NULL)
        {
            temp = temp->stanza_destra;
        }else if(temp->stanza_sinistra != NULL)
        {
            temp = temp->stanza_sinistra;
        }else if (temp->stanza_sopra != NULL)
        {
            temp = temp->stanza_sopra;
        }else if (temp->stanza_sotto != NULL)
        {
            temp = temp->stanza_sotto;
        }else
        {
           switch (direzione)
           {
            case 0:
            return;
            break;

            case 1:
            temp->stanza_destra = stanza;
            break;

            case 2:
            temp->stanza_sinistra = stanza;
            break;

            case 3:
            temp->stanza_sopra = stanza;
            break;

            case 4:
            temp->stanza_sotto = stanza;
            break;
            
           default:
            break;
           }
           return;
        }
    }
    
}

static void canc_stanza()
{
    if (numeroStanze == 0) {
        printf("Errore: non ci sono stanze da cancellare.\n");
        return;
    }

    Stanza* temp = pfirst;
    
    while (1)
    {
        if(temp->stanza_destra != NULL)
        {
            temp = temp->stanza_destra;
        }else if(temp->stanza_sinistra != NULL)
        {
            temp = temp->stanza_sinistra;
        }else if (temp->stanza_sopra != NULL)
        {
            temp = temp->stanza_sopra;
        }else if (temp->stanza_sotto != NULL)
        {
            temp = temp->stanza_sotto;
        }else
        {
           free(temp);
           numeroStanze--;
           return;
        }
    }
    
    
}

static void stampa_stanze()
{
    printf("\n");
    Stanza* temp = pfirst;
    int count = 1;
    while (1)
    {

        printf("Stanza numero %d: \n", count);
        //Se una direzione non porta da nessuna parte, non viene stampato il suo valore altrimenti stampa un indirizzo in esadecimale
        printf("Stanza a destra: ");
        temp->stanza_destra ? printf("%p\n", temp->stanza_destra) : printf("Vicolo cieco\n");

        printf("Stanza a sinistra: ");
        temp->stanza_sinistra ? printf("%p\n", temp->stanza_sinistra) : printf("Vicolo cieco\n");

        printf("Stanza sopra: ");
        temp->stanza_sopra ? printf("%p\n", temp->stanza_sopra) : printf("Vicolo cieco\n");

        printf("Stanza sotto: ");
        temp->stanza_sotto ? printf("%p\n", temp->stanza_sotto) : printf("Vicolo cieco\n");
        printf("Tipo di stanza: %s\n", (tipoStanza(temp->tipo_stanza)));
        printf("Tipo di trabocchetto: %s\n", (tipoTrabocchetto(temp->tipo_trabocchetto)));
        printf("Tipo di tesoro: %s\n", (tipoTesoro(temp->tipo_tesoro)));
        printf("\n");

        if(temp->stanza_destra != NULL)
        {
            temp = temp->stanza_destra;
        }else if(temp->stanza_sinistra != NULL)
        {
            temp = temp->stanza_sinistra;
        }else if (temp->stanza_sopra != NULL)
        {
            temp = temp->stanza_sopra;
        }else if (temp->stanza_sotto != NULL)
        {
            temp = temp->stanza_sotto;
        }else
        {
            return;
        }
        count++;
    }
    
}

static char* tipoStanza(Tipo_stanza stanza)
{
    switch (stanza) {
        case corridoio:      return "Corridoio";
        case scala:          return "Scala";
        case sala_banchetto: return "Sala Banchetto";
        case magazzino:      return "Magazzino";
        case posto_Guardia:  return "Posto di Guardia";
        case prigione:       return "Prigione";
        case armeria:        return "Armeria";
        case moschea:        return "Moschea";
        case torre:          return "Torre";
        case bagni:          return "Bagni";
        default:             return "Sconosciuto";
    }
}

static char* tipoTrabocchetto(Tipo_trabocchetto trabocchetto) 
{
    switch (trabocchetto) {
        case nessuno:    return "Nessuno";
        case tegola:     return "Tegola";
        case lame:       return "Lame";
        case caduta:     return "Caduta";
        case burrone:    return "Burrone";
        default:         return "Sconosciuto";
    }
}

static char* tipoTesoro(Tipo_tesoro tesoro) 
{
    switch (tesoro) {
        case nessun_tesoro:        return "Nessun Tesoro";
        case verde_veleno:         return "Verde Veleno";
        case blu_guarigione:       return "Blu Guarigione";
        case rosso_aumenta_vita:   return "Rosso Aumenta Vita";
        case spada_tagliente:      return "Spada Tagliente";
        case scudo:                return "Scudo";
        case totem_curativo:       return "Totem curativo";
        default:                   return "Sconosciuto";
    }
}

static void genera_random() 
{
    int sceltaMenu;
    //Il game master viene avvisato che in caso di generazione randomica delle stanze, tutte quelle create precedentemente verranno eliminate.
    sceltaMenu = input_int("\nAttenzione, con questa opzione vengono cancellate tutte le stanze precedentemente create, sei sicuro di voler proseguire?\nInserire un numero qualsiasi per continuare oppure 0 per ritornare al menù di creazione della mappa.\n[Input]: ", 0, 1);
    printf("\n");
    if (sceltaMenu == 0) {
        return;
    }

    numeroStanze = 0;
    resetta_mappa();
    for (int i = 0; i < 15; i++) {
        Stanza* stanza = malloc(sizeof(Stanza));

        stanza->stanza_destra = NULL;
        stanza->stanza_sinistra = NULL;
        stanza->stanza_sopra = NULL;
        stanza->stanza_sotto = NULL;

        //Tutto si basa sul tiro dei dadi, anche la generazione degli attributi delle stanze, in questo caso ho usato la funzione rand().
        //I valori rispettando quelli dettati dalla consegna.
        if (i > 0) {
           int randDirezione = (rand() % 4) + 1;
            
            posiziona_stanza(stanza, randDirezione);
        } else {
            pfirst = stanza;
        }

        stanza->tipo_stanza = rand() % 10;

        int randomTrappola = rand() % 100;
        if (randomTrappola < 65)
            stanza->tipo_trabocchetto = nessuno;
        else if (randomTrappola < 75)
            stanza->tipo_trabocchetto = tegola;
        else if (randomTrappola < 84)
            stanza->tipo_trabocchetto = lame;
        else if (randomTrappola < 92)
            stanza->tipo_trabocchetto = caduta;
        else
            stanza->tipo_trabocchetto = burrone;

        //Una feature extra del mio gioco è rappresentata dall'aggiunta di un totem curativo in grado rianimare gli altri giocatori.
        int randomTesoro = rand() % 100;
        if (randomTesoro < 20)
            stanza->tipo_tesoro = nessun_tesoro;
        else if (randomTesoro < 40)
            stanza->tipo_tesoro = verde_veleno;
        else if (randomTesoro < 60)
            stanza->tipo_tesoro = blu_guarigione;
        else if (randomTesoro < 75)
            stanza->tipo_tesoro = rosso_aumenta_vita;
        else if (randomTesoro < 90)
            stanza->tipo_tesoro = spada_tagliente;
        else if (randomTesoro < 98)
            stanza->tipo_tesoro = scudo;
        else
            stanza->tipo_tesoro = totem_curativo; //E' molto raro e ogni giocatore può portarne solo uno alla volta.

        numeroStanze++;
    }
}

void gioca(Giocatore** giocatori, char** vincitori)
{
    int sceltaMenu, fineTurno = 1;
    char** nemico = malloc(numGiocatori * sizeof(char*)); //Per tenere conto dei nemici ho creato un array "nemico" lungo quanto quello dei giocatori
    Giocatore** giocatori_misch = malloc(numGiocatori * sizeof(Giocatore));

    
    if(numeroStanze<14)
    {
        printf("\nParametri di gioco non impostati correttamente!\nRitorno al menù principale!\n");
        free(giocatori_misch);
        free(nemico);
        resetta_partita(giocatori);
        return;
    }
    for (int i = 0; i < numGiocatori; i++) {
        giocatori_misch[i] = &(*giocatori)[i];
        nemico[i] = malloc((strlen("Vuoto") + 1) * sizeof(char));
        strcpy(nemico[i], "Vuoto");
    }
    //faccio una copia dell'array giocatori per evitare di mischiare il principale.
    
    do
    {
        //con questo ciclo for posso mischiare l'array in modo da far cominciare il turno ogni volta a un giocatore causale.
        for (int i = numGiocatori - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            Giocatore* temp = giocatori_misch[i];
            giocatori_misch[i] = giocatori_misch[j];
            giocatori_misch[j] = temp;

            char* nemicoTemp = malloc((strlen(nemico[i]) + 1) * sizeof(char));
            strcpy(nemicoTemp, nemico[i]);

            strcpy(nemico[i], nemico[j]);
            strcpy(nemico[j], nemicoTemp);

            free(nemicoTemp);

        }

        //prima di continuare controllo che tutti i giocatori sian vivi, altrimenti il gioco si interrompe e annuncia la sconfitta, resettando la partita.
        for (int i = 0; i < numGiocatori; i++)
        {
            gioco_finito = 1;

            for (int i = 0; i < numGiocatori; i++)
            {
                if (giocatori_misch[i]->p_vita > 0) {
                    gioco_finito = 0;
                    break;
                }
            }

            if (gioco_finito == 1)
            {
                printf("\n");
                printf("Tutti i giocatori sono stati eliminati! Il gioco è finito.\n");
                break;
            }

            if(giocatori_misch[i]->incapacitato || giocatori_misch[i]->p_vita==0)
            {
                printf("Il turno del giocatore %s è stato annullato perché il giocatore è stato incapacitato o eliminato!\n", giocatori_misch[i]->nome_giocatore);
                giocatori_misch[i]->incapacitato = 0;
                continue;
            }
            printf("E' il turno di %s.\n", giocatori_misch[i]->nome_giocatore);
            do
            {
                sceltaMenu = input_int("\n---Menù di gioco---\nInserici:\n1)Per avanzare;\n2)Per combattere;\n3)Per scappare;\n4)Per vedere i valori del giocatore;\n5)Per ottenere info sulla stanza;\n6)Per prendere il tesoro;\n7)Per cercare la porta segreta;\n8)Per passare il turno;\n9)Per rianimare un giocatore;\n[Input]: ", 1, 9);
                printf("\n");
                switch (sceltaMenu)
                {
                case 1:
                    if(strcmp(nemico[i], "Vuoto"))
                    {
                        printf("Non puoi avanzare, c'è un nemico da affrontare!\n");
                    }else{
                        avanza(giocatori_misch[i], nemico[i]);
                        fineTurno = 0;
                    }
                    break;
                    
                case 2:
                    if(strcmp(nemico[i], "Vuoto")==0)
                    {
                        printf("Non c'è nessun nemico da combattere!\n");
                    }else
                    {
                        combatti(giocatori_misch[i], nemico[i], vincitori);
                        fineTurno = 0;
                    }
                    break;

                case 3:
                    if(strcmp(nemico[i], "Vuoto")==0)
                    {
                        printf("Non c'è nessun nemico da cui scappare!\n");
                    }else
                    {
                        if(giocatori_misch[i]->pos_fuga > 0)
                        {
                            scappa(giocatori_misch[i], &nemico[i]);
                            fineTurno = 0;
                        }else
                        {
                            printf("Non puoi più scappare, hai esaurito tutte le possibilità di fuga!\n");
                        }
                    }
                    break;

                case 4:
                    stampa_giocatore(giocatori_misch[i]);
                    break;

                case 5:
                    stampa_zona(giocatori_misch[i]->posizione);
                    break;

                case 6:
                    if(strcmp(nemico[i], "Vuoto")==0)
                    {
                        if(tesoroCercatoCheck(giocatori_misch[i]))
                        {
                            if(giocatori_misch[i]->posizione->tipo_tesoro != nessun_tesoro)
                            {
                                prendi_tesoro(giocatori_misch[i], giocatori_misch[i]->posizione);
                                fineTurno = 0;
                            }else{
                                printf("Nella stanza non è presente nessun tesoro!\n");
                            }
                        }else
                        {
                            printf("Hai già cercato il tesoro in questa stanza!\n");
                        }
                    }else{
                        printf("Non puoi cercare il tesoro, prima devi afffrontare il nemico!\n");
                    }
                    break;

                case 7:
                    if(strcmp(nemico[i], "Vuoto")==0)
                    {
                        if(stanzaCercataCheck(giocatori_misch[i]))
                        {
                            cerca_porta_segreta(giocatori_misch[i]);
                            fineTurno = 0;
                        }else{
                            printf("Hai già cercato la porta segreta in questa stanza!\n");
                        }
                    }else{
                        printf("Non puoi cercare la porta segreta, prima devi afffrontare il nemico!\n");
                    }
                    break;

                case 8:
                        passa();
                        fineTurno = 0;
                        break;
                    break;
                
                case 9:
                        //prima di aprire il menù di rianimazione, controllo se attualmente sono presenti giocatori con 0 di vita.
                        int valido = 0;
                        if(giocatori_misch[i]->totem_curativo==1)
                        {
                            for (int i = 0; i < numGiocatori; i++)
                            {
                                if(giocatori_misch[i]->p_vita == 0)
                                {
                                    valido = 1;
                                    break;
                                }
                            }
                            if(valido)
                            {
                                rianima_giocatore(giocatori_misch);
                                giocatori_misch[i]->totem_curativo=0;
                                fineTurno = 0;
                                break;
                            }else
                            {
                                printf("Non ci sono giocatori da rianimare!\n");
                            }
                        }else
                        {
                            printf("Non hai un totem curativo nel tuo inventario!");
                        }
                    break;
                
                default:
                    break;
                }
            } while (fineTurno);
            fineTurno = 1;

        }
    } while (gioco_finito!=1);
    
    //A fine gioco, viene resettata la partita.
    gioco_finito = 0;
    free(giocatori_misch);
    free(nemico);
    resetta_partita(giocatori);
    

}

static int stanzaCercataCheck(Giocatore* giocatori_misch)
{
    //controllo se il giocatore, nella posizione attuale, ha già cercato la stanza segreta.
    for (int i = 0; i < giocatori_misch->num_segretaContr; i++)
    {
        if(giocatori_misch->segretaContr[i]==giocatori_misch->posizione)
        {
            return 0;
        }
    }
    //In caso non l'avesse ancora cercata, aggiorno la lista e mando il segnale per continuare il gioco
    giocatori_misch->num_segretaContr++;
    giocatori_misch->segretaContr = realloc(giocatori_misch->segretaContr, giocatori_misch->num_segretaContr * sizeof(Stanza));
    giocatori_misch->segretaContr[giocatori_misch->num_segretaContr-1] = giocatori_misch->posizione;
    return 1;
}

static int tesoroCercatoCheck(Giocatore* giocatori_misch)
{
    //Questa funzione è uguale, nella sua struttura, a stanzaCercataCheck.
    for (int i = 0; i < giocatori_misch->num_tesoroContr; i++)
    {
        if(giocatori_misch->tesoroContr[i]==giocatori_misch->posizione)
        {
            return 0;
        }
    }
    giocatori_misch->num_tesoroContr++;
    giocatori_misch->tesoroContr = realloc(giocatori_misch->tesoroContr, giocatori_misch->num_tesoroContr * sizeof(Stanza));
    giocatori_misch->tesoroContr[giocatori_misch->num_tesoroContr-1] = giocatori_misch->posizione;
    return 1;
}

static void avanza(Giocatore* giocatore, char* tipoNemico)
{
    //Per prima cosa, faccio avanzare il giocatore nell'unica direzione possibile (le altre sono impostate a NULL).
    if(giocatore->posizione->stanza_destra != NULL)
    {
        giocatore->vecchiaStanza = giocatore->posizione;
        giocatore->posizione = giocatore->posizione->stanza_destra;
    }else if(giocatore->posizione->stanza_sinistra != NULL)
    {
        giocatore->vecchiaStanza = giocatore->posizione;
        giocatore->posizione = giocatore->posizione->stanza_sinistra;
    }else if (giocatore->posizione->stanza_sopra != NULL)
    {
        giocatore->vecchiaStanza = giocatore->posizione;
        giocatore->posizione = giocatore->posizione->stanza_sopra;
    }else if (giocatore->posizione->stanza_sotto != NULL)
    {
        giocatore->vecchiaStanza = giocatore->posizione;
        giocatore->posizione = giocatore->posizione->stanza_sotto;
    }
    //Se il giocatore è già passato per questa stanza (E' scappato da un nemico), non attiva il trabocchetto e non appare nessun nemico.
    if(giocatore->stanzaFuga!=giocatore->posizione)
    {
        effetto_trabocchetto(giocatore, giocatore->posizione);
        if(giocatore->p_vita==0)
        {
            return;
        }
    }else
    {
        printf("Sei già passato per questa stanza, i trabocchetti non vengono attivati!\n");
    }

    //Se la stanza non ha direzioni possibili, vuol dire che si tratta dell'ultima, ergo è ora di affrontare Jaffar.
    if(giocatore->posizione->stanza_destra == NULL && giocatore->posizione->stanza_sinistra == NULL && giocatore->posizione->stanza_sopra == NULL && giocatore->posizione->stanza_sotto == NULL)
    {
        printf("Sei arrivato all'ultima stanza, preparati a scontrarti con Jaffar!\n");
        strcpy(tipoNemico, "Jaffar");
    }else
    {
        int prob = rand() % 100;
        if (prob < 25) {
            int nemico = rand() % 100;
            if (nemico < 60) {
                printf("Un nemico appare: uno Scheletro si avvicina!\n");
                strcpy(tipoNemico, "Scheletro");
            } else {
                printf("Un nemico appare: una Guardia si fa avanti!\n");
                strcpy(tipoNemico, "Guardia");
            }
        } else {
            printf("Non appare alcun nemico.\n\n");
        }
    }
    
}

static void combatti(Giocatore* giocatore, char* nemico, char** vincitori)
{
    Nemico* nemico_attivo = malloc(sizeof(Nemico));
    //Data la stringa "nemico", precedentemente assegnata, vado a inserire i valori corrispondenti all'interno di "nemico_attivo".
    if(strcmp(nemico, "Scheletro"))
    {
        nemico_attivo->dadi_attacco = 1;
        nemico_attivo->dadi_difesa = 1;
        strcpy(nemico_attivo->nome_nemico, nemico);
        nemico_attivo->p_vita = 1;
    }else if(strcmp(nemico, "Guardia"))
    {
        nemico_attivo->dadi_attacco = 2;
        nemico_attivo->dadi_difesa = 2;
        strcpy(nemico_attivo->nome_nemico, nemico);
        nemico_attivo->p_vita = 2;
    }else if(strcmp(nemico, "Jaffar"))
    {
        nemico_attivo->dadi_attacco = 3;
        nemico_attivo->dadi_difesa = 2;
        strcpy(nemico_attivo->nome_nemico, nemico);
        nemico_attivo->p_vita = 3;
    }

    //il ciclo for continua finché uno dei due combattenti non finisce i punti vita.
    for (int i = 0; giocatore->p_vita>0 && nemico_attivo->p_vita>0; i++)
    {
        int dado_giocatore;
        int dado_nemico;

        do
        {
            //All'inizio di ogni turno, lancio un dado per vedere chi comincia prima.
            dado_giocatore = (rand() % 6) + 1;
            dado_nemico = (rand() % 6) + 1;
            if(dado_giocatore==dado_nemico)
            {
                printf("Sono usciti due numeri uguali! Rilancio i dadi!\n");
            }
        } while (dado_giocatore==dado_nemico);

        if(dado_giocatore>dado_nemico)
        {
            printf("\n---%s attacca per primo!---\n", giocatore->nome_giocatore);
            turno_combattimento_giocatore(giocatore, nemico_attivo);
            //Se il nemico finisce i punti vita, termina il round (stessa cosa per l'if sotto).
            if(nemico_attivo->p_vita<=0)
            {
                break;
            }
            printf("\n---Adesso tocca a %s!---\n", nemico_attivo->nome_nemico);
            turno_combattimento_nemico(giocatore, nemico_attivo);

        }else
        {
            printf("\n---%s attacca per primo!---\n", nemico_attivo->nome_nemico);
            turno_combattimento_nemico(giocatore, nemico_attivo);
            if(giocatore->p_vita<=0)
            {
                break;
            }
            printf("\n---Adesso tocca a %s!---\n", giocatore->nome_giocatore);
            turno_combattimento_giocatore(giocatore, nemico_attivo);
        }
        
        
    }

    //Con una serie di controlli, annuncio la vittoria o la sconfitta del giocatore.
    if(giocatore->p_vita==0)
    {
        printf("%s ha perso!\n\n", giocatore->nome_giocatore);
        gioco_finito = 1;
        free(nemico_attivo);
        return;
    }else
    {
        if(!strcmp(nemico_attivo->nome_nemico, "Jaffar"))
        {
            printf("%s ha sconfitto Jaffar e salvato la principessa, il gioco è finito!\n\n", giocatore->nome_giocatore);
            inserisci_vincitori(giocatore->nome_giocatore, vincitori);
            gioco_finito = 1;
            free(nemico_attivo);
            return;
        }
        printf("%s ha sconfitto %s e guadagnato un punto vita!\n\n", giocatore->nome_giocatore, nemico_attivo->nome_nemico);
        if(giocatore->p_vita == giocatore->p_vita_max)
        {
            printf("%s è già al massimo dei suoi punti vita!\n\n", giocatore->nome_giocatore);
        }else{
            giocatore->p_vita += 1;
        }
    }
    strcpy(nemico, "Vuoto");
    
}

static void turno_combattimento_giocatore(Giocatore* giocatore, Nemico* nemico_attivo)
{
    int dadi_attacco[giocatore->dadi_attacco];
    int dadi_difesa[nemico_attivo->dadi_difesa];

    for (int i = 0; i < giocatore->dadi_attacco; i++)
    {
        dadi_attacco[i] = (rand() % 6) + 1;
    }
    for (int i = 0; i < nemico_attivo->dadi_difesa; i++)
    {
        dadi_difesa[i] = (rand() % 6) + 1;
    }
    
    int danni_totali = 0;
    int colpi_parati = 0;

    for (int i = 0; i < nemico_attivo->dadi_difesa; i++) {
        if (dadi_difesa[i] >= 4) {
            if (dadi_difesa[i] == 6) { 
                colpi_parati += 2;
            } else {
                colpi_parati++;
            }
        }
    }

    for (int i = 0; i < giocatore->dadi_attacco; i++) {
        if (dadi_attacco[i] >= 4) { 
            if (colpi_parati > 0) {
                colpi_parati--;
            } else {
                if (dadi_attacco[i] == 6) {
                    danni_totali += 2;
                } else {
                    danni_totali++;
                }
            }
        }
    }
    
    printf("%s ha ottenuto: \n", giocatore->nome_giocatore);
    for(int i = 0; i < giocatore->dadi_attacco; i++)
    {
        printf("%d° dado: %d\n", i+1, dadi_attacco[i]);
    }
    printf("Mentre %s ha ottenuto: \n", nemico_attivo->nome_nemico);
    for(int i = 0; i < nemico_attivo->dadi_difesa; i++)
    {
        printf("%d° dado: %d\n", i+1, dadi_difesa[i]);
    }

    if(danni_totali==1)
    {
        printf("%s ha ricevuto %d danno!\n", nemico_attivo->nome_nemico, danni_totali);
    }else
    {
        printf("%s ha ricevuto %d danni!\n", nemico_attivo->nome_nemico, danni_totali);
    }
    //Alla fine del combattimento, decremento i punti vita (C'è un controllo per evitare errori nella gestione degli unsigned char).
    nemico_attivo->p_vita = (nemico_attivo->p_vita > danni_totali) ? (nemico_attivo->p_vita - danni_totali) : 0;
    printf("Punti vita %s: %d\n", nemico_attivo->nome_nemico, nemico_attivo->p_vita);
}

static void turno_combattimento_nemico(Giocatore* giocatore, Nemico* nemico_attivo)
{
    int dadi_attacco[nemico_attivo->dadi_attacco];
    int dadi_difesa[giocatore->dadi_difesa];

    for (int i = 0; i < nemico_attivo->dadi_attacco; i++)
    {
        dadi_attacco[i] = (rand() % 6) + 1;
    }
    for (int i = 0; i < giocatore->dadi_difesa; i++)
    {
        dadi_difesa[i] = (rand() % 6) + 1;
    }
    
    int danni_totali = 0;
    int colpi_parati = 0;

    for (int i = 0; i < giocatore->dadi_difesa; i++) {
        if (dadi_difesa[i] >= 4) {
            if (dadi_difesa[i] == 6) { 
                colpi_parati += 2;
            } else {
                colpi_parati++;
            }
        }
    }

    for (int i = 0; i < nemico_attivo->dadi_attacco; i++) {
        if (dadi_attacco[i] >= 4) { 
            if (colpi_parati > 0) {
                colpi_parati--;
            } else {
                if (dadi_attacco[i] == 6) {
                    danni_totali += 2;
                } else {
                    danni_totali++;
                }
            }
        }
    }
    
    printf("%s ha ottenuto: \n", nemico_attivo->nome_nemico);
    for(int i = 0; i < nemico_attivo->dadi_attacco; i++)
    {
        printf("%d° dado: %d\n", i+1, dadi_attacco[i]);
    }
    printf("Mentre %s ha ottenuto: \n", giocatore->nome_giocatore);
    for(int i = 0; i < giocatore->dadi_difesa; i++)
    {
        printf("%d° dado: %d\n", i+1, dadi_difesa[i]);
    }

    if(danni_totali==1)
    {
        printf("%s ha ricevuto %d danno!\n", giocatore->nome_giocatore, danni_totali);
    }else
    {
        printf("%s ha ricevuto %d danni!\n", giocatore->nome_giocatore, danni_totali);
    }
    giocatore->p_vita = (giocatore->p_vita > danni_totali) ? (giocatore->p_vita - danni_totali) : 0;
    printf("Punti vita %s: %d\n", giocatore->nome_giocatore, giocatore->p_vita);
}

static void scappa(Giocatore* giocatore, char* nemico[10])
{
    printf("Sei scappato dallo scontro!\n");
    giocatore->stanzaFuga = giocatore->posizione;
    giocatore->posizione = giocatore->vecchiaStanza;
    giocatore->pos_fuga--;
    //Ovviamente il nemico scompare se il player scappa dallo scontro.
    strcpy(*nemico, "Vuoto");
}

static void stampa_giocatore(Giocatore* giocatore)
{
    //La mia struct "Giocatore" è leggermente diversa da quella dettata nella consegna, tuttavia ho deciso di stampare anche i campi come i contatori e le stanze in cui il player ha cercato il tesoro o le porte segrete.
    printf("---Scheda Giocatore---\n");
    printf("Nome: %s\n", giocatore->nome_giocatore);
    printf("Classe giocatore: ");
    giocatore->classe_giocatore == 0 ? printf("Principe\n") : printf("Doppleganger\n");
    printf("Punti vita: %d\n", giocatore->p_vita);
    printf("Punti vita massimi: %d\n", giocatore->p_vita_max);
    printf("Dadi attacco: %d\n", giocatore->dadi_attacco);
    printf("Dadi difesa: %d\n", giocatore->dadi_difesa);
    printf("Flag giocatore incapacitato: %d\n", giocatore->incapacitato);
    printf("Posizione: %p\n", giocatore->posizione);
    printf("Stanze in cui hai cercato la porta segreta: \n");
    for (int i = 0; i < giocatore->num_segretaContr; i++)
    {
        printf("%d)%p\n", i+1, giocatore->segretaContr);
    }
    if(!giocatore->num_segretaContr)
    {
        printf("Nessuna\n");
    }
    
    printf("Stanze in cui hai cercato il tesoro: \n");
    for (int i = 0; i < giocatore->num_tesoroContr; i++)
    {
        printf("%d)%p\n", i+1, giocatore->tesoroContr);
    }
    if(!giocatore->num_tesoroContr)
    {
        printf("Nessuna\n");
    }
    printf("Contatore stanze dove hai cercato la porta segreta: %d\n", giocatore->num_segretaContr);
    printf("Contatore stanze dove hai cercato il tesoro: %d\n", giocatore->num_tesoroContr);
    giocatore->totem_curativo ? printf("Hai un totem curativo\n") : printf("Non hai un totem curativo\n");
}

static void stampa_zona(Stanza* stanza)
{
    //Per evitare informazioni inutili, ho deciso di stampare solo le direzioni in cui è presente una porta (con relativo indirizzo).
    printf("---Dettagli della stanza---\n");
    if(stanza->stanza_destra != 0)
    {
        printf("E' presente una porta a destra! (Indirizzo: %p)\n", stanza->stanza_destra);
    }else if(stanza->stanza_sinistra != 0)
    {
        printf("E' presente una porta a sinistra! (Indirizzo: %p)\n", stanza->stanza_sinistra);
    }else if (stanza->stanza_sopra != 0)
    {
        printf("E' presente una porta sopra! (Indirizzo: %p)\n", stanza->stanza_sopra);
    }else if(stanza->stanza_sotto != 0)
    {
        printf("E' presente una porta sotto! (Indirizzo: %p)\n", stanza->stanza_sotto);
    }

    switch (stanza->tipo_stanza)
    {
    case 0:
        printf("Ti trovi in un corridoio\n");
        break;

    case 1:
        printf("Ti trovi su una scala!\n");
        break;

    case 2:
        printf("Ti trovi in una sala dei banchetti!\n");
        break;

    case 3:
        printf("Ti trovi in un magazzino!\n");
        break;

    case 4:
        printf("Ti trovi in un posto di Guardia!\n");
        break;

    case 5:
        printf("Ti trovi in una prigione!\n");
        break;

    case 6:
        printf("Ti trovi in un'armeria!\n");
        break;

    case 7:
        printf("Ti trovi in una moschea!\n");
        break;

    case 8:
        printf("Ti trovi in una torre!\n");
        break;

    case 9:
        printf("Ti trovi in dei bagni!\n");
        break;
    
    default:
        break;
    }

    switch (stanza->tipo_trabocchetto)
    {
    case 1:
        printf("Ci sono delle tegole che cadono dal soffitto!\n");
        break;

    case 2:
        printf("Ci sono delle lame a scomparsa!\n");
        break;

    case 3:
        printf("C'è una buca!\n");
        break;

    case 4:
        printf("C'è un burrone!\n");
        break;
        
    default:
        printf("Nella stanza non sono presenti trabocchetti!\n");
        break;
    }

    stanza->tipo_tesoro != 0 ? printf("Nella stanza è presente un tesoro!\n") : printf("Nella stanza non è presente nessun tesoro!\n");
}

static void prendi_tesoro(Giocatore* giocatore, Stanza* stanza)
{
    //E' presente un controllo al momento della decrementazione, in modo da evitare errori nella gestione delle variabili unsigned char.
    switch (stanza->tipo_tesoro)
    {
    case verde_veleno:
        printf("Oh no, hai trovato del veleno! Hai perso un punto vita!\n");
        giocatore->p_vita = (giocatore->p_vita > 1) ? (giocatore->p_vita - 1) : 0;
        break;

    case blu_guarigione:
        giocatore->p_vita == giocatore->p_vita_max ? printf("Hai trovato una cura! Tuttavia la tua salute è già al massimo!\n") : (printf("Hai trovato una cura! La tua salute viene incrementata di un punto!\n"), giocatore->p_vita++);
        break;

    case rosso_aumenta_vita:
        printf("Congratulazioni, hai trovato un bonus di punti vita! La tua salute viene ripristinata e i punti massimi vengono incrementati di 1!\n");
        giocatore->p_vita_max++;
        giocatore->p_vita = giocatore->p_vita_max;
        break;

    case spada_tagliente:
        printf("Hai trovato una spada tagliente! Il numero di dadi attacco viene aumentato di uno!\n");
        giocatore->dadi_attacco++;
        break;

    case scudo:
        printf("Hai trovato uno scudo! Il numero di dadi difesa viene aumentato di uno!\n");
        giocatore->dadi_difesa++;
        break;

    case totem_curativo:
        giocatore->totem_curativo == 0 ? (printf("Hai trovato un totem curativo! Puoi usarlo per curare gli altri giocatori!\n"), giocatore->totem_curativo = 1) : printf("Hai trovato un totem curativo! Tuttavia non puoi raccoglierlo perché ne possiedi già uno!\n");
        break;
    
    default:
        printf("Non è presente nessun tesoro nella stanza!\n");
        break;
    }
}

static void cerca_porta_segreta(Giocatore* giocatore)
{
    char stanze[3][2][20];
    int sceltaMenu, contatoreStanze = 0;
    int probabilità[3] = {33, 20, 15}, contatoreProb = 0; //Probabilità e contatoreProb servono per gestire la probabilità con cui è possibile scoprire una porta segreta.
    if(giocatore->posizione->stanza_destra==NULL)
    {
        strcpy(&stanze[contatoreStanze][0][0],"Destra");
        strcpy(&stanze[contatoreStanze][1][0],"NC");
        contatoreStanze++;
    }

    if(giocatore->posizione->stanza_sinistra==NULL)
    {
        strcpy(&stanze[contatoreStanze][0][0],"Sinistra");
        strcpy(&stanze[contatoreStanze][1][0],"NC");
        contatoreStanze++;
    }

    if(giocatore->posizione->stanza_sopra==NULL)
    {
        strcpy(&stanze[contatoreStanze][0][0],"Sopra");
        strcpy(&stanze[contatoreStanze][1][0],"NC");
        contatoreStanze++;
    }

    if(giocatore->posizione->stanza_sotto==NULL)
    {
        strcpy(&stanze[contatoreStanze][0][0],"Sotto");
        strcpy(&stanze[contatoreStanze][1][0],"NC");
        contatoreStanze++;
    }

    do
    {
        printf("In quale di queste direzioni vuoi cercare la porta segreta?\n");
        for (int i = 0; i < 3; i++)
        {
            printf("%d)%s\n", i+1, &stanze[i][0][0]);
        }
        sceltaMenu = input_int("\n[Input]: ", 1, 3);
        printf("\n");

        if(!strcmp(&stanze[sceltaMenu-1][1][0], "NC"))
        {
            int random = rand() % 100;
            if (random < probabilità[contatoreProb]) {
                printf("Hai trovato una porta segreta!\n");
                genera_stanza_segreta(giocatore, sceltaMenu, stanze);
                return;
            } else {
                printf("Non hai trovato nulla!\n");
                contatoreProb++;
                strcpy(&stanze[sceltaMenu-1][1][0],"Controllato");
            }
        }else
        {
            printf("Hai già controllato in questa direzione! Riprova!\n");
        }
    
    } while (contatoreProb<3);
}

static void passa()
{
    printf("Hai deciso di passare questo turno!\n");
}

static void genera_stanza_segreta(Giocatore* giocatore, int sceltaMenu, char stanze[3][2][20])
{
    Stanza* stanza = malloc(sizeof(Stanza));
    Stanza** stanzaRitorno = malloc(sizeof(Stanza));
    *stanzaRitorno = giocatore->posizione; //Memorizzo la stanza in cui devo ritornare alla fine della funzione.

    stanza->stanza_destra = NULL;
    stanza->stanza_sinistra = NULL;
    stanza->stanza_sopra = NULL;
    stanza->stanza_sotto = NULL;
    

    stanza->tipo_stanza = (rand() % 10);

    int randomTrappola = rand() % 100;
    if (randomTrappola < 65)
        stanza->tipo_trabocchetto = nessuno;
    else if (randomTrappola < 75)
        stanza->tipo_trabocchetto = tegola;
    else if (randomTrappola < 84)
        stanza->tipo_trabocchetto = lame;
    else if (randomTrappola < 92)
        stanza->tipo_trabocchetto = caduta;
    else
        stanza->tipo_trabocchetto = burrone;

    int randomTesoro = rand() % 100;
    if (randomTesoro < 20)
        stanza->tipo_tesoro = nessun_tesoro;
    else if (randomTesoro < 40)
        stanza->tipo_tesoro = verde_veleno;
    else if (randomTesoro < 60)
        stanza->tipo_tesoro = blu_guarigione;
    else if (randomTesoro < 75)
        stanza->tipo_tesoro = rosso_aumenta_vita;
    else if (randomTesoro < 90)
        stanza->tipo_tesoro = spada_tagliente;
    else if (randomTesoro < 98)
        stanza->tipo_tesoro = scudo;
    else
        stanza->tipo_tesoro = totem_curativo;

    //Aggiungo una stanza e la cancello a fine funzione (era scritto così da consegna).
    posiziona_stanza(stanza, 0);
    giocatore->posizione = stanza;

    stanza_segreta(giocatore, stanza);

    giocatore->posizione = *stanzaRitorno;
    canc_stanza();


    free(stanza);
    free(stanzaRitorno);
}

static void stanza_segreta(Giocatore* giocatore, Stanza* stanza)
{
    effetto_trabocchetto(giocatore, stanza);

    int sceltaMenu = 0;
    do
    {
        
        sceltaMenu = input_int("\n---Stanza segreta---\nInserici:\n1)Per vedere i valori del giocatore;\n2)Per ottenere info sulla stanza;\n3)Per prendere il tesoro;\n4)Torna sulla strada principale;\n[Input]: ", 1, 4);
        
        switch (sceltaMenu)
        {
        case 1:
            stampa_giocatore(giocatore);
            break;

        case 2:
            stampa_zona(stanza);
            break;

        case 3:
            prendi_tesoro(giocatore, stanza);
            printf("Tornando sulla strada principale!\n");
            return; 
            break;

        case 4:
            printf("Tornando sulla strada principale!\n");
            return; 
            break;
        
        default:
            break;
        }
    } while (1);
    
}

static void effetto_trabocchetto(Giocatore* giocatore, Stanza* stanza)
{
    int danno = 0;

    switch (giocatore->posizione->tipo_trabocchetto)
    {
    case 1:
        danno = 1;
        giocatore->p_vita = (giocatore->p_vita > danno) ? (giocatore->p_vita - danno) : 0;
        printf("%s perde 1 punto vita a causa di una tegola caduta in testa. Punti vita: %d\n", giocatore->nome_giocatore, giocatore->p_vita);
        break;

    case 2:
        danno = 2;
        giocatore->p_vita = (giocatore->p_vita > danno) ? (giocatore->p_vita - danno) : 0;
        printf("%s perde 2 punti vita a causa delle lame. Punti vita: %d\n", giocatore->nome_giocatore, giocatore->p_vita);
        break;

    case 3:
        danno = (rand() % 2) + 1;
        giocatore->p_vita = (giocatore->p_vita > danno) ? (giocatore->p_vita - danno) : 0;
        printf("%s perde %d punti vita a causa di una caduta. Punti vita: %d\n", giocatore->nome_giocatore, danno, giocatore->p_vita);
        break;

    case 4:
        danno = (rand() % 2) + 1;
        giocatore->p_vita = (giocatore->p_vita > danno) ? (giocatore->p_vita - danno) : 0;
        printf("%s perde %d punti vita a causa di un burrone e viene incapacitato per il prossimo turno. Punti vita: %d\n", giocatore->nome_giocatore, danno, giocatore->p_vita);
        giocatore->incapacitato = 1;
        break;
        
    default:
        printf("Nella stanza non sono presenti trabocchetti!\n");
        break;
    }

}

void termina_gioco(Giocatore** giocatori)
{
    int grandezza_array = 0;

    //Calcolo la grandezza dell'array contenente il messaggio finale.
    for (int i = 0; i < numGiocatori; i++) {
        grandezza_array += strlen((*giocatori)[i].nome_giocatore) + 12;
    }

    //adesso creo un stringa "temporanea" per salutare i giocatori.
    char* buffer = malloc(grandezza_array + 1);
    buffer[0] = '\0';
    for (int i = 0; i < numGiocatori; i++) {
        char temporanea[100];
        //snprintf serve per scrivere una stringa formattata in un buffer di memoria, fa parte della libreria standard di C.
        if (i == numGiocatori - 1 && numGiocatori > 1) {
            snprintf(temporanea, sizeof(temporanea), " e %s", (*giocatori)[i].nome_giocatore);
        } else if (i > 0) {
            snprintf(temporanea, sizeof(temporanea), ", %s", (*giocatori)[i].nome_giocatore);
        } else {
            snprintf(temporanea, sizeof(temporanea), "%s", (*giocatori)[i].nome_giocatore);
        }
        strcat(buffer, temporanea);
    }

    if (numGiocatori > 0) {
        strcat(buffer, ".");
    }

    printf("---Grazie per aver giocato a Prince of Inertia---\n");
    printf("Arrivederci %s\n", buffer);

    free(buffer);

    numGiocatori=0;
}

void crediti(Giocatore** giocatori, char** vincitori)
{
    printf("---Questo gioco è stato sviluppato da Angelo Guerrieri---\n");
    printf("Vincitori delle ultime 3 partite:\n");
    for (int i = 0; i < 3; i++)
    {
        printf("-%s\n", vincitori[i]);
    }
    
}

static void resetta_partita(Giocatore** giocatori)
{
    //In questa funzione resetto i valori base dal gioco, in modo da permettere l'inizio di una nuova partita.
    for (int i = 0; i < numGiocatori; i++)
    {
        (*giocatori)[i].p_vita = 3;
        (*giocatori)[i].p_vita_max = 3;
        (*giocatori)[i].dadi_attacco = 2;
        (*giocatori)[i].dadi_difesa = 2;
        (*giocatori)[i].segretaContr = realloc((*giocatori)[i].segretaContr, sizeof(Stanza));
        (*giocatori)[i].segretaContr = NULL;
        (*giocatori)[i].tesoroContr = realloc((*giocatori)[i].tesoroContr, sizeof(Stanza));
        (*giocatori)[i].tesoroContr = NULL;
        (*giocatori)[i].num_segretaContr = 0;
        (*giocatori)[i].num_tesoroContr = 0;
        (*giocatori)[i].totem_curativo = 0;
        (*giocatori)[i].vecchiaStanza = NULL;
        (*giocatori)[i].stanzaFuga = NULL;

        if((*giocatori)[i].classe_giocatore == principe)
        {
            (*giocatori)[i].pos_fuga = 1;
        }else{
            (*giocatori)[i].pos_fuga = 2;
        }
    
    }

    for (int i = 0; i < numGiocatori; i++)
    {
        (*giocatori)[i].posizione = pfirst;
    }

}

int input_int(char *prompt, int min, int max)
{
    //Grazie a questa funzione posso controllare che tutti i valori inseriti siano validi, senza causare errori e segmentation fault.
    char sceltaMenuChar[10];
    int sceltaMenu = 0;

    do
    {
        printf("%s", prompt);
        if (fgets(sceltaMenuChar, sizeof(sceltaMenuChar), stdin) == NULL) {
            printf("Errore di input. Riprova.\n");
            continue;
        }

        if (sceltaMenuChar[strlen(sceltaMenuChar) - 1] != '\n') {
            printf("Input troppo lungo. Riprova.\n");
            while (getchar() != '\n');
            continue;
        }

        sceltaMenuChar[strcspn(sceltaMenuChar, "\n")] = '\0';

        if (sceltaMenuChar[0] == '\0') {
            printf("Input vuoto, per favore inserisci un valore valido.\n");
            continue;
        }

        //Verifica che ogni carattere sia un numero o un segno negativo valido

        int valido = 1;

        for (int i = 0; i < strlen(sceltaMenuChar); i++) {
            if (!((sceltaMenuChar[i] >= '0' && sceltaMenuChar[i] <= '9') || 
                  (i == 0 && sceltaMenuChar[i] == '-'))) {
                valido = 0;
                break;
            }
        }

        if (!valido) {
            printf("Errore di input. Inserisci solo numeri interi. Riprova.\n");
            continue;
        }

        if (sscanf(sceltaMenuChar, "%d", &sceltaMenu) != 1) {
            printf("Errore di input. Riprova.\n");
            continue;
        }

        if(sceltaMenu > max || sceltaMenu < min)
        {
            printf("Il numero deve essere compreso tra %d e %d. Riprova!\n", min, max);
            continue;
        }else
        {
            break;
        }

    } while(1);

    return sceltaMenu;
}

char* input_string(char *prompt, int lunghezza_stringa) {
    //Il concetto è uguale a input_int ma per l'input sicuro delle stringhe.
    char *stringa = malloc(lunghezza_stringa * sizeof(char));

    while (1) {
        printf("%s", prompt);
        
        if (fgets(stringa, lunghezza_stringa, stdin) == NULL) {
            printf("Errore di input. Riprova.\n");
            while (getchar() != '\n');
            continue;
        }

        stringa[strcspn(stringa, "\n")] = '\0';

        if (strlen(stringa) == lunghezza_stringa - 1 && stringa[lunghezza_stringa - 2] != '\0') {
            printf("La stringa è troppo lunga. Deve essere al massimo di %d caratteri. Riprova.\n", lunghezza_stringa - 1);
            while (getchar() != '\n');
            continue;
        }

        if (stringa[0] == '\0') {
            printf("Input vuoto, per favore inserisci un valore valido.\n");
            continue;
        }

        int valido = 1;
        for (int i = 0; i < strlen(stringa); i++) {
        char c = stringa[i];
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) {
                valido = 0;
                break;
            }
        }

        if (!valido) {
            printf("Input non valido. La stringa deve contenere solo lettere e numeri.\n");
            continue;
        }

        break;
    }
    return stringa;
}

static void inserisci_vincitori(char nome[21], char** vincitori)
{
    //Controllo se sono rimasti slot vuoti per inserire i vincitori.
    if (vincitori[0] != NULL && !strcmp(vincitori[0], "Slot Vuoto"))
    {
        char* temp = realloc(vincitori[0], strlen(nome) + 1);
        vincitori[0] = temp;
        strcpy(vincitori[0], nome);
        return;
    }

    //In caso non fossero rimasti slot vuoti, sposto l'array di una posizione e inserisco in cima il nuovo vincitore.
    for (int i = 2; i >= 0; i--)
    {
        if (vincitori[i] != NULL && !strcmp(vincitori[i], "Slot Vuoto") && strcmp(vincitori[0], "Slot Vuoto"))
        {
            char* temp = realloc(vincitori[i], strlen(nome) + 1);
            vincitori[i] = temp;
            strcpy(vincitori[i], nome);
            
            for (int i = 1; i >= 0; i--) {
                char* temporaneo = realloc(vincitori[i + 1], strlen(vincitori[i]) + 1);
                vincitori[i + 1] = temporaneo;
                strcpy(vincitori[i + 1], vincitori[i]);

            }

            char* temporaneo = realloc(vincitori[0], strlen(nome) + 1);
            vincitori[0] = temporaneo;
            strcpy(vincitori[0], nome);
            return;
        }
    }

    for (int i = 1; i >= 0; i--) {
        char* temporaneo = realloc(vincitori[i + 1], strlen(vincitori[i]) + 1);
        vincitori[i + 1] = temporaneo;
        strcpy(vincitori[i + 1], vincitori[i]);

    }

    char* temporaneo = realloc(vincitori[0], strlen(nome) + 1);
    vincitori[0] = temporaneo;
    strcpy(vincitori[0], nome);

}

void stampa_introduzione()
{
    //Questa è una funzione extra, con il compito di leggere da file una piccola introduzione al gioco.
    FILE *file = fopen("introduzione", "r");
    
    if (file == NULL) {
        printf("Errore.\n");
        return;
    }

    printf("\n");

    char ch; 
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    fclose(file);

    printf("\n");
}

static void rianima_giocatore(Giocatore** giocatori)
{
    //La funzione si occupa di impostare nuovamente i punti vita, dei giocatori selezionati, al massimo.
    int sceltaMenu;
    printf("Quale dei seguenti giocatori vuoi rianimare?\n");
    for (int i = 0; i < numGiocatori; i++)
    {
        printf("1)%s;\n", giocatori[i]->nome_giocatore);
    }

    sceltaMenu = input_int("\n[Input]: ", 1, numGiocatori);
    giocatori[sceltaMenu-1]->p_vita = giocatori[sceltaMenu-1]->p_vita_max;
}

static void resetta_mappa()
{
    Stanza* temp = pfirst;
    
    while (temp != NULL)
    {
        Stanza* next = NULL;

        if (temp->stanza_destra != NULL)
        {
            next = temp->stanza_destra;
            temp->stanza_destra = NULL;
        }
        else if (temp->stanza_sinistra != NULL)
        {
            next = temp->stanza_sinistra;
            temp->stanza_sinistra = NULL;
        }
        else if (temp->stanza_sopra != NULL)
        {
            next = temp->stanza_sopra;
            temp->stanza_sopra = NULL;
        }
        else if (temp->stanza_sotto != NULL)
        {
            next = temp->stanza_sotto;
            temp->stanza_sotto = NULL;
        }

        free(temp);
        
        temp = next;
    }
}