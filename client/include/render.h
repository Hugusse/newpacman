#ifndef RENDER_H
#define RENDER_H

#include <SDL/SDL.h>
#include "map.h"
#include "config.h"

// Structure contenant les sprites
typedef struct {
    SDL_Surface *wall;
    SDL_Surface *dot;
    SDL_Surface *power_pellet;
    SDL_Surface *pacman;
    SDL_Surface *ghost[4];
} Sprites;

// Initialiser SDL et créer la fenêtre
SDL_Surface* init_sdl(GameConfig *config);

// Charger tous les sprites
int load_sprites(Sprites *sprites);

// Libérer les sprites
void free_sprites(Sprites *sprites);

// Créer un sprite coloré simple (pour tests sans images)
SDL_Surface* create_colored_sprite(int size, Uint32 color);

// Dessiner la map
void render_map(SDL_Surface *screen, Map *map, Sprites *sprites, int tile_size);

// Dessiner un sprite à une position
void render_sprite(SDL_Surface *screen, SDL_Surface *sprite, int x, int y);

// Mettre à jour l'écran
void update_screen(SDL_Surface *screen);

// Nettoyer SDL
void cleanup_sdl(SDL_Surface *screen);

#endif