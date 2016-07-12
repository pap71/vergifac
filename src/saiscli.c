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
#ifdef WIN32
#include <windows.h>
#include <shellapi.h>
#endif

int 	fonc_sfcli (DEF_S_FQ *pzw, int temchang);
void 	bout_sfcli (GtkWidget *widx);
void 	enreg_sfcli (GtkWidget *widget, gpointer x);
void 	efface ();
void 	suprim_sfcli (GtkWidget *widget, gpointer x);
void 	affi1c_sfcli ();
void errOn(int n);
void errOff(int n);
void 	raz_sfcli ();
void 	activ_sfcli ();
void 	deactiv_sfcli ();
void activ_supcli();
void 	chang_sfcli (DEF_S_FQ *pzw);

#define MEM 0
extern S_FCLI lsfcli; // a copier apres lecture
extern GtkWidget* winli;
extern int bloqchfq; // inhibe signal change si affichage
extern S_DAT today;
extern void act_sfcli(GtkWidget *widget, gpointer data);

enum {	// pour saisie Fiche client
CLECLI,
CMS,
TYPCLI,
ADD1,
ADD2,
ADD3,
PAYS,
CODPOS,
ADD4,
DATCRE,
DATMOD,
TEL,
MAIL,
NOTES,
};

S_FCLI sfcli;

static char zz [1024];
static GtkWidget *boutenreg;
static GtkWidget *boutefface;
GtkWidget *wtext;  // pour textview notes
static GtkWidget *boutsuprim;
static char rkey2[12];

#define YAD 55
#define DEPS(zz) (char*)&sfcli.zz-(char*)&sfcli
//#define DETR(zl,zz) (char*)&sfcli.rg[zl].zz-(char*)&sfcli
DEF_S_FQ s_fsfcli[]= {
{80,10,CLECLI,DEPS(clecli),NULL,MEM,SSPEC,11,&fonc_sfcli,"%s"},
{208,10,CMS,DEPS(cms),NULL,MEM,SAISCHAR,1,&fonc_sfcli,"%s"},
{275,10,TYPCLI,DEPS(typcli),NULL,MEM,SAISCHAR,2,&fonc_sfcli,"%s"},
//{270,10,MATCOM,DEPS(matcom),NULL,MEM,SSPEC,8,&fonc_sfcli,"%s"},
{10,YAD,ADD1,DEPS(add1),NULL,MEM,SAISCHAR,50,&fonc_sfcli,"%s"},
{10,YAD+25,ADD2,DEPS(add2),NULL,MEM,SAISCHAR,50,&fonc_sfcli,"%s"},
{10,YAD+50,ADD3,DEPS(add3),NULL,MEM,SAISCHAR,50,&fonc_sfcli,"%s"},
{10,YAD+95,PAYS,DEPS(pays),NULL,MEM,SAISCHAR,5,&fonc_sfcli,"%s"},
{60,YAD+95,CODPOS,DEPS(codpos),NULL,MEM,SAISCHAR,8,&fonc_sfcli,"%s"},
{130,YAD+95,ADD4,DEPS(add4),NULL,MEM,SAISCHAR,30,&fonc_sfcli,"%s"},
{180,415,DATCRE,DEPS(datcre),NULL,MEM,LIBCHAR,12,NULL,"%s"},
{180,430,DATMOD,DEPS(datmod),NULL,MEM,LIBCHAR,12,NULL,"%s"},
{80,YAD+120,TEL,DEPS(teleph),NULL,MEM,SAISCHAR,18,&fonc_sfcli,"%s"},
{80,YAD+145,MAIL,DEPS(mail),NULL,MEM,SAISCHAR,39,&fonc_sfcli,"%s"},
//{10,YAD+205,NOTES,DEPS(notes),NULL,MEM,DEFMAN,70,&fonc_sfcli,"%s"},
{-1,-1,0,0,NULL,0,0,0,NULL,NULL}
};
#undef DEPS
//#undef DETR

DEF_L_FQ lib_fsfcli[]= {
{6,15,NULL,0,"Clef Client"},
//{6,22,NULL,0,"Mnemo"},
{180,15,NULL,0,"cms"},
{235,15,NULL,0,"Type"},
{30,420,NULL,0,"Dat.creation"},
{30,435,NULL,0,"Dat.modification"},
{15,YAD-20,NULL,0,"Adresse"},
{15,YAD+75,NULL,0,"Pays    Code.Postal       Ville"},
{6,YAD+125,NULL,0,"Telephone"},
//{6,YAD+150,NULL,0,"Ad.Mails"},
{10,YAD+185,NULL,0,"Notes"},
{-1,-1,NULL,0,NULL}
};

FENQ fen_sfcli = {
NULL,//GtkWidget *win;
"Saisie Fiche Client",	//char *fentit;
355,	//int fenlarg;	/*largeur fenetre*/
330,	//int fenhaut;	/*hauteur  fenetre*/
650,	//int posx;	/* coin gauche x*/
650,	//int posy;	/* coin gauche y*/
//0,	//int sizsvt;	// sizeof struct des variables
&s_fsfcli[0],	// pointeur sur definition zones saisie
&lib_fsfcli[0],	// pointeur sur definition libelle
&bout_sfcli	// fonction ajout de bouton
};

void deactiv_sfcli()
{
 gtk_widget_set_sensitive(boutenreg,FALSE);
 gtk_widget_set_sensitive(boutsuprim,FALSE);
}

void activ_sfcli()
{
 gtk_widget_set_sensitive(boutenreg,TRUE);
}

void activ_supcli()
{
 gtk_widget_set_sensitive(boutsuprim,TRUE);
}

int fonc_sfcli(DEF_S_FQ *pzw,int temchang)
{
int lt,ret = 0;
GtkEditable *editable;
gchar *text;
 if ( pzw == NULL) return ret;
switch (pzw->idcol)
{
case CLECLI:
 if ( temchang == 1)	{	// traite change SSPEC
  if (GTK_IS_WINDOW (winli)) 	{
   editable = GTK_EDITABLE (pzw->wdg);
   text = gtk_editable_get_chars (editable, 0, -1);
   strcpy(rkey2,text);
   lt = strlen(rkey2);
   if (lt > 0 && lt < 5 && *(rkey2) != ' ') 	{
    rechcli((char*)rkey2);
				        	}
   g_free(text);
				}
   break;
			}
 if ( sfcli.clecli[0] == ' ')  {
   ret = -1;   message(9);
   errOn(CLECLI);
   break;
				}
 sfcli.cleon = lit1c_fcli(sfcli.clecli);
 if ( sfcli.cleon == 1) affi1c_sfcli();	// si inconnu struct lect precedente
 raz_cms();	// oblige a saisir cms apres saisie de clecli
 errOff(CLECLI);
 break;
case CMS:
 if (sfcli.clecli == NULL)	{
   raz_cms(); message(8);
   gtk_widget_grab_focus (s_fsfcli[CLECLI].wdg); // ret = 0
   break;
				}
 if (sfcli.cms == '\0') sfcli.cms = ' ';
 if (sfcli.cms != ' ' && sfcli.cms != 'c' &&
   sfcli.cms != 'm' && sfcli.cms != 's' ) {
    ret = -1;
    message(3);
    break;
			}
 if ( sfcli.cleon == 0)	{	//clecli inconnu
    if ( sfcli.cms != 'c') 	{
      //ret = -1;
      raz_cms(); message(8);
      gtk_widget_grab_focus (s_fsfcli[CLECLI].wdg); // ret = 0
				 }
    else { raz_sfcli(); affi1c_sfcli();	activ_sfcli();}
			}
 else if ( sfcli.cleon == 1)	{	//clecli connu
    if ( sfcli.cms == ' ') deactiv_sfcli();
    else if ( sfcli.cms == 'm') activ_sfcli();
    else if ( sfcli.cms == 'c') {ret = -1; raz_cms();  message(7); }
    else if ( sfcli.cms == 's') activ_supcli();
				}
 break;
}
return ret;
}

gboolean focout_notes (GtkWidget *widget, GdkEventKey *event, DEF_S_FQ *pzw)
{
int lenotes;
GtkTextBuffer* text_buffer=0;
GtkTextIter start;
GtkTextIter end;
gchar* buf=0;
//printf("notes 1\n");
	//On recupere le buffer
text_buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
//text_buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(wtext));
	//On recupere l'origine du buffer
gtk_text_buffer_get_start_iter(text_buffer,&start);
	//On recupere la fin du buffer
gtk_text_buffer_get_end_iter(text_buffer,&end);
	//On copie le contenu du buffer dans une variable
buf =gtk_text_buffer_get_text(text_buffer,&start, &end,TRUE);
//printf(" taille notes= %d\n",&end-&start);
lenotes = strlen(buf);
//printf(" taille notes= %d\n",lenotes);
if ( lenotes < DIMNOCLI)	{
 strcpy( sfcli.notes,buf);
				}
else	{
 message(2);	raz_cms();
	}
g_free(buf);
 return FALSE;
}


void errOn(int n)
{
int  no, of;
 no = n / 8;
 of = n % 8;
 sfcli.ue.erreur[no] = sfcli.ue.erreur[no] | (1 << of);
}

void errOff(int n)
{
int  no, of;
 no = n / 8;
 of = n % 8;
 sfcli.ue.erreur[no] = sfcli.ue.erreur[no] | (1 << of);
 sfcli.ue.erreur[no] = sfcli.ue.erreur[no] ^ (1 << of);
}

static char sqlins_fcli[]="INSERT INTO Cli VALUES('%s','%s',\"%s\",\"%s\",\"%s\",'%s','%s',\"%s\", '%s','%s','%s','%s',\"%s\")";
static char sqlmaj_fcli[]="UPDATE Cli SET Type='%s',Add1=\"%s\",Add2=\"%s\",Add3=\"%s\",Pays='%s',Codpos='%s',Add4=\"%s\",Datmod='%s',Tel='%s',Mail='%s',Notes=\"%s\" WHERE Clefacc='%s'";
//Datcre='%s',

void enreg_sfcli(GtkWidget *widget, gpointer x)
{
char datmod[13]="2012.12.24\0";
char zr[12];
if ( sfcli.ue.erd != 0)	{
   printf("erenreg = %lld\n",sfcli.ue.erd);
   message(0);	raz_cms();
   return;
			}
strncpy(datmod,today.sdat,12);
if ( sfcli.cms == 'm' ) {  // maj
 sprintf(zz,sqlmaj_fcli,
sfcli.typcli,sfcli.add1,sfcli.add2,sfcli.add3,sfcli.pays,sfcli.codpos,sfcli.add4,
datmod,sfcli.teleph,sfcli.mail,sfcli.notes,sfcli.clecli);
			}
else if (sfcli.cms == 'c') {			// creation
 sprintf(zz,sqlins_fcli,
sfcli.clecli,sfcli.typcli,sfcli.add1,sfcli.add2,sfcli.add3,sfcli.pays,sfcli.codpos,sfcli.add4,
datmod,datmod,sfcli.teleph,sfcli.mail,sfcli.notes);
     }
else printf("pb enreg sfcli !\n");
  execdb(zz,'u');
//printf("ins/maj  %s\n",zz);
if (GTK_IS_WINDOW (winli))	{
 strcpy(zr,sfcli.clecli);
 if ( strlen(zr) > 4) zr[3] = '\0';
 rechcli(zr);
				}
efface();
}

char sqldel_fcli[]="DELETE  FROM Cli WHERE Clefacc='%s'";

void suprim_sfcli(GtkWidget *widget, gpointer x)
{
if (sfcli.cms == 's') {			// suppression
 sprintf(zz,sqldel_fcli,sfcli.clecli);
 execdb(zz,'u');
 efface();
		   }
}

static char sql_fcli[]="SELECT * FROM Cli WHERE Clefacc='%s'";
//static char sql2_fcli[]="SELECT * FROM Cli WHERE Matcom='%s'";

int lit1c_fcli(char *klecli)
{
//raz_sfcli();
lsfcli.clecli[0]='\0'; //	bizarre modifie klecli
sprintf(zz,sql_fcli,klecli);
//printf("lect  %s \n",zz);
execdb(zz,'c'); // met a jour la struct si trouve sinon lecture precedente
	// inconnu
if ( lsfcli.clecli[0] == '\0') return 0;	// toute la struct a 0
	//trouve
memcpy((char*)&sfcli,(char*)&lsfcli,sizeof(S_FCLI));// sauve struc compte saisie
//printf("li sfcli.nucop = %s %s\n",sfcli.nucop,(char*)sfcli.rg[0].nucorep);
sfcli.ue.erd = 0L;	//strncpy(sfcli.errenreg,"0000",4);
// existe ou pas
//if ( strcmp(klecli,sfcli.clecli) == 0) return 0;
return 1;
}

void raz_sfcli()
{
//strcpy(sfcli.clecli," ");	// garder si creation
 sfcli.typcli[0] = '\0';
 sfcli.add1[0] = '\0';
 sfcli.add2[0] = '\0';
 sfcli.add3[0] = '\0';
 sfcli.pays[0] = '\0';
 sfcli.codpos[0] = '\0';
 sfcli.add4[0] = '\0';
 sfcli.datcre[0] = '\0';
 sfcli.datmod[0] = '\0';
 sfcli.teleph[0] = '\0';
 sfcli.mail[0] = '\0';
 sfcli.notes[0] = '\0';
sfcli.ue.erd = 0L;	//strcpy(sfcli.errenreg,"0000");
}

void raz_cms()
{
bloqchfq=1;
sfcli.cms=' ';
gtk_entry_set_text(GTK_ENTRY (s_fsfcli[CMS].wdg),(char*) "");
deactiv_sfcli();
// gtk_widget_set_sensitive(boutenreg,FALSE);
bloqchfq=0;
}

void efface()
{
raz_sfcli();
//strcpy(sfcli.nucop," ");
sfcli.clecli[0] ='\0';
raz_cms();
affi1c_sfcli();
}

void affi1c_sfcli()
{
GtkTextIter start;
bloqchfq=1;	//temaffich =1;
gtk_entry_set_text(GTK_ENTRY (s_fsfcli[CLECLI].wdg),sfcli.clecli);
gtk_entry_set_text(GTK_ENTRY (s_fsfcli[TYPCLI].wdg), sfcli.typcli);
gtk_entry_set_text(GTK_ENTRY (s_fsfcli[ADD1].wdg), sfcli.add1);
gtk_entry_set_text(GTK_ENTRY (s_fsfcli[ADD2].wdg), sfcli.add2);
gtk_entry_set_text(GTK_ENTRY (s_fsfcli[ADD3].wdg), sfcli.add3);
gtk_entry_set_text(GTK_ENTRY (s_fsfcli[PAYS].wdg), sfcli.pays);
gtk_entry_set_text(GTK_ENTRY (s_fsfcli[CODPOS].wdg), sfcli.codpos);
gtk_entry_set_text(GTK_ENTRY (s_fsfcli[ADD4].wdg), sfcli.add4);
 gtk_label_set_text(GTK_LABEL (s_fsfcli[DATCRE].wdg), sfcli.datcre);
 gtk_label_set_text(GTK_LABEL (s_fsfcli[DATMOD].wdg), sfcli.datmod);
gtk_entry_set_text(GTK_ENTRY (s_fsfcli[TEL].wdg), sfcli.teleph);
gtk_entry_set_text(GTK_ENTRY (s_fsfcli[MAIL].wdg), sfcli.mail);
GtkTextBuffer* text_buffer=0;
//GtkTextIter end;
//gchar* buf=0;

//On recupere le buffer
//text_buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(s_fsfcli[NOTES].wdg));
text_buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(wtext));

gtk_text_buffer_set_text( text_buffer, sfcli.notes, -1);
//On recupere l'origine du buffer
gtk_text_buffer_get_start_iter(text_buffer,&start);
	// cursor au debut
gtk_text_buffer_place_cursor(text_buffer,&start);
//On recupere la fin du buffer
//gtk_text_buffer_get_end_iter(text_buffer,&end);

//deactiv_sfcli();
bloqchfq=0;	//temaffich =0;
}

void acces_sfcli(gchar *text)
{
act_sfcli(NULL,NULL);		// cree fen si besoin
sfcli.cleon = lit1c_fcli(text);
if ( sfcli.cleon == 1) affi1c_sfcli();	// si inconnu struct lect precedente
//g_free(text);
raz_cms();	// oblige a saisir cms apres saisie de clecli
}

	// gnome-open mailto:gnome-list@mail.gnome.org
	// xdg-open mailto:gnome-list@mail.gnome.org
gboolean mail_sfcli(GtkWidget *widget, gpointer data)
{
char csys[80];
#ifdef WIN32
sprintf(csys,"mailto:%s",sfcli.mail);
ShellExecute ( NULL, "open", csys, NULL, NULL, SW_SHOWNORMAL);
#else
sprintf(csys,"xdg-open mailto:%s&",sfcli.mail);
system(csys);
//system("xdg-open factest.pdf&"); // & permet de garder la main ?
#endif
return FALSE;
}

void bout_sfcli(GtkWidget *widx)
{
GtkWidget *wscro;  // pour scroll notes
GtkWidget *boutmail;
GdkColor   colortabs;
colortabs.red=30000;//65535;
colortabs.green=45000;//65535;
colortabs.blue=0;
 // La scrolled_window
  wscro = gtk_scrolled_window_new (NULL,NULL);
  gtk_scrolled_window_set_policy  (GTK_SCROLLED_WINDOW(wscro)
				   ,GTK_POLICY_AUTOMATIC
				   ,GTK_POLICY_AUTOMATIC);
//gtk_container_add (GTK_CONTAINER (widx) ,wscro);// il faut put
gtk_fixed_put(GTK_FIXED(widx),wscro,10,255);

wtext = gtk_text_view_new();
//s_fsfcli[NOTES].wdg = wtext;
gtk_container_add (GTK_CONTAINER (wscro) ,wtext);
 gtk_widget_set_size_request (wtext ,350 ,140);// necessaire

 g_signal_connect_after(G_OBJECT(wtext), "focus-out-event", G_CALLBACK (focout_notes), (void*) (s_fsfcli));

boutenreg = gtk_button_new_with_label("Enregistrer");
  gtk_widget_set_size_request(boutenreg, 90, 25);
  gtk_fixed_put(GTK_FIXED(widx), boutenreg,260,470);
  gtk_widget_modify_bg (boutenreg, GTK_STATE_NORMAL, &colortabs);
g_signal_connect(G_OBJECT(boutenreg), "clicked", G_CALLBACK (enreg_sfcli),NULL);
//gtk_widget_hide(boutenreg);
boutefface = gtk_button_new_with_label("Efface");
  gtk_widget_set_size_request(boutefface, 70, 25);
  gtk_fixed_put(GTK_FIXED(widx), boutefface,150,470);
  gtk_widget_modify_bg (boutefface, GTK_STATE_NORMAL, &colortabs);
g_signal_connect(G_OBJECT(boutefface), "clicked", G_CALLBACK (efface),NULL);
boutsuprim= gtk_button_new_with_label("Supprime");
  gtk_widget_set_size_request(boutsuprim, 85, 25);
  gtk_fixed_put(GTK_FIXED(widx), boutsuprim,10,470);
  gtk_widget_modify_bg (boutsuprim, GTK_STATE_NORMAL, &colortabs);
g_signal_connect(G_OBJECT(boutsuprim), "clicked", G_CALLBACK (suprim_sfcli),NULL);
boutmail= gtk_button_new_with_label("Mail");
  gtk_widget_set_size_request(boutmail, 70, 25);
  gtk_fixed_put(GTK_FIXED(widx), boutmail,6,200);
  gtk_widget_modify_bg (boutmail, GTK_STATE_NORMAL, &colortabs);
g_signal_connect(G_OBJECT(boutmail), "clicked", G_CALLBACK (mail_sfcli),NULL);
//printf("bou 2\n");

}


