/*
 *  Copyright (c) 2015,2018 Michel Delorme
 *
 * This file is part of vergifac vergisc etc
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
#include <string.h>
#include <gtk/gtk.h>
#include "genfen.h"

void 	OnRemove (GtkWidget *pButton, FCBOX *pf);
void 	OnCancel (GtkWidget *pButton, FCBOX *pf);
void 	OnAdd (GtkWidget *pButadd, FCBOX *pf);

void OnRemove(GtkWidget *pButton, FCBOX* pf)
{
gint i;
//gchar *text;
i = gtk_combo_box_get_active(GTK_COMBO_BOX(pf->cbox));
//text = gtk_combo_box_get_active_text((GtkComboBox*)pf->cbox);
//printf("elem=%d text=%s\n",i,text);
if(i != -1) {
 gtk_combo_box_remove_text(GTK_COMBO_BOX(pf->cbox), i);
 gtk_label_set_text(GTK_LABEL(pf->libretour), "Suppression O.K");
 pf->modif = 1;
		}
else 	{
 gtk_label_set_text(GTK_LABEL(pf->libretour), "Erreur Saisie");
	}
//g_free(text);
}

void OnCancel(GtkWidget *pButton, FCBOX* pf)
{
pf->modif = 0;
gtk_widget_destroy(pf->win);
}

void OnAdd(GtkWidget *pButadd, FCBOX* pf)
{
GtkWidget *pDialog;
DEF_L_FCBOX *pti = pf->ptle;
int j;
    /* Creation de boite de dialogue avec des boutons "Annuler" et "Ok" */
pDialog = gtk_dialog_new_with_buttons(pf->titadd, 
	GTK_WINDOW(pf->win),  GTK_DIALOG_MODAL,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
        GTK_STOCK_OK, GTK_RESPONSE_OK,  NULL);

    /* Creation des saisies */
for (j=0; j < pf->nbdon; ++j)	{
   (pti+j)->wlibel =  gtk_label_new((char *) (pti+j)->libel);
   gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pDialog)->vbox),(pti+j)->wlibel , FALSE, FALSE, 0);
   (pti+j)->wdg = gtk_entry_new_with_max_length((pti+j)->laf); 
   gtk_widget_set_size_request((pti+j)->wdg, (pti+j)->laf * 8, 25);
   gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pDialog)->vbox),(pti+j)->wdg , FALSE, FALSE, 0);
					}
gtk_widget_show_all(GTK_DIALOG(pDialog)->vbox);
    /* Lancement de la boite de dialogue */
switch(gtk_dialog_run(GTK_DIALOG(pDialog))){
case GTK_RESPONSE_OK:
	(*(pf->ftsais)) (pf);  
   break;
 case GTK_RESPONSE_CANCEL:
   break;
 default:
   break;
    }
gtk_widget_destroy(pDialog);
}

void combo_box_clear (GtkComboBox *p_combo_box)
{
  GtkTreeModel *p_tree_model = NULL;
  g_return_if_fail (p_combo_box != NULL);
  p_tree_model = gtk_combo_box_get_model (p_combo_box);
  gtk_list_store_clear (GTK_LIST_STORE (p_tree_model));
}

void cre_fcbox(FCBOX* pf)
{
GtkWidget* boxglob;
GtkWidget *pHBox;
GtkWidget *pButadd;
GtkWidget *pButsup;
GtkWidget *pButsave;
GtkWidget *pButcancel;

 pf->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_window_move(GTK_WINDOW(pf->win), pf->posx, pf->posy);
 gtk_window_set_default_size(GTK_WINDOW(pf->win), pf->fenlarg, pf->fenhaut);
 gtk_window_set_title(GTK_WINDOW(pf->win), pf->fentit);
 pf->modif = 0;
	//  vbox globale
boxglob = gtk_vbox_new(FALSE, 1);
 gtk_container_add(GTK_CONTAINER(pf->win),boxglob);

    /* Creation de GtkBox horizontal et boutons*/
pHBox = gtk_hbox_new(TRUE, 0);
gtk_box_pack_start(GTK_BOX(boxglob), pHBox, FALSE, TRUE, 0);
pButadd = gtk_button_new_from_stock(GTK_STOCK_ADD);
   gtk_box_pack_start(GTK_BOX(pHBox), pButadd, FALSE, FALSE, 0);
pButsup = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start(GTK_BOX(pHBox), pButsup, FALSE,FALSE, 0);
pButcancel = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  gtk_box_pack_start(GTK_BOX(pHBox), pButcancel, FALSE,FALSE, 0);
pButsave = gtk_button_new_from_stock(GTK_STOCK_SAVE);
  gtk_box_pack_start(GTK_BOX(pHBox), pButsave, FALSE,FALSE, 0);
    //  GtkComboBox  
//pf->cbox = gtk_combo_box_text_new();  // creation cbox ici
pf->cbox = gtk_combo_box_new_text();  // creation cbox ici
 gtk_box_pack_start(GTK_BOX(boxglob), pf->cbox, FALSE, FALSE, 0);
pf->libretour = gtk_label_new(NULL);
 gtk_box_pack_start(GTK_BOX(boxglob), pf->libretour, FALSE, FALSE, 0);

//g_signal_connect_after(G_OBJECT(pf->cbox), "changed", G_CALLBACK(chang_cbox), pf);

g_signal_connect(G_OBJECT(pButadd), "clicked", G_CALLBACK(OnAdd), pf);
g_signal_connect(G_OBJECT(pButsup), "clicked", G_CALLBACK(OnRemove), pf);
g_signal_connect(G_OBJECT(pButsave), "clicked", G_CALLBACK(pf->fquitfen), pf);
g_signal_connect(G_OBJECT(pButcancel), "clicked", G_CALLBACK(OnCancel), pf);

g_signal_connect(G_OBJECT(pf->win), "destroy", G_CALLBACK(pf->fquitfen), pf);
    gtk_widget_show_all(pf->win);

}

