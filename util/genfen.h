#include <stdlib.h>

#ifdef WIN32
#define SLAF '\\'
#else
#define SLAF '/'
# endif


#define LIBCHAR 0
#define LIBNUM 1
#define LIBINT 2
#define SAISCHAR 10
#define DEFMAN 18
#define SSPEC 19  // saisie speciale reprise par fonc utilisateur
#define SAISNUM 20
#define SAISNUP 21	// idem SAISNUM mais saisie et affich en %
#define SAISMON 24
#define SAISINT 25
#define SAISDAT 26
#define DIMC	7	// dimension 1 car a afficher
#define DIMCH	23	// dimension hauteur vergisc vergifac 25

//******************fenetre quelconque
typedef struct  {	//def zones saisie
int cox;
int lyg;
int idcol;		// ident colonne (numéro)
int deps;		 // deplacement dans structure
GtkWidget *wdg;		// entry
void *memcell;		// adresse absolue de memorisation de la variable
int typ;		// type l (libelle pas de saisie) s char n numerique
int laf;		// longueur zone en caractere
int (*fonc)();		// fonction utilisateur controle action
char *forma;		// format affichage
} DEF_S_FQ;

typedef struct  {	//def libelle
int cox;
int lyg;
GtkWidget *wdgl;	//  libel
int typ;		// normal ou separateur 0,1
char *libel;		// libelle
} DEF_L_FQ;

typedef struct  {	//fen quelconque
GtkWidget *win;
char *fentit;        //  pointeur sur titre  fenetre
int fenlarg;	// largeur fenetre
int fenhaut;	// hauteur  fenetre
int posx;	//  coin gauche x
int posy;	//  coin gauche y
//int sizsvt;	// sizeof struct des variables
DEF_S_FQ *pdefz;	// pointeur sur definition zones saisie
DEF_L_FQ *pdeflib;	// pointeur sur definition libelle
void (*fbouton)();	// fonction ajout de bouton
} FENQ;

//******************fenetre tableau
typedef struct  {	//def colonne  tableau a n col
GtkWidget *wlibel;	// widget libelle initialise avec libel sur 1er ligne
int idcol;		// ident colonne (numéro)
int deps;		 // deplacement dans structure
int typ;		// type l (libelle pas de saisie) s char n numerique
int laf;		// longueur zone
int (*fonc)();		// fonction utilisateur controle action
char *forma;		// format affichage
char *libel;		// libelle de la colonne dans 1er ligne ou guide de frappe
} DEF_L_FnC;   	// remplace SZTAB;

typedef struct  {	// memorise widget tableau n col
GtkWidget *wdg;		// entry ou libel
void *memcell;		// adresse absolue de memorisation de la variable
DEF_L_FnC *pdefc;	// pointeur sur colonne active
int liga;		// ligne active
} MEMO_T_WIDG;

typedef struct  {	//fen tableau a n col
GtkWidget *win;
//GtkWidget *vboxwin;
//GtkWidget *tabwin;
char *fentit;        //  pointeur sur titre  fenetre
int fenlarg;	// largeur fenetre
int fenhaut;	// hauteur  fenetre
int posx;	//  coin gauche x
int posy;	//  coin gauche y
int tli;	//  nb lignes
int tco;	//  nb col
int sizsvt;	// sizeof struct des variables
DEF_L_FnC *pdefct;	// pointeur sur definition colonnes
MEMO_T_WIDG *ptwidg; // pointeur sur tableau widget
void (*fbouton)();	// fonction ajout de bouton
} FnC;		// remplace TABLO

//**************fenetre combobox
typedef struct  {
GtkWidget *wdg;	// entry ou libel
GtkWidget *wlibel;	// widget libelle initialise avec libel
//int typ;		// type l (libelle pas de saisie) s char n numerique
int laf;		// longueur zone
//char *forma;		// format affichage
char *libel;
} DEF_L_FCBOX;

typedef struct  {	//fen combobox
GtkWidget *win;
GtkWidget *cbox;	// combobox
GtkWidget *libretour;	//wdg libel retour
char *fentit;        //  pointeur sur titre  fenetre
char *titadd;	// titre dialogue add elem
int fenlarg;	// largeur fenetre
int fenhaut;	// hauteur  fenetre
int posx;	//  coin gauche x
int posy;	//  coin gauche y
int nbdon;	// nb de zones a saisir
int modif;	// temoin modif
void (*ftsais)();	// fonction traite saisie
void (*fquitfen)();	// fonction quite fenetre
DEF_L_FCBOX *ptle;	// pointeur sur tableau saisie
} FCBOX;


//************************
//definition d'une ligne de fenetre tableau 2 col avec saisie possible
// une ligne = un libelle constant associe a une variable calculee et/ou saisie
typedef struct  {
GtkWidget *winit;	// entry ou libel
GtkWidget *wlibel;	// widget libelle initialise avec libel
void *memcell;		// adresse absolue de memorisation de la variable
int deps; 		// deplacement dans structure
int typ;		// type l (libelle pas de saisie) s char n numerique
int laf;		// longueur zone
int (*fonc)();		// fonction utilisateur controle action
char *forma;		// format affichage
char *libel;
} DEF_L_F2C;	// remplace TABINI

typedef struct  {	//fen 2 col libel + val
GtkWidget *win;
//GtkWidget *vboxwin;
//GtkWidget *tabwin;
char *fentit;        //  pointeur sur titre  fenetre
int fenlarg;	// largeur fenetre
int fenhaut;	// hauteur  fenetre
int posx;	//  coin gauche x
int posy;	//  coin gauche y
int tli;	//  nb lignes
//int tco;	//  nb col tjs 2
DEF_L_F2C *ptabini;	//  pointeur sur tableau Libelle,etc
void (*fbouton)();	// fonction ajout de bouton
} F2C;		// remplace FENEV

//********** fen graphe
typedef struct  {
GtkWidget *wingrap;
GtkWidget *da;  // drawing_area
//GdkGC *kontext;  //  context graphique
//GdkColormap *colormap; //  colormap
GdkPixmap *pixmap;     //  pixmap
//GdkColor *coulac;	couleur active
char *fentit;     //    pointeur sur titre  fenetre
int fenlarg;	//largeur fenetre
int fenhaut;	//hauteur  fenetre
int posx;	// coin gauche x
int posy;	// coin gauche y
} FENGRA;
//**********************************
typedef struct  {
int num;
char* messag;
} S_MES;

typedef struct  {
char sdat[12];
int datq;
int j;
int m;
int a;
} S_DAT;

//void message(int num); duplic avec compta
gboolean message (int num);
//void affich (FENEV *pf,void *pd);
void ltof(long nl,char s[],int ll);           //  formate entier long
void dqdat(int q);
void inidatej();
int ctcar(char c, char *pt);
int contdate(char* sdat);
int qdat();
int mdatof(char* pt);
int chang_ctrl(GtkEntry *widls, int typ );
void finchait(char z[],int ll);
void crefenq(FENQ* pf,void* pd);
void cretablo(FnC* pf,void* pd);
void cre_fen2c_sais (F2C* pf,void* pd);
int mdctoi(char* pt);
void cre_fcbox(FCBOX* pf);
void aftablo (FnC *pf, void *pd, int liga);
void combo_box_clear (GtkComboBox *p_combo_box);
void testgch( gchar *p, int l, int ori);
void choicoul(FENGRA* pfg,gint ccou);
void crefengra(FENGRA* pf);
void aff_fen2c_sais(F2C* pf,void* pd);
void act_l_f2c (DEF_L_F2C *pl);
void maj_l_f2c (GtkEntry *widls, DEF_L_F2C* pl);
gboolean messavar(char* vmes);
int question(int num);
long arron2(double ff, long ll);
void creparfenq(DEF_L_FQ *pli,DEF_S_FQ *pz,GtkWidget* fixg,void* pd);
void formfloat(char *ss,double ff);
void charreplace(char *ss,char ci, char cf);



