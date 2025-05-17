CC = clang
CFLAGS = -Wall -Wextra -std=c99 -g

# Si tu veux nommer le premier fichier "principal.c", sinon remplace-le
PRINCIPAL_SRC = principal.c
SIMULATEUR_SRC = simulateur.c

all: principal simulateur

principal: $(PRINCIPAL_SRC)
	$(CC) $(CFLAGS) -o principal $(PRINCIPAL_SRC)

simulateur: $(SIMULATEUR_SRC)
	$(CC) $(CFLAGS) -o simulateur $(SIMULATEUR_SRC)

clean:
	rm -f *.o principal simulateur