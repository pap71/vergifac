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

gboolean maj_fenq ();
gboolean focout_fenq (GtkWidget *widget, GdkEventKey *event, DEF_S_FQ *pzw);
void 	chang_fenq (GtkEntry *widls, DEF_S_FQ *pzw);
void 	activ_fenq (GtkWidget *widls, DEF_S_FQ *pzw);
gboolean quit_fenq (GtkWidget *widls, FENQ *pf);

extern S_DAT da;

DEF_S_FQ *svpzw;
int bloqchfq=0; // inhibe signal change si affichage
static char zmes[64];

gboolean maj_fenq ()	//  controle, maj
{
int lt,ret=0;
DEF_S_FQ *pzw = svpzw;	// widget actif dans change
GtkEditable *editable;
gchar *text;
char *pa;
double *paf; // modif 2/10/2015 pb edition float* paf;
int* pai; 
if ( svpzw == NULL)	{
	printf("maj_fenq svpzw=NULL ?\n");
	return TRUE;
			}
editable = GTK_EDITABLE (pzw->wdg);
text = gtk_editable_get_chars (editable, 0, -1);
lt = strlen(text);
// test saisie trop longue
if ( lt > pzw->laf)	{
 sprintf(zmes,"zone saisie trop longue de %d car",lt-pzw->laf);
 messavar(zmes);	//  message(1);
  ret = -1;
					}
if (ret == 0) 	{
 pa = pzw->memcell;
 if (pzw->typ == SAISNUM)    {
  paf = (double*) pa;
  *(paf) = g_strtod(text,NULL);
							}
 else if (pzw->typ == SAISINT)   {
  pai = (int*) pa;
  *(pai) = atoi(text);
								}
 else if (pzw->typ == SAISDAT)   {
  if ( (ret = contdate((char*)text)) >= 0)
   strncpy((char*)pa,da.sdat,pzw->laf); 
  else message(13);
								}
 else	{ 	 // SAISMON, SAICHAR,SSPEC 
  if (pzw->laf == 1)	{
   if ( text[0] == ' ' && lt > 1)	text[0] = text[1];
   pa[0]=text[0];  
					}
  else	{	
  strncpy((char*)pa,text,pzw->laf); // efface fin de saisie precedente
		}
		}
 if (pzw->fonc && ret >= 0) 	 {		// ret =date vide ou ok
	ret = (*(pzw->fonc)) (pzw,0) ;  // fonctions utilisateur
								 }
		}	// fin ret == 0
svpzw = NULL;	// tout le traitement apres saisie a du etre fait
//printf("maj_fenq-2 pzw=%p ret=%d\n",pzw,ret);
if (ret == -1) {
 if (pzw->laf == 1)  gtk_editable_delete_text((GtkEditable*) editable,0,-1);
 gtk_editable_set_position((GtkEditable*) editable,0);
 g_free(text);	
 gtk_widget_grab_focus (pzw->wdg); 
 return TRUE;
		}
g_free(text);		
return FALSE;
}

gboolean focout_fenq (GtkWidget *widget, GdkEventKey *event, DEF_S_FQ *pzw)
{
//pw fait ref au widget suivant
//printf("focus-out  pzw=%p svpzw=%p\n",pzw,svpzw);
if (svpzw != NULL) maj_fenq ();
  return FALSE;
}

void chang_fenq (GtkEntry *widls, DEF_S_FQ *pzw)	// CHANGE
{		// attention active par affichage sans saisie
int lt;
GtkEditable *editable;
gchar *text;
if (bloqchfq == 1) return;
svpzw = pzw;	//sauvegarde du widget saisie  a traiter
//printf("chang_fenq pzw=%p \n",pzw);
if (pzw->typ == SSPEC  || pzw->typ == DEFMAN) { // traitement specifique
 if (pzw->fonc)  {
  (*(pzw->fonc)) (pzw,1) ;  
				 }
  return;
 											}
else if (pzw->typ > 19 )  		    { //&& lt > 0)   {  
    lt=chang_ctrl(widls,pzw->typ);
									}
/* la suite de la fonction ne sert qu'au passage forcé 
 à la zone suivante ce qui perturbe les modif (insertion) 
  à reserver aux saisies d'un seul caractere ? */
else if ( pzw->laf == 1)	{
    editable = GTK_EDITABLE (widls);
    text = gtk_editable_get_chars (editable, 0, -1);
    lt = strlen(text);
    g_free(text);
	if ( lt == pzw->laf )
  g_signal_emit_by_name(widls,"move-focus",GTK_DIR_TAB_FORWARD,pzw);
							}
}


void activ_fenq (GtkWidget *widls, DEF_S_FQ *pzw)
{
//if (svpzw != NULL) maj_fenq (); //(widls); focout va le faire
g_signal_emit_by_name(widls,"move-focus",GTK_DIR_TAB_FORWARD,pzw);
//printf("activ-focus pw=%p svpw=%p\n",pw,svpw);
}

gboolean quit_fenq (GtkWidget *widls, FENQ* pf)
{
pf->win = NULL;
return FALSE;
}

void crefenq(FENQ* pf,void* pd)	// creation fenetre qq
{
int  iz,lafsais;
DEF_L_FQ *pli = pf->pdeflib;
DEF_S_FQ *pz = pf->pdefz;
GtkWidget* fixg;  //pf->vboxwin
char *pa;

pf->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_move(GTK_WINDOW(pf->win), pf->posx, pf->posy);
if (pf->fenlarg != 0)
 gtk_window_set_default_size(GTK_WINDOW(pf->win), pf->fenlarg, pf->fenhaut);
gtk_window_set_title(GTK_WINDOW(pf->win), pf->fentit);
gtk_container_set_border_width(GTK_CONTAINER(pf->win), 0);
	//  fix globale
fixg = gtk_fixed_new();
gtk_container_add(GTK_CONTAINER(pf->win), fixg);

	//  box bouton 
 if (pf->fbouton)  {
	 (*(pf->fbouton)) (fixg) ;  
		 }
	// libelle
iz=0;
while ((pli+iz)->cox > 0)	{
 (pli+iz)->wdgl =  gtk_label_new((pli+iz)->libel);
 gtk_fixed_put(GTK_FIXED(fixg), (pli+iz)->wdgl, (pli+iz)->cox, (pli+iz)->lyg); 
 ++iz;
			}
	// entry saisie zone variable
iz=0;
while ((pz+iz)->cox > 0)	{
 pa = (char*)pd +(int)(pz+iz)->deps;
 (pz+iz)->memcell = pa;
if ( (pz+iz)->typ < 10)	{
 (pz+iz)->wdg = gtk_label_new(NULL);
 gtk_label_set_max_width_chars((GtkLabel*) (pz+iz)->wdg,(pz+iz)->laf); 
 gtk_fixed_put(GTK_FIXED(fixg), (pz+iz)->wdg, (pz+iz)->cox, (pz+iz)->lyg); 
 gtk_widget_set_size_request((pz+iz)->wdg, (pz+iz)->laf * DIMC, 25);
			}
	// DEFMAN definition  faite dans fbouton
else if ( (pz+iz)->typ == DEFMAN)	{ ++iz;	continue; }
else 	{
	// saisie sur 1 car force a 2
 if ((pz+iz)->laf == 1)  (pz+iz)->wdg = gtk_entry_new_with_max_length(2);  
 else	{
	lafsais = (pz+iz)->laf * 2;
	(pz+iz)->wdg =  gtk_entry_new_with_max_length(lafsais); 
		}
 gtk_fixed_put(GTK_FIXED(fixg), (pz+iz)->wdg, (pz+iz)->cox, (pz+iz)->lyg); 
 gtk_widget_set_size_request((pz+iz)->wdg, (pz+iz)->laf * DIMC + 15, 25);

 g_signal_connect(G_OBJECT((pz+iz)->wdg), "activate", G_CALLBACK (activ_fenq),(void*) (pz+iz));
 g_signal_connect(G_OBJECT((pz+iz)->wdg), "changed", G_CALLBACK (chang_fenq),(void*) (pz+iz));
 g_signal_connect_after(G_OBJECT((pz+iz)->wdg), "focus-out-event", G_CALLBACK (focout_fenq),(void*) (pz+iz));
	}
 ++iz;
			}
g_signal_connect_after(G_OBJECT(pf->win), "destroy", G_CALLBACK (quit_fenq), pf);

}

void creparfenq(DEF_L_FQ *pli,DEF_S_FQ *pz,GtkWidget* fixg,void* pd)
	// creation partie fenetre qq
{
int  iz,lafsais;
char *pa;
	// libelle
iz=0;
while ((pli+iz)->cox > 0)	{
 (pli+iz)->wdgl =  gtk_label_new((pli+iz)->libel);
 gtk_fixed_put(GTK_FIXED(fixg), (pli+iz)->wdgl, (pli+iz)->cox, (pli+iz)->lyg); 
 ++iz;
				}
	// entry saisie zone variable
iz=0;
while ((pz+iz)->cox > 0)	{
 pa = (char*)pd +(int)(pz+iz)->deps;
 (pz+iz)->memcell = pa;
if ( (pz+iz)->typ < 10)	{
 (pz+iz)->wdg = gtk_label_new(NULL);
 gtk_label_set_max_width_chars((GtkLabel*) (pz+iz)->wdg,(pz+iz)->laf); 
 gtk_fixed_put(GTK_FIXED(fixg), (pz+iz)->wdg, (pz+iz)->cox, (pz+iz)->lyg); 
 gtk_widget_set_size_request((pz+iz)->wdg, (pz+iz)->laf * DIMC, 25);
			}
	// DEFMAN definition  faite dans fbouton
else if ( (pz+iz)->typ == DEFMAN)	{ ++iz;	continue; }
else 	{
	// saisie sur 1 car force a 2
 if ((pz+iz)->laf == 1)  (pz+iz)->wdg = gtk_entry_new_with_max_length(2);  
 else	{
	lafsais = (pz+iz)->laf * 2;
	(pz+iz)->wdg =  gtk_entry_new_with_max_length(lafsais); 
		}
// else	  (pz+iz)->wdg =  gtk_entry_new_with_max_length((pz+iz)->laf); 
 gtk_fixed_put(GTK_FIXED(fixg), (pz+iz)->wdg, (pz+iz)->cox, (pz+iz)->lyg); 
 gtk_widget_set_size_request((pz+iz)->wdg, (pz+iz)->laf * DIMC + 15, 25);

 g_signal_connect(G_OBJECT((pz+iz)->wdg), "activate", G_CALLBACK (activ_fenq),(void*) (pz+iz));
 g_signal_connect(G_OBJECT((pz+iz)->wdg), "changed", G_CALLBACK (chang_fenq),(void*) (pz+iz));
 g_signal_connect_after(G_OBJECT((pz+iz)->wdg), "focus-out-event", G_CALLBACK (focout_fenq),(void*) (pz+iz));
	}
 ++iz;
				}
}
