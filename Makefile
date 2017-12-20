TARGET0 = ./vergifac

LIB = `pkg-config --libs --cflags gtk+-2.0 sqlite3` 
CFLAGS =  -Wall `pkg-config --cflags gtk+-2.0 sqlite3`
#CFLAGS = -g  -Wall `pkg-config --cflags gtk+-2.0 sqlite3`  # debug

DU = ./util/
DS = ./src/
INC = ./util/

# Nom des objets
SRCOK =   $(DS)fastmenu.o $(DS)saiscli.o $(DS)mesfast.o $(DS)sqlcliDB.o \
	$(DS)listcli.o $(DS)listprod.o $(DS)saiprod.o \
	$(DS)paramsfac.o $(DS)memodoc.o $(DS)editfast.o $(DS)saifaccod.o \
	$(DS)saifac.o 

UTIL = $(DU)genfq.o $(DU)genctrl.o $(DU)genftablo.o $(DU)gencbox.o


#SRC =  $(DS)memodoc.c $(DS)editfast.c $(DS)saifaccod.c \
#	$(DS)saifac.c $(DS)paramsfac.c

# $(DU)genfq.c $(DU)genctrl.c $(DU)genftablo.c

TARGET = ./user-vergifac/$(TARGET0)

all:	$(TARGET)

%.o: %.c $(DS)fast.h $(DU)genfen.h
	 echo fic%.o; gcc -o $@ $(CFLAGS) -I $(DU) -c $(LIB) $< 

$(TARGET): $(SRCOK) $(UTIL)
#	gcc -g -o $(TARGET) $(SRC) -Wall -I $(INC) $(LIB)
	gcc -o $(TARGET) $(SRCOK) $(UTIL) -Wall -I $(INC) $(LIB)
#	gcc -o $(TARGET) $(SRC) $(UTIL) -Wall  $(LIB)

	cd user-vergifac;  ./$(TARGET0)
#	gdb ./$(TARGET)

exe:
	cd user-vergifac;  ./$(TARGET0)

