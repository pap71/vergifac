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
#include <cairo.h>
#include <cairo-pdf.h>
#ifdef WIN32
#include <windows.h>
#include <shellapi.h>
#endif

int edit_fac();
int isolmot(char* tex, int ll);
void interp();
int param0();
int cherpara(char *para);
void cairotext();
void memopol();
void implf();
void cairotextlf(int nc);
int paramlf(int nc);
void sauvfac();
int entete();

extern S_SENF senf;
extern S_LFAC *ligfac;
extern GtkWidget *cbtypdoc;
extern int derlig;
extern S_CODFAC ligcod[];

// variable fichier
static FILE *fis =NULL;
char ficcdimp[80];
static char ze [256];
/* static char zm [128]; init si besoin */
static char nomfic[]="factest.pdf";

struct sptmo {
  char *pmot;
  short lmot;
};
static struct sptmo tabmot[24];
static int nmot=0;
static char separ[] = " ,'=(){};:*\"\t";

static double width,height;
static int nbpol,ydeb,dy,yfin,nuli;
typedef  struct {
  char pol[8];
  //int slant;
  //int weight;
  char face[30];
} S_TABPOL;
static S_TABPOL tabpol[20];

typedef struct {
  int x;
  int y;
  int nupol;
  int slant;
  int weight;
  double siz;
} S_PARAF;
static S_PARAF par0;

enum {
  CSFcodprod,
  CSFlibprod,
  CSFquant,
  CSFunit,
  CSFctax,
  CSFpu,
  CSFval,
  CSFlibvar,
  CSFlibtot,
  CSFvaltot,
  CSFstot,
  CSFnco
};
static S_PARAF parlf[CSFnco];

cairo_surface_t *surface;
cairo_t *cr;
cairo_pattern_t *pattern;
cairo_surface_t *etsurf;
cairo_status_t   status;

void sfac_imprim()
{
  int ret=0;
  //GError **err;
  fis=fopen(ficcdimp,"r");
  if ( fis == NULL) { message(16); return;  }
  //printf("open %s\n",ficcdimp);
  ret = edit_fac();
  fclose (fis);	fis = NULL;
  if ( ret == -1)	{
    message (26);
    return;
  }
  //gboolean  g_spawn_command_line_async (const gchar *command_line, GError **error);
#ifdef WIN32
  //g_spawn_command_line_async("ShellExecute(0,'open','factest.pdf',0,0,1)",*err);
  ShellExecute(0,"open","factest.pdf",0,0,SW_SHOWNORMAL);
#else
  //system("start evince factest.pdf");
  system("xdg-open factest.pdf&"); // & permet de garder la main ?
  //system("gnome-open factest.pdf");
#endif

  if ( question(22) == 0)	sauvfac();
}

void sauvfac()
{
  GtkWidget *dialog;
  gchar *filename;
  gchar *nf = NULL;
  gchar *csys = NULL; /* Évite un dépassement mémoire */
  char typ[]="-F\0";
  int cbi;

  cbi = gtk_combo_box_get_active(GTK_COMBO_BOX(cbtypdoc));
  if ( cbi == 0)	typ[1] = 'F';
  else if ( cbi == 1)	typ[1] = 'D';
  else if ( cbi == 2)	typ[1] = 'P';
  else if ( cbi == 3)	typ[1] = 'C';
  else if ( cbi == 4)	typ[1] = 'X';
  nf = g_strdup_printf ("%s%s%s.pdf",senf.clecli,typ,senf.numfac);
  /* sprintf(nf,"%s%s%s.pdf",senf.clecli,typ,senf.numfac); */
  dialog = gtk_file_chooser_dialog_new ("Saisir un Nom pour archiver le document",
					GTK_WINDOW(NULL),
					GTK_FILE_CHOOSER_ACTION_SAVE,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
					NULL);
  gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER (dialog),nf);
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog),"./histo-fac");
  //gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);
  gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

  g_free (nf); nf = NULL;

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)  {
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    // filename = chemin absolu

#ifdef WIN32
    csys = g_strdup_printf ("copy factest.pdf %s",filename);
    /* sprintf(csys,"copy factest.pdf %s",filename); */
#else
    csys = g_strdup_printf ("cp factest.pdf %s",filename);
    /* sprintf(csys,"cp factest.pdf %s",filename); */
# endif
    g_free (filename);
  }
  gtk_widget_destroy (dialog);
  system(csys);

  g_free (csys);
}

// 1er lig fic txt windows 3 car EF BB BF = utf8
int lect_fis()
{
  int nlu;
  while ( fgets(ze,256,fis) != NULL)	{
    ++nuli;
    nlu = strlen(ze);
    --nlu;
    if ( nlu > 0 && ze[nlu-1] < 20)	{
      ze[nlu-1] = '\0';// CR+LF 0d 0a fin de ligne windows
      --nlu;
    }
    // printf("nuli= %d nlu=%d %s..\n",nuli,nlu,ze);
    if ( nlu == 0) continue;
    ze[nlu] = '\0';
    if ( strncmp(ze, "/*", 2) == 0) continue;
    if ( ze[0] == ' ')	continue;
    return nlu;
  }
  return -1;
}

void sfac_spag()
{
  int nlu;
  fclose (fis);	fis = NULL;
  fis=fopen(ficcdimp,"r");
  if ( fis == NULL) { message(16); return;  }
  nuli=0;
  while ( (nlu=lect_fis()) > 0 )		{
    if ( strncmp(ze, "PAGSUIV",7) == 0) break;
  }
  while ( (nlu=lect_fis()) > 0 )		{
    nmot = isolmot(ze,nlu);
    interp();
  }
}

int edit_fac()
{
  int nlu,temwh,i,temet;
  width = 0; height=0; nbpol =0; temwh = 0; temet = 0;
  ydeb=0; dy=0;	nuli=0;
  for ( i=0; i < CSFnco; ++i) parlf[i].y = 0;

  while ( (nlu=lect_fis()) > 0 )		{
    if ( strncmp(ze, "PAGSUIV",7) == 0) break;
    if ( ze[0] == '#')	{
      nmot = isolmot(ze,nlu);
      memopol();
      continue;
    }
    else if ( ze[0] == '$')	{
      nmot = isolmot(ze,nlu);
      temet = entete();
      continue;
    }
    // param lu  active cairo
    if (temwh == 0 && width != 0 && height != 0)	{
      surface = cairo_pdf_surface_create(nomfic, width,height);
      status = cairo_surface_status(surface);
      if (!surface || status!=CAIRO_STATUS_SUCCESS)	{
	//     printf("erreur cairo fic pdf\n");
	message(25);
	return -1;
      }
      cr = cairo_create(surface);
      if ( temet == 1)	{	// active en tete
	cairo_save(cr);
	//cairo_scale(cr, 1./dpp, 1./dpp);
	pattern = cairo_pattern_create_for_surface(etsurf);
	cairo_translate(cr,0,0);
	cairo_pattern_set_filter(pattern, CAIRO_FILTER_BEST);
	cairo_set_source(cr,pattern);
	cairo_paint(cr);
	cairo_restore(cr);
      }
      cairo_set_source_rgb(cr, 0, 0, 0);
      temwh = 1;
    }
    if (temwh == 0) continue;	// cr,surface non crees
    // ou erreur ligne avant creation cairo systematique 1er lig windows fic txt
    nmot = isolmot(ze,nlu);
    interp();
  } // fin boucle lect fgets
  if (temwh == 0) return -1;		// cr, surface non crees
  for ( i=0; i < CSFnco; ++i)	{
    if (parlf[i].y == 1) { i=-1; break; } // y a-t-il des cde lignes ?
  }
  if ( i == -1)	implf();	// imprim lignes details
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
  return 0;
}

int isolmot(char* tex,int ll)    /*  isole les mots */
{
  short i,p,nmot;
  i = 0; nmot = 0;
  // printf("%s..\n",tex);
  //while ( tex[i] != '\0' )   {
  while ( i < ll )   {
    if ( tex[i] != '\0' && ctcar((char)tex[i], separ) == 1)  {
      tabmot[nmot].pmot = &tex[i];
      p = i;
      if ( i > 0 && tex[i - 1] == '"')
	while (tex[i] != '\0' && tex[i] != '"')  ++i;
      while (tex[i] != '\0' && ctcar((char)tex[i], separ) == 1)  ++i;
      if ( tex[i] == '=') ++i;
      else tex[i] = '\0';  // ecrase 1 car apres =
      tabmot[nmot].lmot = i - p;
      ++nmot;
    }
    else      ++i;
  }
  /*  for ( i= 0; i < nmot; ++i)   {
      printf("%s \t",tabmot[i].pmot);
      }
      printf("\n"); */
  return(nmot);
}

void interp()
{
  int cbi,nlu,k,j,svnuli;
  int x0,y0,xf,yf,dtxy;
  double wep=0.8;
  char typdoc[]=".doc0\0";
  char *pze;
if ( strcmp( ze,"ad1") == 0)    {
     if (param0() ==-1) return;
	 cairotext();	cairo_show_text(cr,senf.add1);
							    }
else if ( strcmp( ze,"ad2") == 0)	{
    if (param0() ==-1) return;	 
	cairotext();	cairo_show_text(cr,senf.add2);
								    }
else if ( strcmp( ze,"ad3") == 0)	{
    if (param0() ==-1) return;	
	cairotext();	cairo_show_text(cr,senf.add3);
								    }
else if ( strcmp( ze,"pays") == 0)	{
    if (param0() ==-1) return;	
	cairotext();	cairo_show_text(cr,senf.pays);
								    }
else if ( strcmp( ze,"codpos") == 0)	{
	if (param0() ==-1) return;
	cairotext();	cairo_show_text(cr,senf.codpos);
									    }
else if ( strcmp( ze,"ad4") == 0)   {
     if (param0() ==-1) return;	
	 cairotext();	cairo_show_text(cr,senf.add4);
								    }
else if ( strcmp( ze,"datdoc") == 0)    {
    if (param0() ==-1) return;	 
	cairotext();	cairo_show_text(cr,senf.datfac);
									    }
else if ( strcmp( ze,"numdoc") == 0)	{
    if (param0() ==-1) return;	
	cairotext();	cairo_show_text(cr,senf.numfac);
									    }
else if ( strcmp( ze,"typdoc") == 0)	{
    if (param0() ==-1) return;
    cbi = gtk_combo_box_get_active(GTK_COMBO_BOX(cbtypdoc));
    typdoc[4]= '0' + cbi;
    j=wheretabcode(typdoc);
    if ( j >= 0)	{
	  // text = gtk_combo_box_get_active_text((GtkComboBox*)cbtypdoc);
	  //printf("typ = %d %s\n", cbi,text);
	  cairotext();
	  cairo_show_text(cr, ligcod[j].lib);
					}
      // g_free(text);
    }
else if ( strcmp( ze,"entvar") == 0)   {
    if (param0() ==-1) return;
    if (fgets(ze,256,fis) == NULL)	return;
    ++nuli;
    nlu = strlen(ze);
    --nlu;
    if ( nlu > 0 && ze[nlu-1] < 20)	{
	  ze[nlu-1] = '\0';// CR+LF 0d 0a fin de ligne windows
	  --nlu;
									}
    ze[nlu] = '\0';
    cairotext();
    cairo_show_text(cr, ze);
									    }
else if ( strcmp( ze,"textml") == 0)	{
    if (param0() ==-1) return;
    if ( (k = cherpara( "dtxy=")) != -1)  dtxy = atoi(tabmot[++k].pmot);
    else dtxy = 12;		// dtxy = espacement entre lignes
    svnuli=nuli;
 while ( (pze = fgets(ze,256,fis)) != NULL)	{
  ++nuli;
  if ( strncmp( ze,"fintextml",9) == 0) break;
  nlu = strlen(ze);
  --nlu;
  if ( nlu > 0 && ze[nlu-1] < 20)	{
      ze[nlu-1] = '\0';// CR+LF 0d 0a fin de ligne windows
      --nlu;
								    }
  ze[nlu] = '\0';
  cairotext();
  cairo_show_text(cr, ze);
  par0.y += dtxy;
											} // fin while
  if ( pze == NULL)	{
	  gchar *zm = g_strdup_printf ("Ligne %d commande 'textml' (texte multilignes)\nn'a pas de fin (commande 'fintextml')",svnuli);
	  /* sprintf(zm,"Ligne %d commande 'textml' (texte multilignes)\nn'a pas de fin (commande 'fintextml')",svnuli); */
	  messavar(zm);
	  g_free (zm);
					}
								    }  // fin 'textml'
else if ( strcmp( ze,"line") == 0)    {
      x0 = y0 = 0;
      if ( (k = cherpara( "x=")) != -1)   x0 = atoi(tabmot[++k].pmot);
      if ( (k = cherpara( "y=")) != -1)   y0 = atoi(tabmot[++k].pmot);
      if ( (k = cherpara( "xf=")) != -1)   xf = atoi(tabmot[++k].pmot);
      else xf = x0;
      if ( (k = cherpara( "yf=")) != -1)   yf = atoi(tabmot[++k].pmot);
      else yf = y0;
      if ( (k = cherpara( "w=")) != -1)   wep = g_strtod(tabmot[++k].pmot,NULL);
      cairo_set_line_width (cr, wep);
      cairo_move_to(cr, x0,y0);
      cairo_line_to(cr, xf,yf);
      cairo_stroke(cr);
								    }
else if ( strcmp( ze,"rect") == 0)    {
      x0 = y0 = xf = yf = 100;
      if ( (k = cherpara( "x=")) != -1)   x0 = atoi(tabmot[++k].pmot);
      if ( (k = cherpara( "y=")) != -1)   y0 = atoi(tabmot[++k].pmot);
      if ( (k = cherpara( "xl=")) != -1)   xf = atoi(tabmot[++k].pmot);
      if ( (k = cherpara( "yh=")) != -1)   yf = atoi(tabmot[++k].pmot);
      // else yf = y0;
      if ( (k = cherpara( "w=")) != -1)   wep = g_strtod(tabmot[++k].pmot,NULL);
      cairo_set_line_width (cr, wep);
      cairo_rectangle(cr, x0, y0, xf, yf);
      //  cairo_move_to(cr, x0,y0);
      //  cairo_line_to(cr, xf,yf);
      cairo_stroke(cr);
 								   }
else if ( strcmp( ze,"ydeb") == 0) 	   {
      if ( (k = cherpara( "dy=")) != -1)   dy = atoi(tabmot[++k].pmot);
      if ( (k = cherpara( "y=")) != -1)   ydeb = atoi(tabmot[++k].pmot);
      if ( (k = cherpara( "yfin=")) != -1)   yfin = atoi(tabmot[++k].pmot);
									    }
else if ( strcmp( ze,"ydebsuiv") == 0) 	   {
      if ( (k = cherpara( "dy=")) != -1)   dy = atoi(tabmot[++k].pmot);
      if ( (k = cherpara( "y=")) != -1)   ydeb = atoi(tabmot[++k].pmot);
      if ( (k = cherpara( "yfin=")) != -1)   yfin = atoi(tabmot[++k].pmot);
										    }
else if ( strcmp( ze,"libprod") == 0)  paramlf(CSFlibprod);
else if ( strcmp( ze,"quant") == 0)  paramlf(CSFquant);
else if ( strcmp( ze,"unit") == 0)  paramlf(CSFunit);
else if ( strcmp( ze,"ctax") == 0)  paramlf(CSFctax);
else if ( strcmp( ze,"pu") == 0)  paramlf(CSFpu);
else if ( strcmp( ze,"val") == 0)  paramlf(CSFval);
else if ( strcmp( ze,"libvar") == 0)  paramlf(CSFlibvar);
else if ( strcmp( ze,"libtot") == 0)  paramlf(CSFlibtot);
else if ( strcmp( ze,"valtot") == 0)  paramlf(CSFvaltot);
else if ( strcmp( ze,"stot") == 0)  paramlf(CSFstot);
else if ( strcmp( ze,"codprod") == 0)  paramlf(CSFcodprod);
else    {
      gchar *zm = g_strdup_printf ("Fichier Mise en Page\nCode de ligne inconnu '%s' ligne %d",ze,nuli);
      /* sprintf(zm,"Fichier Mise en Page\nCode de ligne inconnu '%s' ligne %d",ze,nuli); */
      messavar(zm);
      g_free (zm);
	    }
}

void cairotext()
{
  cairo_select_font_face(cr, tabpol[par0.nupol].face, par0.slant,
			 par0.weight);
  cairo_set_font_size(cr, par0.siz);
  cairo_move_to(cr, par0.x, par0.y);
  //  cairo_show_text(cr, p);
}

void memopol()
{
  int k;
  if ( nbpol >= 20)	{
    message(18);
    return;
					  }
  if ( strcmp( ze,"#WIDTH") == 0)  width = g_strtod(tabmot[1].pmot,NULL);
  else if ( strcmp( ze,"#HEIGHT") == 0)  height = g_strtod(tabmot[1].pmot,NULL);
  else if ( strncmp( ze,"#p",2) == 0)	{
    strcpy(tabpol[nbpol].pol,tabmot[1].pmot);
    if ( (k = cherpara( "font=")) != -1)
      strcpy(tabpol[nbpol].face,tabmot[++k].pmot);
    ++nbpol;
									  }
}

int entete()
{
  //int ret;
  char *filePNG;
  if ( strcmp( ze,"$entet") == 0)  filePNG = tabmot[1].pmot;
  etsurf=cairo_image_surface_create_from_png(filePNG);
  status = cairo_surface_status(etsurf);
  if (!etsurf || status!=CAIRO_STATUS_SUCCESS)	{
    //     printf("erreur fic PNG\n");
    message(25);
    return -1;
  }
  //ww = cairo_image_surface_get_width(etsurf);
  //hh = cairo_image_surface_get_height(etsurf);
  /*
    cairo_save(cr);
    //cairo_scale(cr, 1./dpp, 1./dpp);
    //cairo_scale(cr, 2./dpp, 2./dpp);
    pattern = cairo_pattern_create_for_surface(etsurf);
    cairo_translate(cr,0,0);
    cairo_pattern_set_filter(pattern, CAIRO_FILTER_BEST);
  */
  return 1;
}

void cairotextlf(int nc)
{
  cairo_select_font_face(cr, tabpol[parlf[nc].nupol].face,
			 parlf[nc].slant, parlf[nc].weight);
  cairo_set_font_size(cr, parlf[nc].siz);
  //  cairo_move_to(cr, parlf[nc].x, par0.y);
  //  cairo_show_text(cr, p);
}

int paramlf(int nc)
{
  int k,i;
  // .y utilise pour marquer que l'info est demandee
  parlf[nc].y = 1;
  parlf[nc].x = -1;
  if ( (k = cherpara( "x=")) != -1)   parlf[nc].x = atoi(tabmot[++k].pmot);
  if ( parlf[nc].x == -1 )	{
    gchar *zm = g_strdup_printf ("Fichier Mise en Page\n x=  absent ligne '%s'",ze);
    /* sprintf(zm,"Fichier Mise en Page\n x=  absent ligne '%s'",ze); */
    messavar(zm);
    g_free (zm);
    return -1;
  }
  //if ( (k = cherpara( "y=")) != -1)   parlf[nc].y = atoi(tabmot[++k].pmot);
  if ( (k = cherpara( "siz=")) != -1)
    parlf[nc].siz = g_strtod(tabmot[++k].pmot,NULL);
  else  parlf[nc].siz =6;
  if ( (k = cherpara( "pol=")) != -1)	{
    ++k; parlf[nc].nupol=0;
    for (i=0; i < nbpol; ++i)	{
      if ( strcmp(tabmot[k].pmot,tabpol[i].pol) == 0)
		 { parlf[nc].nupol= i; break;}
 							   }
									  }
  else parlf[nc].nupol = 0;	// 1er police
  parlf[nc].slant = 0;
  parlf[nc].weight = 0;
  if ( (k = cherpara( "slant=")) != -1)	{
    ++k;
    if (strcmp(tabmot[k].pmot,"italic") == 0) parlf[nc].slant = 1;
    else if (strcmp(tabmot[k].pmot,"oblique") == 0) parlf[nc].slant = 2;
									  }
  if ( (k = cherpara( "w=")) != -1)	{
    ++k;
    if (strcmp(tabmot[k].pmot,"bold") == 0) parlf[nc].weight = 1;
								  }
  return 0;
}

int param0()
{
  int k,i;
  par0.x = -1;  par0.y = -1;
  if ( (k = cherpara( "x=")) != -1)   par0.x = atoi(tabmot[++k].pmot);
  if ( (k = cherpara( "y=")) != -1)   par0.y = atoi(tabmot[++k].pmot);
  if ( par0.x == -1 || par0.y == -1)	{
    gchar *zm = g_strdup_printf ("Fichier Mise en Page\n x= et/ou y= absents ligne '%s'",ze);
    /* sprintf(zm,"Fichier Mise en Page\n x= et/ou y= absents ligne '%s'",ze); */
    messavar(zm);
    g_free (zm);
    return -1;
									  }
if ( (k = cherpara( "siz=")) != -1)
	 par0.siz = g_strtod(tabmot[++k].pmot,NULL);
else par0.siz =6;
if ( (k = cherpara( "pol=")) != -1)	{
    ++k; par0.nupol=0;
    for (i=0; i < nbpol; ++i)	{
      if ( strcmp(tabmot[k].pmot,tabpol[i].pol) == 0) { par0.nupol= i; break;}
 							   }
								  }
else	{
  gchar *zm = g_strdup_printf ("Fichier Mise en Page\n pol= absent ligne '%s'",ze);
    /* sprintf(zm,"Fichier Mise en Page\n pol= absent ligne '%s'",ze); */
  messavar(zm);
  g_free (zm);
  par0.nupol = 0;	// 1er police
		  }
par0.slant = 0;
par0.weight = 0;
if ( (k = cherpara( "slant=")) != -1)	{
    ++k;
    if (strcmp(tabmot[k].pmot,"italic") == 0) par0.slant = 1;
    else if (strcmp(tabmot[k].pmot,"oblique") == 0) par0.slant = 2;
									  }
if ( (k = cherpara( "w=")) != -1)	{
    ++k;
    if (strcmp(tabmot[k].pmot,"bold") == 0) par0.weight = 1;
								  }
  //printf("xx= %d yy= %d zz=%lf\n",par0.x,par0.y,par0.siz);
return 0;
}

int cherpara(char *para)  /*cherche parametre et valeur */
{
  int nmv, ll;
  ll = strlen(para);
  for ( nmv = 0; nmv < nmot; ++nmv)  {
    //  for ( nmv = 0; nmv <= nmot; nmv++)  { vieux c !
    //	printf("nmot=%d para %s.. %s.. k= %d\n",nmot,para,tabmot[nmv].pmot,nmv);
    if ( strncmp( para, tabmot[nmv].pmot, ll) == 0) 	return(nmv);
 									 }
  return(-1);
}

void implf()
{
  int lf,yv,xv;
  double wep;
  //char zt[17] = "                \0";
  char  zn[20],zn2[20];

  yv = ydeb;
for ( lf=0; lf < derlig; ++lf)	{
  if ( yv >= yfin || ligfac[lf].typlig == SPAG)	{	// saut de page
      cairo_show_page(cr);
      sfac_spag();
      yv = ydeb;
											    }
if (ligfac[lf].typlig == DETAIL )	{
   if (  parlf[CSFlibprod].y != 0) {
	cairotextlf(CSFlibprod);
	cairo_move_to(cr, parlf[CSFlibprod].x, yv);
	cairo_show_text(cr, ligfac[lf].libprod);
							      }
   if (  parlf[CSFquant].y != 0)	{
	cairotextlf(CSFquant);
	cairo_move_to(cr, parlf[CSFquant].x, yv);
	formfloat(zn2,ligfac[lf].quant);
	sprintf(zn,"%10s",zn2);
	cairo_show_text(cr, zn);
 								     }
   if (  parlf[CSFunit].y != 0)	{
	cairotextlf(CSFunit);
	cairo_move_to(cr, parlf[CSFunit].x, yv);
	cairo_show_text(cr, ligfac[lf].unit);
						      }
   if (  parlf[CSFctax].y != 0)	{
	cairotextlf(CSFctax);
	cairo_move_to(cr, parlf[CSFctax].x, yv);
	cairo_show_text(cr, ligfac[lf].codtax);
						      }
   if (  parlf[CSFpu].y != 0)	{
	cairotextlf(CSFpu);
	cairo_move_to(cr, parlf[CSFpu].x, yv);
	cairo_show_text(cr, ligfac[lf].spriunit);
 							     }
   if (  parlf[CSFval].y != 0)	{
	cairotextlf(CSFval);
	cairo_move_to(cr, parlf[CSFval].x, yv);
	cairo_show_text(cr, ligfac[lf].svalor);
	//   ltof(ligfac[lf].valor,zt,13);
	//   cairo_show_text(cr, zt);
						      }
   if (  parlf[CSFcodprod].y != 0) {
	cairotextlf(CSFcodprod);
	cairo_move_to(cr, parlf[CSFcodprod].x, yv);
	cairo_show_text(cr, ligfac[lf].codprod);
 							     }
 									}
else if (ligfac[lf].typlig == DETLIB )	{
   if (  parlf[CSFlibprod].y != 0) {
	cairotextlf(CSFlibprod);
	cairo_move_to(cr, parlf[CSFlibprod].x, yv);
	cairo_show_text(cr, ligfac[lf].libprod);
 							     }
									    }
else if (ligfac[lf].typlig == LQQ )	{
      if (  parlf[CSFlibvar].y != 0) {
	cairotextlf(CSFlibvar);
	cairo_move_to(cr, parlf[CSFlibvar].x, yv);
	cairo_show_text(cr, ligfac[lf].libprod);
								      }
								    }
else if (ligfac[lf].typlig == TAX ||
   ligfac[lf].typlig == TIX)	{	// on se base sur DETAIL
   if (  parlf[CSFlibprod].y != 0) {
	cairotextlf(CSFlibprod);
	cairo_move_to(cr, parlf[CSFlibprod].x, yv);
	cairo_show_text(cr, ligfac[lf].libprod);
 							     }
   if (  parlf[CSFpu].y != 0)	{
	cairotextlf(CSFpu);
	cairo_move_to(cr, parlf[CSFpu].x, yv);
	cairo_show_text(cr, ligfac[lf].spriunit);
						      }
   if (  parlf[CSFval].y != 0)	{
	cairotextlf(CSFval);
	cairo_move_to(cr, parlf[CSFval].x, yv);
	cairo_show_text(cr, ligfac[lf].svalor);
	//   ltof(ligfac[lf].valor,zt,13);
	//   cairo_show_text(cr, zt);
						      }
 								   }
else if (ligfac[lf].typlig == CALTOT )	{
   if (  parlf[CSFlibtot].y != 0) {
	cairotextlf(CSFlibtot);
	cairo_move_to(cr, parlf[CSFlibtot].x, yv);
	cairo_show_text(cr, ligfac[lf].libprod);
 							     }
   if (  parlf[CSFvaltot].y != 0) {
	cairotextlf(CSFvaltot);
	cairo_move_to(cr, parlf[CSFvaltot].x, yv);
	cairo_show_text(cr, ligfac[lf].svalor);
	//   ltof(ligfac[lf].valor,zt,13);
	//   cairo_show_text(cr, zt);
 							     }
 									   }
else if (ligfac[lf].typlig == GENCAL )	{	// traits
   if (  parlf[CSFvaltot].y != 0) {	// on se base sur valtot
	cairotextlf(CSFvaltot);
	if ( strcmp(ligfac[lf].codprod,".ttt") == 0) wep = 0.7;
	else  wep = 1.4;
	xv = parlf[CSFvaltot].x + 5;
	cairo_set_line_width (cr, wep);
	cairo_move_to(cr, xv, yv);
	cairo_line_to(cr, xv + 80, yv); // 6 x 16
	cairo_stroke(cr);
 							     }
								    }
else if (ligfac[lf].typlig == TPP )	{
   if (  parlf[CSFlibtot].y != 0) {
	cairotextlf(CSFlibtot);
	cairo_move_to(cr, parlf[CSFlibtot].x, yv);
	cairo_show_text(cr, ligfac[lf].libprod);
 							     }
  if (  parlf[CSFstot].y != 0) {
	cairotextlf(CSFstot);
	cairo_move_to(cr, parlf[CSFstot].x, yv);
	cairo_show_text(cr, ligfac[lf].spriunit);
	//   ltof(ligfac[lf].valor,zt,13);
	//   cairo_show_text(cr, zt);
 						     }
 								   }
    yv += dy;
					  }	// fin lecture lignes fac
}





