#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
<---FONTI PER PROBLEMI VARI--->
https://stackoverflow.com/questions/19641597/what-is-segmentation-fault-core-dumped
 https://stackoverflow.com/questions/72239283/have-to-hit-enter-twice-with-fgets-in-c
 https://stackoverflow.com/questions/42894522/trying-to-understand-fgets
 https://stackoverflow.com/questions/2963394/finding-character-in-string-c-language
 https://stackoverflow.com/questions/3219393/stdlib-and-colored-output-in-c
 https://stackoverflow.com/questions/66913390/what-does-statement-with-no-effect-mean-in-c
https://patorjk.com/software/taag/#p=display&f=Graffiti&t=Type+Something+&x=none&v=4&h=4&w=80&we=false
https://askubuntu.com/questions/115369/how-to-play-mp3-files-from-the-command-line


 <--FONTI PER MODALITA COMBATTIMENTO-->
cari amici giocatori di D&D e immaginazione

    */

char input[10];
int gioco_impostato = 0;
int idoneita = 1;

int main()
{
    time_t t;
    srand((unsigned)time(&t));

    // inizializza la musica
    system("mpg123 -q --loop -1 intro.mp3 > /dev/null 2>&1 &");

    // Ridimensiona terminale (opzionale)
    printf("\x1b[8;%d;%dt", 25, 110);

    ripulisci_schermo();
    stampa_titolo();

    // menu iniziale
    printf("\n  1) Imposta gioco\n");
    printf("  2) Gioca\n");
    printf("  3) Termina gioco\n");
    printf("  4) Visualizza crediti\n\n");

    while (idoneita)
    {
        freccia();
        int input = leggi_input(1, 4);

        switch (input)
        {
        case 1:
            gioco_impostato = imposta_gioco();
            // Ristampa menu pulito al ritorno
            ripulisci_schermo();
            stampa_titolo();
            printf("\n  1) Imposta gioco\n");
            printf("  2) Gioca\n");
            printf("  3) Termina gioco\n");
            printf("  4) Visualizza crediti\n\n");
            break;

        case 2:
            if (gioco_impostato == 0)
            {
                printf(RED "\n[ERRORE] Prima di giocare devi impostare il gioco.\n" RESET);
                printf(YELLOW "Premi 1 per continuare...\n" RESET);
                leggi_input(1, 1);

                
                ripulisci_schermo();
                stampa_titolo();
                printf("\n  1) Imposta gioco\n");
                printf("  2) Gioca\n");
                printf("  3) Termina gioco\n");
                printf("  4) Visualizza crediti\n\n");
            }
            else
            {
                gioca();
                // Al ritorno da gioca(), ristampa il menu
                ripulisci_schermo();
                stampa_titolo();
                printf("\n  1) Imposta gioco\n");
                printf("  2) Gioca\n");
                printf("  3) Termina gioco\n");
                printf("  4) Visualizza crediti\n\n");
            }
            break;

        case 3:
            // Stop musica
            system("pkill mpg123");
            printf(RED "Uscita dal gioco...\n" RESET);
            idoneita = 0; // Esce dal while
            break;

        case 4:
            crediti();

            ripulisci_schermo();
            stampa_titolo();
            printf("\n  1) Imposta gioco\n");
            printf("  2) Gioca\n");
            printf("  3) Termina gioco\n");
            printf("  4) Visualizza crediti\n\n");
            break;

        default:
            printf(RED "Input non valido.\n" RESET);
            break;
        }
    }

    system("pkill mpg123");
    return 0;
}