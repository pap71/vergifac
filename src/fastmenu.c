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
#include <sqlite3.h>
#include <string.h>
#include "genfen.h"
#include "fast.h"
#ifdef WIN32
#include <windows.h>
#include <shellapi.h>
#endif

void inifen ();
GtkWidget *menup ();
void act_sfcli (GtkWidget *widget, gpointer data);
void act_db (GtkButton *boutopen, gpointer data);
void act_licli (GtkWidget *widget, gpointer data);
void act_liprod (GtkWidget *widget, gpointer data);
void act_sfprod (GtkWidget *widget, gpointer data);
void cbparam_sfac(GtkWidget *widget, gpointer data);

/* Permet de retrouver un widget inclus dans le widget transmis. Il faut pour
 * cela que les widget inclus portent un nom. Pour se fair il suffit lors de
 * leur création de leur affecter un nom via la fonction gtk_widget_set_name();
 *
 * Si aucun widget n'est trouvé la fonction retourne NULL. Il en va de même si
 * le widget transmis n'est pas valide.
 */
GtkWidget *get_widget_from_window (GtkWidget *winp, const gchar *name);

extern FENQ fen_sfcli;
extern void* sfcli; 
extern FENQ fen_sfprod;
extern void* sfprod; 
extern sqlite3 *db;
extern GtkWidget* winli;
extern GtkWidget* winliprod;
extern FnC  fen_sfac;
extern FCBOX  fen_taux;
extern FENQ fen_para;
extern void* para; 

/* GtkWidget *winp; */	// globale pour messages
/* GtkWidget *cbparam; */
extern S_DAT da;
S_DAT today;
char nomdb[80];

/* static GtkWidget *boutsfac; */
/* static GtkWidget *boutsfcli; */
/* static GtkWidget *boutsprod; */
/* static GtkWidget *boutliprod; */
/* static GtkWidget *boutopen; */
/* static GtkWidget *boutlicli; */
/* static GtkWidget *labdb; */

static void
callback_find_widget (GtkWidget *widget, gpointer data)
{
  /* Création d'une structure interne pour pouvoir récupérer les différents
   * widgets de l'interface mais aussi le résultat de la comparaison des noms.
   */
  typedef struct
  {
    GtkWidget *widget;
    const gchar *name;
    gboolean resultcomp;
    GtkWidget *ptrfind;
  } Sresult;
  
  Sresult *result = (Sresult*)data;

  if (g_strcmp0 (gtk_widget_get_name (widget), result->name)==0)
    {
      result->resultcomp = TRUE;
      result->ptrfind = widget;
    }

  result->widget = widget;
}

GtkWidget *
get_widget_from_window (GtkWidget *winp, const gchar *name)
{
  /* Retourne NULL si le widget n'est pas valide */
  g_return_val_if_fail (GTK_IS_WIDGET (winp), NULL);
  /* Retourne NULL si name = NULL */
  g_return_val_if_fail (name!=NULL, NULL);

  /* Création d'uns structure interne pour pouvoir récupérer les différents
   * widgets de l'interface mais aussi le résultat de la comparaison des noms.
   */
  typedef struct
  {
    GtkWidget *widget;
    const gchar *name;
    gboolean resultcomp;
    GtkWidget *ptrfind;
  } Sresult;
  
  Sresult result;
  GtkWidget *widget = NULL;

  /* Initialisation de result */
  result.widget = winp;
  result.name = name;
  result.resultcomp = FALSE;
  widget = winp;
  while (1)
    {
      if (GTK_IS_CONTAINER (widget))
	gtk_container_foreach (GTK_CONTAINER (widget),(GtkCallback)callback_find_widget, &result);

      if (result.resultcomp == TRUE)
	return result.ptrfind;
      else
	{
	  /* Permet de sortir de la boucle lorsque tous les widgets ont été parcourus */
	  if (widget==result.widget)
	    break;
	  else
	    widget = result.widget;
	}
    }

  return NULL;
}

void deactiv_bmenu(GtkWidget *winp)
{
  gtk_widget_set_sensitive(get_widget_from_window (winp, "boutsfac"), FALSE);
  gtk_widget_set_sensitive(get_widget_from_window (winp, "boutlicli"), FALSE);
  gtk_widget_set_sensitive(get_widget_from_window (winp, "boutsfcli"), FALSE);
  gtk_widget_set_sensitive(get_widget_from_window (winp, "boutsprod"), FALSE);
  gtk_widget_set_sensitive(get_widget_from_window (winp, "boutliprod"), FALSE);
  gtk_widget_set_sensitive(get_widget_from_window (winp, "cbparam"), FALSE);
  /* gtk_widget_set_sensitive(boutlicli,FALSE); */
  /* gtk_widget_set_sensitive(boutsfcli,FALSE); */
  /* gtk_widget_set_sensitive(boutsprod,FALSE); */
  /* gtk_widget_set_sensitive(boutliprod,FALSE); */
  /* gtk_widget_set_sensitive(cbparam,FALSE); */
}

void activ_bmenu(GtkWidget *winp)
{
  gtk_widget_set_sensitive(get_widget_from_window (winp, "boutsfac"), TRUE);
  gtk_widget_set_sensitive(get_widget_from_window (winp, "boutlicli"), TRUE);
  gtk_widget_set_sensitive(get_widget_from_window (winp, "boutsfcli"), TRUE);
  gtk_widget_set_sensitive(get_widget_from_window (winp, "boutsprod"), TRUE);
  gtk_widget_set_sensitive(get_widget_from_window (winp, "boutliprod"), TRUE);
  gtk_widget_set_sensitive(get_widget_from_window (winp, "cbparam"), TRUE);
  /* gtk_widget_set_sensitive(boutlicli,TRUE); */
  /* gtk_widget_set_sensitive(boutsfcli,TRUE); */
  /* gtk_widget_set_sensitive(boutsprod,TRUE); */
  /* gtk_widget_set_sensitive(boutliprod,TRUE); */
  /* gtk_widget_set_sensitive(cbparam,TRUE); */
}

void inifen(GtkWidget *winp)
{
  activ_bmenu(winp);
  inidatej();
  memcpy((char*)&today,(char*)&da,sizeof(S_DAT));
  lidb_taux();
  /*crefen_listcli();
    act_fenPara();*/
}

void act_sfac(GtkWidget *widget, gpointer data)
{
  if ( !GTK_IS_WINDOW (fen_sfac.win) )	init_sfac();
  gtk_window_deiconify((GtkWindow*)fen_sfac.win);
  gtk_widget_show_all(fen_sfac.win);
}

/*
  void act_para(GtkButton *boutpara, gpointer data)
  {
  act_fenPara();
  }

  void act_fenPara()
  {
  if ( !GTK_IS_WINDOW (fen_para.win)) {
  crefenq ((FENQ*) &fen_para, (void*) &para);
  }
  gtk_window_deiconify((GtkWindow*)fen_para.win);
  affi_para();
  gtk_widget_show_all(fen_para.win);
  }
*/

void act_liprod(GtkWidget *widget, gpointer data)
{
  if ( !GTK_IS_WINDOW (winliprod)) {
    crefen_listprod();
  }
  gtk_window_deiconify((GtkWindow*)winliprod);
  gtk_widget_show_all(winliprod);
}

void act_sfprod(GtkWidget *widget, gpointer data)
{
  if ( !GTK_IS_WINDOW (fen_sfprod.win)) {
    crefenq ((FENQ*) &fen_sfprod, (void*) &sfprod);
    deactiv_sfprod();
    clear_sfprod();
  }
  gtk_window_deiconify((GtkWindow*)fen_sfprod.win);
  gtk_widget_show_all(fen_sfprod.win);
}

void act_licli(GtkWidget *widget, gpointer data)
{
  if ( !GTK_IS_WINDOW (winli)) {
    crefen_listcli();
  }
  gtk_window_deiconify((GtkWindow*)winli);
  gtk_widget_show_all(winli);
}

void act_sfcli(GtkWidget *widget, gpointer data)
{
  if ( !GTK_IS_WINDOW (fen_sfcli.win)) {
    crefenq ((FENQ*) &fen_sfcli, (void*) &sfcli);
    deactiv_sfcli();
    raz_cms();
  }
  gtk_window_deiconify((GtkWindow*)fen_sfcli.win);
  gtk_widget_show_all(fen_sfcli.win);
}

void cbparam_sfac(GtkWidget *widget, gpointer data)
{
  int cbi;
  //gchar *text;
  cbi = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
  // text = gtk_combo_box_get_active_text((GtkComboBox*)widget);
  //printf("Param = %d %s\n", cbi,text);
  //g_free(text);
  if ( cbi == 1)	{			//  taux tva
    if ( !GTK_IS_WINDOW (fen_taux.win))	{
      cre_fcbox((FCBOX*) &fen_taux);
      initcbox_taux((GtkWidget*) fen_taux.cbox);
    }
    gtk_window_deiconify((GtkWindow*)fen_taux.win);
    gtk_widget_show_all(fen_taux.win);
  }
  else if ( cbi == 2)	{
    if ( !GTK_IS_WINDOW (fen_para.win))	{
      crefenq ((FENQ*) &fen_para, (void*) &para);
    }
    gtk_window_deiconify((GtkWindow*)fen_para.win);
    affi_para();
    gtk_widget_show_all(fen_para.win);
  }
  else if ( cbi == 3)	{
    crenewdb();
  }
  gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);
}

static char nltit[]="CREATION d'un nouveau dossier (base de données)\nChoisir un nom (15 car max) +'.db'";
static char nlini[]=".db";

void crenewdb(GtkWidget *winp)
{
  //cp ./outils/credbini dbdb
  char ss[20],cdsys[80];
  int rc;
  if ( dialogtext(winp, "Creation Dossier",nltit,nlini,ss) == 0)	{
    // printf("new db %s\n",ss);
    sqlite3_close(db);
    deactiv_bmenu(winp);
    // test si existe
    rc = sqlite3_open(ss, &db);
    if   ( rc )	{
      message(7);
      sqlite3_close(db);
    }
    else	{	// n'existe pas
#ifdef WIN32
      sprintf(cdsys,"copy .\\outils\\initdb.db %s",ss);
#else
      sprintf(cdsys,"cp ./outils/initdb.db %s",ss);
#endif
      system(cdsys);
    } 
  }
  //free(ss);
}

void act_db(GtkButton *boutopen, gpointer data)
{
  GtkWidget *winp = (GtkWidget*) data;
  GtkWidget *dialog,*diames;
  char *filename;
  int rc;
  GtkFileFilter *filter = gtk_file_filter_new ();
  gtk_file_filter_add_pattern (filter, "*.db");
  //gtk_file_filter_add_pattern (filter, "*.sq*");
  dialog = gtk_file_chooser_dialog_new("Choix de dossiers",
				       GTK_WINDOW(winp),
				       GTK_FILE_CHOOSER_ACTION_OPEN,
				       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				       GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				       NULL);
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog),"./");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)   {
    sqlite3_close(db);
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    rc = sqlite3_open(filename, &db);
    if   ( rc )	{
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      deactiv_bmenu(winp);
      diames = gtk_message_dialog_new(GTK_WINDOW(winp),
				      GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
				      "Impossible d'ouvrir le fichier : \n%s",filename);
      gtk_dialog_run(GTK_DIALOG(diames));
      gtk_widget_destroy(diames);
      //  return ;//exit(1);
    }
    else	{
      strncpy(nomdb,filename,80);
      char *pnf = NULL;
      //  pnf = strrchr (nomdb, '/');
      pnf = strrchr (nomdb, SLAF);
      if ( pnf != NULL)  
	gtk_label_set_text(GTK_LABEL(get_widget_from_window (winp, "labdb")),pnf+1);
      inifen(winp);
    }
    g_free (filename);
  }
  gtk_widget_destroy (dialog);
}

void act_manuel(GtkWidget *widget, gpointer data)
{
#ifdef WIN32
  ShellExecute(0,"open",".\\doc-aide\\ManuelVERGIFAC.html",0,0,SW_SHOWNORMAL);
#else
  system("xdg-open ./doc-aide/ManuelVERGIFAC.html&");
#endif
}

gboolean winp_quit(GtkWidget *widget, gpointer data)
{
  if ( GTK_IS_WINDOW (fen_sfac.win) )	{
    if  (sfac_quit(fen_sfac.win, NULL) == TRUE)	{
      gtk_window_deiconify((GtkWindow*)fen_sfac.win);
      gtk_widget_show_all(fen_sfac.win);
      return TRUE;	// reste lignes on return
    }
  }
  gtk_main_quit();
  return FALSE;
}

GtkWidget *menup()  //    Menu principal
{
  GtkWidget *winp;
  GtkWidget *frawp;
  /* GtkWidget *wman; */
  GtkWidget *button = NULL;
  int py;
  winp = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(winp), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(winp), 150, 300);
  gtk_window_move(GTK_WINDOW(winp), 0, 700);
  gtk_window_set_title(GTK_WINDOW(winp), "Menu Vergifac");
	
  // frame fen principale
  frawp = gtk_fixed_new ();
  gtk_container_add(GTK_CONTAINER(winp), frawp);

  py=5; 
  button = gtk_button_new_with_label("Saisie Facture");
  gtk_widget_set_size_request(button, 120, 25);
  gtk_fixed_put(GTK_FIXED(frawp), button,20,py);
  gtk_widget_set_name (button, "boutsfac");
  g_signal_connect(button, "clicked", G_CALLBACK(act_sfac), NULL);
  py += 30;
  button = gtk_button_new_with_label("Saisie Produits");
  gtk_widget_set_size_request(button, 120, 25);
  gtk_fixed_put(GTK_FIXED(frawp), button, 5,py);
  gtk_widget_set_name (button, "boutsprod");
  g_signal_connect(button, "clicked", G_CALLBACK(act_sfprod), NULL);
  py += 30;
  button = gtk_button_new_with_label("Liste Produits");
  gtk_widget_set_size_request(button, 120, 25);
  gtk_fixed_put(GTK_FIXED(frawp), button,20,py);
  gtk_widget_set_name (button, "boutliprod");
  g_signal_connect(button, "clicked", G_CALLBACK(act_liprod), NULL);
  py += 30;
  button = gtk_button_new_with_label("Saisie Clients");
  gtk_widget_set_size_request(button, 120, 25);
  gtk_fixed_put(GTK_FIXED(frawp), button, 5,py);
  gtk_widget_set_name (button, "boutsfcli");
  g_signal_connect(button, "clicked", G_CALLBACK(act_sfcli), NULL);
  py += 30;
  button = gtk_button_new_with_label("Liste Clients");
  gtk_widget_set_size_request(button, 120, 25);
  gtk_fixed_put(GTK_FIXED(frawp), button,20,py);
  gtk_widget_set_name (button, "boutlicli");
  g_signal_connect(button, "clicked", G_CALLBACK(act_licli), NULL);
  py += 30;
  // cbparam = gtk_combo_box_text_new();  // creation cbox ici
  button = gtk_combo_box_new_text();  // creation cbox ici
  gtk_fixed_put(GTK_FIXED(frawp), button, 5,py);
  gtk_widget_set_name (button, "cbparam");
  gtk_combo_box_append_text(GTK_COMBO_BOX(button), "Parametres");
  gtk_combo_box_append_text(GTK_COMBO_BOX(button), "Taux T.V.A");
  gtk_combo_box_append_text(GTK_COMBO_BOX(button), "Parametres divers");
  gtk_combo_box_append_text(GTK_COMBO_BOX(button), "Creation Dossier");
  gtk_combo_box_set_active(GTK_COMBO_BOX(button), 0);
  g_signal_connect(button, "changed", G_CALLBACK(cbparam_sfac), NULL);

  py += 60;
  button = gtk_button_new_with_label("Ouvrir un Dossier");
  gtk_widget_set_size_request(button, 140, 25);
  gtk_fixed_put(GTK_FIXED(frawp), button, 5,py);
  g_signal_connect(button, "clicked", G_CALLBACK(act_db), winp);
  py += 30;
  button = gtk_label_new(NULL); 
  gtk_fixed_put(GTK_FIXED(frawp), button, 20,py);
  gtk_widget_set_name (button, "labdb");
  py += 25;
  button = gtk_button_new_with_label("Manuel d'aide");
  gtk_widget_set_size_request(button, 120, 25);
  gtk_fixed_put(GTK_FIXED(frawp), button,20,py);
  g_signal_connect(button, "clicked", G_CALLBACK(act_manuel), NULL);

  gtk_widget_show_all(winp);
  //  g_signal_connect(winp, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  /* g_signal_connect(boutsfcli, "clicked", G_CALLBACK(act_sfcli), NULL); */
  /* g_signal_connect(boutsprod, "clicked", G_CALLBACK(act_sfprod), NULL); */
  /* g_signal_connect(boutliprod, "clicked", G_CALLBACK(act_liprod), NULL); */
  /* g_signal_connect(boutopen, "clicked", G_CALLBACK(act_db), NULL); */
  /* g_signal_connect(boutlicli, "clicked", G_CALLBACK(act_licli), NULL); */
  /* g_signal_connect(wman, "clicked", G_CALLBACK(act_manuel), NULL); */
  /* g_signal_connect(boutsfac, "clicked", G_CALLBACK(act_sfac), NULL); */
  /* g_signal_connect(cbparam, "changed", G_CALLBACK(cbparam_sfac), NULL); */

  g_signal_connect(winp, "delete_event", G_CALLBACK (winp_quit), NULL);

  return winp;
}

int main(int argc, char** argv)
{
  GtkWidget *winp = NULL;
  gtk_init(&argc, &argv);
  winp = menup();
  deactiv_bmenu(winp);
  gtk_main();
  sqlite3_close(db);
  return 0;
}

