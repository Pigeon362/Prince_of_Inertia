#ifndef GAMELIB_H
#define GAMELIB_H

extern struct Stanza* pfirst;

typedef enum {principe, doppleganger} tipo_giocatore;

typedef enum Tipo_stanza {
    corridoio,
    scala,
    sala_banchetto,
    magazzino,
    posto_Guardia,
    prigione,
    armeria,
    moschea,
    torre,
    bagni
} Tipo_stanza;

typedef enum Tipo_trabocchetto {
    nessuno,
    tegola,
    lame,
    caduta,
    burrone
} Tipo_trabocchetto;

typedef enum Tipo_tesoro {
    nessun_tesoro,
    verde_veleno,
    blu_guarigione,
    rosso_aumenta_vita,
    spada_tagliente,
    scudo,
    totem_curativo //Questo è un tipo di tesoro extra, può rianimare un altro giocatore ma è estremamene raro ed è possibile portarne solo uno alla volta.
} Tipo_tesoro;

typedef struct Stanza {
    struct Stanza* stanza_destra;
    struct Stanza* stanza_sinistra;
    struct Stanza* stanza_sopra;
    struct Stanza* stanza_sotto;
    Tipo_stanza tipo_stanza;
    Tipo_trabocchetto tipo_trabocchetto;
    Tipo_tesoro tipo_tesoro;
} Stanza;

typedef struct Giocatore {
    char nome_giocatore[21];
    tipo_giocatore classe_giocatore; 
    unsigned char p_vita_max; 
    unsigned char p_vita;
    unsigned char dadi_attacco;
    unsigned char dadi_difesa;
    unsigned char pos_fuga;
    unsigned char incapacitato;
    Stanza* vecchiaStanza;
    Stanza* stanzaFuga;
    Stanza* posizione;
    Stanza** segretaContr;
    Stanza** tesoroContr;
    int num_segretaContr, num_tesoroContr, totem_curativo; //Ho apportato alcune modifiche alla struct giocatore in modo da tenere un registro delle stanze dove il player è stato e dove ha cercato il tesoro o la porta segreta, oltre a un contatore per il totem curativo.
} Giocatore;

typedef struct Nemico {
    char nome_nemico[30];
    unsigned char p_vita;
    unsigned char dadi_attacco;
    unsigned char dadi_difesa;          
} Nemico;

void imposta_gioco(Giocatore** giocatori);
void gioca(Giocatore** giocatori, char** vincitori);
void termina_gioco(Giocatore** giocatori);
void crediti(Giocatore** giocatori, char** vincitori);
void stampa_introduzione();

//Queste due funzioni vengono chiamate in entrambi i file per leggere interi e stringhe
int input_int(char *prompt, int min, int max);
char* input_string(char *prompt, int lunghezza_stringa);

#endif