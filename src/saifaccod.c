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
#include <gtk/gtk.h>
#include <string.h>
#include "genfen.h"
#include "fast.h"

gboolean scod_dupli (GtkWidget *widget, gpointer data);
//void afcell_scod(int idcol, int liga);
gboolean scod_efflig (GtkWidget *widget, gpointer data);
gboolean scod_inslig (GtkWidget *widget, gpointer data);
gboolean scod_supflig (GtkWidget *widget, gpointer data);
void razl_scod(int liga);
//int derlig_scod(int liga);
void scod_sauvdb();
void affi_derlic();
gboolean verifinpw2();


extern MEMO_T_WIDG *inpw;
extern int bloqchang;

static char zz [256];
static int derlic = 0;
static GtkWidget *lablig;

//_______________fenetre saisie codes facture ____________
enum {
CODcode,
CODsuit,
CODlib,
CODnco
};

#define CODnli 60


S_CODFAC ligcod[CODnli];
S_CODFAC dbligcod;

MEMO_T_WIDG ligcod_wgt[CODnli][CODnco];

extern  int traicod(MEMO_T_WIDG *pw);
extern  void boutscod(GtkWidget *box);
#define DEPS(zz) (char*)&ligcod[0].zz-(char*)&ligcod[0].code
static DEF_L_FnC colscod[CODnco] ={
{NULL,CODcode,DEPS(code),SAISCHAR,8,&traicod,"%s","Code"},
{NULL,CODsuit,DEPS(suit),SAISCHAR,1,&traicod,"%c","Suite"},
//  long libelle idem saifac
{NULL,CODlib,DEPS(lib),SAISCHAR,49,&traicod,"%s","Libelle"},
};
#undef DEPS

//          definition fenetres
FnC fen_scod = {
NULL,
"Saisie Code Facture",        /* pointeur sur titre  fenetre */
0,	//	/*largeur fenetre*/
450,	//CODnli*15,	/*hauteur  fenetre*/
//120,	/*hauteur  fenetre*/
0,	/* coin gauche x*/
0,	/* coin gauche y*/
CODnli,	/* nb lignes*/
CODnco,	/* nb col */
sizeof(S_CODFAC),
&colscod[0],	// pointeur sur definition colonnes
&ligcod_wgt[0][0],	// pointeur sur tableau widget
&boutscod// fonction ajout de bouton
};

	// verif inpw si plusieurs fen tablo inpw est -il le bon ?
gboolean verifinpw2()
{
DEF_L_FnC *pl;
int col;
pl = inpw->pdefc;
col = pl->idcol;
if ( pl == &colscod[col]) return TRUE;
//printf("inpw scod FALSE\n");
message(17);
return FALSE;
}

int traicod(MEMO_T_WIDG *pw)
{
//DEF_L_FnC *pl;
int ret = 0, liga;
 if ( pw == NULL) return ret;
// pl = pw->pdefc;
 liga=pw->liga;
if ( liga >= derlic)	{
 derlic = liga + 1;
 affi_derlic();
			}
return ret;
}

void act_fenscod(GtkWidget *widget, gpointer data)
{
int i;
if ( !GTK_IS_WINDOW (fen_scod.win)) {
 cretablo ((FnC*) &fen_scod, (void*) &ligcod);
		}
gtk_window_deiconify((GtkWindow*)fen_scod.win);
for (i = 0; i < CODnli; ++i)
  aftablo((FnC*) &fen_scod,(S_CODFAC*) &ligcod,i);
gtk_widget_show_all(fen_scod.win);
affi_derlic();
}

gboolean scod_efflig (GtkWidget *widget, gpointer data)
{
int liga;
//DEF_L_FnC *pl;
if ( verifinpw2() == FALSE) return FALSE;
bloqchang = 1;	// inhibe chang
//pl = inpw->pdefc;
liga=inpw->liga;
 razl_scod(liga);
bloqchang = 0;	// inhibe chang
return FALSE;
}

void razl_scod(int liga)
{
//int i;
memset ((char*)&ligcod[liga],0, sizeof(S_CODFAC));
//for ( i=0; i < CODnco; ++i)
aftablo((FnC*) &fen_scod,(S_CODFAC*) &ligcod,liga);
// afcell_scod(i,liga);
}

void affi_derlic()
{
char form[]="%d/%d";
char zef[10];
sprintf(zef,form,derlic,CODnli);
gtk_label_set_text(GTK_LABEL(lablig),zef);
}

gboolean scod_inslig (GtkWidget *widget, gpointer data)
{
int liga,i;
//DEF_L_FnC *pl;
if ( verifinpw2() == FALSE) return FALSE;
bloqchang = 1;	// inhibe chang
//pl = inpw->pdefc;
liga=inpw->liga;
if ( derlic >= (CODnli - 1))	{
  message(11);
  return TRUE;
			}
for ( i = derlic; i > liga; --i)	{
 memcpy((char*)&ligcod[i],(char*)&ligcod[i-1],sizeof(S_CODFAC));
 aftablo((FnC*) &fen_scod,(S_CODFAC*) &ligcod,i);
 //afilig_scod(i);
				}
 razl_scod(liga);
 ++derlic;
 affi_derlic();
bloqchang = 0;	// inhibe chang
return FALSE;
}

gboolean scod_suplig (GtkWidget *widget, gpointer data)
{
int liga,i;
//DEF_L_FnC *pl;
if ( verifinpw2() == FALSE) return FALSE;
bloqchang = 1;	// inhibe chang
//pl = inpw->pdefc;
liga=inpw->liga;
//derl = derlig_scod(liga);
for ( i = liga; i < derlic; ++i)	{
 memcpy((char*)&ligcod[i],(char*)&ligcod[i+1],sizeof(S_CODFAC));
 aftablo((FnC*) &fen_scod,(S_CODFAC*) &ligcod,i);
// afilig_scod(i);
				}
--derlic;
 razl_scod(derlic);
--derlic;
affi_derlic();
bloqchang = 0;	// inhibe chang
return FALSE;
}

int wheretabcode(char *code)
{
int i;
for (i = 0; i < CODnli; ++i)	{
 if ( strcmp(ligcod[i].code,code) == 0) return i;
				}
return -1;
}

static char sqlins_scod[]="INSERT INTO Codfac VALUES(%d,'%s','%c',\"%s\")";
static char sqlli_scod[]="SELECT * FROM Codfac WHERE  Nuli=%d";
static char sqlmaj_scod[]="UPDATE Codfac SET Nuli=%d, Code='%s',Suit='%c',Lib=\"%s\" WHERE Nuli=%d";
//att sqlite refuse Suit null

void scod_lidb()
{
int i,idb;
for ( i=0; i < CODnli; ++i) 	{
// if ( ligcod[i].lib[0] == '\0') continue;
 dbligcod.nuli = -1;
 idb = i +1;
 sprintf(zz,sqlli_scod, idb);
 execdb(zz,'o');
//printf("idb=%d %d zz= %s..\n ",idb,dbligcod.nuli,zz);
 if ( dbligcod.nuli != -1)	{	// existe
  ligcod[i].nuli = dbligcod.nuli;
  strcpy(ligcod[i].code,dbligcod.code);
  ligcod[i].suit = dbligcod.suit;
//printf("idb=%d %d lib= %s..\n ",idb,dbligcod.nuli,dbligcod.lib);
  strcpy(ligcod[i].lib,dbligcod.lib);
  derlic = i+1;
				}
				}
}

void scod_sauvdb()
{
int i,idb;
//for ( i=0; i < CODnli; ++i) 	{
for ( i=0; i <= derlic; ++i) 	{
// if ( ligcod[i].lib[0] == '\0') continue;
 dbligcod.nuli = -1;
 idb = i +1;
 sprintf(zz,sqlli_scod, idb);
//printf("zz= %s..\n ",zz);
 execdb(zz,'o');
if ( ligcod[i].suit == '\0') ligcod[i].suit = ' ';
 if ( dbligcod.nuli != -1)	{	// existe
 sprintf(zz,sqlmaj_scod, idb,ligcod[i].code,ligcod[i].suit,ligcod[i].lib,idb);
 execdb(zz,'u');

				}
 else 	{
 sprintf(zz,sqlins_scod, idb,ligcod[i].code,ligcod[i].suit,ligcod[i].lib); // sqlite incremente nuli a partir de 1
//printf("zz= %s..\n ",zz);
 execdb(zz,'u');
	}
//printf("idb=%d %d zz= %s..\n ",idb,dbligcod.nuli,zz);
				}
}


void boutscod(GtkWidget *box)
{
GtkWidget *boxlibel;
GtkWidget *boutinsl;
GtkWidget *boutsupl;
GtkWidget *boutsauv;
//GtkWidget *bouton2;
GtkWidget *bouefface;
GdkColor   colortabs;
colortabs.red=30000;//65535;
colortabs.green=45000;//65535;
colortabs.blue=0;
  boxlibel = gtk_hbox_new(FALSE, 1);	// horizontal
  gtk_box_pack_start(GTK_BOX(box), boxlibel, FALSE, FALSE, 0);
boutinsl = gtk_button_new_with_label("Ins.ligne");
  gtk_widget_set_size_request(boutinsl, 80, 25);
  gtk_box_pack_start(GTK_BOX(boxlibel), boutinsl, FALSE, FALSE, 0);
boutsupl = gtk_button_new_with_label("Sup.ligne");
  gtk_widget_set_size_request(boutsupl, 80, 25);
  gtk_box_pack_start(GTK_BOX(boxlibel), boutsupl, FALSE, FALSE, 0);
bouefface = gtk_button_new_with_label("Efface Ligne");
  gtk_widget_set_size_request(bouefface, 100, 20);
  gtk_box_pack_start(GTK_BOX(boxlibel), bouefface, FALSE, FALSE, 0);
boutsauv = gtk_button_new_with_label("SAUVE");
  gtk_widget_set_size_request(boutsauv, 80, 25);
  gtk_box_pack_start(GTK_BOX(boxlibel), boutsauv, FALSE, FALSE, 0);
/*bouton2 = gtk_button_new_with_label("Duplic");
  gtk_widget_set_size_request(bouton2, 80, 25);
  gtk_box_pack_start(GTK_BOX(boxlibel), bouton2, FALSE, FALSE, 0);*/
lablig = gtk_label_new(NULL);
  gtk_widget_set_size_request(lablig, 100, 25);
  gtk_box_pack_start(GTK_BOX(boxlibel), lablig, FALSE, FALSE, 0);
gtk_widget_modify_bg (boutinsl, GTK_STATE_NORMAL, &colortabs);
gtk_widget_modify_bg (boutsupl, GTK_STATE_NORMAL, &colortabs);
gtk_widget_modify_bg (boutsauv, GTK_STATE_NORMAL, &colortabs);
//gtk_widget_modify_bg (bouton2, GTK_STATE_NORMAL, &colortabs);
gtk_widget_modify_bg (bouefface, GTK_STATE_NORMAL, &colortabs);
 g_signal_connect(G_OBJECT(bouefface), "clicked", G_CALLBACK (scod_efflig),NULL);
 g_signal_connect(G_OBJECT(boutinsl), "clicked", G_CALLBACK (scod_inslig),NULL);
 g_signal_connect(G_OBJECT(boutsupl), "clicked", G_CALLBACK (scod_suplig),NULL);
 g_signal_connect(G_OBJECT(boutsauv), "clicked", G_CALLBACK (scod_sauvdb),NULL);
// g_signal_connect(G_OBJECT(bouton2), "clicked", G_CALLBACK (scod_dupli),NULL);

}
