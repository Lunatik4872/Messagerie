# Récupérer tous les fichiers .c dans le répertoire courant
SOURCES = $(wildcard *.c)
# Enlever l'extension .c des noms de fichiers
EXECUTABLES = $(SOURCES:.c=)

# Définir les options du compilateur
CFLAGS = -Wall -Wextra -Werror

all: $(EXECUTABLES)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(EXECUTABLES)
