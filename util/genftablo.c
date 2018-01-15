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
//  Routines de creation de fenetre et tableaux
//  Routines de creation de fenetre et tableaux
#include <gtk/gtk.h>
//#include <gdk/gdkkeysyms.h>
#include <string.h>
#include "genfen.h"

void 	chang_l_fnc (GtkEntry *widls, MEMO_T_WIDG *pw);
gboolean maj_l_fnc ();
void 	activ_l_fnc (GtkWidget *widls, MEMO_T_WIDG *pw);
gboolean focouteve (GtkWidget *widget, GdkEventKey *event, MEMO_T_WIDG *pw);
gboolean focineve (GtkWidget *widget, GdkEventKey *event, MEMO_T_WIDG *pw);
void 	crewdg_l_fnc (MEMO_T_WIDG *pw);
gboolean quit_fnc (GtkWidget *widls, FnC *pf);

extern S_DAT da;

MEMO_T_WIDG *svpw,*inpw;
int bloqchang=0; // inhibe signal change si affichage

static char buf[80];
static char zmes[64];

//__________________fenetre tableau
void chang_l_fnc (GtkEntry *widls, MEMO_T_WIDG *pw)
{		// attention active par affichage sans saisie
int lt;
GtkEditable *editable;
gchar *text;
DEF_L_FnC *pl = pw->pdefc;
if (bloqchang == 1) return;

svpw = pw;	//sauvegarde du widget saisie  a traiter
if (pl->typ == SSPEC)	{ // traitement specifique
  if (pl->fonc) {
   (*(pl->fonc)) (pw,1) ;
				}
   return;
 						}
else if (pl->typ > 19 )	{//&& lt > 0)   {
    lt=chang_ctrl(widls,pl->typ);
										}
/* la suite de la fonction ne sert qu'au passage forcé 
 à la zone suivante ce qui perturbe les modif (insertion) 
  à reserver aux saisies d'un seul caractere ? */
else if ( pl->laf == 1)	{
    editable = GTK_EDITABLE (widls);
    text = gtk_editable_get_chars (editable, 0, -1);
    lt = strlen(text);
    g_free(text);
	if ( lt == pl->laf )
  g_signal_emit_by_name(widls,"move-focus",GTK_DIR_TAB_FORWARD,pl);
						}
}

gboolean maj_l_fnc()	//  controle, maj
{
int lt,ret=0;
MEMO_T_WIDG *pw = svpw;  // widget actif dans change
DEF_L_FnC *pl = pw->pdefc;
GtkEditable *editable;
gchar *text;
char *pa;
double *paf; // modif 2/10/2015 pb edition float* paf;
int* pai;
editable = GTK_EDITABLE (pw->wdg);
text = gtk_editable_get_chars (editable, 0, -1);
lt = strlen(text);
// test saisie trop longue
if ( lt > pl->laf)	{
 sprintf(zmes,"zone saisie trop longue de %d car",lt-pl->laf);
 messavar(zmes);	//  message(1);
  ret = -1;
					}
if (ret == 0) 	{
 pa = pw->memcell;
 if (pl->typ == SAISNUM)   {
  paf = (double*) pa;
  *(paf) = g_strtod(text,NULL);
			}
 else if (pl->typ == SAISINT)   {
  pai = (int*) pa;
  *(pai) = atoi(text);
			}
 else if (pl->typ == SAISDAT)   {
  if ( (ret = contdate((char*)text)) >= 0)
   strncpy((char*)pa,da.sdat,pl->laf);
  else message(13);
			}
 else {  // SAISMON, SAICHAR,SSPEC
	// saisie sur 1 car force a 2
  if (pl->laf == 1)	{
   if ( text[0] == ' ' && lt > 1)	text[0] = text[1];
   pa[0]=text[0];
				}
  else	{
   memset ((char*)pa,0, pl->laf);   //efface saisie prec si besoin
   strcpy((char*)pa,text);
//   strncpy((char*)pa,text,pl->laf);
	}
	}
 if (pl->fonc && ret >= 0)  {	// ret =date vide ou ok
   ret = (*(pl->fonc)) (pw,0);
		 }
//printf("maj col %d lig %d\n",pl->idcol,pw->liga);
		}	// fin ret == 0
svpw = NULL;	// tout le traitement apres saisie a du etre fait
if (ret == -1) {
 if (pl->laf == 1)  gtk_editable_delete_text((GtkEditable*) editable,0,-1);
 gtk_editable_set_position((GtkEditable*) editable,0);
 g_free(text);
 gtk_widget_grab_focus (pw->wdg);
 return TRUE;
		}
g_free(text);
return FALSE;
}

void activ_l_fnc (GtkWidget *widls, MEMO_T_WIDG *pw)
{
//if (svpw != NULL) maj_l_fnc (widls); focouteve va le faire
g_signal_emit_by_name(widls,"move-focus",GTK_DIR_TAB_FORWARD,pw);
//printf("activ-focus pw=%p svpw=%p\n",pw,svpw);
}

gboolean focouteve (GtkWidget *widget, GdkEventKey *event, MEMO_T_WIDG *pw)
{
//pw fait ref au widget suivant
//GtkWidget *parent;
// parent = widget->parent;
//printf("focus-out  pw=%p svpw=%p parent=%p\n",pw,svpw,parent);
if (svpw != NULL) maj_l_fnc();
  return FALSE;
}

gboolean focineve (GtkWidget *widget, GdkEventKey *event, MEMO_T_WIDG *pw)
{ // on a widget=pw->wdg
//int ret;
inpw=pw;
 /* si test
DEF_L_FnC *pl = pw->pdefc;
const gchar *text;
    text = gtk_entry_get_text (GTK_ENTRY(pw->wdg));
printf("foc-int1 pw->w=%p lig=%d co=%d text=%s\n",pw->wdg,pw->liga,pl->idcol,text);
if ( svpw != NULL) {
pl = svpw->pdefc;
printf("foc-int2 svpw->w=%p  lig=%d co=%d\n",svpw->wdg,svpw->liga,pl->idcol);
		}
// fin test */
  return FALSE;
}

void crewdg_l_fnc (MEMO_T_WIDG *pw)
{
int lafsais;
DEF_L_FnC *pl = pw->pdefc;
if ( pl->typ < 10)	{
 pw->wdg = gtk_label_new(NULL);
 gtk_label_set_max_width_chars((GtkLabel*) pw->wdg,pl->laf);
 gtk_widget_set_size_request(pw->wdg, pl->laf * DIMC, DIMCH);
			}
else {
	// saisie sur 1 car force a 2
 if (pl->laf == 1)  pw->wdg = gtk_entry_new_with_max_length(2);
 else	{
	lafsais = pl->laf * 2;
	pw->wdg = gtk_entry_new_with_max_length(lafsais);
		}
// else	  pw->wdg = gtk_entry_new_with_max_length(pl->laf);
 gtk_widget_set_size_request(pw->wdg, pl->laf * DIMC, DIMCH);

 g_signal_connect(G_OBJECT(pw->wdg), "activate", G_CALLBACK (activ_l_fnc),(void*) pw);
 g_signal_connect(G_OBJECT(pw->wdg), "changed", G_CALLBACK (chang_l_fnc),(void*) pw);
 g_signal_connect_after(G_OBJECT(pw->wdg), "focus-out-event", G_CALLBACK (focouteve),(void*) pw);
 g_signal_connect(G_OBJECT(pw->wdg), "focus-in-event", G_CALLBACK (focineve),(void*) pw);
//g_signal_connect(G_OBJECT(pw->wdg),"key-release-event",G_CALLBACK (key_fnc),(void*) pw);
//test(pw);
	}
}

gboolean quit_fnc (GtkWidget *widls, FnC* pf)
{
pf->win = NULL;
//printf("quit fnc = 0\n");
return FALSE;
}

void cretablo(FnC* pf,void* pd)	// creation fenetre tableau a n col
{
int  jll,kco,flarg=0;
DEF_L_FnC *pti = pf->pdefct;
MEMO_T_WIDG *pwv,*pw =pf->ptwidg;
GtkWidget* boxglob;
GtkWidget* boxlib;
GtkWidget* boxsais;
GtkWidget* tablibel;
GtkWidget *tabsais;
GtkWidget *scrollbar;
char *pa;

 pf->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_window_move(GTK_WINDOW(pf->win), pf->posx, pf->posy);
if (pf->fenlarg != 0)
  gtk_window_set_default_size(GTK_WINDOW(pf->win), pf->fenlarg, pf->fenhaut);
 gtk_window_set_title(GTK_WINDOW(pf->win), pf->fentit);
	//  vbox globale
 boxglob = gtk_vbox_new(FALSE, 1);
 gtk_container_add(GTK_CONTAINER(pf->win),boxglob);

	//   bouton cree sa box
 if (pf->fbouton)  {
 	 (*(pf->fbouton)) (boxglob) ;
 		 }
	//  box libel
boxlib = gtk_hbox_new(FALSE, 1);
gtk_box_pack_start(GTK_BOX(boxglob), boxlib, FALSE, TRUE, 1);
	//  table libelle
  tablibel = gtk_table_new(1, pf->tco, FALSE);	//
  gtk_box_pack_start(GTK_BOX(boxlib), tablibel, TRUE, TRUE, 1);
	//  affichage libelle sur 1 ligne
    for( kco=0; kco < pf->tco; kco++) {
      flarg += (pti+kco)->laf * DIMC;
      (pti+kco)->wlibel =  gtk_label_new((char *) (pti+kco)->libel);
       gtk_widget_set_size_request((pti+kco)->wlibel,(pti+kco)->laf * DIMC, DIMCH);
 gtk_table_attach_defaults(GTK_TABLE(tablibel),(pti+kco)->wlibel ,kco,kco+1,0,1 );
	}
	// ajuste largeur fenetre
 if (pf->fenlarg == 0) {
   flarg += pf->tco * 32;
   gtk_window_set_default_size(GTK_WINDOW(pf->win), flarg, pf->fenhaut);
			}

	// scroll
scrollbar = gtk_scrolled_window_new(NULL, NULL);
gtk_box_pack_start(GTK_BOX(boxglob), scrollbar, TRUE, TRUE, 0);
	//  box saisie h
  boxsais = gtk_vbox_new(FALSE, 1);
 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrollbar),boxsais);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	// table de saisie
  tabsais = gtk_table_new(pf->tli, pf->tco, FALSE);	//n col
	// couleur table pour label
    gtk_table_set_row_spacings(GTK_TABLE(tabsais), 0);
    gtk_table_set_col_spacings(GTK_TABLE(tabsais), 0);
  gtk_box_pack_start(GTK_BOX(boxsais), tabsais, TRUE, TRUE, 1);
  for( jll=0; jll < pf->tli; jll++) {
    for( kco=0; kco < pf->tco; kco++) {
       pa = (char*)pd +(int)(pti+kco)->deps + (int)pf->sizsvt * jll;
      pwv = pw+pf->tco*jll + kco;
      (pwv)->memcell = pa;
      (pwv)->pdefc = pti+kco;
      (pwv)->liga = jll;
       crewdg_l_fnc(pwv);
 gtk_table_attach_defaults(GTK_TABLE(tabsais),(pwv)->wdg,kco,kco+1,jll,jll+1 );
	} }
g_signal_connect_after(G_OBJECT(pf->win), "destroy", G_CALLBACK (quit_fnc), pf);
	// focus sur 1er widget
gtk_widget_grab_focus (pw->wdg);
}


// affiche fenetre tableau a n col
void aftablo(FnC* pf,void* pd, int liga)
{	// buf doit etre declare en global ?
int jll,kco,jld,jla;
DEF_L_FnC *pti = pf->pdefct;
MEMO_T_WIDG *pwv,*pw =pf->ptwidg;
char *pa;
double *paf;    //gfloat* paf;
int* pai;
//bloqchang = 0;	// inhibe signal changed
if ( liga ==0) { jld=0; jla=pf->tli;  }
else { jld=liga;  jla = jld+1; }
for( jll=jld; jll < jla; jll++) {
 for( kco=0; kco < pf->tco; kco++) {
	// calcul adresse cellule a afficher
    pa = (char*)pd +(int)(pti+kco)->deps + (int)pf->sizsvt * jll;
    pwv = pw+pf->tco*jll + kco;
//    sprintf(buf, "%s",  " ");
  if ( *((pti+kco)->forma + 1) == 's')
	sprintf(buf, (pti+kco)->forma,  pa);
  else if ( *((pti+kco)->forma + 1) == 'c')
	sprintf(buf, (pti+kco)->forma,  pa[0]);
  else if ( *((pti+kco)->forma + 1) == 'd')	{
	pai = (int*) pa;
	if (liga == 0 && *pai == 0) buf[0]='\0'; // ??
	else sprintf(buf, (pti+kco)->forma,  *pai);
								}
  else {	// double
	paf = (double*) pa;
	if (liga == 0 && *paf == 0) buf[0]='\0';
//	else buf = (char*) g_strdup_printf((pti+kco)->forma, *paf);
	else sprintf(buf, (pti+kco)->forma,  *paf);
	}

  if ( (pti+kco)->typ < 10)
   gtk_label_set_text(GTK_LABEL((pwv)->wdg), buf);
  else
//     active signal changed (supprime la virgule !)
   gtk_entry_set_text(GTK_ENTRY((pwv)->wdg), buf);
	}	}
//bloqchang = 0;	// restore signal changed
}

/*
gboolean key_fnc (GtkWidget *widget, GdkEventKey *event, MEMO_T_WIDG *pw)
{
//pw fait ref au widget suivant
switch (event->keyval)
{
case GDK_Tab:
//case GDK_Left //0xff51
case GDK_Up: //0xff52
//#define GDK_Right 0xff53
case GDK_Down: //0xff54
printf("key pw=%p svpw=%p\n",pw,svpw);
maj_l_fnc (widget);
//default:
}
  return FALSE;
}
*/

