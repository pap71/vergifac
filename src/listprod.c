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

void listprod ();
//void selection_prod(GtkTreeSelection *selection, gpointer data);
void chang_selprod (GtkEntry *widls);
gboolean quit_winliprod(GtkWidget *widget, gpointer data);
//void doubcli_prod (GtkTreeView *view, GtkTreePath *path,
//         GtkTreeViewColumn *col, gpointer userdata);

GtkWidget *winliprod;
GtkWidget *prod_clist;
GtkListStore *prod_listor;
static GtkWidget *cb_rechp1;
static GtkWidget *val_rechp1;
static GtkWidget *cb_rechp2;
static GtkWidget *val_rechp2;
//GtkTreeIter pIter_prod;
char zx[256],zy[128];
static char sn2[12];
#ifdef WIN32
static int inhibsel;    // vers windows inhibe sel crefen_prod
# endif

enum {
    CLEPROD_C,
    LIBPROD_C,
    PU_C,
    CTAX_C,
    UNIT_C,
    NCOLP_C
};

static DEF_C_LIST   col_cplist[]= {
//nucol,typ,laf,titcol,
{CLEPROD_C,LIBCHAR,11,'g',"Clef.Produit"},
{LIBPROD_C,LIBCHAR,30,'g',"    Libelle   "},
{PU_C,LIBCHAR,12,'g',"Prix.Unit"},
{CTAX_C,LIBCHAR,2,'g',"Cod.Tax"},
{UNIT_C,LIBCHAR,4,'g',"Unit"},
};

static char sqlist_prod[]="SELECT Cleprod,Libprod,Priunit,Codtax,Unit FROM Prod ORDER BY %s";
static char sqlrech_prod[]="SELECT Cleprod,Libprod,Priunit,Codtax,Unit  FROM Prod  WHERE %s LIKE '%s%%' ORDER BY Cleprod";

void listprod()
{
if (prod_listor != NULL) gtk_list_store_clear((GtkListStore*) prod_listor);
 sprintf(zx,sqlist_prod,"Cleprod");
execdb(zx,'s');
}

void rechprod( char* nup)
{
char zt[12];
if (prod_listor != NULL) gtk_list_store_clear((GtkListStore*) prod_listor);
strcpy(zt,nup);
sprintf(zx,sqlrech_prod,"Cleprod",zt);
execdb(zx,'s');
//printf("rechprod %s\n",zx);
//printf("%s\n",(char*)testmem);
}

void versfac_prod(GtkWidget *menuitem, gpointer userdata)
{
GtkTreeSelection *select;
GtkTreeIter iter;
GtkTreeModel *model;
gchar *text;
GtkTreeView *treeview = GTK_TREE_VIEW(userdata);
select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
if (gtk_tree_selection_get_selected (select, &model, &iter))  {
  gtk_tree_model_get (model, &iter, CLEPROD_C, &text, -1);
  actilistprod_sfac(text);
//     g_print ("Vers Facture   %s\n", text);
     g_free (text);
        }
}

void affif_prod (GtkWidget *menuitem, gpointer userdata)
{
GtkTreeSelection *select;
GtkTreeIter iter;
GtkTreeModel *model;
gchar *text;
    /*  userdata = treeview dans le signal */
GtkTreeView *treeview = GTK_TREE_VIEW(userdata);
select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
if (gtk_tree_selection_get_selected (select, &model, &iter))  {
 gtk_tree_model_get (model, &iter, CLEPROD_C, &text, -1);
 acces_sfprod(text);
 g_free(text);
        }
}

void popupmenu_prod(GtkWidget *treeview,GdkEventButton *event,gpointer userdata)
{
GtkWidget *menu, *menuitem;
menu = gtk_menu_new();
 menuitem = gtk_menu_item_new_with_label("Vers ligne facture");
g_signal_connect(menuitem, "activate",(GCallback) versfac_prod,treeview);
gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
 menuitem = gtk_menu_item_new_with_label("Fiche Produit");
g_signal_connect(menuitem, "activate",(GCallback) affif_prod, treeview);
gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
    gtk_widget_show_all(menu);
    /* Note: event can be NULL here when called from view_onPopupMenu;
     *  gdk_event_get_time() accepts a NULL argument */
gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
    (event != NULL) ? event->button : 0,
    gdk_event_get_time((GdkEvent*)event));
}

gboolean  popupsignal_prod(GtkWidget *treeview, gpointer userdata)
{
popupmenu_prod(treeview, NULL, userdata);
return TRUE; /* we handled this */
}

gboolean butpress_prod(GtkWidget *treeview,GdkEventButton *event, gpointer userdata)
{
	// simple click droit sur souris
if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3)	{
//      g_print ("Single right click on the tree view.\n");
popupmenu_prod(treeview, event, userdata);
return TRUE; /* we handled this */
								}
/*else if (event->type == GDK_BUTTON_PRESS  &&  event->button == 1)    {
  g_print ("click simple gauche\n");
  return FALSE; // we did not handle this
								     }*/
else  return FALSE; // we did not handle this
}

void chang_selprod(GtkEntry *widls)
{
GtkEditable *editable;
gchar *text;
int lt;
editable = GTK_EDITABLE (widls);
text = gtk_editable_get_chars (editable, 0, -1);
strcpy(sn2,text);
lt = strlen(sn2);
if (lt > 0 && lt < 5 && *(sn2) != ' ') 	{
  rechprod((char*)sn2);
				}
g_free(text);
}

void new_selprod(GtkWidget *widget, GtkWidget *ckw)
{
gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ckw), FALSE);
 if (prod_listor != NULL) gtk_list_store_clear((GtkListStore*) prod_listor);
}

static char *rubdb[]={
"***",
"Libprod",
"Priunit",
"Codtax",
"Unit",
"Datcre",
"Datmod",
"Notes",
NULL
};

static char sqlrech_db[]="SELECT Cleprod,Libprod,Priunit,Codtax,Unit  FROM Prod  WHERE %s %s '%s' %s ORDER BY Cleprod";
static char sq4[]="AND %s %s '%s' ";
static char sq1[12],sq2[6],sq3[40];
static char sq5[12],sq6[6],sq7[40];

void dbprod_select(GtkWidget *widget, gpointer ww)
{
const gchar *val1,*val2;
gchar *rub1, *rub2;
if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
 if (prod_listor != NULL) gtk_list_store_clear((GtkListStore*) prod_listor);
 rub1 = gtk_combo_box_get_active_text((GtkComboBox*)cb_rechp1);
 val1 =  gtk_entry_get_text((GtkEntry*) val_rechp1);
 rub2 = gtk_combo_box_get_active_text((GtkComboBox*)cb_rechp2);
 val2 =  gtk_entry_get_text((GtkEntry*) val_rechp2);
// if ( rub1 == NULL || strcmp(rub1,"---") == 0)	return; modif 27:9:2015
 if ( rub1 == NULL)	return;
 if ( strcmp(rub1,"***") == 0)  {
    listprod();
 	return;                     }
 strcpy(sq1,rub1);
 if ( strcmp(rub1,"Unit") == 0)	{
  strcpy(sq2,"=");
  strcpy(sq3,val1);
				}
 else if ( strcmp(rub1,"Codtax") == 0)	{
  strcpy(sq2,"=");
  strcpy(sq3,val1);
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
 if ( strcmp(rub2,"Unit") == 0)	{
  strcpy(sq6," = ");
  strcpy(sq7,val2);
				}
 else if ( strcmp(rub2,"Codtax") == 0)	{
  strcpy(sq6," = ");
  strcpy(sq7,val2);
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
 execdb(zx,'s');
 g_free(rub1);	g_free(rub2);
							     }
}

void crefen_listprod()	/* creation fenetre list */
{
GtkWidget *pScrollbar;
GtkWidget *vbox, *hbox, *hbox2;
GtkWidget *ck_sel;
GtkWidget *clef_sel;
GType typc[NCOLP_C];
int nc,i,j;
GtkTreeViewColumn *pColumn;
GtkCellRenderer *pCellRenderer;
GtkWidget *lisview;
GtkTreeSelection *select;

winliprod = gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_move(GTK_WINDOW(winliprod), 0, 0);
gtk_window_set_default_size(GTK_WINDOW(winliprod), 230,400);
gtk_window_set_title(GTK_WINDOW(winliprod), "LISTE PRODUITS");

vbox=gtk_vbox_new(FALSE, 5);
gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
/* GtkVBox container */
gtk_container_add(GTK_CONTAINER(winliprod), vbox);
//gtk_widget_show(vbox);

	// selection
hbox = gtk_hbox_new(FALSE, 0);
gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
ck_sel = gtk_check_button_new_with_label("Selection");
//b_list = gtk_button_new_with_label("Tous les Prod");
gtk_box_pack_start(GTK_BOX(hbox), ck_sel, FALSE, FALSE, 0);
gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck_sel), FALSE);
//gtk_box_pack_start(GTK_BOX(hbox), b_list, FALSE, FALSE, 0);
//g_signal_connect(GTK_OBJECT(b_list),"clicked",GTK_SIGNAL_FUNC(listprod),NULL);
 cb_rechp1 = gtk_combo_box_new_text();  // creation cbox ici
gtk_box_pack_start(GTK_BOX(hbox), cb_rechp1, FALSE, FALSE, 0);
j=0;
while ( rubdb[j] != NULL)	{
 gtk_combo_box_append_text((GtkComboBox*) cb_rechp1,rubdb[j++]);
				}

val_rechp1 = gtk_entry_new();
 gtk_widget_set_size_request(val_rechp1, 30 * DIMC , 25);
gtk_box_pack_start(GTK_BOX(hbox), val_rechp1, FALSE, FALSE, 0);

hbox2 = gtk_hbox_new(FALSE, 0);
gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);

clef_sel = gtk_entry_new_with_max_length(8);
 gtk_widget_set_size_request(clef_sel, 10 * DIMC + 15, 25);
gtk_box_pack_start(GTK_BOX(hbox2), clef_sel, FALSE, FALSE, 0);
g_signal_connect(G_OBJECT(clef_sel), "changed", G_CALLBACK (chang_selprod),NULL);

 cb_rechp2 = gtk_combo_box_new_text();  // creation cbox ici
gtk_box_pack_start(GTK_BOX(hbox2), cb_rechp2, FALSE, FALSE, 0);
j=0;
while ( rubdb[j] != NULL)	{
 gtk_combo_box_append_text((GtkComboBox*) cb_rechp2,rubdb[j++]);
				}

val_rechp2 = gtk_entry_new();
 gtk_widget_set_size_request(val_rechp2, 30 * DIMC , 25);
gtk_box_pack_start(GTK_BOX(hbox2), val_rechp2, FALSE, FALSE, 0);

g_signal_connect(GTK_OBJECT(ck_sel),"toggled", G_CALLBACK(dbprod_select),NULL);
g_signal_connect(G_OBJECT(cb_rechp1), "changed", G_CALLBACK (new_selprod),ck_sel);
g_signal_connect(G_OBJECT(cb_rechp2), "changed", G_CALLBACK (new_selprod),ck_sel);
g_signal_connect(G_OBJECT(val_rechp1), "changed", G_CALLBACK (new_selprod),ck_sel);
g_signal_connect(G_OBJECT(val_rechp2), "changed", G_CALLBACK (new_selprod),ck_sel);

for (nc = 0; nc < NCOLP_C; ++nc)	{
 if ( col_cplist[nc].typ == LIBCHAR) typc[nc] = G_TYPE_STRING;
				}
prod_listor = gtk_list_store_newv(NCOLP_C, &typc[0]);

lisview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(prod_listor));
// gtk_widget_modify_font (lisview,
//  pango_font_description_from_string ("Monospace 10"));
for (i=0; i < NCOLP_C; ++i)		{
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
inhibsel = 1;    // vers windows inhibe sel crefen_listprod
# endif
//listprod();

select = gtk_tree_view_get_selection (GTK_TREE_VIEW (lisview));
gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
//g_signal_connect(G_OBJECT(select),"changed",G_CALLBACK (selection_prod),NULL);
g_signal_connect(lisview, "button-press-event",(GCallback) butpress_prod,NULL);
g_signal_connect(lisview, "popup-menu", (GCallback) popupsignal_prod, NULL);

g_signal_connect_after(G_OBJECT(winliprod), "destroy", G_CALLBACK (quit_winliprod),NULL);

gtk_widget_show_all(winliprod);
gtk_widget_grab_focus(clef_sel);
}

gboolean quit_winliprod(GtkWidget *widget, gpointer data)
{
winliprod = NULL;
return FALSE;
}

/*	double clic marche avec
	//  pb sur ligne suivante ??
//g_signal_connect(GTK_TREE_VIEW (lisview),"row-actived",G_CALLBACK (doubcli_prod),  NULL);
	//  celle ci marche !
// g_signal_connect(lisview, "row-activated", (GCallback) doubcli_prod, NULL);

void doubcli_prod (GtkTreeView *view, GtkTreePath *path,
         GtkTreeViewColumn *col, gpointer userdata)
{
GtkTreeIter   iter;
GtkTreeModel *model;
model = gtk_tree_view_get_model(view);
if (gtk_tree_model_get_iter(model, &iter, path))  {
    gchar *name;
// gtk_tree_model_get(model, &iter, col, &name, -1); plante erreur sur col
gtk_tree_model_get(model, &iter, CLEPROD_C, &name, -1);
 g_print ("The row containing the name '%s' has been double-clicked.\n", name);
  g_free(name);
  }
}


void selection_prod(GtkTreeSelection *selection, gpointer data)
{
gchar *text;
// bidouille poue empecher la selection a la creation de la fenetre (vers windows)
#ifdef WIN32
if (inhibsel == 1)  {
    inhibsel = 0;    // vers windows inhibe sel crefen_listprod
    return;         }
# endif

if (gtk_tree_selection_get_selected (selection, &model, &iter))   {
 gtk_tree_model_get (model, &iter, CLEPROD_C, &text, -1);
 acces_sfprod(text);
//printf("iter.stamp = %d\n",iter.stamp);
 g_free(text);
    					    }
}

*/
