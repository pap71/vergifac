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
#include <math.h>
#include "genfen.h"
#include "fast.h"

void open_ficsauv();
void open_ficrelit();
void sauvd();
void relitd();

extern S_SENF senf;
extern S_LFAC *ligfac;
extern GtkWidget *cbtypdoc;
extern int derlig;
extern int CSFnli;

char ficsauvdoc[160];
static FILE *fiout = NULL;

static char zout[256];
static int lzout;

gboolean sauvdoc(GtkWidget *widget, gpointer data)
{
ficsauvdoc[0] = '\0';
open_ficsauv();
if (ficsauvdoc[0] != '\0')	{
  sauvd();
  affi_doclu();
  fclose(fiout); fiout = NULL;
				}
return FALSE;
}

void open_ficsauv()
{
GtkWidget *dialog;//,*diames;
gchar *filename;
int ll;
//GtkFileFilter *filter = gtk_file_filter_new ();
//gtk_file_filter_add_pattern (filter, "*.txt");

dialog = gtk_file_chooser_dialog_new ("Saisir un Nom pour le document à sauver",
  GTK_WINDOW(NULL),
  GTK_FILE_CHOOSER_ACTION_SAVE,
  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
  GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
  NULL);
gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER (dialog),"*.txt");
gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog),"./memo-doc");
//gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);
gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)  {
  filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
  fiout=fopen(filename,"w");
  ll = strlen(filename);
  if ( ll > 160)
   printf("pb size filename %s %d\n",filename,ll);
  strcpy(ficsauvdoc,filename);
  g_free (filename);
  								}
gtk_widget_destroy (dialog);
}

#define PVI(qq) charreplace(qq,';',':')

void sauvd()
{
char zt1[16] = "               \0";
char zt2[16] = "               \0";
int cbi,lf;
char typdoc;
 cbi = gtk_combo_box_get_active(GTK_COMBO_BOX(cbtypdoc));
 typdoc= '0' + cbi;
	// ignore date
 sprintf(zout,"%c;%s;%s;\n",
  typdoc,senf.numfac,senf.clecli);
 fwrite(zout,1,strlen(zout),fiout);
  // si ; dans texte !
 PVI(senf.add1);
 PVI(senf.add2);
 PVI(senf.add3);
 PVI(senf.pays);
 PVI(senf.codpos);
 PVI(senf.add4);
 sprintf(zout,"%s;%s;%s;%s;%s;%s;\n",
  senf.add1,senf.add2,senf.add3,senf.pays,senf.codpos,senf.add4);
 fwrite(zout,1,strlen(zout),fiout);

for ( lf=0; lf < derlig; ++lf)	{
 PVI(ligfac[lf].codprod);
 PVI(ligfac[lf].libprod);
 PVI(ligfac[lf].unit);
 	// sauve avec deci car pb si gros montants
  ltof(ligfac[lf].priunit,zt1,14);
  ltof(ligfac[lf].valor,zt2,14);
// sprintf(zout,"%s;%s;%f;%s;%s;%ld;%ld;%d;\n",
 sprintf(zout,"%s;%s;%f;%s;%s;%s;%s;%d;\n",
  ligfac[lf].codprod,ligfac[lf].libprod,ligfac[lf].quant,ligfac[lf].unit,
  ligfac[lf].codtax,zt1,zt2,ligfac[lf].typlig);
 fwrite(zout,1,strlen(zout),fiout);
				}
}

void relitdoc()
{
ficsauvdoc[0] = '\0';
open_ficrelit();
if (ficsauvdoc[0] != '\0')	{
  relitd();
  affi_doclu();
  fclose(fiout); fiout = NULL;
				}
}

void open_ficrelit()
{
GtkWidget *dialog,*diames;
char *filename;
GtkFileFilter *filter = gtk_file_filter_new ();
gtk_file_filter_add_pattern (filter, "*.txt");

dialog = gtk_file_chooser_dialog_new ("Choix du Fichier à charger",
  GTK_WINDOW(NULL),
  GTK_FILE_CHOOSER_ACTION_OPEN,
  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
  GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
  NULL);
gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog),"./memo-doc");
gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);

if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)  {
  filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
  fiout=fopen(filename,"r");
  if ( fiout == NULL )	{
   printf("Erreur d'ouverture du fichier %s ",filename);
   diames = gtk_message_dialog_new(GTK_WINDOW(NULL),
   GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
   "Impossible d'ouvrir le fichier : \n%s",filename);
   gtk_dialog_run(GTK_DIALOG(diames));
   gtk_widget_destroy(diames);
		  }
 else	  strcpy(ficsauvdoc,filename);
  g_free (filename);
  								}
gtk_widget_destroy (dialog);
}

int isol(int nbl)
{
 while ( nbl < lzout && zout[nbl] != ';') ++nbl;
 zout[nbl] = '\0';
return ++nbl;
}

#define DECOT(ww)  nbv = nbl; nbl=isol(nbl); strcpy(ww,&zout[nbv]);

void relitd()
{
int nbl,nbv,nl,cbi,lf;
nl=0;	lf=0;
while ( fgets(zout,256,fiout) != NULL && !feof(fiout))	{
 ++nl;
 lzout=strlen(zout);
 nbl = 0;
if ( lf >= CSFnli)	{
  message (19);
  derlig = --lf;
  return;
			}
if (nl > 2)	{
  DECOT(ligfac[lf].codprod);
  DECOT(ligfac[lf].libprod);
  nbv =nbl;	nbl=isol(nbl);
  ligfac[lf].quant = g_strtod(&zout[nbv],NULL);

  DECOT(ligfac[lf].unit);
  DECOT(ligfac[lf].codtax);
  nbv =nbl;	nbl=isol(nbl);
  if ( zout[nbv] == '\0') ligfac[lf].priunit = 0;
  else	{
    mdctoi(&zout[nbv]);
    ligfac[lf].priunit = zslon;
	}
  nbv =nbl;	nbl=isol(nbl);
  if ( zout[nbv] == '\0') ligfac[lf].valor = 0;
  else	{
    mdctoi(&zout[nbv]);
    ligfac[lf].valor = zslon;
	}
  nbv = nbl;
  nbl=isol(nbl);
  ligfac[lf].typlig = atoi(&zout[nbv]);
  ++lf;
  continue;
		}
else if (nl == 1)	{
  //typdoc
  nbv = nbl;
  nbl=isol(nbl);
  cbi = atoi(&zout[nbv]);
  gtk_combo_box_set_active(GTK_COMBO_BOX(cbtypdoc), cbi);
  DECOT(senf.numfac);
  DECOT(senf.clecli);
		}
else if (nl == 2)	{
  DECOT(senf.add1);
  DECOT(senf.add2);
  DECOT(senf.add3);
  DECOT(senf.pays);
  DECOT(senf.codpos);
  DECOT(senf.add4);
			}
							}
derlig = lf;
}
