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

int 	fonc_sfprod (DEF_S_FQ *pzw, int temchang);
void 	bout_sfprod (GtkWidget *widx);
void 	enreg_sfprod (GtkWidget *widget, gpointer x);
void 	suprim_sfprod (GtkWidget *widget, gpointer x);
void 	affi1c_sfprod ();
void erpOn(int n);
void erpOff(int n);
void raz_smc();
void 	raz_sfprod ();
void 	activ_sfprod ();
void 	deactiv_sfprod ();
void activ_supprod();
void 	chang_sfprod (DEF_S_FQ *pzw);
void affi_pu();

#define MEM 0
extern S_PROD lsfprod; // a copier apres lecture
extern S_TAUX taux[];
extern int bloqchfq; // inhibe signal change si affichage
extern S_DAT today;
extern void act_sfprod(GtkWidget *widget, gpointer data);
extern long zslon;
extern GtkWidget* winliprod;

enum {	// pour saisie Fiche produit
CLEPROD,
CMS,
//LIBPROD,
PU,
CTAX,
UNIT,
DATCRE,
DATMOD,
NOTES,
};

S_PROD sfprod;

static char zep [1024];
static GtkWidget *boutsave;
static GtkWidget *boutclear;
GtkWidget *wnotes;  // pour textview notes
GtkWidget *wlibel;  // pour textview libelle
static GtkWidget *boutdelete;
//static GtkWidget *wnotesbuf;
//#define YAD 150
#define DEPS(zz) (char*)&sfprod.zz-(char*)&sfprod
DEF_S_FQ s_fsfprod[]= {
{100,10,CLEPROD,DEPS(cleprod),NULL,MEM,SSPEC,11,&fonc_sfprod,"%s"},
{250,10,CMS,DEPS(cms),NULL,MEM,SAISCHAR,1,&fonc_sfprod,"%s"},
//{10,53,LIBPROD,DEPS(libprod),NULL,MEM,SAISCHAR,39,&fonc_sfprod,"%s"},
{105,160,PU,DEPS(spriunit),NULL,MEM,SAISMON,13,&fonc_sfprod,"%s"},
{100,190,CTAX,DEPS(codtax),NULL,MEM,SAISCHAR,1,&fonc_sfprod,"%s"},
{200,190,UNIT,DEPS(unit),NULL,MEM,SAISCHAR,4,&fonc_sfprod,"%s"},
{170,355,DATCRE,DEPS(datcre),NULL,MEM,LIBCHAR,12,NULL,"%s"},
{170,380,DATMOD,DEPS(datmod),NULL,MEM,LIBCHAR,12,NULL,"%s"},
//{10,YAD+205,NOTES,DEPS(notes),NULL,MEM,DEFMAN,70,&fonc_sfprod,"%s"},
{-1,-1,0,0,NULL,0,0,0,NULL,NULL}
};
#undef DEPS

DEF_L_FQ lib_fsfprod[]= {
{10,15,NULL,0,"Clef Produit"},
{220,15,NULL,0,"cms"},
{25,35,NULL,0,"Libelle (40 car / ligne)"},
//{100,35,NULL,0,"1234567890"},
{10,165,NULL,0,"Prix Unitaire"},
{10,195,NULL,0,"Code Taxe"},
{150,195,NULL,0,"Unite"},
{10,360,NULL,0,"Dat.creation"},
{10,385,NULL,0,"Dat.modification"},
{20,215,NULL,0,"Notes"},
{-1,-1,NULL,0,NULL}
};

FENQ fen_sfprod = {
NULL,//GtkWidget *win;
"Saisie Fiche Produit",	//char *fentit;
305,	//int fenlarg;	/*largeur fenetre*/
430,	//int fenhaut;	/*hauteur  fenetre*/
50,	//int posx;	/* coin gauche x*/
550,	//int posy;	/* coin gauche y*/
//0,	//int sizsvt;	// sizeof struct des variables
&s_fsfprod[0],	// pointeur sur definition zones saisie
&lib_fsfprod[0],	// pointeur sur definition libelle
&bout_sfprod	// fonction ajout de bouton
};

void deactiv_sfprod()
{
 gtk_widget_set_sensitive(boutsave,FALSE);
 gtk_widget_set_sensitive(boutdelete,FALSE);
}

void activ_sfprod()
{
 gtk_widget_set_sensitive(boutsave,TRUE);
}

void activ_supprod()
{
 gtk_widget_set_sensitive(boutdelete,TRUE);
}

int fonc_sfprod(DEF_S_FQ *pzw,int temchang)
{
int ret = 0,lt;
GtkEditable *editable;
gchar *text;
char rkey3[12];
 if ( pzw == NULL) return ret;
	// traite change SSPEC   codprod
 if ( temchang == 1)	{
  if (GTK_IS_WINDOW (winliprod)) 	{
   editable = GTK_EDITABLE (pzw->wdg);
   text = gtk_editable_get_chars (editable, 0, -1);
   if ( *(text) != ' ' && *(text) != '.')	{
    strcpy(rkey3,text);
    lt = strlen(rkey3);
    if (lt > 0 && lt < 5 )  rechprod((char*)rkey3);
					       	}
   g_free(text);
					}
   return ret;
			}
switch (pzw->idcol)
{
case CLEPROD:
 if ( sfprod.cleprod[0] == ' ' || sfprod.cleprod[0] == '.')  {
   ret = -1;   message(6);
   erpOn(CLEPROD);
   break;
				}
 sfprod.cleon = lit1c_fprod(sfprod.cleprod);
 if ( sfprod.cleon == 1) affi1c_sfprod();// si inconnu struct lect precedente
 raz_smc();	// oblige a saisir cms apres saisie de cleprod
 erpOff(CLEPROD);
 break;
case CMS:
 if (sfprod.cleprod == NULL)	{
   raz_smc(); message(8);
   gtk_widget_grab_focus (s_fsfprod[CLEPROD].wdg); // ret = 0
   break;
				 }
 if (sfprod.cms == '\0') sfprod.cms = ' ';
 if (sfprod.cms != ' ' && sfprod.cms != 'c' &&
   sfprod.cms != 'm' && sfprod.cms != 's' ) {
    ret = -1;    message(3);    break;
					}
 if ( sfprod.cleon == 0)	{	//cleprod inconnu
    if ( sfprod.cms != 'c') 	{
      raz_smc(); message(8);
      gtk_widget_grab_focus (s_fsfprod[CLEPROD].wdg);
				 }
    else {
      //raz_sfprod(); affi1c_sfprod();
      activ_sfprod();
	 }
				}
 else if ( sfprod.cleon == 1)	{	//cleprod connu
    if ( sfprod.cms == ' ') deactiv_sfprod();
    else if ( sfprod.cms == 'm') activ_sfprod();
    else if ( sfprod.cms == 'c') {ret = -1; raz_smc();  message(7); }
    else if ( sfprod.cms == 's') activ_supprod();
				}
 break;
case PU:
  if ( (ret = mdctoi(sfprod.spriunit)) == 0) {
   sfprod.priunit = zslon;		//maj ici
					}
  else  sfprod.priunit = 0;
  affi_pu();
 break;
case CTAX:
 if ( valtau(sfprod.codtax[0]) == -1)	{
   erpOn(CTAX);
   ret = -1;   message(10); break;
//printf("errtax taxe\n");
					}
 erpOff(CTAX);
 break;
}
return ret;
}

gboolean focout_notprod (GtkWidget *widget, GdkEventKey *event, DEF_S_FQ *pzw)
{
int lenotes;
GtkTextBuffer* texbufnot=0;
GtkTextIter start;
GtkTextIter end;
gchar* buf=0;
texbufnot=gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
gtk_text_buffer_get_start_iter(texbufnot,&start);
gtk_text_buffer_get_end_iter(texbufnot,&end);
buf =gtk_text_buffer_get_text(texbufnot,&start, &end,TRUE);
lenotes = strlen(buf);
//printf(" taille notes= %d\n",lenotes);
if ( lenotes < DIMNOTES)	{
 strcpy( sfprod.notes,buf);
				}
else {   message(2);	raz_smc();	}
g_free(buf);
 return FALSE;
}

gboolean focout_libelle (GtkWidget *widget, GdkEventKey *event, DEF_S_FQ *pzw)
{
int lelibel;
GtkTextBuffer* texbuflib=0;
GtkTextIter start;
GtkTextIter end;
gchar* buf=0;
texbuflib=gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
gtk_text_buffer_get_start_iter(texbuflib,&start);
gtk_text_buffer_get_end_iter(texbuflib,&end);
buf =gtk_text_buffer_get_text(texbuflib,&start, &end,TRUE);
lelibel = strlen(buf);
//printf(" taille libel= %d\n",lelibel);
if ( lelibel < DIMNOTES)	{
 strcpy( sfprod.libprod,buf);
				}
else {   message(2);	raz_smc();	}
g_free(buf);
 return FALSE;
}


void erpOn(int n)
{
int  no, of;
 no = n / 8;
 of = n % 8;
 sfprod.ue.erreur[no] = sfprod.ue.erreur[no] | (1 << of);
}

void erpOff(int n)
{
int  no, of;
 no = n / 8;
 of = n % 8;
 sfprod.ue.erreur[no] = sfprod.ue.erreur[no] | (1 << of);
 sfprod.ue.erreur[no] = sfprod.ue.erreur[no] ^ (1 << of);
}

static char sqlins_fprod[]="INSERT INTO Prod VALUES('%s',\"%s\",'%10.2f','%s','%s','%s','%s',\"%s\")";
static char sqlmaj_fprod[]="UPDATE Prod SET Libprod=\"%s\",Priunit='%10.2f',Codtax='%s',Unit='%s',Datmod='%s',Notes=\"%s\" WHERE Cleprod='%s'";

void enreg_sfprod(GtkWidget *widget, gpointer x)
{
float ff;
char datmod[13]="2012.12.24\0";
char zr[12];
if ( sfprod.ue.erd != 0)	{
   printf("erenreg = %lld\n",sfprod.ue.erd);
   message(0);	raz_smc();
   return;
			}
strncpy(datmod,today.sdat,12);
  ff = (float)sfprod.priunit / 100;
//printf("tax=%s unit=%s pu=%ld\n",sfprod.codtax,sfprod.unit,sfprod.priunit);
if ( sfprod.cms == 'm' ) {  // maj
 sprintf(zep,sqlmaj_fprod,
sfprod.libprod,ff,sfprod.codtax,sfprod.unit,
datmod,sfprod.notes,sfprod.cleprod);
			}
else if (sfprod.cms == 'c') {			// creation
 sprintf(zep,sqlins_fprod,sfprod.cleprod,
sfprod.libprod,ff,sfprod.codtax,sfprod.unit,
datmod,datmod,sfprod.notes);
     }
else printf("pb enreg sfprod !\n");
 execdb(zep,'u');
//printf("ins/maj  %s\n",zep);
if (GTK_IS_WINDOW (winliprod))	{
 strcpy(zr,sfprod.cleprod);
 if ( strlen(zr) > 4) zr[3] = '\0';
 rechprod(zr);
				}
clear_sfprod();
}

static char sqldel_fprod[]="DELETE  FROM Prod WHERE Cleprod='%s'";

void suprim_sfprod(GtkWidget *widget, gpointer x)
{
char zr[12];
if (sfprod.cms == 's') {			// suppression
 sprintf(zep,sqldel_fprod,sfprod.cleprod);
 execdb(zep,'u');
if (GTK_IS_WINDOW (winliprod))	{
 strcpy(zr,sfprod.cleprod);
 if ( strlen(zr) > 4) zr[3] = '\0';
 rechprod(zr);
				}
clear_sfprod();
		   }
}

static char sql_fprod[]="SELECT * FROM Prod WHERE Cleprod='%s'";

int lit1c_fprod(char *kleprod)
{
//raz_sfprod();
lsfprod.cleprod[0]='\0'; //	bizarre modifie kleprod
sprintf(zep,sql_fprod,kleprod);
//printf("lect  %s \n",zep);
execdb(zep,'r'); // met a jour la struct si trouve sinon lecture precedente
	// inconnu
if ( lsfprod.cleprod[0] == '\0') return 0;	// toute la struct a 0
	//trouve
memcpy((char*)&sfprod,(char*)&lsfprod,sizeof(S_PROD));// sauve struc compte saisie
//printf("li sfprod.nucop = %s %s\n",sfprod.nucop,(char*)sfprod.rg[0].nucorep);
sfprod.ue.erd = 0L;
return 1;
}

void raz_sfprod()
{
//strcpy(sfprod.cleprod," ");	// garder si creation
 sfprod.libprod[0] = '\0';
 sfprod.spriunit[0] = '\0';
 sfprod.priunit = 0L;
 sfprod.codtax[0] = '\0';
 sfprod.unit[0] = '\0';
 sfprod.datcre[0] = '\0';
 sfprod.datmod[0] = '\0';
 sfprod.notes[0] = '\0';
sfprod.ue.erd = 0L;
}

void raz_smc()
{
bloqchfq=1;
sfprod.cms=' ';
gtk_entry_set_text(GTK_ENTRY (s_fsfprod[CMS].wdg),(char*) "");
deactiv_sfprod();
bloqchfq=0;
}

void clear_sfprod()
{
raz_sfprod();
//strcpy(sfprod.nucop," ");
sfprod.cleprod[0] ='\0';
raz_smc();
affi1c_sfprod();
}

void affi_pu()
{
char zt[14] = "             \0";
 if ( sfprod.priunit != 0)  {
   ltof(sfprod.priunit,zt,13);
  strcpy(sfprod.spriunit,zt);
				}
 else strcpy(sfprod.spriunit,"\0");	// pb ligne vide blanc si zéro
//gtk_entry_set_alignment(GTK_ENTRY (s_fsfprod[PU].wdg),1); gene modif
gtk_entry_set_text(GTK_ENTRY (s_fsfprod[PU].wdg), sfprod.spriunit);
}

void affi1c_sfprod()
{
GtkTextIter start;
bloqchfq=1;
gtk_entry_set_text(GTK_ENTRY (s_fsfprod[CLEPROD].wdg),sfprod.cleprod);

GtkTextBuffer* texbuflib=0;
texbuflib=gtk_text_view_get_buffer(GTK_TEXT_VIEW(wlibel));
gtk_text_buffer_set_text( texbuflib, sfprod.libprod, -1);
//On recupere l'origine du buffer
gtk_text_buffer_get_start_iter(texbuflib,&start);
	// cursor au debut
gtk_text_buffer_place_cursor(texbuflib,&start);

affi_pu();
gtk_entry_set_text(GTK_ENTRY (s_fsfprod[CTAX].wdg), sfprod.codtax);
gtk_entry_set_text(GTK_ENTRY (s_fsfprod[UNIT].wdg), sfprod.unit);
 gtk_label_set_text(GTK_LABEL (s_fsfprod[DATCRE].wdg), sfprod.datcre);
 gtk_label_set_text(GTK_LABEL (s_fsfprod[DATMOD].wdg), sfprod.datmod);

GtkTextBuffer* texbufnot=0;
texbufnot=gtk_text_view_get_buffer(GTK_TEXT_VIEW(wnotes));
gtk_text_buffer_set_text( texbufnot, sfprod.notes, -1);
gtk_text_buffer_get_start_iter(texbufnot,&start);
gtk_text_buffer_place_cursor(texbufnot,&start);
bloqchfq=0;
}

void acces_sfprod(gchar *text)
{
act_sfprod(NULL,NULL);		// cree fen si besoin
sfprod.cleon = lit1c_fprod(text);
if ( sfprod.cleon == 1) affi1c_sfprod();// si inconnu struct lect precedente
raz_smc();	// oblige a saisir cms apres saisie de cleprod
}

void bout_sfprod(GtkWidget *widx)
{
GtkWidget *wscro;  // pour scroll notes
GtkWidget *wscrolib;  // pour scroll libelle
//GtkWidget *wnotes;  // pour textview notes
GdkColor   colortabs;
colortabs.red=30000;//65535;
colortabs.green=45000;//65535;
colortabs.blue=0;

 // La scrolled_window libelle
  wscrolib = gtk_scrolled_window_new (NULL,NULL);
  gtk_scrolled_window_set_policy  (GTK_SCROLLED_WINDOW(wscrolib)
				   ,GTK_POLICY_AUTOMATIC
				   ,GTK_POLICY_AUTOMATIC);
//gtk_container_add (GTK_CONTAINER (widx) ,wscrolib);// il faut put
gtk_fixed_put(GTK_FIXED(widx),wscrolib,10,53);
wlibel = gtk_text_view_new();
//s_fsfprod[NOTES].wdg = wlibel;
gtk_container_add (GTK_CONTAINER (wscrolib) ,wlibel);
 gtk_widget_set_size_request (wlibel ,300 ,100);// necessaire
 g_signal_connect_after(G_OBJECT(wlibel), "focus-out-event", G_CALLBACK (focout_libelle), (void*) (s_fsfprod));

 // La scrolled_window notes
  wscro = gtk_scrolled_window_new (NULL,NULL);
  gtk_scrolled_window_set_policy  (GTK_SCROLLED_WINDOW(wscro)
				   ,GTK_POLICY_AUTOMATIC
				   ,GTK_POLICY_AUTOMATIC);
//gtk_container_add (GTK_CONTAINER (widx) ,wscro);// il faut put
gtk_fixed_put(GTK_FIXED(widx),wscro,10,235);
wnotes = gtk_text_view_new();
//s_fsfprod[NOTES].wdg = wnotes;
gtk_container_add (GTK_CONTAINER (wscro) ,wnotes);
 gtk_widget_set_size_request (wnotes ,300 ,115);// necessaire
 g_signal_connect_after(G_OBJECT(wnotes), "focus-out-event", G_CALLBACK (focout_notprod), (void*) (s_fsfprod));

boutsave = gtk_button_new_with_label("Enregistrer");
  gtk_widget_set_size_request(boutsave, 90, 25);
  gtk_fixed_put(GTK_FIXED(widx), boutsave,215,415);
  gtk_widget_modify_bg (boutsave, GTK_STATE_NORMAL, &colortabs);
g_signal_connect(G_OBJECT(boutsave), "clicked", G_CALLBACK (enreg_sfprod),NULL);
boutclear = gtk_button_new_with_label("Efface");
  gtk_widget_set_size_request(boutclear, 70, 25);
  gtk_fixed_put(GTK_FIXED(widx), boutclear,120,415);
  gtk_widget_modify_bg (boutclear, GTK_STATE_NORMAL, &colortabs);
g_signal_connect(G_OBJECT(boutclear), "clicked", G_CALLBACK (clear_sfprod),NULL);
boutdelete= gtk_button_new_with_label("Supprime");
  gtk_widget_set_size_request(boutdelete, 85, 25);
  gtk_fixed_put(GTK_FIXED(widx), boutdelete,10,415);
  gtk_widget_modify_bg (boutdelete, GTK_STATE_NORMAL, &colortabs);
g_signal_connect(G_OBJECT(boutdelete), "clicked", G_CALLBACK (suprim_sfprod),NULL);

}


