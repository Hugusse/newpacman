#include <stdio.h>
#include "render.h"
#include "config.h"

SDL_Surface* init_sdl(GameConfig *config) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Surface *screen = SDL_SetVideoMode(
        config->screen_width,
        config->screen_height,
        32,
        SDL_HWSURFACE | SDL_DOUBLEBUF
    );

    if (!screen) {
        printf("Erreur SDL_SetVideoMode: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_WM_SetCaption("Pacman", NULL);
    return screen;
}

SDL_Surface* create_colored_sprite(int size, Uint32 color) {
    SDL_Surface *sprite = SDL_CreateRGBSurface(
        SDL_HWSURFACE,
        size, size,
        32,
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
    );

    if (sprite) {
        SDL_FillRect(sprite, NULL, color);
    }
    return sprite;
}

int load_sprites(Sprites *sprites) {
    // Pour l'instant, on crée des sprites colorés simples
    // Tu pourras remplacer par SDL_LoadBMP() quand tu auras les vraies images
    
    int size = 20; // Taille par défaut
    
    // Bleu pour les murs
    sprites->wall = create_colored_sprite(size, SDL_MapRGB(
        SDL_GetVideoSurface()->format, 0, 0, 255));
    
    // Blanc pour les pac-gommes (petit)
    sprites->dot = create_colored_sprite(size/3, SDL_MapRGB(
        SDL_GetVideoSurface()->format, 255, 255, 255));
    
    // Blanc plus gros pour power pellet
    sprites->power_pellet = create_colored_sprite(size/2, SDL_MapRGB(
        SDL_GetVideoSurface()->format, 255, 255, 200));
    
    // Jaune pour Pacman
    sprites->pacman = create_colored_sprite(size, SDL_MapRGB(
        SDL_GetVideoSurface()->format, 255, 255, 0));
    
    // Couleurs différentes pour chaque fantôme
    Uint32 ghost_colors[4] = {
        SDL_MapRGB(SDL_GetVideoSurface()->format, 255, 0, 0),   // Rouge
        SDL_MapRGB(SDL_GetVideoSurface()->format, 255, 128, 255), // Rose
        SDL_MapRGB(SDL_GetVideoSurface()->format, 0, 255, 255),   // Cyan
        SDL_MapRGB(SDL_GetVideoSurface()->format, 255, 128, 0)    // Orange
    };
    
    for (int i = 0; i < 4; i++) {
        sprites->ghost[i] = create_colored_sprite(size, ghost_colors[i]);
    }

    // Vérifier que tout est chargé
    if (!sprites->wall || !sprites->dot || !sprites->power_pellet || 
        !sprites->pacman) {
        printf("Erreur lors de la création des sprites\n");
        return 0;
    }

    printf("Sprites chargés (mode coloré temporaire)\n");
    return 1;
}

void free_sprites(Sprites *sprites) {
    if (sprites->wall) SDL_FreeSurface(sprites->wall);
    if (sprites->dot) SDL_FreeSurface(sprites->dot);
    if (sprites->power_pellet) SDL_FreeSurface(sprites->power_pellet);
    if (sprites->pacman) SDL_FreeSurface(sprites->pacman);
    
    for (int i = 0; i < 4; i++) {
        if (sprites->ghost[i]) SDL_FreeSurface(sprites->ghost[i]);
    }
}

void render_sprite(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
    if (!sprite) return;
    
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    
    SDL_BlitSurface(sprite, NULL, screen, &dest);
}

void render_map(SDL_Surface *screen, Map *map, Sprites *sprites, int tile_size) {
    // Fond noir
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            int screen_x = x * tile_size;
            int screen_y = y * tile_size;
            
            TileType tile = map->tiles[y][x];
            
            switch (tile) {
                case TILE_WALL:
                    render_sprite(screen, sprites->wall, screen_x, screen_y);
                    break;
                    
                case TILE_DOT:
                    // Centrer la pac-gomme dans la case
                    render_sprite(screen, sprites->dot, 
                        screen_x + tile_size/3, 
                        screen_y + tile_size/3);
                    break;
                    
                case TILE_POWER_PELLET:
                    // Centrer le power pellet
                    render_sprite(screen, sprites->power_pellet,
                        screen_x + tile_size/4,
                        screen_y + tile_size/4);
                    break;
                    
                case TILE_SPAWN_PLAYER:
                case TILE_SPAWN_GHOST:
                    // On ne dessine rien pour les spawns (ils seront marqués autrement)
                    break;
                    
                case TILE_EMPTY:
                default:
                    // Rien à dessiner
                    break;
            }
        }
    }
}

void update_screen(SDL_Surface *screen) {
    SDL_Flip(screen);
}

void cleanup_sdl(SDL_Surface *screen) {
    SDL_Quit();
}