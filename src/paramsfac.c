/*
 *  Copyright (c) 2015,2016 Michel Delorme
 *
 * This file is part of vergifac
 *
 * vergifac is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * vergifac is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with vergifac.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
//#include <stdio.h>
#include <ctype.h>
#include <string.h>
//#include <gtk/gtk.h>
#include "fast.h"
#include "genfen.h"

//les fonctions  gtk_combo_box_text_.. etc sont nouvelles a partir de gtk 2.4
// inconnues des versions precedentes
// j'ai remis les anciennes (deprecied) gtk_combo_box_.. etc sans text au milieu

void 	fs_taux (FCBOX *pf);
gboolean qf_taux (GtkWidget *w, FCBOX *pf);
void open_ficdiv();
void boutpara(GtkWidget *widx, gpointer data);
void bchoix_para(GtkWidget *widget, gpointer data);

//extern double zsdoubl;
extern GtkWidget *winp;
extern long zslon;
extern int bloqchfq; // inhibe signal change si affichage
extern char ficcdimp[];
extern int CSFnli;

char zz [256];
S_DBPARAM rdbp;

//***************************TAUX
#define TAUXLI 20
#define TAUXCO 2
S_TAUX taux[TAUXLI];

DEF_L_FCBOX zs_taux[TAUXCO] = {
//wdg,wlibel,laf,libel
{NULL,NULL,1,"Code (1 lettre)"},
{NULL,NULL,11,"Taux en % (2 décimales au maximum)"}
//{NULL,NULL,11,"Taux en % marque décimale = '.'"}
};

extern  void fs_taux(FCBOX* pf);
extern  gboolean qf_taux(GtkWidget* w,FCBOX* pf);

FCBOX fen_taux = {
NULL,	//GtkWidget *win;
NULL,	// combobox
NULL,	//wdg libel retour
//GtkWidget *tabwin;
"Taux T.V.A",        /* pointeur sur titre  fenetre */
"Ajout/sup code taux",	// titre dialogue add elem
320,	/*largeur fenetre*/
120,	/*hauteur  fenetre*/
200,	/* coin gauche x*/
200,	/* coin gauche y*/
TAUXCO,	// nb de zones a saisir
0,	// temoin modif
&fs_taux,	// fonction traite saisie
(void*)&qf_taux,	// fonction quite fenetre
&zs_taux[0],	// pointeur sur tableau saisie
};

long valtau(char c)       //   verif et retour valeur taux
{
short i;
 for ( i=0; i < TAUXLI; ++i)  {
   if (c == taux[i].stau[0])  return( taux[i].mtau);
   if ( taux[i].mtau == -1) break;
                                  }
                 return(-1);
}

void initcbox_taux(GtkWidget* cob)
{
int j;
//pf->cbox = cob creation avec la fenetre fen_taux
    // Ajout des elements  depuis table taux
for (j = 0; j < TAUXLI; ++j)	{
 if ( taux[j].mtau == -1) break;
// gtk_combo_box_text_append_text((GtkComboBoxText*) cob,taux[j].stau);
 gtk_combo_box_append_text((GtkComboBox*) cob,taux[j].stau);
				}
gtk_combo_box_set_active(GTK_COMBO_BOX(cob), 0);
}

gboolean qf_taux(GtkWidget* w,FCBOX* pf)
{
int i=0,k=0;
const gchar *text;
if (pf->modif == 0) return FALSE;
while ( k >= 0) 	{
 gtk_combo_box_set_active(GTK_COMBO_BOX(pf->cbox),i);
 k = gtk_combo_box_get_active(GTK_COMBO_BOX(pf->cbox));
 if ( k < 0) break;
 text = gtk_combo_box_get_active_text((GtkComboBox*)pf->cbox);
	// sauve saisie ici dans table taux
 strcpy(taux[i].stau,text);
 mdctoi((char*)text+2);
 taux[i].mtau = zslon;
 ++i;
 if ( i >= TAUXLI) 	{
   printf("limite TAUXLI atteinte\n");
   return FALSE;
  			}
			}
	// fin de table taux
for ( ; i < TAUXLI; ++i) taux[i].mtau = -1;
pf->modif = 0;	// sinon sauvdb 2 fois
gtk_widget_destroy(pf->win);
pf->win = NULL;
//  sauve taux dans db
sauvdb_taux();
return FALSE;
}

void fs_taux(FCBOX* pf)
{
int ret =0,i,lt;
DEF_L_FCBOX *pti = pf->ptle;
const gchar *tcod,*tdon;//,*ttrav;
char ttrav[50];
char *zvnv="1234567890.";
 tcod = gtk_entry_get_text(GTK_ENTRY((pti)->wdg));
 tdon = gtk_entry_get_text(GTK_ENTRY((pti+1)->wdg));
	// controle
 lt = strlen(tdon);
 i = strspn((char*)tdon,zvnv);
 if ( i < lt || mdctoi((char*)tdon) == -1)   ret = -1;
ttrav[0]= *tcod;
if ( !isalpha(ttrav[0]) ) ret = -1;
ttrav[1]= ' ';
strcpy((char*)ttrav+2,(char*)tdon);
//printf("ttrav %s %p ll=%d\n",ttrav,pf->cbox,strlen(ttrav));
if(g_ascii_strcasecmp(ttrav, " ") == 0) ret = -1;
//if( strlen(ttrav) == 0) ret = -1;
if ( ret == 0)	{
 i = gtk_combo_box_get_active(GTK_COMBO_BOX(pf->cbox));
 if(i != -1)
 gtk_combo_box_insert_text((GtkComboBox*) pf->cbox, i,(gchar*) ttrav);
 else {
 gtk_combo_box_prepend_text((GtkComboBox*) pf->cbox,(gchar*) ttrav);
 i=0;
	}
 gtk_label_set_text(GTK_LABEL(pf->libretour), "Saisie O.K");
 gtk_combo_box_set_active(GTK_COMBO_BOX(pf->cbox),i);
 pf->modif = 1;
		}
else 	{
 gtk_label_set_text(GTK_LABEL(pf->libretour), "Erreur Saisie");
 gtk_combo_box_set_active(GTK_COMBO_BOX(pf->cbox),-1);
	}
}

char sqlins_taux[]="INSERT INTO Ttaux VALUES('TAUX',NULL,'%s')";
char sqldel_taux[]="DELETE  FROM Ttaux";
//char sqldel_taux[]="DELETE  FROM Ttaux WHERE Idparam='TAUX'"; risque de locked database ?

void sauvdb_taux()
{
int i;
execdb(sqldel_taux,'u'); // efface
for ( i=0; i < TAUXLI; ++i) 	{
 if ( taux[i].mtau == -1) break;
 sprintf(zz,sqlins_taux, taux[i].stau); // sqlite incremente nuli a partir de 1
 execdb(zz,'u');
				}
}


char sqlli_taux[]="SELECT * FROM Ttaux WHERE Idparam='TAUX' AND Nuli=%d";

void lidb_taux()
{
int i;
for ( i=0; i < TAUXLI; ++i) 	{
 rdbp.nuli=-1;
 sprintf(zz,sqlli_taux, i+1);
 execdb(zz,'p');
 if ( rdbp.nuli == -1)  break;	// fin
 strcpy(taux[i].stau,rdbp.don1);
 mdctoi(rdbp.don1+2);
// printf(" zslon= %ld\n",zslon);
 taux[i].mtau = zslon;
/* mdatof(rdbp.don1+2);
 zsdoubl *=10000;	// erreur avec compilateur windows
 printf(" zsdoubl= %lf\n",zsdoubl); // corrige erreur windows ??
 taux[i].mtau = zsdoubl;	*/
				}
	// fin de table taux
for ( ; i < TAUXLI; ++i) taux[i].mtau = -1;
}

void choix_fimipa(GtkWidget *widget, gpointer data)
{
ficcdimp[0] = '\0';
open_ficdiv();
if (ficcdimp[0] != '\0')  affi_ficcde();
}

void open_ficdiv()
{
GtkWidget *dialog,*diames;
char *filename;
FILE *fiss = NULL;
GtkFileFilter *filter = gtk_file_filter_new ();
gtk_file_filter_add_pattern (filter, "*.txt");
dialog = gtk_file_chooser_dialog_new("Fichier a lire",
   	GTK_WINDOW(NULL),
	GTK_FILE_CHOOSER_ACTION_OPEN,
	GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
	NULL);
gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog),"./misenpage");
gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);
if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)   {
 filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
 fiss=fopen(filename,"r");
 if   ( fiss == NULL )	 {
  printf("Erreur d'ouverture du fichier %s ",filename);
  diames = gtk_message_dialog_new(GTK_WINDOW(NULL),
  GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
  "Impossible d'ouvrir le fichier : \n%s",filename);
  gtk_dialog_run(GTK_DIALOG(diames));
  gtk_widget_destroy(diames);
			  }
 else	{
  strcpy(ficcdimp,"./misenpage/");
char *pnf = NULL;
//  pnf = strrchr (filename, '/');
  pnf = strrchr (filename, SLAF);
  if ( pnf != NULL)  strcat(ficcdimp,pnf+1);
  fclose(fiss); fiss = NULL;
	}
g_free (filename);
						  }
gtk_widget_destroy (dialog);
//return *fiss;
}

//*****************param

typedef struct {
char fimipa[80];
int lifa;
} S_PARA;
S_PARA para;

char sqlsv_para[]="UPDATE Param SET Don1=\"%s\" WHERE Idparam='%s'";

void sauvdb_param(char* idparam, char* don1)
{
 sprintf(zz,sqlsv_para, don1, idparam);
 execdb(zz,'u');
}

char sqlli_para[]="SELECT * FROM Param WHERE Idparam='%s'";

char* lidb_param(char* idparam)
{
 rdbp.nuli=-1;
 sprintf(zz,sqlli_para, idparam);
 execdb(zz,'p');
 if ( rdbp.nuli == -1) printf("param %s inconnu db\n",idparam);
 return ((char*)rdbp.don1);
}

void init_para()
{
strcpy((char*)para.fimipa,(char*) lidb_param("MIPA"));
strcpy(ficcdimp,para.fimipa);
para.lifa=atoi((char*) lidb_param("LIFA"));
CSFnli = para.lifa;
}

enum {
MIPA,
LIFA,
XXX
};


#define DEPS(zz) (char*)&para.zz-(char*)&para
static DEF_S_FQ s_fpara[]= {
{190,15,MIPA,0,NULL,NULL,LIBCHAR,32,NULL,"%s"},
{355,45,LIFA,DEPS(lifa),NULL,NULL,SAISINT,5,NULL,"%d"},
{-1,-1,0,0,NULL,0,0,0,NULL,NULL}
};
#undef DEPS

static DEF_L_FQ lib_fpara[]= {
{6,10,NULL,0,"Fichier de mise en page"},
{10,25,NULL,0,"impression par defaut"},
{6,47,NULL,0,"Nombre maximal de ligne de facture (de 80 à 999)"},
{6,62,NULL,0,"Fermer 'Saisie Facture' (Sauv.doc si besoin)"},
{-1,-1,NULL,0,NULL}
};

FENQ fen_para = {
NULL,//GtkWidget *win;
"Parametres",	//char *fentit;
150,	//int fenlarg;	/*largeur fenetre*/
150,	//int fenhaut;	/*hauteur  fenetre*/
400,	//int posx;	/* coin gauche x*/
0,	//int posy;	/* coin gauche y*/
//0,	//int sizsvt;	// sizeof struct des variables
&s_fpara[0],	// pointeur sur definition zones saisie
&lib_fpara[0],	// pointeur sur definition libelle
&boutpara	// fonction ajout de bouton
};

void bok_para(GtkWidget *widget, gpointer data)
{
char zn[12];
sauvdb_param("MIPA", para.fimipa);
sprintf(zn,"%d",para.lifa);
sauvdb_param("LIFA", zn);
//CSFnli = para.lifa;
gtk_widget_destroy(fen_para.win);
fen_para.win = NULL;
}

void bcancel_para(GtkWidget *widget, gpointer data)
{
gtk_widget_destroy(fen_para.win);
fen_para.win = NULL;
}

void affi_para()
{
char zn[12];
init_para();
char *pnf = NULL;
//bloqchfq=1;
//pnf = strrchr (para.fimipa, '/');
pnf = strrchr (para.fimipa, SLAF);
if ( pnf != NULL)
  gtk_label_set_text(GTK_LABEL(s_fpara[MIPA].wdg),pnf+1);
sprintf(zn,"%d",para.lifa);
gtk_entry_set_text(GTK_ENTRY (s_fpara[LIFA].wdg),zn);
//bloqchfq=0;
//   gtk_widget_grab_focus (bcancel);
}

void bchoix_para(GtkWidget *widget, gpointer data)
{
char *pnf = NULL;
ficcdimp[0] = '\0';
open_ficdiv();
if (ficcdimp[0] != '\0')	{
  strcpy(para.fimipa,ficcdimp);
//   affi_para(); fait init
// pnf = strrchr (para.fimipa, '/');
 pnf = strrchr (para.fimipa, SLAF);
if ( pnf != NULL)
  gtk_label_set_text(GTK_LABEL(s_fpara[MIPA].wdg),pnf+1);
				}
}

void boutpara(GtkWidget *widx, gpointer data)
{
GtkWidget *bpara;
GtkWidget *bcancel;
GtkWidget *bchoix;
GdkColor   colortabs;
colortabs.red=30000;//65535;
colortabs.green=45000;//65535;
colortabs.blue=0;

 bchoix =  gtk_button_new_with_label("Choix");
  gtk_widget_set_size_request(bchoix, 80, 25);
  gtk_fixed_put(GTK_FIXED(widx), bchoix,330,13);

 bpara = gtk_button_new_from_stock(GTK_STOCK_SAVE);
//  gtk_widget_set_size_request(bpara, 60, 25);
  gtk_fixed_put(GTK_FIXED(widx), bpara,200,90);
 bcancel =  gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  gtk_fixed_put(GTK_FIXED(widx), bcancel,100,90);

gtk_widget_modify_bg (bpara, GTK_STATE_NORMAL, &colortabs);
gtk_widget_modify_bg (bcancel, GTK_STATE_NORMAL, &colortabs);
gtk_widget_modify_bg (bchoix, GTK_STATE_NORMAL, &colortabs);

g_signal_connect(G_OBJECT(bpara), "clicked", G_CALLBACK (bok_para),NULL);
g_signal_connect(G_OBJECT(bcancel), "clicked", G_CALLBACK (bcancel_para),NULL);
g_signal_connect(G_OBJECT(bchoix), "clicked", G_CALLBACK (bchoix_para),NULL);
}

int dialogtext(char* tit,char* mes,char* initex, char* ss)
{
int ret;
GtkWidget* pBoite;
GtkWidget* pEntry;
GtkWidget* messex;
const gchar* ssent;
pBoite = gtk_dialog_new_with_buttons(tit,
        GTK_WINDOW(winp),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,GTK_RESPONSE_OK,
        GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
        NULL);
//  gtk_window_set_default_size(GTK_WINDOW(pBoite), 500, 100);
messex = gtk_label_new(NULL);
gtk_label_set_text(GTK_LABEL(messex),mes);
gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pBoite)->vbox), messex, TRUE, FALSE, 0);
pEntry = gtk_entry_new();
gtk_entry_set_text(GTK_ENTRY(pEntry), initex);
gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pBoite)->vbox), pEntry, TRUE, FALSE, 0);
gtk_widget_show_all(GTK_DIALOG(pBoite)->vbox);
switch (gtk_dialog_run(GTK_DIALOG(pBoite)))
{
case GTK_RESPONSE_OK:
  ssent = gtk_entry_get_text(GTK_ENTRY(pEntry));
  strcpy(ss,ssent);
  ret = 0;
break;
case GTK_RESPONSE_CANCEL:
case GTK_RESPONSE_NONE:
default:
  ret = -1;
break;
}
gtk_widget_destroy(pBoite);
//free(ssent);
return ret;
}

