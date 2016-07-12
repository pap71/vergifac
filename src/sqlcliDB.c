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

static int callback_li (void *pArg, int argc, char **argv, char **azColName);
static int callbackmaj (void *NotUsed, int argc, char **argv, char **azColName);
static int callback_prod(void *pArg, int argc, char **argv, char **azColName);
static int callback_c(void *pArg, int argc, char **argv, char **azColName);
static int callb_scod(void *pArg, int argc, char **argv, char **azColName);
static int callb_param(void *pArg, int argc, char **argv, char **azColName);

extern GtkListStore *cli_listor;
extern GtkCList *cli_clist;
extern GtkListStore *prod_listor;
extern GtkCList *prod_clist;
extern S_CODFAC dbligcod;
extern S_DBPARAM rdbp;
extern long zslon;

S_FCLI lsfcli; // a copier apres lecture
S_PROD lsfprod; // a copier apres lecture
sqlite3 *db;

// retour lecture d'un element de la table produit
static int callback_prod(void *pArg, int argc, char **argv, char **azColName)
{		// active que si clef trouvee
int i=0;
memset ((char*)&lsfprod,0, sizeof(S_PROD));	//raz
strcpy(lsfprod.cleprod,argv[i]);
strcpy(lsfprod.libprod,argv[++i]);
++i;
 strcpy(lsfprod.spriunit,argv[i]);	// float(10.2)
 mdctoi(argv[i]);
 lsfprod.priunit=zslon;		// float(10.2)  ,

strcpy(lsfprod.codtax,argv[++i]);
strcpy(lsfprod.unit,argv[++i]);
strcpy(lsfprod.datcre,argv[++i]);
strcpy(lsfprod.datmod,argv[++i]);
strcpy(lsfprod.notes,argv[++i]);
//printf("li lsfprod.clecli = %s %s\n",lsfprod.clecli,lsfprod.datcre);
  return 0;
}

// retour liste produit
static int callback_liprod(void *pArg, int argc, char **argv, char **azColName)
{
GtkTreeIter pIter;
int indx;
//gchar* sUtf8;
// probleme avec window
 gtk_list_store_append(prod_listor, &pIter);
for (indx=0 ; indx < argc; ++indx) {
// sUtf8 = g_locale_to_utf8(argv[indx], -1, NULL, NULL, NULL);
// gtk_list_store_set(prod_listor, &pIter,indx,sUtf8, -1);
 gtk_list_store_set(prod_listor, &pIter,indx,argv[indx], -1);
				}
// g_free(sUtf8);
  return 0;
}
// retour liste client
static int callback_li(void *pArg, int argc, char **argv, char **azColName)
{
GtkTreeIter pIter;
int indx;
//gchar* sUtf8; // a voir si reprise fichier
 gtk_list_store_append(cli_listor, &pIter);
for (indx=0 ; indx < argc; ++indx) {
// sUtf8 = g_locale_to_utf8(argv[indx], -1, NULL, NULL, NULL);
// gtk_list_store_set(cli_listor, &pIter,indx,sUtf8, -1);
 gtk_list_store_set(cli_listor, &pIter,indx,argv[indx], -1);
//  printf("%d %s\n",indx,argv[indx]);
				}
// g_free(sUtf8);
  return 0;
}

// retour de insert et maj pour toutes les tables
static int callbackmaj(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

// retour lecture d'un element de la table client
static int callback_c(void *pArg, int argc, char **argv, char **azColName)
{		// active que si clef trouvee
int i=0;
//memset (void * s, int c, c_size_t n);
memset ((char*)&lsfcli,0, sizeof(S_FCLI));	//raz
strcpy(lsfcli.clecli,argv[i]);
// strcpy(retcli.nucop,argv[i]);
strcpy(lsfcli.typcli,argv[++i]);
// strcpy(retcli.matcom,argv[i]);
  // si le fic a ete repris utf8  marche pas
strcpy(lsfcli.add1,argv[++i]);
strcpy(lsfcli.add2,argv[++i]);
strcpy(lsfcli.add3,argv[++i]);
strcpy(lsfcli.pays,argv[++i]);
strcpy(lsfcli.codpos,argv[++i]);
strcpy(lsfcli.add4,argv[++i]);
strcpy(lsfcli.datcre,argv[++i]);
strcpy(lsfcli.datmod,argv[++i]);
strcpy(lsfcli.teleph,argv[++i]);
strcpy(lsfcli.mail,argv[++i]);
strcpy(lsfcli.notes,argv[++i]);
//printf("li lsfcli.clecli = %s %s\n",lsfcli.clecli,lsfcli.datcre);
  return 0;
}

// retour lecture  de la table codfac
static int callb_scod(void *pArg, int argc, char **argv, char **azColName)
{
int i=0;
/*	grosse erreur 
		dbligcod.nuli=(int)argv[i];*/
dbligcod.nuli=atoi(argv[i]);
strcpy(dbligcod.code,argv[++i]);
dbligcod.suit=*argv[++i];
strcpy(dbligcod.lib,argv[++i]);
  return 0;
}

// retour lecture  de la table Param
static int callb_param(void *pArg, int argc, char **argv, char **azColName)
{
int i=0;
strncpy(rdbp.idparam,argv[i],4);	// def size sqlite varie pb v64bits
//strcpy(rdbp.idparam,argv[i]);
/* erreur idem table codfac
		rdbp.nuli=int)argv[i++];
	// warning cast def integer dans sqlite
*/
rdbp.nuli=atoi(argv[i++]);
strcpy(rdbp.don1,argv[++i]);
  return 0;
}

int execdb( char * sql,char tt)
{
char *zErrMsg = 0;
int rc;
//printf("av execdb\n %s\n",sql);
if (tt=='c')  rc = sqlite3_exec(db, sql, callback_c, 0, &zErrMsg);
else if (tt=='r')  rc = sqlite3_exec(db, sql, callback_prod, 0, &zErrMsg);
else if (tt=='u')  rc = sqlite3_exec(db, sql,callbackmaj, 0, &zErrMsg); //maj
else if (tt=='s')  rc = sqlite3_exec(db, sql, callback_liprod, 0, &zErrMsg);
else if (tt=='l')  rc = sqlite3_exec(db, sql, callback_li, 0, &zErrMsg);
else if (tt=='o')  rc = sqlite3_exec(db, sql, callb_scod, 0, &zErrMsg);
else if (tt=='p')  rc = sqlite3_exec(db, sql, callb_param, 0, &zErrMsg);
/* if (tt=='m')  rc = sqlite3_exec(db, sql, callback_limv, 0, &zErrMsg);
else if (tt=='d')  rc = sqlite3_exec(db, sql, callback_d, 0, &zErrMsg);
*/
//printf("ap execdb\n %s\n",sql);
  if( rc!=SQLITE_OK ) {
//On affiche le texte dans une boite de dialogue.
// ne signale pas erreur id ?
GtkWidget *dialog;
dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,
"SQL error:\n%s", zErrMsg);
gtk_dialog_run(GTK_DIALOG(dialog));
gtk_widget_destroy(dialog);
  fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
 		 }
return rc;
}


