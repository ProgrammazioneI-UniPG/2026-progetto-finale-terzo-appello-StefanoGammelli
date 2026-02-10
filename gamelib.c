// RICONTROLLARE COMBATTIMENTO

#include "gamelib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

static struct giocatore *lista_giocatori[4];
static int n_giocatori = 0;
static struct zona_mondoreale *mappa_reale = NULL;
static struct zona_soprasotto *mappa_sopra = NULL;

// variabile per crediti
static char vincitore1[20] = "Nessuno";
static char vincitore2[20] = "Nessuno";
static char vincitore3[20] = "Nessuno";

// Definizioni delle funzioni in gamelib.c.
// Piu altre funzioni di supporto.
// Le funzioni richiamabili in main.c non devono essere static.
// Le altre devono essere static (non visibili all'esterno).
static const char *tipo_zona_str[] = {
    "Bosco", "Scuola", "Laboratorio", "Caverna", "Strada",
    "Giardino", "Supermercato", "Centrale elettrica", "Deposito abbandonato", "Stazione polizia", "vuota"};

static const char *tipo_nemico_str[] = {
    "Nessun nemico", "Billi", "Democane", "Demotorzone"};

static const char *tipo_oggetto_str[] = {
    "Nessun oggetto", "Bicicletta", "Maglietta fuocoinferno", "Bussola", "Schitarrata metallica", "vuota"};

// Libera tutta la mappa creata fino ad ora
static void svuota_mappa(struct zona_mondoreale **testa_reale, struct zona_soprasotto **testa_sopra)
{
    // Ciclo dove itera per va continuamente avanti e libera la precedente
    while (*testa_reale != NULL)
    {
        struct zona_mondoreale *temp = *testa_reale;
        *testa_reale = (*testa_reale)->avanti;
        free(temp);
    }

    while (*testa_sopra != NULL)
    {
        struct zona_soprasotto *temp = *testa_sopra;
        *testa_sopra = (*testa_sopra)->avanti;
        free(temp);
    }
    *testa_reale = NULL;
    *testa_sopra = NULL;
}

// Stampa il punto dove l'utente scrive
void freccia()
{
    printf(YELLOW "-->" RESET);
}

// Per sistemi Linux e macOS elimina tutte le scritte per un esperienza più professionale
void ripulisci_schermo()
{
    system("clear");
}

// Stampa un titolo elegante e professionale per il gioco (vedere file main per fonti)
void stampa_titolo()
{

    printf(BLUE " _______  _______  _______  _______     _______ _________ _______  _______  _        _______ \n");
    printf("(  ____ \\(  ___  )(  ____ \\(  ____ \\   (  ____ \\\\__   __/(  ____ )(  ___  )( (    /|(  ____ \\\n");
    printf("| (    \\/| (   ) || (    \\/| (    \\/   | (    \\/   ) (   | (    )|| (   ) ||  \\  ( || (    \\/\n");
    printf("| |      | |   | || (_____ | (__       | (_____    | |   | (____)|| (___) ||   \\ | || (__    \n");
    printf("| |      | |   | |(_____  )|  __)      (_____  )   | |   |     __)|  ___  || (\\ \\) ||  __)   \n");
    printf("| |      | |   | |      ) || (               ) |   | |   | (\\ (   | (   ) || | \\   || (      \n");
    printf("| (____/\\| (___) |/\\____) || (____/\\   /\\____) |   | |   | ) \\ \\__| )   ( || )  \\  || (____/\\\n");
    printf("(_______/(_______)\\_______)(_______/   \\_______)   )_(   |/   \\__/|/     \\||/    )_)(_______/\n" RESET);
    printf("\n");
}

// Usata nel input dei nome dei giocatori, controlla se la stringa è valida
int check_stringa(char str1[])
{
    int p = strlen(str1);

    for (int i = 0; i < p; i++)
        // Funzione che controlla se i valori passati sono numeri o lettere
        if (isalnum(str1[i]) || (str1[i] == ' '))
        {
            continue;
        }
        else
        {
            return 0;
        }

    return 1;
}

// Funzione usata per estrarre un numero a caso fra 0 e una variabile num
static int caso(int num)
{

    int caso = rand() % num;

    return caso;
}

// Funzione che svuota il buffer in caso la stringa inserita superi il limite
static void svuotabuffer()
{
    int scarto = 0;
    // Controllo della presenza di \n, il flush viene ripulito sse \n non è presente nella sringa
    while ((scarto = getchar()) != '\n' && scarto != EOF)
        ;
}

// Funzione che come dice il nome si occupa della personalizzazione dei giocatori: nome, abilità, correzzioni
static void personalizzazione(int giocatori_TOT, struct giocatore *lista_giocatori[])
{
    // Itero per il numero di giocatori che ho
    for (int i = 0; i < giocatori_TOT; i++)
    {
        int id_attacco = 0;
        int id_difesa = 0;
        int id_fortuna = 0;
        int idoneita_nome = 0;
        char name[50];

        ripulisci_schermo();
        stampa_titolo();

        printf(GREEN "\n<----- CREAZIONE GIOCATORE %d ----->\n" RESET, i + 1);
        printf(YELLOW "Inserisci il tuo nome (Max 15 caratteri):\n" RESET);
        freccia();

        do
        {
            fgets(name, 50, stdin);
            if (strchr(name, '\n') != NULL)
                *strchr(name, '\n') = '\0';
            else
                svuotabuffer();

            if (strlen(name) == 0)
            {
                printf(RED "Sei per caso Ulisse? Reinserisci.\n" RESET);
                freccia();
            }
            else if (strlen(name) > 15)
            {
                printf(RED "Nome troppo lungo (max 15 caratteri).\n" RESET);
                freccia();
            }
            else if (check_stringa(name) == 0)
            {
                printf(RED "Caratteri non validi. Usa solo lettere o numeri.\n" RESET);
                freccia();
            }
            else
            {
                int duplicato = 0;
                for (int j = 0; j < i; j++)
                {
                    if (lista_giocatori[j] != NULL && strcmp(name, lista_giocatori[j]->nome) == 0)
                    {
                        duplicato = 1;
                        break;
                    }
                }

                if (duplicato)
                {
                    printf(RED "Nome già preso! Scegline un altro.\n" RESET);
                    freccia();
                }
                else
                {
                    idoneita_nome = 1;
                    printf(GREEN "Nome registrato!\n" RESET);
                }
            }

        } while (idoneita_nome == 0);

        strcpy(lista_giocatori[i]->nome, name);

        // --- INIZIO INPUT POTERI ---
        do
        {
            ripulisci_schermo();
            stampa_titolo();
            printf(GREEN "\n<----- ASSEGNAZIONE POTERI: %s ----->\n" RESET, lista_giocatori[i]->nome);

            // Stampa stato attuale
            printf("Attacco: %d | Difesa: %d | Fortuna: %d\n\n",
                   (lista_giocatori[i]->attacco_psichico == 25) ? 0 : lista_giocatori[i]->attacco_psichico,
                   (lista_giocatori[i]->difesa_psichica == 25) ? 0 : lista_giocatori[i]->difesa_psichica,
                   (lista_giocatori[i]->fortuna == 25) ? 0 : lista_giocatori[i]->fortuna);

            printf("  1) Lancia dado Attacco Psichico\n");
            printf("  2) Lancia dado Difesa Psichica\n");
            printf("  3) Lancia dado Fortuna\n");

            if (id_attacco && id_difesa && id_fortuna)
            {
                printf(GREEN "\n  Tutti i dadi lanciati. Procedi con le correzioni...\n" RESET);
            }
            else
            {
                printf(YELLOW "\nScegli quale caratteristica tirare:\n" RESET);
                freccia();
            }

            // Se abbiamo finito i lanci, usciamo dal loop di lancio e andiamo alle correzioni
            if (id_attacco && id_difesa && id_fortuna)
                break;

            int val = leggi_input(1, 3);

            if (val == 1)
            {
                if (lista_giocatori[i]->attacco_psichico == 25)
                {
                    lista_giocatori[i]->attacco_psichico = caso(20);
                    if (lista_giocatori[i]->attacco_psichico == 0)
                        lista_giocatori[i]->attacco_psichico = 1;
                    id_attacco = 1;
                }
                else
                {
                    printf(RED "Hai già tirato questo dado!\n" RESET);
                }
            }
            else if (val == 2)
            {
                if (lista_giocatori[i]->difesa_psichica == 25)
                {
                    lista_giocatori[i]->difesa_psichica = caso(20);
                    if (lista_giocatori[i]->difesa_psichica == 0)
                        lista_giocatori[i]->difesa_psichica = 1;
                    id_difesa = 1;
                }
                else
                {
                    printf(RED "Hai già tirato questo dado!\n" RESET);
                }
            }
            else if (val == 3)
            {
                if (lista_giocatori[i]->fortuna == 25)
                {
                    lista_giocatori[i]->fortuna = caso(20);
                    if (lista_giocatori[i]->fortuna == 0)
                        lista_giocatori[i]->fortuna = 1;
                    id_fortuna = 1;
                }
                else
                {
                    printf(RED "Hai già tirato questo dado!\n" RESET);
                }
            }

        } while (!(id_attacco && id_difesa && id_fortuna));

        // Correzzioni opzionali
        ripulisci_schermo();
        stampa_titolo();
        printf(GREEN "\n<----- CORREZIONI FINALI: %s ----->\n" RESET, lista_giocatori[i]->nome);

        printf("Statistiche attuali: Attacco %d | Difesa %d | Fortuna %d\n\n",
               lista_giocatori[i]->attacco_psichico,
               lista_giocatori[i]->difesa_psichica,
               lista_giocatori[i]->fortuna);

        printf("  1) +3 Attacco / -3 Difesa\n");
        printf("  2) -3 Attacco / +3 Difesa\n");
        printf("  3) Nessuna modifica\n");

        printf(YELLOW "\nScegli una modifica:\n" RESET);
        freccia();
        int temp = leggi_input(1, 3);

        if (temp == 1)
        {
            lista_giocatori[i]->attacco_psichico += 3;
            if (lista_giocatori[i]->attacco_psichico > 20)
                lista_giocatori[i]->attacco_psichico = 20;

            lista_giocatori[i]->difesa_psichica -= 3;
            if (lista_giocatori[i]->difesa_psichica < 1)
                lista_giocatori[i]->difesa_psichica = 1;
            printf(GREEN "Modifica applicata.\n" RESET);
        }
        else if (temp == 2)
        {
            lista_giocatori[i]->difesa_psichica += 3;
            if (lista_giocatori[i]->difesa_psichica > 20)
                lista_giocatori[i]->difesa_psichica = 20;

            lista_giocatori[i]->attacco_psichico -= 3;
            if (lista_giocatori[i]->attacco_psichico < 1)
                lista_giocatori[i]->attacco_psichico = 1;
            printf(GREEN "Modifica applicata.\n" RESET);
        }
        else
        {
            printf(GREEN "Nessuna modifica applicata.\n" RESET);
        }

        printf(YELLOW "\nPremi 1 per confermare il giocatore.\n" RESET);
        leggi_input(1, 1);
    }

    // Riepilogo dei dati dei giocatori
    ripulisci_schermo();
    stampa_titolo();

    printf(GREEN "<----- RIEPILOGO SQUADRA ----->\n" RESET);
    for (int i = 0; i < giocatori_TOT; i++)
    {

        printf("G%d) %s [Attacco: %d | Difesa: %d | Fortuna: %d]\n",
               i + 1, lista_giocatori[i]->nome,
               lista_giocatori[i]->attacco_psichico,
               lista_giocatori[i]->difesa_psichica,
               lista_giocatori[i]->fortuna);
    }

    // Opzione su Undicivirgolacinque
    printf(YELLOW "\nVuoi trasformare un giocatore in 'UndiciVirgolaCinque'? (1=Si, 0=No)\n" RESET);
    freccia();

    if (leggi_input(0, 1) == 1)
    {
        printf(YELLOW "Scegli il giocatore (1-%d):\n" RESET, giocatori_TOT);
        int undici_vr = leggi_input(1, giocatori_TOT);
        int undici = undici_vr - 1;

        if (lista_giocatori[undici]->attacco_psichico < 16)
            lista_giocatori[undici]->attacco_psichico += 4;
        else
            lista_giocatori[undici]->attacco_psichico = 20;

        if (lista_giocatori[undici]->difesa_psichica < 16)
            lista_giocatori[undici]->difesa_psichica += 4;
        else
            lista_giocatori[undici]->difesa_psichica = 20;

        if (lista_giocatori[undici]->fortuna < 8)
            lista_giocatori[undici]->fortuna = 1;
        else
            lista_giocatori[undici]->fortuna -= 7;

        printf(GREEN "\nPotenziamento riuscito! %s è ora UndiciVirgolaCinque.\n" RESET, lista_giocatori[undici]->nome);
        printf(YELLOW "Premi 1 per continuare.\n" RESET);
        leggi_input(1, 1);
    }
}

// Funzion molto importante all'interno del gioco, dato un qualsiasi input, controlla se rientra nel range fra min e max
int leggi_input(int min, int max)
{
    int id = 0;
    int numero;
    char controllo;
    while (!id)
    {

        // controllo il ritorno di scanf, doppio controllo per tutti le tipologie di input, controllo deve essere per forza \n
        if (scanf("%d%c", &numero, &controllo) == 2)
        {
            if (controllo == '\n')
            {

                if (numero >= min && numero <= max)
                {
                    id = 1;
                }
                else if (min == max)
                {
                    printf(RED "Inserire un input valido.\n" RESET);
                    freccia();
                }
                else
                {
                    printf(RED "Input out of range, Reinserire fra %d e %d.\n", min, max);
                    freccia();
                }
            }
            else
            {
                printf(RED "Inserire input valido. \n" RESET);
                freccia();
                svuotabuffer();
            }
        }
        else
        {
            printf(RED "Inserire un input valido. \n" RESET);
            freccia();
            svuotabuffer();
        }
    }

    return numero;
}

// Funzione richiamata nel momento di generazione mappa automatica
static int genera_mappa(struct zona_mondoreale **testa_reale, struct zona_soprasotto **testa_sopra)
{
    struct zona_mondoreale *ultimo_reale = NULL;
    struct zona_soprasotto *ultimo_sopra = NULL;

    int pos_demotorzone = caso(15);

    for (int i = 0; i < 15; i++)
    {
        // Alloco le struct in memoria dinamica
        struct zona_mondoreale *nuovo_reale = (struct zona_mondoreale *)malloc(sizeof(struct zona_mondoreale));
        struct zona_soprasotto *nuovo_sopra = (struct zona_soprasotto *)malloc(sizeof(struct zona_soprasotto));

        // Le linko verticalmente
        nuovo_reale->link_soprasotto = nuovo_sopra;
        nuovo_sopra->link_mondoreale = nuovo_reale;

        // Gestione dei puntatori avanti/indietro (lista doppiamente concatenata)
        if (i == 0)
        {
            *testa_reale = nuovo_reale;
            *testa_sopra = nuovo_sopra;

            nuovo_reale->indietro = NULL;
            nuovo_sopra->indietro = NULL;
        }
        else
        {
            ultimo_reale->avanti = nuovo_reale;
            nuovo_reale->indietro = ultimo_reale;

            ultimo_sopra->avanti = nuovo_sopra;
            nuovo_sopra->indietro = ultimo_sopra;
        }

        // Assegno le zone
        nuovo_reale->zona = (enum tipo_zona)caso(10);

        // nemico e oggetto
        nuovo_reale->nemico = (enum tipo_nemico)caso(2);

        nuovo_reale->oggetto = (enum tipo_oggetto)caso(5);

        nuovo_sopra->zona = (enum tipo_zona)caso(10);

        int nemico_sopra = caso(2);
        if (nemico_sopra == 1)
        {
            // Democane
            nemico_sopra = 2;
        }
        nuovo_sopra->nemico = (enum tipo_nemico)nemico_sopra;

        // Sovrascrivo con il demotrozne
        if (i == pos_demotorzone)
        {
            // Demotorzone
            nuovo_sopra->nemico = (enum tipo_nemico)3;
        }

        ultimo_reale = nuovo_reale;
        ultimo_sopra = nuovo_sopra;

        nuovo_reale->avanti = NULL;
        nuovo_sopra->avanti = NULL;
    }
    return 1;
}

// Funzione richiamata nel menu generazione mappa per stampare tutta la mappa
static void stampa_mappa(struct zona_mondoreale *testa_reale, struct zona_soprasotto *testa_sopra)
{
    printf(YELLOW "\n<-----MAPPA GENERATA----->\n" RESET);

    struct zona_mondoreale *corrente_mondoreale = testa_reale;
    struct zona_soprasotto *corrente_sopra = testa_sopra;

    for (int i = 0; i < 15; i++)
    {

        printf(
            GREEN "--- ZONA %d ---\n" RESET YELLOW
                  "Tipo zona Mondoreale:" UNDERLINE "%s\n" RESET YELLOW
                  "Tipo nemico Mondoreale:" UNDERLINE "%s" RESET YELLOW "\n"
                  "Tipo oggetto Mondoreale:" UNDERLINE "%s" RESET YELLOW "\n\n"
                  "Tipo zona Sottosopra:" UNDERLINE "%s\n" RESET YELLOW
                  "Tipo nemico Sottosopra:" UNDERLINE "%s" RESET YELLOW "\n"
                  "--------------------------\n\n" RESET,
            (i + 1),
            tipo_zona_str[(int)corrente_mondoreale->zona],
            tipo_nemico_str[(int)corrente_mondoreale->nemico],
            tipo_oggetto_str[(int)corrente_mondoreale->oggetto],
            tipo_zona_str[(int)corrente_sopra->zona],
            tipo_nemico_str[(int)corrente_sopra->nemico]);

        corrente_mondoreale = corrente_mondoreale->avanti;
        corrente_sopra = corrente_sopra->avanti;
    }
}

// FUnzione richiamata nel menu generazione mappa per inserire la singola zona
static int inserisci_zona(struct zona_mondoreale **testa_reale, struct zona_soprasotto **testa_sopra)
{

    if (testa_reale == NULL || *testa_reale == NULL || testa_sopra == NULL || *testa_sopra == NULL)
    {
        printf(RED "Mappa non presente. Generala prima di modificare.\n" RESET);
        return 0;
    }

    ripulisci_schermo();
    stampa_titolo();

    printf(GREEN "Scegli quale mappa modificare:\n" RESET);
    printf(YELLOW "1) Mondo reale\n2) Soprasotto\n" RESET);
    freccia();
    int scelta_mondo = leggi_input(1, 2);

    printf(YELLOW "Inserisci indice zona (1-15):\n" RESET);
    freccia();
    int indice = leggi_input(1, 15);

    if (scelta_mondo == 1)
    {
        struct zona_mondoreale *corrente = *testa_reale;

        // Itero fino a giungere alla zona richiesta
        for (int k = 1; k < indice && corrente != NULL; k++)
        {
            corrente = corrente->avanti;
        }
        if (corrente == NULL)
        {
            printf(RED "Indice fuori range.\n" RESET);
            return 0;
        }

        printf(YELLOW "\nZona %d - Mondo reale attuale:\n" RESET, indice);
        printf("Tipo zona: %s\n", tipo_zona_str[(int)corrente->zona]);
        printf("Tipo nemico: %s\n", tipo_nemico_str[(int)corrente->nemico]);
        printf("Tipo oggetto: %s\n\n" RESET, tipo_oggetto_str[(int)corrente->oggetto]);

        // Una volta scelta la zona da modificare e trovata, stampo attraverso un array tutte le caratteristiche da scegliere
        printf(GREEN "Scegli nuovo tipo zona:\n" RESET);
        for (int j = 0; j < 10; j++)
        {
            printf(YELLOW "%d) %s\n" RESET, j + 1, tipo_zona_str[j]);
        }

        freccia();
        corrente->zona = (enum tipo_zona)(leggi_input(1, 10) - 1);

        printf(GREEN "Scegli nuovo nemico:\n" RESET);
        for (int j = 0; j < 4; j++)
        {
            printf(YELLOW "%d) %s\n" RESET, j + 1, tipo_nemico_str[j]);
        }
        freccia();
        corrente->nemico = (enum tipo_nemico)(leggi_input(1, 4) - 1);

        printf(GREEN "Scegli nuovo oggetto:\n" RESET);
        for (int j = 0; j < 5; j++)
        {
            printf(YELLOW "%d) %s\n" RESET, j + 1, tipo_oggetto_str[j]);
        }
        freccia();
        corrente->oggetto = (enum tipo_oggetto)(leggi_input(1, 5) - 1);

        printf(GREEN "Zona %d del mondo reale aggiornata.\n" RESET, indice);
    }
    else
    {
        struct zona_soprasotto *corrente = *testa_sopra;
        for (int k = 1; k < indice && corrente != NULL; k++)
            corrente = corrente->avanti;

        if (corrente == NULL)
        {
            printf(RED "Indice fuori range.\n" RESET);
            return 0;
        }

        printf(GREEN "\nZona %d - Soprasotto attuale:\n" RESET, indice);
        printf(YELLOW "Tipo zona: %s\n", tipo_zona_str[(int)corrente->zona]);
        printf("Tipo nemico: %s\n\n" RESET, tipo_nemico_str[(int)corrente->nemico]);

        printf(YELLOW "Scegli nuovo tipo zona:\n" RESET);
        for (int j = 0; j < 10; j++)
        {
            printf(GREEN "%d) %s\n" RESET, j + 1, tipo_zona_str[j]);
        }
        freccia();
        corrente->zona = (enum tipo_zona)(leggi_input(1, 10) - 1);

        printf(GREEN "Scegli nuovo nemico:\n" RESET);
        for (int j = 0; j < 4; j++)
        {
            printf(YELLOW "%d) %s\n" RESET, j + 1, tipo_nemico_str[j]);
        }
        freccia();
        corrente->nemico = (enum tipo_nemico)(leggi_input(1, 4) - 1);

        printf(GREEN "Zona %d del soprasotto aggiornata.\n" RESET, indice);
    }

    printf(YELLOW "zona %d modificata con successo!\n Premi 1 per tornare al menu generazione mappa. \n" RESET, indice);
    leggi_input(1, 1);
    return 1;
}

// Funzione che "cancella la zona", aggiunge semplicemente un flag "nessuna zona" in modo che nel controllo finale si riconoscola
static int cancella_zona(struct zona_mondoreale **testa_reale, struct zona_soprasotto **testa_sopra)
{
    if (testa_reale == NULL || *testa_reale == NULL || testa_sopra == NULL || *testa_sopra == NULL)
    {
        printf(RED "Mappa non presente. Generala prima di modificare.\n" RESET);
        return 0;
    }
    ripulisci_schermo();
    stampa_titolo();

    printf(GREEN "Scegli quale mappa eliminare:\n" RESET);
    printf(YELLOW "1) Mondo reale\n2) Soprasotto\n" RESET);
    freccia();
    int scelta_mondo = leggi_input(1, 2);

    printf(YELLOW "Inserisci indice zona (1-15):\n" RESET);
    freccia();
    int indice = leggi_input(1, 15);

    struct zona_mondoreale *corrente_reale = *testa_reale;
    struct zona_soprasotto *corrente_sopra = *testa_sopra;

    if (scelta_mondo == 1)
    {
        // Itera dato un indice per poi aggiungere il flag
        for (int i = 1; i < indice; i++)
        {
            corrente_reale = corrente_reale->avanti;
        }
        corrente_reale->oggetto = (enum tipo_oggetto)5;
        corrente_reale->zona = (enum tipo_zona)10;
        corrente_reale->nemico = (enum tipo_nemico)4;
    }
    if (scelta_mondo == 2)
    {
        for (int i = 1; i < indice; i++)
        {
            corrente_sopra = corrente_sopra->avanti;
        }
        corrente_sopra->zona = (enum tipo_zona)10;
        corrente_sopra->nemico = (enum tipo_nemico)4;
    }

    printf(YELLOW "zona %d cancellata con successo!\n Premi 1 per tornare al menu generazione mappa. \n" RESET, indice);
    leggi_input(1, 1);
    return 1;
}

// Una volta finita la generazione della mappa, si controlla la funzione per verificare che sia idonea seguendo le regole del gioco
static int controllofinale(struct zona_mondoreale *testa_reale, struct zona_soprasotto *testa_sopra)
{
    if (testa_reale == NULL || testa_sopra == NULL)
    {
        return 0;
    }

    struct zona_mondoreale *corrente_reale = testa_reale;
    struct zona_soprasotto *corrente_sopra = testa_sopra;
    int validita = 0;
    int conta_demotorzone = 0;

    for (int i = 0; i < 15; i++)
    {
        validita = 1;

        if (corrente_sopra->zona == (enum tipo_zona)10 || corrente_reale->zona == (enum tipo_zona)10)
        {
            validita = 0;
            break;
        }
        if (corrente_sopra->nemico == (enum tipo_nemico)3)
        {
            conta_demotorzone++;
        }

        corrente_reale = corrente_reale->avanti;
        corrente_sopra = corrente_sopra->avanti;
    }

    if (conta_demotorzone != 1)
    {
        validita = 0;
    }
    return validita;
}

// Stampa la singola zona decisa dal giocatore
static void stampa_zona(struct zona_mondoreale *testa_reale, struct zona_soprasotto *testa_sopra)
{
    if (testa_reale == NULL || testa_sopra == NULL)
    {
        printf("ERRORE: Lista non trovata!\n");
        return;
    }

    struct zona_mondoreale *corrente_mondoreale = testa_reale;
    struct zona_soprasotto *corrente_sopra = testa_sopra;

    printf("Inserisci una zona (1,15)\n");
    int zona = leggi_input(1, 15);

    // Itero per trovare la zona
    for (int i = 1; i < zona; i++)
    {
        corrente_mondoreale = corrente_mondoreale->avanti;
        corrente_sopra = corrente_sopra->avanti;
    }
    printf(
        GREEN "--- ZONA %d ---\n" RESET YELLOW
              "Tipo zona Mondoreale:" UNDERLINE "%s\n" RESET YELLOW
              "Tipo nemico Mondoreale:" UNDERLINE "%s" RESET YELLOW "\n"
              "Tipo oggetto Mondoreale:" UNDERLINE "%s" RESET YELLOW "\n\n"
              "Tipo zona Sottosopra:" UNDERLINE "%s\n" RESET YELLOW
              "Tipo nemico Sottosopra:" UNDERLINE "%s" RESET YELLOW "\n"
              "--------------------------\n\n" RESET,
        zona,
        tipo_zona_str[(int)corrente_mondoreale->zona],
        tipo_nemico_str[(int)corrente_mondoreale->nemico],
        tipo_oggetto_str[(int)corrente_mondoreale->oggetto],
        tipo_zona_str[(int)corrente_sopra->zona],
        tipo_nemico_str[(int)corrente_sopra->nemico]);
}

// Cambia la posizione del giocatore, da una lista all'altra
static int cambia_mondo(struct giocatore *giocatore)
{

    if (giocatore->mondo == 0)
    {
        giocatore->mondo = 1;
        giocatore->pos_soprasotto = giocatore->pos_mondoreale->link_soprasotto;
        printf(BLUE "\n<--- PASSAGGIO DIMENSIONALE --->\n" RESET);
        printf(BLUE "Sei entrato in un portale interdimensionale... Benvenuto nel SOPRASOTTO.\n" RESET);
        printf("Posizione: %s\n", tipo_zona_str[giocatore->pos_soprasotto->zona]);
    }
    else
    {
        giocatore->mondo = 0;
        giocatore->pos_mondoreale = giocatore->pos_soprasotto->link_mondoreale;
        printf(YELLOW "\n<--- PASSAGGIO DIMENSIONALE --->\n" RESET);
        printf(YELLOW "Sei entrato in un portale interdimensionale... Benvenuto nel MONDO REALE.\n" RESET);
        printf("Posizione: %s\n", tipo_zona_str[giocatore->pos_mondoreale->zona]);
    }
    return 1;
}

// Sposta il giocatore di zona in zona
static int avanza(struct giocatore *giocatore, int ignora_ostacolo)
{
    if (giocatore->mondo == 0)
    {
        // Se c'è un nemico E non abbiamo il lasciapassare (ignora_ostacolo è 0)
        if (!ignora_ostacolo && giocatore->pos_mondoreale != NULL && giocatore->pos_mondoreale->nemico != nessun_nemico)
        {
            printf(RED "\nC'è un %s che ti sbarra la strada! Sconfiggilo prima.\n" RESET, tipo_nemico_str[giocatore->pos_mondoreale->nemico]);
            return 0;
        }
        else if (giocatore->pos_mondoreale->avanti != NULL)
        {
            giocatore->pos_mondoreale = giocatore->pos_mondoreale->avanti;
            printf(GREEN "Sei arrivato in: %s (Mondo Reale)\n" RESET, tipo_zona_str[giocatore->pos_mondoreale->zona]);
            printf("Nemico presente: %s\n", tipo_nemico_str[giocatore->pos_mondoreale->nemico]);
            return 1;
        }
        else
        {
            printf(RED "Sei all'ultima zona! Non puoi avanzare oltre.\n" RESET);
            return 0;
        }
    }
    else
    {
        // Stessa logica per il soprasotto
        if (!ignora_ostacolo && giocatore->pos_soprasotto != NULL && giocatore->pos_soprasotto->nemico != nessun_nemico)
        {
            printf(RED "\nUn'ombra (%s) ti blocca il passaggio! Combatti.\n" RESET, tipo_nemico_str[giocatore->pos_soprasotto->nemico]);
            return 0;
        }

        if (giocatore->pos_soprasotto == NULL)
        {
            giocatore->pos_soprasotto = mappa_sopra;
            printf(GREEN "Sei apparso nella prima zona oscura: %s\n" RESET, tipo_zona_str[giocatore->pos_soprasotto->zona]);
            return 1;
        }
        else if (giocatore->pos_soprasotto->avanti != NULL)
        {
            giocatore->pos_soprasotto = giocatore->pos_soprasotto->avanti;
            printf(GREEN "Sei arrivato in: %s (Soprasotto)\n" RESET, tipo_zona_str[giocatore->pos_soprasotto->zona]);
            printf("Nemico presente: %s\n", tipo_nemico_str[giocatore->pos_soprasotto->nemico]);
            return 1;
        }
        else
        {
            printf(RED "Sei alla fine dell'incubo! Non puoi avanzare.\n" RESET);
            return 0;
        }
    }
}

// Sposta il giocatore di zona in zona a ritroso
static int indietreggia(struct giocatore *giocatore, int ignora_ostacolo)
{
    // Controllo presenza nemici: blocca solo se ignora_ostacolo è falso (0)
    if (!ignora_ostacolo &&
        ((giocatore->mondo == 0 && giocatore->pos_mondoreale != NULL && giocatore->pos_mondoreale->nemico != nessun_nemico) ||
         (giocatore->mondo == 1 && giocatore->pos_soprasotto != NULL && giocatore->pos_soprasotto->nemico != nessun_nemico)))
    {
        printf(RED "Il nemico ti impedisce di ritirarti! Devi combattere.\n" RESET);
        return 0;
    }

    if (giocatore->mondo == 0)
    {
        if (giocatore->pos_mondoreale->indietro == NULL)
        {
            printf(RED "Sei all'inizio! Non puoi andare indietro.\n" RESET);
            return 0;
        }
        giocatore->pos_mondoreale = giocatore->pos_mondoreale->indietro;
        printf(GREEN "Sei tornato a: %s (Mondo Reale)\n" RESET, tipo_zona_str[giocatore->pos_mondoreale->zona]);
        return 1;
    }
    else
    {
        if (giocatore->pos_soprasotto->indietro == NULL)
        {
            printf(RED "Sei all'inizio! Non puoi indietreggiare.\n" RESET);
            return 0;
        }
        giocatore->pos_soprasotto = giocatore->pos_soprasotto->indietro;
        printf(BLUE "Sei tornato a: %s (Soprasotto)\n" RESET, tipo_zona_str[giocatore->pos_soprasotto->zona]);
        return 1;
    }
}

// Funzione che gestisce il combattimento, 1 se si è sconfitto il demotorzone, 0 se il combattimento è andato a buon fine, -1 se è morto il giocatore e 2 se e stato sconfitto il nemico ma persiste nella zona
static int combatti(struct giocatore *giocatore)
{    /*
----STILE DI COMBATTIMENTO----
Su consiglio di amici e giocatori di d&d effettivi, le regole del combattimento sono molto basilari ma rendono il gioco abbastanza giocabile.
Oltre alle regole definite dalla traccia il gioco funziona:
attacco-->si tira un dado da 0-att.psic.
difesa--> si tira un dado da 0-dif.psic.
attacco e difesa si invertono in base a chi attacca, nemico o giocatore effettivo, per vincere lo scontro, ambo le parti devono fare un valore maggiore della difesa avversaria.
in caso l'attacco del nemico è maggiore alla propria difesa, si puo tirare un D20 e se risulta inferiore alla proprià fortuna si sopravvive e si continua normalmente.
Ogni nemico ha delle sue statistiche.
*/
    struct zona_mondoreale *z_reale = giocatore->pos_mondoreale;
    struct zona_soprasotto *z_sopra = giocatore->pos_soprasotto;
    enum tipo_nemico nemico_id;

    // 1. Identificazione nemico
    if (giocatore->mondo == 0)
        nemico_id = z_reale->nemico;
    else
        nemico_id = z_sopra->nemico;

    if (nemico_id == nessun_nemico || nemico_id == nemico_vuoto)
    {
        printf(GREEN "\nNon c'è nessuno qui con cui combattere.\n" RESET);
        return 0;
    }

    printf(RED "\nInizia il combattimento contro: %s!\n" RESET, tipo_nemico_str[nemico_id]);

    int forza_nemico = 0;
    if (nemico_id == billi)
        forza_nemico = 5;
    else if (nemico_id == democane)
        forza_nemico = 10;
    else if (nemico_id == demotorzone)
        forza_nemico = 15;

    int combattimento_attivo = 1;

    while (combattimento_attivo)
    {
        int scelta = 0;

        if (giocatore->mondo == 1)
        {
            printf(YELLOW "\n1) Attacca\n2) Tenta Fuga (Cambia Mondo)\n" RESET);
            scelta = leggi_input(1, 2);
        }
        else
        {
            printf(YELLOW "\nPremi 1 per sferrare un attacco!\n" RESET);
            leggi_input(1, 1);
            scelta = 1;
        }

        if (scelta == 2)
        {
            int dado = (rand() % 20) + 1;
            if (dado < giocatore->fortuna)
            {
                printf(GREEN "Fuga riuscita grazie alla tua fortuna!\n" RESET);
                cambia_mondo(giocatore);
                return 0;
            }
            printf(RED "Fuga fallita! Sei inciampato... il nemico ne approfitta!\n" RESET);
        }
        else
        {
            int tuo_colpo = rand() % (giocatore->attacco_psichico + 1);
            int parata_nemico = rand() % (forza_nemico + 1);

            printf("Tuo attacco: %d vs Difesa nemico: %d\n", tuo_colpo, parata_nemico);

            if (tuo_colpo > parata_nemico)
            {
                printf(GREEN "Hai sconfitto il %s!\n" RESET, tipo_nemico_str[nemico_id]);

                // Vittoria Suprema
                if (nemico_id == demotorzone)
                    return 1;

                int probabilita_sparizione = rand() % 100;

                if (probabilita_sparizione < 50)
                {
                    printf(YELLOW "Il nemico scappa via... La zona è libera.\n" RESET);
                    if (giocatore->mondo == 0)
                        z_reale->nemico = nessun_nemico;
                    else
                        z_sopra->nemico = nessun_nemico;
                    // Vittoria standard
                    return 0;
                }
                else
                {
                    printf(YELLOW "Il nemico è stato sconfitto, ma rimane a difendere la zona.\n" RESET);
                    // Ritorna 2 per il nemico sconfitto ma persistente
                    return 2;
                }
            }
            else
            {
                printf(RED "Il nemico ha parato il colpo!\n" RESET);
            }
        }

        printf(RED "\nIl nemico contrattacca!\n" RESET);
        int attacco_nemico = rand() % (forza_nemico + 5);
        int tua_parata = rand() % (giocatore->difesa_psichica + 1);

        printf("Attacco nemico: %d vs Tua Difesa: %d\n", attacco_nemico, tua_parata);

        if (attacco_nemico > tua_parata)
        {
            printf(RED "Ha trapassato al tua difesa!\n" RESET);
            int dado_salvezza = (rand() % 20) + 1;
            printf("Tiro salvezza (Fortuna): %d su %d\n", dado_salvezza, giocatore->fortuna);

            if (dado_salvezza >= giocatore->fortuna)
            {
                printf(RED "COLPO MORTALE! Non sei sopravvissuto.\n" RESET);
                return -1;
            }
            else
            {
                printf(GREEN "Per miracolo sei sopravvissuto al colpo!\n" RESET);
            }
        }
        else
        {
            printf(GREEN "Hai parato il colpo con successo!\n" RESET);
        }
    }
    return 0;
}

// Funzione di gestione di personalizzazione, genera mappa e di tutte le modifiche di impostazione gioco
int imposta_gioco()
{

    if (mappa_reale != NULL || mappa_sopra != NULL)
    {
        svuota_mappa(&mappa_reale, &mappa_sopra);
    }

    if (n_giocatori > 0)
    {
        for (int i = 0; i < 4; i++)
        {
            if (lista_giocatori[i] != NULL)
            {
                free(lista_giocatori[i]);
                lista_giocatori[i] = NULL;
            }
        }
        n_giocatori = 0;
    }

    ripulisci_schermo();
    stampa_titolo();

    printf(YELLOW "Inserisci numero giocatori (1-4):\n" RESET);
    freccia();

    n_giocatori = leggi_input(1, 4);

    for (int i = 0; i < n_giocatori; i++)
    {
        lista_giocatori[i] = (struct giocatore *)malloc(sizeof(struct giocatore));
        if (lista_giocatori[i] == NULL)
        {
            printf(RED "Errore critico di memoria!\n" RESET);
            exit(1);
        }
        // Valori più grandi del limite per far capire che il dado non è stato tirato
        lista_giocatori[i]->attacco_psichico = 25;
        lista_giocatori[i]->fortuna = 25;
        lista_giocatori[i]->difesa_psichica = 25;

        for (int k = 0; k < 3; k++)
            lista_giocatori[i]->zaino[k] = nessun_oggetto;

        lista_giocatori[i]->pos_mondoreale = NULL;
        lista_giocatori[i]->pos_soprasotto = NULL;
        lista_giocatori[i]->mondo = 0;
    }

    personalizzazione(n_giocatori, lista_giocatori);

    int termina = 1;
    int mappa_generata = 0;

    do
    {
        ripulisci_schermo();
        stampa_titolo();

        printf(GREEN "\n<----- GESTIONE MAPPA ----->\n" RESET);
        printf("  1) Generazione automatica mappa\n");
        printf("  2) Inserisci zona manuale\n");
        printf("  3) Elimina zona manuale\n");
        printf("  4) Stampa mappa completa\n");
        printf("  5) Stampa singola zona\n");
        printf("  6) Termina e salva configurazione\n");

        if (mappa_generata)
            printf(GREEN "\n[Stato: Mappa Presente]\n" RESET);
        else
            printf(RED "\n[Stato: Nessuna Mappa]\n" RESET);

        printf(YELLOW "Scegli un'opzione:\n" RESET);
        freccia();

        switch (leggi_input(1, 6))
        {
        case 1:
            if (mappa_generata == 1)
            {
                svuota_mappa(&mappa_reale, &mappa_sopra);
            }
            mappa_generata = genera_mappa(&mappa_reale, &mappa_sopra);
            printf(GREEN "\n[OK] Mappa generata con successo!\n" RESET);
            printf(YELLOW "Premi 1 per continuare.\n" RESET);
            leggi_input(1, 1);
            break;

        case 2:
            if (mappa_generata == 1)
            {
                inserisci_zona(&mappa_reale, &mappa_sopra);
            }
            else
            {
                printf(RED "\n[ERRORE] Devi prima generare la mappa (Opzione 1).\n" RESET);
                printf(YELLOW "Premi 1 per continuare.\n" RESET);
                leggi_input(1, 1);
            }
            break;

        case 3:
            cancella_zona(&mappa_reale, &mappa_sopra);
            break;

        case 4:
            stampa_mappa(mappa_reale, mappa_sopra);
            printf(YELLOW "Premi 1 per continuare.\n" RESET);
            leggi_input(1, 1);
            break;

        case 5:
            stampa_zona(mappa_reale, mappa_sopra);
            printf(YELLOW "Premi 1 per continuare.\n" RESET);
            leggi_input(1, 1);
            break;

        case 6:
            if (controllofinale(mappa_reale, mappa_sopra))
            {
                termina = 0;
            }
            else
            {
                printf(RED "\n[ERRORE] La mappa non è valida.\n" RESET);
                printf("Assicurati che ci siano 15 zone e un solo Demotorzone.\n");
                printf(YELLOW "Premi 1 per continuare.\n" RESET);
                leggi_input(1, 1);
            }
        }
    } while (termina == 1);

    return 1;
}

// Raccoglie l'oggetto della zona
static void raccogli_oggetto(struct giocatore *giocatore, int ignora_ostacolo)
{
    // Controllo presenza oggetto
    if (giocatore->mondo == 0 && (giocatore->pos_mondoreale->oggetto == nessun_oggetto || giocatore->pos_mondoreale->oggetto == oggetto_vuoto))
    {
        printf(RED "Non c'è niente da raccogliere qui!\n" RESET);
        return;
    }

    if (giocatore->mondo == 1)
    {
        printf(RED "Non ci sono oggetti nel Soprasotto!\n" RESET);
        return;
    }

    // Controllo presenza nemico: se ignora_ostacolo è 1, salta questo if
    if (!ignora_ostacolo && giocatore->pos_mondoreale->nemico != nessun_nemico)
    {
        printf(RED "Non puoi raccogliere l'oggetto se prima non hai sconfitto il nemico!\n" RESET);
        return;
    }

    int slot_trovato = -1;
    for (int i = 0; i < 3; i++)
    {
        if (giocatore->zaino[i] == nessun_oggetto || giocatore->zaino[i] == oggetto_vuoto)
        {
            slot_trovato = i;
            break;
        }
    }

    if (slot_trovato != -1)
    {
        giocatore->zaino[slot_trovato] = giocatore->pos_mondoreale->oggetto;
        printf(GREEN "Hai raccolto: %s! (Inserito nella Tasca %d)\n" RESET,
               tipo_oggetto_str[giocatore->zaino[slot_trovato]],
               slot_trovato + 1);
        giocatore->pos_mondoreale->oggetto = nessun_oggetto;
    }
    else
    {
        printf(RED "Zaino pieno! Devi usare o scartare un oggetto prima di raccoglierne altri.\n" RESET);
    }
}

// Uso effettivo dell'oggetto dallo zaino
static void usa_oggetto(struct giocatore *giocatore)
{
    int num_oggetti = 0;

    printf(BLUE "\n<--- USA OGGETTO --->\n" RESET);
    printf("Quale oggetto vuoi usare?\n");

    for (int i = 0; i < 3; i++)
    {
        printf("Tasca %d: %s\n", i + 1, tipo_oggetto_str[giocatore->zaino[i]]);

        if (giocatore->zaino[i] != nessun_oggetto && giocatore->zaino[i] != oggetto_vuoto)
        {
            num_oggetti++;
        }
    }

    if (num_oggetti == 0)
    {
        printf(RED "Hai lo zaino vuoto...\n" RESET);
        return;
    }

    printf(YELLOW "\nScegli un oggetto (1-3) oppure 0 per uscire:\n" RESET);
    freccia();

    int scelta = leggi_input(0, 3);

    if (scelta == 0)
    {
        printf(YELLOW "Hai chiuso lo zaino.\n" RESET);
        return;
    }
    else
    {
        int indice = scelta - 1;

        if (giocatore->zaino[indice] == nessun_oggetto || giocatore->zaino[indice] == oggetto_vuoto)
        {
            printf(RED "Nessun oggetto in questa tasca.\n" RESET);
            return;
        }

        enum tipo_oggetto oggetto_usato = giocatore->zaino[indice];

        switch (oggetto_usato)
        {
        case schitarrata_metallica:
            printf(GREEN "WOW! Il tuo attacco è aumentato drasticamente!! (+4)\n" RESET);
            giocatore->attacco_psichico += 4;
            if (giocatore->attacco_psichico > 20)
            {
                giocatore->attacco_psichico = 20;
            }
            break;

        case bicicletta:
            printf(GREEN "WOW! Ora scappi più velocemente! Difesa +5.\n" RESET);
            giocatore->difesa_psichica += 5;
            if (giocatore->difesa_psichica > 20)
            {
                giocatore->difesa_psichica = 20;
            }
            break;

        case maglietta_fuocoinferno:
            printf(GREEN "WOW! Attacco e difesa aumentati di 2!\n" RESET);
            giocatore->difesa_psichica += 2;
            giocatore->attacco_psichico += 2;

            if (giocatore->attacco_psichico > 20)
            {
                giocatore->attacco_psichico = 20;
            }
            if (giocatore->difesa_psichica > 20)
            {
                giocatore->difesa_psichica = 20;
            }
            break;

        case bussola:
            printf(GREEN "WOW! Ora sai dove nasconderti! Fortuna +3.\n" RESET);
            giocatore->fortuna += 3;
            if (giocatore->fortuna > 20)
            {
                giocatore->fortuna = 20;
            }
            break;

        case nessun_oggetto:
        case oggetto_vuoto:
            break;
        }

        giocatore->zaino[indice] = nessun_oggetto;
    }
}

// Stampa tutti i dati utili del giocatore
static void stampa_giocatore(struct giocatore *giocatore)
{
    printf(YELLOW "\n<--- STATISTICHE: %s --->\n" RESET, giocatore->nome);

    printf("Attacco: %d | Difesa: %d | Fortuna: %d\n",
           giocatore->attacco_psichico,
           giocatore->difesa_psichica,
           giocatore->fortuna);

    printf(BLUE "\n[ZAINO]\n" RESET);
    for (int i = 0; i < 3; i++)
    {
        printf("  %d) %s\n", i + 1, tipo_oggetto_str[giocatore->zaino[i]]);
    }

    printf(GREEN "----------------------------------\n" RESET);
}

// Funzione per rimuovere il giocatore una volta deceduto
static void rimuovi_giocatore_morto(int indice)
{
    printf(RED "\n%s è caduto in battaglia. La sua memoria svanisce nel nulla...\n" RESET, lista_giocatori[indice]->nome);

    free(lista_giocatori[indice]);

    for (int i = indice; i < n_giocatori - 1; i++)
    {
        lista_giocatori[i] = lista_giocatori[i + 1];
    }

    n_giocatori--;
    lista_giocatori[n_giocatori] = NULL;
}

// Stampa il creatore e i vincitori passati
void crediti()
{
    ripulisci_schermo();
    stampa_titolo();

    printf(YELLOW "\n<--- CREDITI --->\n" RESET);
    printf("Creato da: Stefano Gammelli\n");

    printf(YELLOW "\n<--- ALBO D'ORO --->\n" RESET);
    printf("1° Classificato: %s\n", vincitore1);
    printf("2° Classificato: %s\n", vincitore2);
    printf("3° Classificato: %s\n", vincitore3);

    printf(YELLOW "\nPremi 1 per uscire.\n" RESET);
    leggi_input(1, 1);
}

// Funzione che gestisce i turni e il menu del gioco attivo
void gioca()
{
    if (mappa_reale == NULL || n_giocatori == 0)
    {
        printf(RED "Errore: Configurazione mancante, mappa o giocatori non impostati.\n" RESET);
        return;
    }

    // Posiziono i giocatori se necessario
    for (int i = 0; i < 4; i++)
    {
        if (lista_giocatori[i] != NULL && lista_giocatori[i]->pos_mondoreale == NULL)
        {
            lista_giocatori[i]->pos_mondoreale = mappa_reale;
            lista_giocatori[i]->pos_soprasotto = mappa_sopra;
            lista_giocatori[i]->mondo = 0;
        }
    }

    int gioco_in_corso = 1;

    while (gioco_in_corso && n_giocatori > 0)
    {
        int ordine_turno[4];
        int contatore_vivi = 0;

        for (int i = 0; i < 4; i++)
        {
            if (lista_giocatori[i] != NULL)
            {
                ordine_turno[contatore_vivi] = i;
                contatore_vivi++;
            }
        }

        // Shuffle ordine
        for (int i = 0; i < contatore_vivi; i++)
        {
            int j = rand() % contatore_vivi;
            int temp = ordine_turno[i];
            ordine_turno[i] = ordine_turno[j];
            ordine_turno[j] = temp;
        }

        printf(YELLOW "\n<--- INIZIO NUOVO ROUND (Ordine Casuale Generato) --->\n" RESET);
        printf("Premi 1 per iniziare il round.\n");
        leggi_input(1, 1);

        for (int k = 0; k < contatore_vivi; k++)
        {
            int indice_corrente = ordine_turno[k];
            struct giocatore *g_corrente = lista_giocatori[indice_corrente];

            if (g_corrente == NULL)
                continue;

            int turno_giocatore_attivo = 1;
            int ha_mosso = 0;
            int esito_combattimento = 0;

            // Variabile per ricordare se ho battuto il nemico in QUESTO turno
            int nemico_battuto = 0;

            while (turno_giocatore_attivo)
            {
                ripulisci_schermo();
                stampa_titolo();

                printf(BLUE "\n>>> È IL TURNO DI: %s <<<\n" RESET, g_corrente->nome);
                stampa_giocatore(g_corrente);

                if (ha_mosso)
                    printf(RED "[MOVIMENTO ESAURITO]\n" RESET);
                else
                    printf(GREEN "[PUOI MUOVERTI]\n" RESET);

                if (g_corrente->mondo == 0)
                {
                    printf(YELLOW "\n[POSIZIONE: MONDO REALE]\n" RESET);
                    printf("Zona: %s | Nemico: %s | Oggetto: %s\n",
                           tipo_zona_str[g_corrente->pos_mondoreale->zona],
                           tipo_nemico_str[g_corrente->pos_mondoreale->nemico],
                           tipo_oggetto_str[g_corrente->pos_mondoreale->oggetto]);
                }
                else
                {
                    printf(RED "\n[POSIZIONE: SOPRASOTTO]\n" RESET);
                    printf("Zona: %s | Nemico: %s\n",
                           tipo_zona_str[g_corrente->pos_soprasotto->zona],
                           tipo_nemico_str[g_corrente->pos_soprasotto->nemico]);
                }

                printf(BLUE "\nAZIONI:\n" RESET);
                printf("1) Avanza\n2) Indietreggia\n3) Cambia mondo\n4) Combatti\n5) Usa Oggetto\n6) Raccogli Oggetto\n7) Passa turno\n8) Esci dal gioco\n");
                freccia();

                int scelta = leggi_input(1, 8);

                switch (scelta)
                {
                case 1:
                    if (ha_mosso)
                        printf(RED "Hai già mosso in questo turno!\n" RESET);
                    // Passo nemico_battuto
                    else if (avanza(g_corrente, nemico_battuto))
                    {
                        ha_mosso = 1;
                        nemico_battuto = 0; // Nuova zona, resetto flag
                    }
                    break;

                case 2:
                    if (ha_mosso)
                        printf(RED "Hai già mosso in questo turno!\n" RESET);
                    // Passo nemico_battuto
                    else if (indietreggia(g_corrente, nemico_battuto))
                    {
                        ha_mosso = 1;
                        nemico_battuto = 0; // Nuova zona, resetto flag
                    }
                    break;

                case 3:
                    if (ha_mosso && g_corrente->mondo == 0)
                    {
                        printf(RED "Hai già mosso in questo turno!\n" RESET);
                    }
                    else
                    {
                        if (cambia_mondo(g_corrente))
                        {
                            if (g_corrente->mondo == 1)
                                ha_mosso = 1;
                            nemico_battuto = 0; // Nuovo mondo, resetto flag
                        }
                    }
                    break;

                case 4:
                    esito_combattimento = combatti(g_corrente);

                    if (esito_combattimento == 1) // Vittoria Suprema
                    {
                        strcpy(vincitore3, vincitore2);
                        strcpy(vincitore2, vincitore1);
                        strcpy(vincitore1, g_corrente->nome);
                        ripulisci_schermo();
                        stampa_titolo();
                        printf(GREEN "\n\n!!! VITTORIA SUPREMA !!!\n" RESET);
                        printf(YELLOW "Il giocatore %s ha sconfitto il Demotorzone!\n" RESET, g_corrente->nome);
                        printf("Premi 1 per terminare la partita.\n");
                        leggi_input(1, 1);
                        return;
                    }
                    else if (esito_combattimento == 2) // Vittoria ma nemico resta
                    {
                        nemico_battuto = 1; // Attiviamo il lasciapassare!
                    }
                    else if (esito_combattimento == -1) // Morte
                    {
                        turno_giocatore_attivo = 0;
                    }
                    break;

                case 5:
                    usa_oggetto(g_corrente);
                    break;

                case 6:
                    // Passo nemico_battuto
                    raccogli_oggetto(g_corrente, nemico_battuto);
                    break;

                case 7:
                    printf(YELLOW "Passi il turno.\n" RESET);
                    turno_giocatore_attivo = 0;
                    break;

                case 8:
                    printf(RED "Uscita in corso...\n" RESET);
                    return;
                }

                if (turno_giocatore_attivo)
                {
                    printf(YELLOW "\nPremi 1 per continuare...\n" RESET);
                    leggi_input(1, 1);
                }
            }

            if (esito_combattimento == -1)
            {
                rimuovi_giocatore_morto(indice_corrente);
                if (n_giocatori == 0)
                {
                    printf(RED "\nGAME OVER. Tutti i giocatori sono caduti.\n" RESET);
                    printf("Premi 1 per tornare al menu principale.\n");
                    leggi_input(1, 1);
                    return;
                }
                printf(YELLOW "Premi 1 per far giocare il prossimo sopravvissuto.\n" RESET);
                leggi_input(1, 1);
            }
        }
    }
}