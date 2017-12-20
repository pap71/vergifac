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
//#include <string.h>
//#include "genfen.h"

typedef struct  {
int num;
char* messag;
} S_MES;

extern GtkWidget *winp;

static S_MES tlmes[] = {
{0,"Enregistrement refusé erreur de saisie non corrigée"},
{1,"zone saisie trop longue"},
{2,"zone Notes est trop longue..\nIl faut la raccourcir"},
{3,"Valeurs possibles   ' '(blanc),\nc(reation),\nm(odification),\ns(uppression)"},
{4,"cette zone ne peut pas rester a blanc (vide)"},
{5,"Clef Mnémo existe déjà"},
{6,"pas de blanc ni de point en début de saisie"},
{7,"Erreur création ou Clef/Dossier existe déjà"},
{8,"Clef Inconnue"},
{9,"pas de blanc en début de saisie"},
{10,"Code taxe inconnu"},
{11,"Tableau plein ! "},
{12,"Code calcul en double\n un seul possible"},
{13,"Erreur de date"},
{14,"Erreur sur montant: ne peut pas être nul\nsi décimales il en faut 2 maxi"},
{15,"Problème Mémoire/n Le nombre de lignes de facture est trop grand"},
{16,"Fichier Mise en page Impression absent ou erreur lecture"},
{17,"La ligne cible est indéterminée\nActiver la bonne ligne par un clic"},
{18,"Fichier Mise en page\nNombre de Police trop grand > 20"},
{19,"Tableau plein. Le fichier est tronqué\nModifier nombre de lignes (Paramètres)"},
{20,"Non fichier a éditer/écrire absent "},
{21,"Tableau non vide !\nVoulez vous vraiment l'effacer ?"},
{22,"L'édition est-t-elle correcte ?\n \
  Si oui imprimez le si besoin à partir de l'affichage\n \
  Voulez-vous SAUVER le fichier dans le répertoire (histo-fac)"},
{23,"Pas de Dossier ouvert !"},
{24,"On change Date Facture en date du jour ?"},
{25,"Fermer affichage du fichier 'factest.pdf'\nou Erreur création fichier pdf"},
{26,"ERREUR Fichier Mise en page Paramètres absents ou erronés"},
{52," "}
}; 
gboolean message(int num)
{
GtkWidget *pAbout;
//pAbout = gtk_message_dialog_new (GTK_WINDOW(winp),
	gdk_beep();
pAbout = gtk_message_dialog_new (NULL,
   GTK_DIALOG_MODAL,
   GTK_MESSAGE_INFO,
   GTK_BUTTONS_OK, //CLOSE, //OK,
   "A CORRIGER\n%s",
   tlmes[num].messag);
    /* Affichage de la boite de message */
gtk_dialog_run(GTK_DIALOG(pAbout));
gtk_widget_destroy(pAbout);
return FALSE;
}

gboolean messavar(char* vmes)
{
GtkWidget *pAbout;
//pAbout = gtk_message_dialog_new (GTK_WINDOW(winp),
	gdk_beep();
pAbout = gtk_message_dialog_new (NULL,
   GTK_DIALOG_MODAL,
   GTK_MESSAGE_INFO,
   GTK_BUTTONS_OK, //CLOSE, //OK,
   "INFO\n%s",
   vmes);
    /* Affichage de la boite de message */
gtk_dialog_run(GTK_DIALOG(pAbout));
gtk_widget_destroy(pAbout);
return FALSE;
}

int question(int num)
{
GtkWidget *pDialog;
int ret=0;
	gdk_beep();
pDialog = gtk_message_dialog_new(NULL, 
	GTK_DIALOG_MODAL,
	GTK_MESSAGE_QUESTION,
	GTK_BUTTONS_YES_NO,
	"Que faire ?\n%s",
	tlmes[num].messag);
switch(gtk_dialog_run(GTK_DIALOG(pDialog)))
{
case GTK_RESPONSE_YES:
 ret = 0;  
   break;
 case GTK_RESPONSE_NO:
 ret = -1;  
   break;
 default:
 ret = -1;  
   break;
    }
gtk_widget_destroy(pDialog);
return ret;

}

