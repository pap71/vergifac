DS = ./src/
DU = ./util/

SRC =   $(DS)fast.h $(DS)fastmenu.c $(DS)saiscli.c $(DS)mesfast.c $(DS)sqlcliDB.c \
	$(DS)listcli.c $(DS)listprod.c $(DS)saiprod.c \
	$(DS)paramsfac.c $(DS)memodoc.c $(DS)editfast.c $(DS)saifaccod.c \
	$(DS)saifac.c \
    $(DU)genftablo.c  $(DU)genfq.c $(DU)genctrl.c $(DU)gencbox.c
INC = ./util/
LIB = `pkg-config --libs --cflags gtk+-2.0 sqlite3` 
TARGET = vergifac
all:
#	gcc -o ./user-vergifac/$(TARGET) $(SRC) -Wall -Wno-deprecated-declarations -I $(INC) $(LIB) -lm
	gcc -g -o ./user-vergifac/$(TARGET) $(SRC) -Wall -Wno-deprecated-declarations -I $(INC) $(LIB) -lm  #  -g  pour debug

exe:
	cd user-vergifac;  ./vergifac
#	gdb ./$(TARGET)

#valgrind  --leak-check=full --suppressions=fisupgtk.txt  ./vergisc64 
#valgrind  --leak-check=full --suppressions=fisupgtk.txt --gen-suppressions=yes ./vergisc64

