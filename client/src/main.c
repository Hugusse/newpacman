#include "../include/global.h"
#include "../include/config.h"
#include "../include/map.h"
#include "../include/render.h"

int main(int argc, char *argv[]) {
    printf("=== Pacman - Test Etapes 1 & 2 ===\n\n");

    // Charger la configuration
    GameConfig config;
    if (!load_game_config("config/game.cfg", &config)) {
        printf("Utilisation des valeurs par défaut\n");
        config.screen_width = 640;
        config.screen_height = 480;
        config.tile_size = 20;
        config.fps = 60;
        config.lives = 3;
    }

    // Charger la config des fantômes
    GhostConfig ghosts[4];
    load_ghosts_config("config/ghosts.cfg", ghosts);

    // Charger les modes
    ModesConfig modes;
    load_modes_config("config/modes.cfg", &modes);

    // Charger une map
    Map *map = load_map("maps/test.map");
    if (!map) {
        printf("Erreur: impossible de charger la map\n");
        return 1;
    }

    // Afficher la map en mode texte (debug)
    print_map(map);

    // Initialiser SDL
    SDL_Surface *screen = init_sdl(&config);
    if (!screen) {
        free_map(map);
        return 1;
    }

    // Charger les sprites
    Sprites sprites;
    if (!load_sprites(&sprites)) {
        cleanup_sdl(screen);
        free_map(map);
        return 1;
    }

    printf("\n=== Affichage de la map (appuie sur ESC pour quitter) ===\n");

    // Boucle principale simple (juste affichage)
    int running = 1;
    SDL_Event event;

    while (running) {
        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
            }
        }

        // Rendu
        render_map(screen, map, &sprites, config.tile_size);
        update_screen(screen);

        // Limiter le framerate
        SDL_Delay(1000 / config.fps);
    }

    // Nettoyage
    free_sprites(&sprites);
    cleanup_sdl(screen);
    free_map(map);

    printf("\nProgramme terminé proprement\n");
    return 0;
}