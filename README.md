
##Billing software           #VERGIFAC

Vergifac captures, archive and print invoice type documents (quotes, orders, etc.).
The final document is saved in a PDF file format universally used which has the advantage of being printed as such by all printers and to be easily attached to an email.
Document layout is fully customizable.
The program includes a management client or third parties' as well as management 'products or services' storing the information necessary for easy billing.

###Version: Linux (2026) 
To generate the vergisc program in the 'user-vergifac' directory
Open Terminal on ../master/build
delete all files from the build directory
cmake ..
Check dependencies. If needed, install gtk+.2.0 sqlite3 and restart cmake ..
make

###Old Version: Windows(2018, need GTK) new in projet !

###Test
The directory 'user-vergifac' contains the files for use vergifac
Open Terminal on 'user-vergifac'
to choose
./vergifac for Linux
./vergifac-raspi (raspberry) for Raspbian GNU/Linux 13 (trixie) (32 b)
vergifac.exe for Windows

<<<<<<< HEAD
=======
### Test database: dbtestfac.db
### IMPORTANT: The Cli table has been modified for the new Linux version
### If you are using the older version, you must convert your database
### Follow the procedure at https://github.com/pap71/addcolsql.git

User documentation is available while "vergisc" is running.


>>>>>>> 6911dba (mise à jour septembre 2026)

##Logiciel de Facturation       #VERGIFAC

Vergifac permet de saisir, d'archiver et d'imprimer des documents de type facture (devis, commande, etc).
Le document final est enregistré dans un fichier de format PDF universellement utilisé qui a l'avantage d'être imprimé tel quel par toutes les imprimantes et d'être facilement joint à un mail.
La présentation du document est complètement paramétrable.
Le programme inclut une gestion 'client ou tiers' ainsi qu'une gestion 'produits ou services' mémorisant les informations nécessaires à une facturation aisée.

###Version: Linux (2026) 
pour generer le prog vergisfac dans le répertoire 'user-vergifac'
ouvrir un terminal sur ../master/build
supprimer tous les fichiers du répertoire build
cmake ..
vérification des dépendances 
si besoin apt install gtk+.2.0 sqlite3 et relancer cmake ..
make

###Old Version: Windows(2018, utilise GTK) reprise en projet !

### Test
Le répertoire 'user-vergifac' contient les fichiers pour utiliser vergifac
Ouvrir un terminal sur 'user-vergifac'
choisir
./vergifac pour linux 
./vergifac-raspi (raspberry) for Raspbian GNU/Linux 13 (trixie) (32 b)
vergifac.exe pour Windows

<<<<<<< HEAD
=======
### base de données de test: dbtestfac.db
### IMPORTANT la table Cli est modifiée pour la nouvelle version linux
### Si vous utilisez l'ancienne version il faut convertir votre base de données 
### Suivre la procédure https://github.com/pap71/addcolsql.git

Encours d'utilisation la documentation utilisateur est disponible.

>>>>>>> 6911dba (mise à jour septembre 2026)

