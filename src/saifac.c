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

//gboolean sfac_dupli (GtkWidget *widget, gpointer data);
void afcell_sfac(int idcol, int liga);
int gencode(char *code,int liga);
gboolean sfac_efflig (GtkWidget *widget, gpointer data);
gboolean sfac_inslig (GtkWidget *widget, gpointer data);
gboolean sfac_inslig2 (int liga);
gboolean sfac_supflig (GtkWidget *widget, gpointer data);
void razl_sfac(int liga);
void cod_tht(int liga);
void cod_ttc(int liga);
void cod_ttc2(int liga);
void cod_ttx(int liga);
void cod_tpp(int liga);
void quellestaxes(int liga);
void basetax(char cta, int liga);
int cod_tax(int liga,int temht);
//int derlig_sfac(int liga);
int isligvid_sfac(int liga);
void actu_sfac();
int wherecode(char *code);
int code_double(char *code, int liga);
void sfac_afilib(char *code,int liga);
int 	fonc_senf (DEF_S_FQ *pzw, int temchang);
void affi_derlig();
void inhilistprod_sfac();
void raz_senf();
void init2_sfac();
gboolean verifinpw();
void afadres2_cli();
gboolean litdoc(GtkWidget *widget, gpointer data);

extern void act_fenscod(GtkWidget *widget, gpointer data);

extern MEMO_T_WIDG *inpw;
extern long zslon;
extern int bloqchang;
extern S_CODFAC ligcod[];
extern S_FCLI lsfcli; // struct de lecture de la db
extern S_PROD lsfprod; // struct de lecture de la db
extern GtkWidget* winli;
extern GtkWidget* winliprod;
extern S_DAT da;
extern S_DAT today;
extern char ficcdimp[];
extern char ficsauvdoc[];

static char tabtax[20];
static int nbtax;
static long btax;
static char rkey[12];
static GtkWidget *boutinsl;
static char libinsN[]="Ins.ligne";
static char libinsP[]="Ins.PROD";
static GdkColor  colorbut = {
0,
30000,	//colortabs.red=30000;
45000,	//colortabs.green=45000;
0	//colortabs.blue=0;
};
static GdkColor  colorins = {
0,
63488,	//  248 x 256
17664,	//  69 x 256
0	//
};
typedef struct  {	//memo codprod depuis list prod
int acti;
char kprod[12];
} S_AUTOPROD;
static S_AUTOPROD autop;

int derlig = 0;	// editfast utilise
GtkWidget *cbtypdoc;

S_SENF senf;

//   pour test
//_______________ code produit ____________

//_______________fenetre saisie facture ____________

enum {
CSFcodprod,
CSFlibprod,
CSFquant,
CSFunit,
CSFcodtax,
CSFpriunit,
CSFvalor,
CSFnco
};

//#define CSFnli 30
int CSFnli=120;

//S_LFAC ligfac[CSFnli];
S_LFAC *ligfac;
S_LFAC ligfacbid;	// pour calcul deps

//tableau de widgets
//MEMO_T_WIDG ligfac_wgt[CSFnli][CSFnco];
MEMO_T_WIDG *ligfac_wgt;

extern  int traifac(MEMO_T_WIDG *pw,int temchang);
extern  void boutsfac(GtkWidget *box);
#define DEPS(zz) (char*)&ligfacbid.zz-(char*)&ligfacbid.codprod
//#define DEPS(zz) (char*)&ligfac[0].zz-(char*)&ligfac[0].codprod
//#define DEPS(zz) (void*)ligfac[0].zz-ligfac
static DEF_L_FnC colsfac[CSFnco] ={
{NULL,CSFcodprod,0,SSPEC,11,&traifac,"%s","Code Produit"},
{NULL,CSFlibprod,DEPS(libprod),SAISCHAR,44,&traifac,"%s"," Libelle"},
{NULL,CSFquant,DEPS(quant),SAISNUM,10,&traifac,"%f","Quantité"},
//{NULL,CSFquant,DEPS(quant),SAISNUM,10,&traifac,"%#0.3f","Quantité"},
{NULL,CSFunit,DEPS(unit),SAISCHAR,4,&traifac,"%s","Unite"},
{NULL,CSFcodtax,DEPS(codtax),SAISCHAR,1,&traifac,"%s","C.Taxe"},
{NULL,CSFpriunit,DEPS(spriunit),SAISMON,13,&traifac,"%s","Prix.Unit"},
{NULL,CSFvalor,DEPS(svalor),LIBNUM,13,&traifac,"%s","Valorisation"},
};
#undef DEPS

//          definition fenetres
FnC fen_sfac = {
NULL,
"Saisie Facture",        /* pointeur sur titre  fenetre */
0,	//CSFnco*80,	/*largeur fenetre*/
600,	//CSFnli*15,	hauteur  fenetre
//120,	/*hauteur  fenetre*/
350,	/* coin gauche x*/
150,	/* coin gauche y*/
100,	//CSFnli,	 nb lignes modifie dans init_sfac
CSFnco,	/* nb col */
sizeof(S_LFAC),
&colsfac[0],	// pointeur sur definition colonnes
NULL,	//&ligfac_wgt[0][0],	// pointeur sur tableau widget dans init_sfac
&boutsfac// fonction ajout de bouton
};

//		en tete saisie facture (part de fenq)
enum {
NUMFAC,
DATFAC,
CLECLI,
ADD1,
ADD2,
ADD3,
PAYS,
CODPOS,
ADD4,
DOCLU,
FICCDE,
NBLIG
};
#define YAD1 5
#define YAD2 35
#define YAD3 70
#define YAD4 105
#define MEM 0
#define DEPS(zz) (char*)&senf.zz-(char*)&senf
//#define DETR(zl,zz) (char*)&senf.rg[zl].zz-(char*)&senf
DEF_S_FQ s_fsenf[]= {
{190,YAD2,NUMFAC,DEPS(numfac),NULL,MEM,SAISCHAR,14,&fonc_senf,"%s"},
{40,YAD3,DATFAC,DEPS(datfac),NULL,MEM,SAISDAT,12,&fonc_senf,"%s"},
{350,YAD2,CLECLI,DEPS(clecli),NULL,MEM,SSPEC,11,&fonc_senf,"%s"},
{455,YAD2,ADD1,DEPS(add1),NULL,MEM,SAISCHAR,50,&fonc_senf,"%s"},
{455,YAD2+25,ADD2,DEPS(add2),NULL,MEM,SAISCHAR,50,&fonc_senf,"%s"},
{455,YAD2+50,ADD3,DEPS(add3),NULL,MEM,SAISCHAR,50,&fonc_senf,"%s"},
{455,YAD2+75,PAYS,DEPS(pays),NULL,MEM,SAISCHAR,5,&fonc_senf,"%s"},
{515,YAD2+75,CODPOS,DEPS(codpos),NULL,MEM,SAISCHAR,8,&fonc_senf,"%s"},
{565,YAD2+75,ADD4,DEPS(add4),NULL,MEM,SAISCHAR,30,&fonc_senf,"%s"},
{120,YAD1,DOCLU,DEPS(doclu),NULL,MEM,LIBCHAR,18,NULL,"%s"},
{590,YAD1,FICCDE,DEPS(ficdi),NULL,MEM,LIBCHAR,22,NULL,"%s"},
{380,YAD4,NBLIG,DEPS(nblig),NULL,MEM,LIBCHAR,8,NULL,"%s"},
{-1,-1,0,0,NULL,0,0,0,NULL,NULL}
};
#undef DEPS

DEF_L_FQ lib_fsenf[]= {
{130,YAD2+5,NULL,0,"Numero"},
{305,YAD2+5,NULL,0,"Client"},
{5,YAD3+5,NULL,0,"Date"},
{-1,-1,NULL,0,NULL}
};

	// verif inpw si plusieurs fen tablo inpw est -il le bon ?
gboolean verifinpw()
{
DEF_L_FnC *pl;
int col;
pl = inpw->pdefc;
col = pl->idcol;
if ( pl == &colsfac[col]) return TRUE;
//printf("inpw sfac FALSE\n");
message(17);
return FALSE;
}

int traifac(MEMO_T_WIDG *pw,int temchang)
{
DEF_L_FnC *pl;
long dvalor;
int ret = 0, liga,ligg,i,j,llib,lt,temactu;
GtkEditable *editable;
gchar *text;
 if ( pw == NULL) return ret;
 pl = pw->pdefc;
 liga=pw->liga;
	// traite change SSPEC   codprod
 if ( temchang == 1)	{
  if ( GTK_IS_WINDOW (winliprod))	{
   editable = GTK_EDITABLE (pw->wdg);
   text = gtk_editable_get_chars (editable, 0, -1);
   if ( *(text) != ' ' && *(text) != '.')	{
    strcpy(rkey,text);
    lt = strlen(rkey);
    if (lt > 0 && lt < 5 )  rechprod((char*)rkey);
					       	}
   g_free(text);
					}
   return ret;
			}
switch (pl->idcol)
{
case CSFcodprod:
 if (ligfac[liga].codprod[0] == '.' )	{
   gencode(ligfac[liga].codprod, liga);
   break;
					}
 if (ligfac[liga].codprod == NULL)	{
   if ( isligvid_sfac(liga) == 0) ligfac[liga].typlig = LQQ;
   break;
					}
		// acces db prod
 if (ligfac[liga].codprod != NULL)	{
  lsfprod.cleon = lit1c_fprod(ligfac[liga].codprod);
  if ( lsfprod.cleon == 1)	{ // trouve
   temactu = 0;
   llib = strlen(lsfprod.libprod);
//printf("%s..%d\n",lsfprod.libprod,llib);
   ligg = liga;  j=0;
   for (i = 0; i < llib; ++i)	{	// fin de ligne = 10
    if ( lsfprod.libprod[i] == 10 || i >= llib - 1)	{
 	if ( ligg > liga && isligvid_sfac(ligg) == -1) 	{
	    if (sfac_inslig2(ligg) == TRUE) return -1;
	    temactu = 1;
							}
      if (i == llib -1)	{	// derniere ligne
	++i;
			}
	// si ya pas de fin de ligne et ligne longue
	// ligfac..libprod 50 car affiche 40 marge pour utf8
      if ( i-j > 44) i = j + 44;
      razl_sfac(ligg);
	// remet codprod
      strcpy(ligfac[liga].codprod,lsfprod.cleprod);
      afcell_sfac(CSFcodprod,ligg);
      strncpy(ligfac[ligg].libprod,&lsfprod.libprod[j],i-j);
      afcell_sfac(CSFlibprod,ligg);
      ligfac[ligg].typlig = DETLIB;
      ++ligg;	j = i +1;
							}
				}
    --ligg;
    strncpy(ligfac[ligg].unit,lsfprod.unit,4);
    afcell_sfac(CSFunit,ligg);
    ligfac[ligg].priunit = lsfprod.priunit;
    afcell_sfac(CSFpriunit,ligg);
	// verif code taxe avant
     strncpy(ligfac[ligg].codtax,lsfprod.codtax,2);
     afcell_sfac(CSFcodtax,ligg);
bloqchang = 1;	// inhibe chang
 /* modif 27:9:2015 verif taxe que si calcul valor
 if ( valtau(ligfac[ligg].codtax[0]) == 0)	{
   ret = -1;   message(10);
						}   */
 if ( temactu == 1) actu_sfac();
bloqchang = 0;	// inhibe chang
 gtk_widget_grab_focus(ligfac_wgt[ligg*CSFnco+CSFquant].wdg);
				}	// fin trouve
/* else	{	// code inconnu
  message(8);
  ligfac[liga].codprod[0] = '\0';
  afcell_sfac(CSFcodprod,liga);
  gtk_widget_grab_focus(ligfac_wgt[liga*CSFnco+CSFcodprod].wdg);
	} */
					}
 break;
case CSFpriunit:
  if ( (ret = mdctoi(ligfac[liga].spriunit)) == 0) {
   ligfac[liga].priunit = zslon;		//maj ici
					}
  else  {
   ligfac[liga].priunit = 0;
	}
  afcell_sfac(CSFpriunit,liga);		// meme si erreur
 /* modif 27:9:2015 verif taxe que si calcul valor
 if ( valtau(ligfac[liga].codtax[0]) == 0)	{
   ret = -1;   message(10);
						}   */
// break;  doit faire calcul de quant
case CSFquant:
 dvalor = ligfac[liga].valor;
 ligfac[liga].valor = arron2(ligfac[liga].quant,ligfac[liga].priunit);
 afcell_sfac(CSFvalor,liga);
 //ligfac[liga].typlig = DETAIL; // modif 26:9:2015
 if ( ligfac[liga].valor != 0)  {
  ligfac[liga].typlig = DETAIL;
 // modif 27:9:2015 verif taxe que si calcul valor
   if ( valtau(ligfac[liga].codtax[0]) == -1)	{
    ret = -1;   message(10);
						}
                                }
 else  ligfac[liga].typlig = LQQ;
 if ( ligfac[liga].valor != dvalor) actu_sfac();
 break;
case CSFcodtax:
 if ( valtau(ligfac[liga].codtax[0]) == -1)	{
   ret = -1;   message(10); break;
//printf("errtax taxe\n");
						}
 actu_sfac();
 break;
//default:
}
if ( liga >= derlig)	{
 derlig = liga + 1;
 affi_derlig();
			}
//printf("traifac.pw=%p cod=%s quant=%f u=%s priu=%f\n",pw,ligfac[liga].codprod,ligfac[liga].quant,ligfac[liga].unit,ligfac[liga].priunit);
return ret;
}

void afcell_sfac(int idcol, int liga)
{
char zt[17] = "                \0";
char zn[20],zn2[20];
switch (idcol)
{
case CSFcodprod:
 gtk_entry_set_text(GTK_ENTRY (ligfac_wgt[liga*CSFnco+idcol].wdg),ligfac[liga].codprod);
 break;
case CSFlibprod:
 gtk_entry_set_text(GTK_ENTRY (ligfac_wgt[liga*CSFnco+idcol].wdg),ligfac[liga].libprod);
 break;
case CSFquant:
 if ( ligfac[liga].quant != 0)	{
   formfloat(zn2,ligfac[liga].quant);
   sprintf(zn,"%s",zn2);
//  sprintf(zn,colsfac[CSFquant].forma,ligfac[liga].quant);
   gtk_entry_set_text(GTK_ENTRY (ligfac_wgt[liga*CSFnco+idcol].wdg),zn);
				}
 else gtk_entry_set_text(GTK_ENTRY (ligfac_wgt[liga*CSFnco+idcol].wdg),"");
 break;
case CSFpriunit:
 if ( ligfac[liga].priunit != 0)  {
   ltof(ligfac[liga].priunit,zt,13);
   strcpy(ligfac[liga].spriunit,zt);
//printf("spri %s %d\n",ligfac[liga].spriunit,strlen(ligfac[liga].spriunit));
				}
 else strcpy(ligfac[liga].spriunit,"\0");	// pb ligne vide blanc si zéro
  gtk_entry_set_alignment(GTK_ENTRY (ligfac_wgt[liga*CSFnco+idcol].wdg),1);
  gtk_entry_set_text(GTK_ENTRY (ligfac_wgt[liga*CSFnco+idcol].wdg),ligfac[liga].spriunit);
 break;
case CSFunit:
 gtk_entry_set_text(GTK_ENTRY (ligfac_wgt[liga*CSFnco+idcol].wdg),ligfac[liga].unit);
 break;
case CSFcodtax:
 gtk_entry_set_text(GTK_ENTRY (ligfac_wgt[liga*CSFnco+idcol].wdg),ligfac[liga].codtax);
 break;
case CSFvalor:
 if ( ligfac[liga].valor != 0)  {
  ltof(ligfac[liga].valor,zt,13);
  strcpy(ligfac[liga].svalor,zt);	// att copie \0 de zt soit 16 car
//printf("sval %s %d\n",ligfac[liga].svalor,strlen(ligfac[liga].svalor));
  gtk_label_set_justify (GTK_LABEL (ligfac_wgt[liga*CSFnco+idcol].wdg), GTK_JUSTIFY_RIGHT );
  gtk_label_set_text(GTK_LABEL(ligfac_wgt[liga*CSFnco+idcol].wdg),zt);
				}
 else   {
    strcpy(ligfac[liga].svalor,zt+3);	// modif 26:9:2015 blanc si valor=0 (edifast)
    gtk_label_set_text(GTK_LABEL(ligfac_wgt[liga*CSFnco+idcol].wdg),NULL);
        }
 break;
}
if ( liga >= derlig) {
 derlig = liga + 1;
 affi_derlig();
//printf("afcell derlig= %d lig= %d col= %d\n",derlig,liga,idcol);
		}
if (autop.acti == 1) inhilistprod_sfac();
}

int gencode(char *code,int liga)	//recurssive
{
int ligg;
//printf("code= %s liga= %d\n",code,liga);
ligfac[liga].typlig = LQQ;
if ( strcmp(code,".v_1") == 0)	{
 gtk_label_set_text(GTK_LABEL(ligfac_wgt[liga*CSFnco+CSFvalor].wdg)
,"_______________");
 ligfac[liga].typlig = GENCAL;
				}
else if ( strcmp(code,".v_2") == 0)	{
 gtk_label_set_text(GTK_LABEL(ligfac_wgt[liga*CSFnco+CSFvalor].wdg)
,"===============");
 ligfac[liga].typlig = GENCAL;
//  strcpy(ligfac[liga].codprod,".v_2");
//  afcell_sfac(CSFcodprod,liga);
					}
else if ( strcmp(code,".tht") == 0)   cod_tht(liga);
else if ( strcmp(code,".ttc") == 0)   cod_ttc(liga);
else if ( strcmp(code,".ttt") == 0)		{	// .ttt
  if ( code_double(".ttt",liga) != -1) return liga;
  gencode(".v_1",liga++);
  if ( isligvid_sfac(liga) == -1 &&
     ligfac[liga].typlig < GENCAL )	{
    if (sfac_inslig2(liga) == TRUE) return -1;
					}
  gencode(".tht",liga++);
  ligg = cod_tax(liga,TAX);
//  ligg = gencode(".tax",liga);
  if ( ligg == -1) return ligg;
  liga = ligg;
  if ( isligvid_sfac(liga) == -1 &&
     ligfac[liga].typlig < GENCAL )	{
    if (sfac_inslig2(liga) == TRUE) return -1;
					}
  strcpy(ligfac[liga].codprod,".v_2");
  afcell_sfac(CSFcodprod,liga);
//  ligfac[liga].typlig = LIBCAL;
  gencode(".v_2",liga++);
  if ( isligvid_sfac(liga) == -1 &&
     ligfac[liga].typlig < GENCAL )	{
    if (sfac_inslig2(liga) == TRUE) return -1;
					}
  gencode(".ttc",liga++);
  gtk_widget_grab_focus(ligfac_wgt[liga*CSFnco+CSFnco].wdg);
						} // fin .ttt
else if ( strcmp(code,".eee") == 0)		{	// .eee
  if ( code_double(".eee",liga) != -1) return liga;
  gencode(".v_2",liga++);
  if ( isligvid_sfac(liga) == -1 &&
     ligfac[liga].typlig < GENCAL )	{
    if (sfac_inslig2(liga) == TRUE) return -1;
					}
  cod_ttc2(liga++);	//gencode(".ttc2",liga++);
  ligg = cod_tax(liga,TIX);
  if ( ligg == -1) return ligg;
  liga = ligg;
	// efface lignes calcul precedent
  for ( ligg = liga; ligg < (liga +10); ++ligg)	{
    if (ligfac[ligg].typlig >= GENCAL) razl_sfac(ligg);
						}
  gtk_widget_grab_focus(ligfac_wgt[liga*CSFnco+CSFnco].wdg);
						} // fin .eee
else if ( strcmp(code,".ttc2") == 0)   cod_ttc2(liga);
else if ( strcmp(code,".ttx") == 0)   cod_ttx(liga);
else if ( strcmp(code,".tpp") == 0)   cod_tpp(liga);
else if ( strcmp(code,".tex") == 0)	{
  if ( code_double(".tex",liga) != -1) return liga;
//  ligfac[liga].typlig = GENCAL;
  gencode(".v_2",liga++);
  cod_ttx(liga++);
	// efface lignes calcul precedent
  for ( ligg = liga; ligg < (liga +10); ++ligg)	{
    if (ligfac[ligg].typlig >= GENCAL) razl_sfac(ligg);
						}
  gtk_widget_grab_focus(ligfac_wgt[liga*CSFnco+CSFnco].wdg);
					}
else if ( strcmp(code,".spag") == 0) ligfac[liga].typlig = SPAG;
else if ( strncmp(code,".tax",4) == 0)	{
// printf("gencode('.tax,'  !!\n");
  return(cod_tax(liga,TAX));
					}
else 	{
  sfac_afilib(code,liga);
// suite
	}
return liga;
}

void actilistprod_sfac(char* kleprod)	// charge ligne depuis list prod
{
autop.acti = 1;
strcpy(autop.kprod,kleprod);
gtk_widget_modify_bg (boutinsl, GTK_STATE_NORMAL, &colorins);
gtk_button_set_label(GTK_BUTTON(boutinsl),libinsP);
}

void inhilistprod_sfac()	// inhibe ligne depuis list prod
{
autop.acti = 0;
gtk_widget_modify_bg (boutinsl, GTK_STATE_NORMAL, &colorbut);
gtk_button_set_label(GTK_BUTTON(boutinsl),libinsN);
}

void actu_sfac()
{
int i,temtax;
temtax = 0;
for (i = 0; i <= derlig; ++i)	{
 if (ligfac[i].typlig >= GENCAL )	{
//printf("actu lig= %d typ= %d cod=%s\n",i,ligfac[i].typlig,ligfac[i].codprod);
  if ( strcmp(ligfac[i].codprod,".ttt") == 0) gencode(".v_1",i);
  else if ( strncmp(ligfac[i].codprod,".tax",4) == 0)	{
    if ( temtax == 0) cod_tax(i,ligfac[i].typlig);
    // 1er ligne taxe genere la suite
//    if ( temtax == 0) gencode(".tax",i); // 1er ligne taxe genere la suite
    temtax = 1;		continue;
							}
  else if ( strcmp(ligfac[i].codprod,".tex") == 0) gencode(".v_2",i);
  else if ( strcmp(ligfac[i].codprod,".eee") == 0) gencode(".v_2",i);
  else gencode(ligfac[i].codprod, i);
					}
				}
}

int wherecode(char *code)
{
int i;
for (i = 0; i < CSFnli; ++i)	{
 if ( strcmp(ligfac[i].codprod,code) == 0) return i;
				}
return -1;
}

int code_double(char *code, int liga)
{
int i;
for (i = 0; i < CSFnli; ++i)	{
 if ( i == liga) continue;
 if ( strcmp(ligfac[i].codprod,code) == 0)	{
  message(12);
  ligfac[liga].codprod[0] = '\0';
  afcell_sfac(CSFcodprod,liga);
  gtk_widget_grab_focus(ligfac_wgt[liga*CSFnco+CSFcodprod].wdg);
  return i;
						}
				}
return -1;
}

void cod_tht(int liga)
{
int i;
long tht = 0L;
if ( code_double(".tht",liga) != -1) return;
for (i = 0; i < liga; ++i)	{
 if (ligfac[i].typlig == DETAIL ) tht += ligfac[i].valor;
				}
ligfac[liga].valor = tht;
sfac_afilib(".tht",liga);
afcell_sfac(CSFvalor,liga);
ligfac[liga].typlig = CALTOT;
strcpy(ligfac[liga].codprod,".tht");
afcell_sfac(CSFcodprod,liga);
}

void cod_ttc(int liga)
{
int i;
long ttc = 0L;
if ( code_double(".ttc",liga) != -1) return;
i = wherecode(".tht");  // cherche ligne tht
if ( i > liga)	return; // ligne .tht apres !
if ( i == -1)	{	// pas trouve on CALTOTcul tht et retour
 cod_tht(liga);
 strcpy(ligfac[liga].codprod,".ttc");
 return;
		}
for ( ; i < liga; ++i)	{	//somme depuis tht
// if (ligfac[i].codprod[0] != '.' )
 ttc += ligfac[i].valor;
				}
ligfac[liga].valor = ttc;
sfac_afilib(".ttc",liga);
afcell_sfac(CSFvalor,liga);
strcpy(ligfac[liga].codprod,".ttc");
ligfac[liga].typlig = CALTOT;
afcell_sfac(CSFcodprod,liga);
}

void cod_ttc2(int liga)
{
  cod_tht(liga);
	// modif libelle
  sfac_afilib(".ttc",liga);
  strcpy(ligfac[liga].codprod,".ttc2");
  afcell_sfac(CSFcodprod,liga);
}

void cod_ttx(int liga)
{
int i;
long tex = 0L;
for (i = 0; i < liga; ++i)	{
 if (ligfac[i].typlig == DETAIL ) tex += ligfac[i].valor;
				}
ligfac[liga].valor = tex;
sfac_afilib(".ttx",liga);
afcell_sfac(CSFvalor,liga);
strcpy(ligfac[liga].codprod,".ttx");
ligfac[liga].typlig = CALTOT;
afcell_sfac(CSFcodprod,liga);
}

void cod_tpp(int liga)
{
int i;
long tpp = 0L;
if ( liga == 0) return;
for (i = liga - 1; i >= 0; --i)	{
 if (ligfac[i].typlig == TPP ) break;
 if (ligfac[i].typlig == DETAIL ) tpp += ligfac[i].valor;
				}
ligfac[liga].priunit = tpp;
sfac_afilib(".tpp",liga);
afcell_sfac(CSFpriunit,liga);
strcpy(ligfac[liga].codprod,".tpp");
ligfac[liga].typlig = TPP;
afcell_sfac(CSFcodprod,liga);
}

void quellestaxes(int liga)
{
int i;
char ct;
tabtax[0]='\0';
nbtax=0;
for (i = 0; i < liga; ++i)	{
 if (ligfac[i].typlig != DETAIL ) continue;
 ct = ligfac[i].codtax[0];
 if ( ct == '\0')   ct = '?';
 if ( ctcar(ct,tabtax) == 1)   {
   tabtax[nbtax++] = ct;
   tabtax[nbtax] = '\0';
				}
				}
}

void basetax(char cta, int liga)
{
int i;
btax = 0L;
for (i = 0; i < liga; ++i)	{
 if (ligfac[i].typlig != DETAIL ) continue;
 if ( cta == ligfac[i].codtax[0]) btax += ligfac[i].valor;
				}
}

int cod_tax(int liga, int temht)
{
double vtax,coef;
int k,j,ligg;
long taxe,bht;
char libtax[40];
char zt[17] = "                \0";
quellestaxes(liga);
	// pas de taxes (lignes detail pas encore saisies)
 if (tabtax[0] == '\0')	{
  razl_sfac(liga);
  strcpy(ligfac[liga].codprod,".tax");
  afcell_sfac(CSFcodprod,liga);
  ligfac[liga].typlig = TAX;
  return ++liga;
			}
	//printf("nbtax= %d tabtax= %s.\n",nbtax,tabtax);
ligg = liga;
for ( k= 0; k < nbtax; ++k)	{
	// cherche taxe ct
  vtax = valtau(tabtax[k]);
//  modif 28:9:2015
  if ( vtax == -1)	   { message(10);  vtax =0; }
  vtax /= 100;
  basetax(tabtax[k],liga);
	// edition ligne taxe

	// ligne deja utilisee ?
  //printf("lig=%d vtax %f %f\n",ligg,vtax,ligfac[ligg].quant);
  if ( isligvid_sfac(ligg) == -1 &&
     strncmp(ligfac[ligg].codprod,".tax",4) != 0 ) {
	if (sfac_inslig2(ligg) == TRUE) return -1;
						   }
 bloqchang = 1;	// inhibe chang
 strcpy(ligfac[ligg].codprod,".tax");
 ligfac[ligg].codprod[4] = tabtax[k];
 afcell_sfac(CSFcodprod,ligg);
 j=wheretabcode(".tax");
 if ( temht == TAX)	{	//  detail est hors taxes
   ligfac[ligg].priunit = btax;
   taxe = arron2((vtax/100.0),btax);
   ligfac[ligg].valor = taxe;
   afcell_sfac(CSFvalor,ligg);
   ligfac[ligg].typlig = TAX;
   if ( j >= 0)	{	// libelle taxe
     sprintf(libtax,"%s %#0.1f %%", ligcod[j].lib, vtax);
     strcpy(ligfac[ligg].libprod,libtax);
		}
			}
 else if ( temht == TIX)	{	//  detail est toutes taxes
   ligfac[ligg].valor = 0;
   coef = 100 / ( 100 + vtax);
   bht = arron2(coef,btax);
   ligfac[ligg].priunit = bht;
   taxe = btax - bht;
   ligfac[ligg].typlig = TIX;
   if ( j >= 0)	{	// libelle taxe
     ltof(taxe,zt,13);
     sprintf(libtax,"%s %#0.1f %%  %s", ligcod[j].lib, vtax,zt);
     strcpy(ligfac[ligg].libprod,libtax);
		}
			}
 afcell_sfac(CSFlibprod,ligg);
 afcell_sfac(CSFpriunit,ligg);

 bloqchang = 0;	//  chang
 ++ligg;
 if ( ligg >= CSFnli)	{
	// message tableau plein
    message(11);
    ligg = -1;
    return -1; //break;
			}
				}   // fin tableau taxes k
	// ligne taxe en trop
if ( isligvid_sfac(ligg) == -1 &&
 strncmp(ligfac[ligg].codprod,".tax",4) == 0 )	{
 razl_sfac(ligg);
						}
return ligg;
}


void sfac_afilib(char *code,int liga)
{
int j;
j=wheretabcode(code);
if ( j == -1)	return;
 strcpy(ligfac[liga].libprod,ligcod[j].lib);
 afcell_sfac(CSFlibprod,liga);
	// suite
while ( ligcod[j].suit == '+')	{
 ++j; ++liga;
 if ( isligvid_sfac(liga) == -1 &&
     strcmp(ligfac[liga].libprod,ligcod[j].lib) != 0 )	{
    if (sfac_inslig2(liga) == TRUE) return;
							}
 strcpy(ligfac[liga].libprod,ligcod[j].lib);
 afcell_sfac(CSFlibprod,liga);
				}
}

void init_sfac()
{
	// malloc tableaux
if ( ligfac != NULL) free(ligfac);	ligfac=NULL;
if ( ligfac_wgt != NULL) free(ligfac_wgt);	ligfac_wgt=NULL;
 init_para();
 fen_sfac.tli = CSFnli;
 ligfac = malloc( CSFnli * sizeof(S_LFAC));
 if ( ligfac == NULL) { message(15); return; }
 ligfac_wgt = malloc( CSFnco * CSFnli * sizeof(MEMO_T_WIDG));
 if ( ligfac_wgt == NULL) { message(15); return; }
 fen_sfac.ptwidg = ligfac_wgt;

// cretablo ((FnC*) &fen_sfac, (void*) &ligfac); ATT avec malloc ligfac=pointeur
 cretablo ((FnC*) &fen_sfac, (void*) ligfac);
 gtk_widget_show_all(fen_sfac.win);
 scod_lidb();
 senf.datfacq = 0;
 init2_sfac();
}

void init2_sfac()
{
int i,svdat,cbi;
for (i = 0; i < CSFnli; ++i)	{
 razl_sfac(i);
				}
raz_senf();
derlig = 0;
svdat = senf.datfacq;
if ( svdat != 0 && svdat != today.datq)	{
  if ( question(24) == 0) svdat = 0;
					}
if ( svdat == 0)	{
  strncpy(senf.datfac,today.sdat,12);
  senf.datfacq = today.datq;
  gtk_entry_set_text(GTK_ENTRY (s_fsenf[DATFAC].wdg),senf.datfac);
			}
cbi = gtk_combo_box_get_active(GTK_COMBO_BOX(cbtypdoc));
if (cbi != 0) messavar("Modifier type de document si besoin");
affi_derlig();
//affi_doclu();
ficsauvdoc[0] = '\0';
gtk_label_set_text(GTK_LABEL(s_fsenf[DOCLU].wdg),NULL);
affi_ficcde();
gtk_widget_grab_focus(s_fsenf[NUMFAC].wdg);
inpw = ligfac_wgt;	// lig 0 col 0
}

void sfac_raztout(GtkWidget *widget, gpointer data)
{
	// date change ?
init2_sfac();
}

void affi_ficcde()
{
char *pnf = NULL;
  pnf = strrchr (ficcdimp, '/');
  if ( pnf == NULL) pnf = strrchr (ficcdimp, '\\');
  if ( pnf != NULL)
   gtk_label_set_text(GTK_LABEL(s_fsenf[FICCDE].wdg),pnf+1);
}

void affi_doclu()
{
char *pnf = NULL;
//  pnf = strrchr (ficsauvdoc, '/');
  pnf = strrchr (ficsauvdoc, SLAF);
  if ( pnf != NULL)
   gtk_label_set_text(GTK_LABEL(s_fsenf[DOCLU].wdg),pnf+1);
}

gboolean sfac_quit(GtkWidget *widget, gpointer data)
{
if ( derlig > 0)	{
 if ( question(21) != 0) return TRUE;
			}
if ( ligfac != NULL) free(ligfac);	ligfac=NULL;
if ( ligfac_wgt != NULL) free(ligfac_wgt);	ligfac_wgt=NULL;
//free(ligfac);
//free(ligfac_wgt);
return FALSE;
}

gboolean sfac_efflig (GtkWidget *widget, gpointer data)
{
int liga,temactu;
//DEF_L_FnC *pl;
if ( verifinpw() == FALSE) return FALSE;
bloqchang = 1;	// inhibe chang
temactu = 0;
//pl = inpw->pdefc;
liga=inpw->liga;
if ( ligfac[liga].valor != 0) temactu = 1;
razl_sfac(liga);
if ( temactu == 1) actu_sfac();
bloqchang = 0;	//  chang
gtk_widget_grab_focus(ligfac_wgt[liga*CSFnco].wdg);
return FALSE;
}

void razl_sfac(int liga)
{
int i;
bloqchang = 1;	// inhibe chang
memset ((char*)&ligfac[liga],0, sizeof(S_LFAC));
for ( i=0; i < CSFnco; ++i)
 afcell_sfac(i,liga);
bloqchang = 0;
}

void afilig_sfac(int liga)
{
int i;
for ( i=0; i < CSFnco; ++i)
 afcell_sfac(i,liga);
}

int isligvid_sfac(int liga)
{
int j;
char *pt;
 pt = (char*)&ligfac[liga];
 for (j =0; j < sizeof(S_LFAC); ++j)	{
   if ( *(pt+j) != '\0')	{
//	printf("lig %d %d =%s..\n",liga,j,pt+j);
	return -1;
				}
					}
return 0;
}

gboolean sfac_inslig2 (int liga)
{
int i;
bloqchang = 1;	// inhibe chang
//printf("ins2 derlig= %d\n",derlig);
if ( derlig >= (CSFnli - 1))	{
	// message
    message(11);
  bloqchang = 0;	// inhibe chang
  return TRUE;
			}
for ( i = derlig; i > liga; --i)	{
 memcpy((char*)&ligfac[i],(char*)&ligfac[i-1],sizeof(S_LFAC));
 afilig_sfac(i);
				}
 razl_sfac(liga);
bloqchang = 0;	// inhibe chang
return FALSE;
}

gboolean sfac_inslig (GtkWidget *widget, gpointer data)
{
int liga;
DEF_L_FnC *pl;
if ( verifinpw() == FALSE) return FALSE;
pl = inpw->pdefc;
liga=inpw->liga;
	// charge ligne prod depuis list
if (autop.acti == 1 && pl->idcol == CSFcodprod)	{
  strcpy(ligfac[liga].codprod,autop.kprod);
  afcell_sfac(CSFcodprod,liga);
  traifac(inpw,0);
  return FALSE;
			}
bloqchang = 1;	// inhibe chang
if ( sfac_inslig2(liga) == TRUE) return TRUE;
actu_sfac();
bloqchang = 0;	// inhibe chang
//gtk_widget_grab_focus(ligfac_wgt[liga*CSFnco+CSFnco].wdg);
gtk_widget_grab_focus(ligfac_wgt[liga*CSFnco].wdg);
return FALSE;
}

gboolean sfac_suplig (GtkWidget *widget, gpointer data)
{
int liga,i,temtax;
//DEF_L_FnC *pl;
if ( verifinpw() == FALSE) return FALSE;
bloqchang = 1;	// inhibe chang
//pl = inpw->pdefc;
liga=inpw->liga;
//printf("lig a sup= %d\n",liga);
	// si ligne taxe pas d'actu qui recree la ligne
temtax = 0;
if ( strncmp(ligfac[liga].codprod,".tax",4) == 0) temtax = 1;
for ( i = liga; i < derlig; ++i)	{
 memcpy((char*)&ligfac[i],(char*)&ligfac[i+1],sizeof(S_LFAC));
 afilig_sfac(i);
				}
--derlig;
 razl_sfac(derlig); // fait derlig + 1
--derlig;
if (temtax == 0) actu_sfac();
affi_derlig();
bloqchang = 0;	//  chang
gtk_widget_grab_focus(ligfac_wgt[liga*CSFnco].wdg);
return FALSE;
}

int fonc_senf (DEF_S_FQ *pzw, int temchang)
{
int lt,ret = 0;
GtkEditable *editable;
gchar *text;
if ( pzw == NULL) return ret;
switch (pzw->idcol)
{
case DATFAC:
 if ( da.datq > 0)	{
  strncpy(senf.datfac,da.sdat,12);
  senf.datfacq = da.datq;		// maj ici
  gtk_entry_set_text(GTK_ENTRY (s_fsenf[DATFAC].wdg),senf.datfac);
			}
 else	{ ret = -1; message(13); }
 break;
case CLECLI:
 if ( temchang == 1)	{	// traite change SSPEC
   if ( GTK_IS_WINDOW (winli)) 	{
    editable = GTK_EDITABLE (pzw->wdg);
    text = gtk_editable_get_chars (editable, 0, -1);
    strcpy(rkey,text);
    lt = strlen(rkey);
    if (lt > 0 && lt < 5 && *(rkey) != ' ') 	{
     rechcli((char*)rkey);
				        	}
    g_free(text);
				}
   break;
			}
 afadres_cli(senf.clecli);
 break;
}
return ret;
}

void afadres_cli(char* klecli)	// pour saisie et depuis liste cli
{
 lsfcli.cleon = lit1c_fcli(klecli);
 if ( lsfcli.cleon == 1)	{ // trouve
  strcpy(senf.clecli,klecli);	// si depuis liste
gtk_entry_set_text(GTK_ENTRY (s_fsenf[CLECLI].wdg), klecli);
  strcpy(senf.add1,lsfcli.add1);
  strcpy(senf.add2,lsfcli.add2);
  strcpy(senf.add3,lsfcli.add3);
  strcpy(senf.pays,lsfcli.pays);
  strcpy(senf.codpos,lsfcli.codpos);
  strcpy(senf.add4,lsfcli.add4);
afadres2_cli();
				}
}

void afadres2_cli()
{
gtk_entry_set_text(GTK_ENTRY (s_fsenf[ADD1].wdg), senf.add1);
gtk_entry_set_text(GTK_ENTRY (s_fsenf[ADD2].wdg), senf.add2);
gtk_entry_set_text(GTK_ENTRY (s_fsenf[ADD3].wdg), senf.add3);
gtk_entry_set_text(GTK_ENTRY (s_fsenf[PAYS].wdg), senf.pays);
gtk_entry_set_text(GTK_ENTRY (s_fsenf[CODPOS].wdg), senf.codpos);
gtk_entry_set_text(GTK_ENTRY (s_fsenf[ADD4].wdg), senf.add4);
}

void raz_senf()
{
senf.clecli[0]= '\0';
senf.numfac[0]= '\0';
senf.add1[0]= '\0';
senf.add2[0]= '\0';
senf.add3[0]= '\0';
senf.pays[0]= '\0';
senf.codpos[0]= '\0';
senf.add4[0]= '\0';
gtk_entry_set_text(GTK_ENTRY (s_fsenf[CLECLI].wdg), senf.clecli);
gtk_entry_set_text(GTK_ENTRY (s_fsenf[NUMFAC].wdg), senf.numfac);
afadres2_cli();
}

void affi_derlig()
{
char form[]="%d/%d";
char zef[10];
sprintf(zef,form,derlig,CSFnli);
gtk_label_set_text(GTK_LABEL(s_fsenf[NBLIG].wdg),zef);
}

gboolean litdoc(GtkWidget *widget, gpointer data)
{
int i;
if ( derlig > 0)	{
 if ( question(21) != 0) return FALSE;
			}
init2_sfac();
relitdoc();
bloqchang = 1;	//  chang
gtk_entry_set_text(GTK_ENTRY (s_fsenf[CLECLI].wdg), senf.clecli);
gtk_entry_set_text(GTK_ENTRY (s_fsenf[NUMFAC].wdg), senf.numfac);
afadres2_cli();
for (i = 0; i < derlig; ++i)	{
 afilig_sfac(i);
				}
affi_derlig();
bloqchang = 0;	//  chang
return FALSE;
}

void boutsfac(GtkWidget *box)
{
GtkWidget* fixg;
GtkWidget *boxent;
GtkWidget *boutreli;
GtkWidget *boutsauv;
GtkWidget *boutcdimp;
GtkWidget *boutrazt;
GtkWidget *boutcode;
//GtkWidget *boutinsl;
GtkWidget *boutsupl;
GtkWidget *bimprim;
GtkWidget *bouefface;
/*GdkColor   colortabs;
colortabs.red=30000;//65535;
colortabs.green=45000;//65535;
colortabs.blue=0;*/
boxent = gtk_hbox_new(FALSE, 1);	// horizontal
 gtk_box_pack_start(GTK_BOX(box), boxent, FALSE, FALSE, 0);
fixg = gtk_fixed_new();
gtk_container_add(GTK_CONTAINER(boxent), fixg);

cbtypdoc = gtk_combo_box_new_text();
  gtk_widget_set_size_request(cbtypdoc, 100, 25);
  gtk_fixed_put(GTK_FIXED(fixg), cbtypdoc, 5,YAD2);
gtk_combo_box_append_text(GTK_COMBO_BOX(cbtypdoc), "Facture_0");
gtk_combo_box_append_text(GTK_COMBO_BOX(cbtypdoc), "Devis_1");
gtk_combo_box_append_text(GTK_COMBO_BOX(cbtypdoc), "Proforma_2");
gtk_combo_box_append_text(GTK_COMBO_BOX(cbtypdoc), "Command_3");
gtk_combo_box_append_text(GTK_COMBO_BOX(cbtypdoc), "Xxx_4");
 gtk_combo_box_set_active(GTK_COMBO_BOX(cbtypdoc), 0);
bimprim = gtk_button_new_with_label("Impression");
  gtk_widget_set_size_request(bimprim, 100, 24);
  gtk_fixed_put(GTK_FIXED(fixg), bimprim,295,YAD3);

creparfenq((DEF_L_FQ*) &lib_fsenf,(DEF_S_FQ*) &s_fsenf,fixg,&senf);

boutreli = gtk_button_new_with_label("Recharg.doc");
  gtk_widget_set_size_request(boutreli, 100, 24);
  gtk_fixed_put(GTK_FIXED(fixg), boutreli, 15,YAD1);
boutsauv = gtk_button_new_with_label("Sauv.doc");
  gtk_widget_set_size_request(boutsauv, 90, 24);
  gtk_fixed_put(GTK_FIXED(fixg), boutsauv, 245,YAD1);
boutcdimp = gtk_button_new_with_label("Fic Mis en page");
  gtk_widget_set_size_request(boutcdimp, 125, 24);
  gtk_fixed_put(GTK_FIXED(fixg), boutcdimp,460,YAD1);
boutrazt = gtk_button_new_with_label("Efface tout");
  gtk_widget_set_size_request(boutrazt, 95, 24);
  gtk_fixed_put(GTK_FIXED(fixg), boutrazt,730,YAD1);
boutcode = gtk_button_new_with_label("Codes");
  gtk_widget_set_size_request(boutcode, 70, 24);
  gtk_fixed_put(GTK_FIXED(fixg), boutcode, 5,YAD4);
boutinsl = gtk_button_new_with_label(libinsN);	// button ins.ligne
  gtk_widget_set_size_request(boutinsl, 80, 24);
  gtk_fixed_put(GTK_FIXED(fixg), boutinsl,85,YAD4);
boutsupl = gtk_button_new_with_label("Sup.ligne");
  gtk_widget_set_size_request(boutsupl, 80, 24);
  gtk_fixed_put(GTK_FIXED(fixg), boutsupl,175,YAD4);
bouefface = gtk_button_new_with_label("Efface Ligne");
  gtk_widget_set_size_request(bouefface, 100, 24);
  gtk_fixed_put(GTK_FIXED(fixg), bouefface,260,YAD4);
gtk_widget_modify_bg (boutreli, GTK_STATE_NORMAL, &colorbut);
gtk_widget_modify_bg (boutsauv, GTK_STATE_NORMAL, &colorbut);
gtk_widget_modify_bg (boutcdimp, GTK_STATE_NORMAL, &colorbut);
gtk_widget_modify_bg (boutrazt, GTK_STATE_NORMAL, &colorbut);
gtk_widget_modify_bg (boutcode, GTK_STATE_NORMAL, &colorbut);
gtk_widget_modify_bg (boutinsl, GTK_STATE_NORMAL, &colorbut);
gtk_widget_modify_bg (boutsupl, GTK_STATE_NORMAL, &colorbut);
gtk_widget_modify_bg (bimprim, GTK_STATE_NORMAL, &colorbut);
gtk_widget_modify_bg (bouefface, GTK_STATE_NORMAL, &colorbut);
 g_signal_connect(G_OBJECT(boutreli), "clicked", G_CALLBACK (litdoc),NULL);
 g_signal_connect(G_OBJECT(boutsauv), "clicked", G_CALLBACK (sauvdoc),NULL);
 g_signal_connect(G_OBJECT(boutcdimp), "clicked", G_CALLBACK (choix_fimipa),NULL);
 g_signal_connect(G_OBJECT(boutrazt), "clicked", G_CALLBACK (sfac_raztout),NULL);
 g_signal_connect(G_OBJECT(boutcode), "clicked", G_CALLBACK (act_fenscod),NULL);
 g_signal_connect(G_OBJECT(bimprim), "clicked", G_CALLBACK (sfac_imprim),NULL);
//  g_signal_connect(cbtypdoc, "changed", G_CALLBACK(cbtypdoc_sfac), NULL);
 g_signal_connect(G_OBJECT(bouefface), "clicked", G_CALLBACK (sfac_efflig),NULL);
 g_signal_connect(G_OBJECT(boutinsl), "clicked", G_CALLBACK (sfac_inslig),NULL);
 g_signal_connect(G_OBJECT(boutsupl), "clicked", G_CALLBACK (sfac_suplig),NULL);
// g_signal_connect(G_OBJECT(bouton2), "clicked", G_CALLBACK (sfac_dupli),NULL);

  g_signal_connect(fen_sfac.win, "delete_event", G_CALLBACK (sfac_quit), NULL);

}

/*

gboolean sfac_dupli (GtkWidget *widget, gpointer data)
{
int liga;
DEF_L_FnC *pl;
char *par,*por;
pl = inpw->pdefc;
liga=inpw->liga;
if (liga == 0) return (-1);
switch (pl->idcol)
{
case CSFquant:
 ligfac[liga].quant = ligfac[liga-1].quant;
 break;
case CSFpriunit:
 ligfac[liga].priunit = ligfac[liga-1].priunit;
 break;
default:
par= inpw->memcell;
//por = (inpw - CSFnco*sizeof(MEMO_T_WIDG))->memcell;
por = (inpw - CSFnco)->memcell;
printf("click bout dupli w=%p \n",inpw->wdg);
	// verif si click vient du bon tableau
if (por < (char*) &ligfac || par > (char*) &ligfac[CSFnli-1].priunit) {
  printf("dupli hors table\n");
  return (-1);	}
strncpy(par,por,pl->laf);
				}
afcell_sfac(pl->idcol,liga);
gtk_widget_grab_focus (inpw->wdg);
//aftablo((FnC*) &fen_sfac,(S_LFAC*) &ligfac,liga);
return FALSE;
}

*/
