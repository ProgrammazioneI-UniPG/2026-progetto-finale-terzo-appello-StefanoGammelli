// Dichiarazione delle funzioni da chiamare in main.c
// Definizione dei tipi necessari alla libreria

#define UNDERLINE "\033[4m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"

int leggi_input(int min, int max);

int imposta_gioco();

void crediti();

void ripulisci_schermo();

void stampa_titolo();

void freccia();

void gioca();

enum tipo_oggetto
{
    nessun_oggetto,
    bicicletta,
    maglietta_fuocoinferno,
    bussola,
    schitarrata_metallica,
    oggetto_vuoto
};

enum tipo_zona
{
    bosco,
    scuola,
    laboratorio,
    caverna,
    strada,
    giardino,
    supermercato,
    centrale_elettrica,
    deposito_abbandonato,
    stazione_polizia,
    zona_vuota
};

enum tipo_nemico
{
    nessun_nemico,
    billi,
    democane,
    demotorzone,
    nemico_vuoto
};

struct zona_mondoreale
{
    enum tipo_zona zona;
    enum tipo_nemico nemico;
    enum tipo_oggetto oggetto;

    // puntatore alla prossima e precedente zona
    struct zona_mondoreale *avanti;
    struct zona_mondoreale *indietro;

    // puntatore alla relativa adiacente zona nel sottosopra
    struct zona_soprasotto *link_soprasotto;
};

struct zona_soprasotto
{
    enum tipo_zona zona;
    enum tipo_nemico nemico;

    // puntatore alla prossima e precedente zona
    struct zona_soprasotto *avanti;
    struct zona_soprasotto *indietro;

    // puntatore alla relativa adiacente zona nel sottosopra
    struct zona_mondoreale *link_mondoreale;
};

struct giocatore
{
    char nome[15];

    // 0 o 1 in base al soprasotto o mondo
    int mondo;

    // valora da 1 a 20
    int attacco_psichico;
    int difesa_psichica;
    int fortuna;

    // contenente due oggetti di tipo enum
    enum tipo_oggetto zaino[3];

    // PUNTATORI
    //  Tiene conto della posizione nel soprasotto, quindi quando int mondo = 1
    struct zona_soprasotto *pos_soprasotto;

    // tiene conto della posizione nel mondo reale, quindi quando int mondo = 0
    struct zona_mondoreale *pos_mondoreale;
};
