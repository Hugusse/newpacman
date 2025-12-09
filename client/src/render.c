#include <stdio.h>
#include "render.h"
#include "config.h"
#include "map.h"

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

// CORRECTION ICI : On utilise le format de l'écran pour créer le sprite
SDL_Surface* create_colored_sprite(int size, Uint32 color) {
    const SDL_PixelFormat *fmt = SDL_GetVideoSurface()->format;
    SDL_Surface *sprite = SDL_CreateRGBSurface(
        SDL_HWSURFACE,
        size, size,
        fmt->BitsPerPixel,
        fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask
    );

    if (sprite) {
        SDL_FillRect(sprite, NULL, color);
    }
    return sprite;
}

int load_sprites(Sprites *sprites) {
    int size = 20; // Taille par défaut (doit correspondre à TILE_SIZE dans config)
    SDL_PixelFormat *fmt = SDL_GetVideoSurface()->format;
    
    // Bleu pour les murs
    sprites->wall = create_colored_sprite(size, SDL_MapRGB(fmt, 0, 0, 255));
    
    // Blanc pour les pac-gommes (petit carré centré simulé ou juste un petit carré)
    // Astuce : pour l'instant on fait un carré simple blanc
    sprites->dot = create_colored_sprite(size/3, SDL_MapRGB(fmt, 255, 255, 255));
    
    // Blanc plus gros pour power pellet
    sprites->power_pellet = create_colored_sprite(size/2, SDL_MapRGB(fmt, 255, 255, 200));
    
    // Jaune pour Pacman
    sprites->pacman = create_colored_sprite(size, SDL_MapRGB(fmt, 255, 255, 0));
    
    // Fantômes
    sprites->ghost[0] = create_colored_sprite(size, SDL_MapRGB(fmt, 255, 0, 0));   // Rouge
    sprites->ghost[1] = create_colored_sprite(size, SDL_MapRGB(fmt, 255, 128, 255)); // Rose
    sprites->ghost[2] = create_colored_sprite(size, SDL_MapRGB(fmt, 0, 255, 255));   // Cyan
    sprites->ghost[3] = create_colored_sprite(size, SDL_MapRGB(fmt, 255, 128, 0));    // Orange

    if (!sprites->wall || !sprites->pacman) {
        printf("Erreur création sprites\n");
        return 0;
    }
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
            
            switch (map->tiles[y][x]) {
                case TILE_WALL:
                    render_sprite(screen, sprites->wall, screen_x, screen_y);
                    break;
                case TILE_DOT:
                    render_sprite(screen, sprites->dot, screen_x + tile_size/3, screen_y + tile_size/3);
                    break;
                case TILE_POWER_PELLET:
                    render_sprite(screen, sprites->power_pellet, screen_x + tile_size/4, screen_y + tile_size/4);
                    break;
                default: break;
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