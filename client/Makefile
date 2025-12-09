CC = gcc
CFLAGS = -Wall -Wextra -g -O2 -std=c99 -Iinclude -I/mingw64/include/SDL
LDFLAGS = -L/mingw64/lib -lmingw32 -lSDLmain -lSDL -lSDL_image -lSDL_ttf -lSDL_gfx -lSDL_mixer -mwindows -lm
TARGET = pacmen
SRCDIR = src
OUTDIR = output
SOURCES = $(wildcard ./$(SRCDIR)/*.c)

.PHONY: all clean debug

all: $(OUTDIR)/$(TARGET)

$(OUTDIR)/$(TARGET): $(SOURCES)
	@mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET) $(LDFLAGS)

run: all
	cd $(OUTDIR) && ./$(TARGET) && cd ..

clean:
	rm -f $(OUTDIR)/$(TARGET)

debug: CFLAGS += -DDEBUG
debug: $(OUTDIR)/$(TARGET)
