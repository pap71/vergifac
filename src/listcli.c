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
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "genfen.h"
#include "fast.h"

void listcli ();
void selection_cli(GtkTreeSelection *selection, gpointer data);
void chang_sel (GtkEntry *widls);
gboolean quit_winli(GtkWidget *widget, gpointer data);

extern FnC  fen_sfac;

GtkWidget *winli;
GtkWidget *cli_clist;
GtkListStore *cli_listor;
static GtkWidget *cb_rech1;
static GtkWidget *val_rech1;
static GtkWidget *cb_rech2;
static GtkWidget *val_rech2;
//GtkTreeIter pIter_cli;
char zx[256],zy[128];
static char sn2 [12];
#ifdef WIN32
static int inhibsel;    // vers windows inhibe sel crefen_cli
# endif

enum {		// =0,1,2,etc
    CLECLI_C,
    TYPCLI_C,
    ADD1_C,
    ADD2_C,
    ADD3_C,
    PAYS_C,
    CODPOS_C,
    ADD4_C,
    NCOL_C
};

static DEF_C_LIST   col_cplist[]= {
//nucol,typ,laf,titcol,
{CLECLI_C,LIBCHAR,11,'g',"Clef.Client"},
{TYPCLI_C,LIBCHAR,2,'g',"Typ"},
{ADD1_C,LIBCHAR,30,'g',"     Add1    "},
{ADD2_C,LIBCHAR,28,'g',"     Add2    "},
{ADD3_C,LIBCHAR,10,'g',"     Add3    "},
{PAYS_C,LIBCHAR,4,'g',"Pays"},
{CODPOS_C,LIBCHAR,4,'g',"Cod.Post"},
{ADD4_C,LIBCHAR,20,'g',"     Add4/Ville"},
};

static char sqlist[]="SELECT Clefacc,Type,Add1,Add2,Add3,Pays,Codpos,Add4 FROM Cli ORDER BY %s";
static char sqlrech_cli[]="SELECT Clefacc,Type,Add1,Add2,Add3,Pays,Codpos,Add4  FROM Cli  WHERE %s LIKE '%s%%' ORDER BY Clefacc";

void listcli()
{
if (cli_listor != NULL) gtk_list_store_clear((GtkListStore*) cli_listor);
 sprintf(zx,sqlist,"Clefacc");
execdb(zx,'l');
}

void rechcli( char* nup)
{
char zt[12];
if (cli_listor != NULL) gtk_list_store_clear((GtkListStore*) cli_listor);
strcpy(zt,nup);
sprintf(zx,sqlrech_cli,"Clefacc",zt);
//printf("rech zx= %s\n",zx);
execdb(zx,'l');
//printf("%s\n",(char*)testmem);
}


void versfac_cli(GtkWidget *menuitem, gpointer userdata)
{
GtkTreeSelection *select;
GtkTreeIter iter;
GtkTreeModel *model;
gchar *text;
GtkTreeView *treeview = GTK_TREE_VIEW(userdata);
select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
if (gtk_tree_selection_get_selected (select, &model, &iter))  {
  gtk_tree_model_get (model, &iter, CLECLI_C, &text, -1);
 if ( GTK_IS_WINDOW (fen_sfac.win) )	afadres_cli(text);
//     g_print ("Vers Facture   %s\n", text);
     g_free (text);
        }
}

void affif_cli (GtkWidget *menuitem, gpointer userdata)
{
GtkTreeSelection *select;
GtkTreeIter iter;
GtkTreeModel *model;
gchar *text;
    /*  userdata = treeview dans le signal */
GtkTreeView *treeview = GTK_TREE_VIEW(userdata);
select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
if (gtk_tree_selection_get_selected (select, &model, &iter))  {
 gtk_tree_model_get (model, &iter, CLECLI_C, &text, -1);
 acces_sfcli(text);
 g_free(text);
        }
}

void popupmenu_cli(GtkWidget *treeview,GdkEventButton *event,gpointer userdata)
{
GtkWidget *menu, *menuitem;
menu = gtk_menu_new();
 menuitem = gtk_menu_item_new_with_label("Charge adresse facturation");
g_signal_connect(menuitem, "activate",(GCallback) versfac_cli,treeview);
gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
 menuitem = gtk_menu_item_new_with_label("Active la Fiche");
g_signal_connect(menuitem, "activate",(GCallback) affif_cli, treeview);
gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
    gtk_widget_show_all(menu);
    /* Note: event can be NULL here when called from view_onPopupMenu;
     *  gdk_event_get_time() accepts a NULL argument */
gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
    (event != NULL) ? event->button : 0,
    gdk_event_get_time((GdkEvent*)event));
}

gboolean  popupsignal_cli(GtkWidget *treeview, gpointer userdata)
{
popupmenu_cli(treeview, NULL, userdata);
return TRUE; /* we handled this */
}

gboolean butpress_cli(GtkWidget *treeview,GdkEventButton *event, gpointer userdata)
{
	// simple click souris
//if (event->type == GDK_BUTTON_PRESS  &&
// (event->button == 1 || event->button == 3))	{
if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3)	{
 popupmenu_cli(treeview, event, userdata);
return TRUE; /* we handled this */
								}
/*else if (event->type == GDK_BUTTON_PRESS  &&  event->button == 1)    {
  g_print ("click simple gauche\n");
  return FALSE; // we did not handle this
								     }*/
else  return FALSE; // we did not handle this
}

void chang_sel(GtkEntry *widls)
{
GtkEditable *editable;
gchar *text;
int lt;
editable = GTK_EDITABLE (widls);
text = gtk_editable_get_chars (editable, 0, -1);
strcpy(sn2,text);
lt = strlen(sn2);
//if ( lt ==0) return;
if (lt > 0 && lt < 5 && *(sn2) != ' ') 	{
  rechcli((char*)sn2);
				}
g_free(text);
}

void new_sel(GtkWidget *widget, GtkWidget *ckw)
{
gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ckw), FALSE);
 if (cli_listor != NULL) gtk_list_store_clear((GtkListStore*) cli_listor);
}

static char sqlrech_db[]="SELECT Clefacc,Type,Add1,Add2,Add3,Pays,Codpos,Add4  FROM Cli  WHERE %s %s '%s' %s ORDER BY Clefacc";
static char sq4[]="AND %s %s '%s' ";
static char sq1[12],sq2[6],sq3[40];
static char sq5[12],sq6[6],sq7[40];

void db_select(GtkWidget *widget, gpointer ww)
{
const gchar *val1,*val2;
gchar *rub1, *rub2;
if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
 if (cli_listor != NULL) gtk_list_store_clear((GtkListStore*) cli_listor);
 rub1 = gtk_combo_box_get_active_text((GtkComboBox*)cb_rech1);
 val1 =  gtk_entry_get_text((GtkEntry*) val_rech1);
 rub2 = gtk_combo_box_get_active_text((GtkComboBox*)cb_rech2);
 val2 =  gtk_entry_get_text((GtkEntry*) val_rech2);
// if ( rub1 == NULL || strcmp(rub1,"---") == 0)	return;  modif 27:9:2015
 if ( rub1 == NULL )	return;
 if ( strcmp(rub1,"***") == 0)  {
    listcli();
 	return;                     }
 strcpy(sq1,rub1);
 if ( strcmp(rub1,"Pays") == 0)	{
  strcpy(sq2,"=");
  strcpy(sq3,val1);
				}
 else if ( strcmp(rub1,"Codpos") == 0)	{
  strcpy(sq2,"LIKE");
  strcpy(sq3,val1);
  strcat(sq3,"%");
				}
 else	{
  strcpy(sq2,"LIKE");
  sq3[0] = '%';
  strcpy(&sq3[1],val1);
  strcat(sq3,"%");
	}
 if ( rub2 == NULL || strcmp(rub2,"***") == 0)	{
  sprintf(zx,sqlrech_db,sq1,sq2,sq3," ");	// 1 selection
						}
 else	{
 strcpy(sq5,rub2);
 if ( strcmp(rub2,"Pays") == 0)	{
  strcpy(sq6," = ");
  strcpy(sq7,val2);
				}
 else if ( strcmp(rub2,"Codpos") == 0)	{
  strcpy(sq6,"LIKE");
  strcpy(sq7,val2);
  strcat(sq7,"%");
				}
 else	{
  strcpy(sq6,"LIKE");
  sq7[0] = '%';
  strcpy(&sq7[1],val2);
  strcat(sq7,"%");
	}
  sprintf(zy,sq4,sq5,sq6,sq7);
  sprintf(zx,sqlrech_db,sq1,sq2,sq3,zy);	// 2 selections
	}
// printf("redb zx= %s\n",zx);
 execdb(zx,'l');
 g_free(rub1);	g_free(rub2);
							     }

}

static char *rubdb[]={
"***",
"Type",
"Add1",
"Add2",
"Add3",
"Pays",
"Codpos",
"Add4",
"Datcre",
"Datmod",
"Tel",
"Mail",
"Notes",
NULL
};

void crefen_listcli()	/* creation fenetre list */
{
GtkWidget *pScrollbar;
GtkWidget *vbox, *hbox, *hbox2;
//GtkWidget *b_list;//, *b_efface, *b_titre;
GtkWidget *ck_sel;
GtkWidget *clef_sel;
GType typc[NCOL_C];
int nc,i,j;
GtkTreeViewColumn *pColumn;
GtkCellRenderer *pCellRenderer;
GtkWidget *lisview;
GtkTreeSelection *select;

winli = gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_move(GTK_WINDOW(winli), 20, 80);
gtk_window_set_default_size(GTK_WINDOW(winli), 230,400);
gtk_window_set_title(GTK_WINDOW(winli), "LISTE CLIENT");

vbox=gtk_vbox_new(FALSE, 5);
gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
/* GtkVBox container */
gtk_container_add(GTK_CONTAINER(winli), vbox);
//gtk_widget_show(vbox);

	// selection
hbox = gtk_hbox_new(FALSE, 0);
gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
ck_sel = gtk_check_button_new_with_label("Selection");
//b_list = gtk_button_new_with_label("Tous les Cli");
gtk_box_pack_start(GTK_BOX(hbox), ck_sel, FALSE, FALSE, 0);
gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck_sel), FALSE);
//gtk_box_pack_start(GTK_BOX(hbox), b_list, FALSE, FALSE, 0);
//g_signal_connect(GTK_OBJECT(b_list),"clicked",GTK_SIGNAL_FUNC(listcli),NULL);
 cb_rech1 = gtk_combo_box_new_text();  // creation cbox ici
gtk_box_pack_start(GTK_BOX(hbox), cb_rech1, FALSE, FALSE, 0);
j=0;
while ( rubdb[j] != NULL)	{
 gtk_combo_box_append_text((GtkComboBox*) cb_rech1,rubdb[j++]);
				}

val_rech1 = gtk_entry_new();
 gtk_widget_set_size_request(val_rech1, 30 * DIMC , 25);
gtk_box_pack_start(GTK_BOX(hbox), val_rech1, FALSE, FALSE, 0);

hbox2 = gtk_hbox_new(FALSE, 0);
gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);

clef_sel = gtk_entry_new_with_max_length(8);
 gtk_widget_set_size_request(clef_sel, 10 * DIMC + 15, 25);
gtk_box_pack_start(GTK_BOX(hbox2), clef_sel, FALSE, FALSE, 0);
g_signal_connect(G_OBJECT(clef_sel), "changed", G_CALLBACK (chang_sel),NULL);

 cb_rech2 = gtk_combo_box_new_text();  // creation cbox ici
gtk_box_pack_start(GTK_BOX(hbox2), cb_rech2, FALSE, FALSE, 0);
j=0;
while ( rubdb[j] != NULL)	{
 gtk_combo_box_append_text((GtkComboBox*) cb_rech2,rubdb[j++]);
				}

val_rech2 = gtk_entry_new();
 gtk_widget_set_size_request(val_rech2, 30 * DIMC , 25);
gtk_box_pack_start(GTK_BOX(hbox2), val_rech2, FALSE, FALSE, 0);

g_signal_connect(GTK_OBJECT(ck_sel),"toggled", G_CALLBACK(db_select),NULL);
g_signal_connect(G_OBJECT(cb_rech1), "changed", G_CALLBACK (new_sel),ck_sel);
g_signal_connect(G_OBJECT(cb_rech2), "changed", G_CALLBACK (new_sel),ck_sel);
g_signal_connect(G_OBJECT(val_rech1), "changed", G_CALLBACK (new_sel),ck_sel);
g_signal_connect(G_OBJECT(val_rech2), "changed", G_CALLBACK (new_sel),ck_sel);

for (nc = 0; nc < NCOL_C; ++nc)	{
 if ( col_cplist[nc].typ == LIBCHAR) typc[nc] = G_TYPE_STRING;
				}
cli_listor = gtk_list_store_newv(NCOL_C, &typc[0]);

lisview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(cli_listor));
// gtk_widget_modify_font (lisview,
//  pango_font_description_from_string ("Monospace 10"));
for (i=0; i < NCOL_C; ++i)		{
 pCellRenderer = gtk_cell_renderer_text_new();
 pColumn = gtk_tree_view_column_new_with_attributes(col_cplist[i].titcol,
        pCellRenderer,  "text",col_cplist[i].nucol, NULL);
//gtk_tree_view_column_set_spacing((GtkTreeViewColumn*) pColumn,100);
// pColumn = gtk_tree_view_column_new_with_attributes(col_cplist[i].titcol,
//        pCellRenderer,"text",col_cplist[i].nucol,"rise",-10, NULL);
 gtk_tree_view_append_column(GTK_TREE_VIEW(lisview), pColumn);
 g_object_set(pCellRenderer,"width",7*col_cplist[i].laf, NULL);
// g_object_set(pCellRenderer,"height",4, NULL);
					}
pScrollbar = gtk_scrolled_window_new(NULL, NULL);
gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(pScrollbar),
GTK_POLICY_AUTOMATIC,   GTK_POLICY_ALWAYS);
gtk_container_add(GTK_CONTAINER(vbox), pScrollbar);
gtk_container_add(GTK_CONTAINER(pScrollbar), lisview);

#ifdef WIN32
inhibsel = 1;    // vers windows inhibe sel crefen_listcli
# endif

select = gtk_tree_view_get_selection (GTK_TREE_VIEW (lisview));
gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
//g_signal_connect(G_OBJECT(select),"changed",G_CALLBACK (selection_cli),  NULL);
g_signal_connect(lisview, "button-press-event",(GCallback) butpress_cli,NULL);
g_signal_connect(lisview, "popup-menu", (GCallback) popupsignal_cli, NULL);

g_signal_connect_after(G_OBJECT(winli), "destroy", G_CALLBACK (quit_winli),NULL);
//listcli();

gtk_widget_show_all(winli);
gtk_widget_grab_focus(clef_sel);
}

gboolean quit_winli(GtkWidget *widget, gpointer data)
{
winli = NULL;
return FALSE;
}

/*
void selection_cli(GtkTreeSelection *selection, gpointer data)
{
gchar *text;
//gchar *libc;
GtkTreeIter iter;
GtkTreeModel *model;
// bidouille poue empecher la selection a la creation de la fenetre (vers windows)
#ifdef WIN32
if (inhibsel == 1)  {
    inhibsel = 0;    // vers windows inhibe sel crefen_listcli
    return;         }
# endif

if (gtk_tree_selection_get_selected (selection, &model, &iter))   {
 gtk_tree_model_get (model, &iter, CLECLI_C, &text, -1);
 acces_sfcli(text);
 g_free(text);
    					    }
}

*/
