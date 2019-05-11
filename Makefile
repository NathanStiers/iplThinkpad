CFLAGS = -std=c11 -pedantic -Wall -Werror -Wvla  -D_DEFAULT_SOURCE 
EXEC = -o $@ $^
OBJECT = -c $<

all : gstat maint serveur client

#### SERVEUR ####

gstat : gstat.o utils.o serveurUtils.o
	cc $(CFLAGS) $(EXEC)
	
gstat.o : gstat.c utils.h serveurUtils.h
	cc $(CFLAGS) $(OBJECT)
	
maint : maint.o utils.o serveurUtils.o
	cc $(CFLAGS) $(EXEC)
	
maint.o : maint.c utils.h serveurUtils.h
	cc $(CFLAGS) $(OBJECT)
	
serveur : serveur.o utils.o serveurUtils.o
	cc $(CFLAGS) $(EXEC)
	
serveur.o : serveur.c utils.h serveurUtils.h message.h
	cc $(CFLAGS) $(OBJECT)
	
#### CLIENT ####
	
client : client.o utils.o clientUtils.o
	cc $(CFLAGS) $(EXEC)
	
client.o : client.c utils.h clientUtils.h message.h
	cc $(CFLAGS) $(OBJECT)
	
#### UTILS ####
	
utils.o : utils.c utils.h
	cc $(CFLAGS) $(OBJECT)
	
serveurUtils.o : serveurUtils.c serveurUtils.h
	cc $(CFLAGS) $(OBJECT)
	
clientUtils.o : clientUtils.c clientUtils.h
	cc $(CFLAGS) $(OBJECT)
	
.PHONY : clean

clean : 
	@rm -f *.o
	rm -f gstat
	rm -f maint
	rm -f serveur
	rm -f client