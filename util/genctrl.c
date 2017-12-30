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
//  Routines de controle et affiche divers

#include <gtk/gtk.h>
#include <string.h>
#include  <time.h>      /* pour   struct tm, mktime et ctime */
#include "genfen.h"

long zslon;
double zsdoubl;
S_DAT da;
int jactuelq;

static int anactuel;
static int jan[13] =
 { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
static int jab[13] =
 { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

static char *zvnv="1234567890.,-";
static char *zvnm=" 1234567890.,-";
static char *zvn="1234567890-";	// 23/4/2017 signe -
//static char *zvn="1234567890";
static char *zvda="1234567890/.";

int chang_ctrl(GtkEntry *widls, int typ ) // controle saisie change
{
int i,lt,cur_pos;
//DEF_L_FnC *pl = pw->pdefc;
GtkEditable *editable;
gchar *text;
 editable = GTK_EDITABLE (widls);
 text = gtk_editable_get_chars (editable, 0, -1);
 lt = strlen(text);
 if ( lt > 0)   {
   if (typ == SAISINT)  i = strspn((char*)text,zvn);
   else if (typ == SAISMON)  i = strspn((char*)text,zvnm);
   else if (typ == SAISNUM)  i = strspn((char*)text,zvnv);
   else if (typ == SAISDAT)  i = strspn((char*)text,zvda);
   if (i < lt)  {
	// delete le car curseur +1
     gtk_editable_delete_text((GtkEditable*) editable,i,i+1);
     cur_pos=gtk_editable_get_position((GtkEditable*) editable);
     gtk_editable_set_position((GtkEditable*) editable,cur_pos-1);
     --lt;
	gdk_beep();
		}
		}
g_free(text);
return (lt);
}

int contdate(char* sdat) // controle date return -1=erreur 0=vide 1=date ok
{
int is,lt,tj,tm;
//int jj,mm,aa;
char zsdat[12];
sdat[11] = '\0';    // au cas ou
strcpy(zsdat,sdat);
lt=strlen(zsdat);
da.datq =0;
da.sdat[0] = '\0';
if ( lt == 0) 	{	// date a blanc
 return 0;
		}
tj=tm=0;
for (is=0; is < lt; ++is) {
 if ( *(zsdat+is) == '/' || *(zsdat+is) == '.') {
  if (tj == 0) tj=is;
  else if (tm == 0) tm=is;
  else return -1;
   *(zsdat+is) = '\0';
			}
			}
//printf("date=%s %d\n",zsdat,lt);
if (tj !=1 && tj != 2) return -1;
if (tj == lt) return -1;  // pas de mois
da.j= atoi(&zsdat[0]);
da.m= atoi(&zsdat[tj+1]);
if (tm !=0 && tm < lt)
 da.a= atoi(&zsdat[tm+1]);
else da.a = -1;  // année en cours
//printf("%d/%d/%d\n",da.j,da.m,da.a);
return qdat();
}

int qdat()		// quantifie date donnee dans struc da
{
int bi,bx,nj;
 da.datq =0;
 --da.m;
 if ( da.j < 1 || da.m < 0 || da.m > 11 ) return -1;
// if ( da.a == -1)   da.a = annsys(); //2011-2000;
 if ( da.a == -1)   da.a = anactuel; //2011-2000;
 else if ( da.a >= 1970)  da.a = da.a % 100;
 if ( da.a > 100)  return -1;
 else {
  if (da.a < 70) da.a += 30;  // an 2000 a 2070
  else if (da.a >= 70) da.a -= 70;  // an 1970 a 2000
	}
//printf("%d/%d/%d\n",da.j,da.m,da.a);

  bi = (da.a +1969)/ 4 - 492;	// nb an bis
  bx = (da.a+2) % 4;	// si 0 an est est bis (1970 non bis donc +2)
  if (bx) {
    nj = jan[da.m] + da.j;
    if ( nj > jan[ da.m+1 ] )  return -1;
           }
  else    {	// an bis
    nj = jab[da.m]+da.j;
    if ( nj > jab[ da.m+1 ] )  return -1;
           }
 da.datq = da.a*365 + nj + bi;
//printf("datq=%d\n",da.datq);
dqdat(da.datq);
return 1;
}

static char edat[]="%02d/%02d/%d";
void dqdat(int q)    //  dequantification de date
{int a,x,j,m;
  a = (q - 1) / 1461;
  x = q - (a * 1461);
  a = a * 4 + 1970;
if ( x <= 365)	{
      m = 0;
      while ( jan[m] < x)     m++;
      j = x - jan[m - 1];
		}
else if ( x <= 730)	{
      a+=1; x -= 365; m = 0;
      while ( jan[m] < x)     m++;
      j = x - jan[m - 1];
		}
else if ( x <= 1096)	{	// an bis
      a+=2; x -= 730; m = 0;
      while ( jab[m] < x)     m++;
      j = x - jab[m - 1];
		}
else if ( x <= 1461)	{
      a+=3; x -= 1096; m = 0;
      while ( jan[m] < x)     m++;
      j = x - jan[m - 1];
		}
sprintf(da.sdat,edat,j,m,a);
//printf("restit %s\n",da.sdat);
//       return(1);
}

void inidatej()
{
struct tm* pdh;
time_t intps;
intps=time(NULL);
pdh = localtime(&intps);
//printf("année en cours= %d\n",pdh->tm_year);
//printf("date du jour= %d/%d/%d\n",pdh->tm_mday,pdh->tm_mon+1,pdh->tm_year+1900);
anactuel= (pdh->tm_year - 100);
sprintf(da.sdat,edat,pdh->tm_mday,pdh->tm_mon+1,pdh->tm_year+1900);
if ( contdate(da.sdat) <= 0)
printf("pb date machine !\n");
jactuelq = da.datq;
}

/*int annsys()
{
struct tm* pdh;
time_t intps;
intps=time(NULL);
pdh = localtime(&intps);
//printf("année en cours= %d\n",pdh->tm_year);
//printf("date du jour= %d/%d/%d\n",pdh->tm_mday,pdh->tm_mon+1,pdh->tm_year+1900);
return (pdh->tm_year - 100);
}*/

int mdctoi(char* pt)     // convertie  chaine en long dans zslon
{
int i,sign,deci,ll,mdec;
char c;
//double dd;
long long int ddl;
sign=1; deci=0; mdec=0; zslon = 0; ddl=0;
ll=strlen(pt);
for (i=0; i<ll; ++i, ++pt )   {
 c = *pt;
 if (c  == ' ' ) ;
 else if (c == '-')         sign = -1;
 else if (c == '.' || c == ',')     mdec++;
 else {
   if (mdec>0)      ++deci;
   ddl  = 10 * ddl  + c - '0';
       }
 		         }
	// complete 2eme deci
if ( deci == 1)	{
 ++deci;
 ddl  = 10 * ddl;
		}

if (mdec > 1) return(-1);	// plusieurs marque deci
if (deci !=0 && deci != 2 ) return(-1);  // si deci il faut 2
if (deci == 0) ddl *=100;
// La valeur stockee dans un long va de -2 147 483 648 (LONG_MIN) a 2 147 483 647 (LONG_MAX).   21 474 836.47
if ( ddl > 2147000000) return (-1);
zslon = ddl * sign;
return(0);
}

int mdatof(char* pt)     // convertie  chaine en double
{
int i,sign,ll,mdec;
//int i,sign,deci,ll,mdec;
char c;
double dd,vv;
sign=1; mdec=0; zsdoubl = 0; dd=0;
//sign=1; deci=0; mdec=0; zsdoubl = 0; dd=0;
vv=1;
ll=strlen(pt);
for (i=0; i<ll; ++i, ++pt )   {
 c = *pt;
 if (c  == ' ' ) continue;
 else if (c == '-')         sign = -1;
 else if (c == '.' || c == ',')     mdec++;
 else if (mdec > 0) break;
 else {
//   if (mdec>0)      ++deci;
   dd  = 10 * dd  + c - '0';
       }
 		         }
if (mdec > 1) return(-1);	// plusieurs marque deci
	//decimales
//for (i; i<ll; ++i, ++pt )   {
for ( ; i<ll; ++i, ++pt )   {
 c = *pt;
 if (c  == ' ' ) continue;
 else if (c == '-')     return(-1);
 else if (c == '.' || c == ',')    return(-1);
 else {
   dd  = 10 * dd  + c - '0';
   vv *= 10;
       }
 		         }
zsdoubl = dd * sign / vv;	//risque d'imprecision suivant compil
return(0);
}

//static char form10[]="9zz zzz zzz zzz";
//static char form10b[]="9zzzzzzzzzzz";
static char form12[]="99,9zz zzz zzz zzz";
static char form12b[]="99,9zzzzzzzzzzz";
//static char form14b[]="9999,9zzzzzzzzz";
static char marqdeci =',';

void ltof(long nl,char s[],int ll)           //  formate entier long
{
short i,j;
char sign,c;
char *ptf;
char ztrav2[16];
if (nl >= 0)     sign = ' ';
else   {         sign = '-';
        nl = -nl;      }
i = 0;
do  ztrav2[i++] = nl % 10 + '0';
 while ((nl /= 10) > 0);
 if (sign == '-')   ztrav2[i++] = sign;
 ztrav2[i] = '\0';
/*     if (typ == 10)  {
        if ( ll >= ( i + (i-1)/3))
                ptf = form10;
        else    ptf = form10b;
                     }
      else if (typ == 14)     ptf = form14b;
*/
// if (typ == 12)  {
 if ( ll >= ( i + i/3))  ptf = form12;
 else  			 ptf = form12b;
//                     }
s[ll] = '\0';
for (i = 0, j = ll  ; j; ++ptf)  {
   c = *ptf;
   --j;
   if (c == ' ')    s[j] = c;
   else if (c == ',')    s[j] = marqdeci;
   else if (ztrav2[i] != '\0') {
     s[j] = ztrav2[i];
     i++;
//   if (ztrav2[i] == '\0') break; arret avant decimal si < 1
			        }
   else if ( c == 'z')          s[j] = ' ';
   else if ( c == '9')          s[j] = '0';
      }
}

long arron2(double ff, long ll)
{
double  arron;
double dmont;
//long long int smont, dlmont;
dmont = ff * ll;
if ( dmont > 0)   arron = 0.5;
else  arron = -0.5;
return ll = dmont + arron;
}

//void formfloat(char *ss,float ff)
void formfloat(char *ss,double ff)	// sup decimales a 0 et ., si besoin
{
int i,j,ll;
char *s1 = ss;
sprintf(s1,"%f",ff);
ll=strlen(s1);
for ( i = ll-1; i >0; --i)	{
  if ( s1[i] == '0')	{
   s1[i] = '\0';
   j = i -1;
   if ( s1[j] == '.' || s1[j] == ',')
     { s1[j] = '\0'; break; }
			}
  else break;
				}
}

void charreplace(char *ss,char ci, char cf)
{
int i,ll = strlen(ss);
for ( i=0; i < ll; ++i)
 if ( ss[i] == ci) ss[i] = cf;
}

int ctcar(char c, char *pt)          //  controle 1 caracter
{
while (*pt)  {
  if (*pt++ == c)  return(0);
             }
return(1);
}

void finchait(char z[],int ll)  // met zero fin chaine
{
int i;
 if ( ll == 0) ll = strlen(z);
 i = --ll;
// while ( i >= 0 && (z[i] == ' ' || z[i] == '\0'))  --i;
 while ( i >= 0 && z[i] == ' ')  --i;
 if ( ++i >= ll)  return;
 if ( i >= 0) z[i] = '\0';
}


//  test de gchar ??
/*static struct {
gchar *gch;
int lt;
} tgch [20];
static int igch = 0;

void testgch( gchar *p, int l, int ori)
{
int i;
if ( igch >= 20) igch = 0;
tgch[igch].gch = p;
tgch[igch].lt = l;
++igch;
for ( i = 0; i <= 20; ++i)	{
 if ( tgch[i].gch != NULL && tgch[i].lt != 0)
  printf(" p%d %p %d %s\t", i, tgch[i].gch, tgch[i].lt, tgch[i].gch);
				}
printf("ff%d\n",ori);
}*/

/*
ddtof(nl,s,ll)          //  formate entier double
double nl;
char s[];
short ll;
{ short i,j;  char sign,c;
char *ptf;
long n1, n2;
              if (nl >= 0)     sign = ' ';
              else   {         sign = '-';
                         nl = -nl;      }
       n1 = nl / ddm1;
       n2 = nl - (n1 * ddm2);
     i = 0;
       do {
         ztrav2[i++] = n2 % 10 + '0';
            n2 /= 10;        }
     while ( i < 6 );
     do  ztrav2[i++] = n1 % 10 + '0';
     while ((n1 /= 10) > 0);
       while (ztrav2[--i] == '0' && i > 0);
     if (sign == '-')
          ztrav2[++i] = sign;
     ztrav2[++i] = '\0';
     if (typ == 10)  {
        if ( ll >= ( i + (i-1)/3))
                ptf = form10;
        else    ptf = form10b;
                     }
     else if (typ == 12)  {
        if ( ll >= ( i + i/3))
                ptf = form12;
        else    ptf = form12b;
                     }
      else if (typ == 14)     ptf = form14b;
  //    s[ll] = '\0';
     for (i = 0, j = ll  ; j; ++ptf)
      {  c = *ptf;  --j;
       if (c == ' ' || c == ',')
         s[j] = c;
       else if (ztrav2[i] != '\0') {
               s[j] = ztrav2[i];
               i++;                }
       else if ( c == 'z')
               s[j] = ' ';
       else if ( c == '9')
               s[j] = '0';
      }
}
*/
