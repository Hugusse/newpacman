# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -g -O2 -std=c99 -I./client/include
LDFLAGS = -lmingw32 -lSDLmain -lSDL -mwindows
TARGET = pacman

# Répertoires
SRCDIR = client/src
INCDIR = client/include
OUTDIR = output
OBJDIR = $(OUTDIR)/obj

# Sources et objets
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Règle par défaut
.PHONY: all clean debug run install-deps

all: $(OUTDIR)/$(TARGET)

# Création de l'exécutable
$(OUTDIR)/$(TARGET): $(OBJECTS) | $(OUTDIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Compilation des fichiers objets
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Création des répertoires
$(OUTDIR):
	@mkdir -p $(OUTDIR)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

# Exécution du programme
run: all
	cd $(OUTDIR) && ./$(TARGET)

# Nettoyage
clean:
	rm -rf $(OUTDIR)

# Mode debug
debug: CFLAGS += -DDEBUG -g3
debug: $(OUTDIR)/$(TARGET)

# Installation des dépendances (pour référence)
install-deps:
	@echo "Installez SDL1.2 avec votre gestionnaire de paquets"
	@echo "Windows: téléchargez depuis https://www.libsdl.org/download-1.2.php"
	@echo "Linux: sudo apt-get install libsdl1.2-dev"
	@echo "macOS: brew install sdl"
