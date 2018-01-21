#include <gtk/gtk.h>

#ifdef WIN32
#define SLAF '\\'
#else
#define SLAF '/'
# endif

// saisie fiche client
#define DIMNOTES 256
#define DIMNOCLI 512
typedef struct  {
char clecli[12];
char typcli[2];
char add1[50];
char add2[50];
char add3[50];
char pays[6];
char codpos[8];
char add4[32];
char datcre[12];
char datmod[12];
char teleph[18];
char mail[40];
char notes[DIMNOCLI];
char cms;
int cleon;
union {
char erreur[8];
long long erd;
} ue;
} S_FCLI;

typedef struct  {	//fiche produit
char cleprod[12];
char libprod[DIMNOTES];
long priunit;
char codtax[2];
char unit[4];
char datcre[12];
char datmod[12];
char notes[DIMNOTES];
char cms;
char spriunit[16];
int cleon;
union {
char erreur[8];
long long erd;
} ue;
} S_PROD;

typedef struct  {	//def colonne list
int nucol;
int typ;
int laf;
char align;
char *titcol;		// libelle
} DEF_C_LIST;

typedef struct  {	//ligne factures
char codprod[12];
char libprod[50];
double quant;   //  avec float imprécision à l'edition
char unit[6];
char codtax[2];
long long int priunit;
long long int valor; // 8 bytes 32 et 64 bits
char spriunit[16];	//(LONG_MAX).   21 474 836.47 +\0 =14 car
char svalor[16];
short typlig;
} S_LFAC;
//typlig
#define LQQ 0
#define DETAIL 1
#define DETLIB 2
#define SPAG 3
#define GENCAL 10
#define CALTOT 11
#define LIBCAL 12
#define TAX 13
#define TPP 14
#define TIX 15

typedef struct  {	//en tete
char clecli[12];
char datfac[13];
char numfac[16];
char add1[50];
char add2[50];
char add3[50];
char pays[6];
char codpos[8];
char add4[32];
char doclu[20];
char ficdi[20];	// ne sert pas
char nblig[8];
int datfacq;
} S_SENF;

typedef struct  {	// codes factures
char code[8];
char suit;
char lib[50];
int nuli;
} S_CODFAC;

typedef struct {
char stau[14];
long mtau;
} S_TAUX;

typedef struct {
char idparam[4];
int nuli;
char don1[128];
} S_DBPARAM;

extern long long int zslon;
void raz_cms ();
//void raz_smc();
void clear_sfprod();
int execdb( char * sql,char tt);
int lit1c_fcli(char *klecli);
int lit1c_fprod(char *kleprod);
void deactiv_sfcli();
void deactiv_sfprod();
void crefen_listcli();
void crefen_listprod();
void acces_sfcli();
void acces_sfprod();
gboolean sfac_quit (GtkWidget *widget, gpointer data);
void init_sfac();
int wheretabcode(char *code);
void scod_lidb();
void initcbox_taux(GtkWidget* cob);
void sauvdb_taux();
void lidb_taux();
long valtau(char c);
void rechcli( char* nup);
void rechprod( char* nup);
void sfac_imprim();
void init_para();
void affi_para();
void choix_fimipa();
void affi_ficcde();
void affi_doclu();
void afadres_cli(char* klecli);
void actilistprod_sfac(char* kleprod);
gboolean sauvdoc(GtkWidget *widget, gpointer data);
void relitdoc();
//void nbligfac();
void crenewdb();
int dialogtext(char* tit,char* mes,char* initex, gchar* ss);




