CC ?= gcc

COBJS ?= client.o bmp.o couleur.o validateur.o
SOBJS ?= serveur.o validateur.o

.SUFFIXES: .c .o

SERVER = serveur
CLIENT = client
VALIDATEUR = VALIDATEUR

all: $(SERVER) $(CLIENT)

$(SERVER): $(SOBJS)
	$(CC) -o $(SERVER) $(SOBJS)

$(CLIENT): $(COBJS)
	$(CC) -o $(CLIENT) $(COBJS)

.c.o: 
	$(CC) -c $*.c

