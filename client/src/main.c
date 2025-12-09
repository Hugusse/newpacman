#include "../include/global.h"
#include "../include/config.h"
#include "../include/menu.h"
#include "../include/game.h"

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    printf("=== Pacman - Menu + Sélection de map ===\n");

    // 1. CHARGEMENT CONFIG
    GameConfig config;
    if (!load_game_config("config/game.cfg", &config)) {
        config.screen_width = 640;
        config.screen_height = 480;
        config.tile_size = 20;
        config.fps = 60;
        config.lives = 3;
    }

    // 2. INITIALISATION SDL
    SDL_Surface *screen = init_sdl(&config);
    if (!screen) {
        printf("Erreur: impossible d'initialiser SDL\n");
        return 1;
    }

    ProgramState state = STATE_MENU;
    char selected_map[256] = "maps/test.map"; // chemin relatif depuis output/

    // 3. BOUCLE PRINCIPALE
    while (state != STATE_QUIT) {
        switch (state) {
            case STATE_MENU:
                if (menu_view(screen, &config, &state, selected_map, sizeof(selected_map))) {
                    state = STATE_QUIT;
                }
                break;

            case STATE_MAP_SELECT:
                // map_select est déclenché depuis menu_view, donc rien ici
                // si on retombe ici, on retourne au menu par sécurité
                state = STATE_MENU;
                break;

            case STATE_GAME:
                if (game_view(screen, &config, &state, selected_map)) {
                    state = STATE_MENU;
                }
                break;

            case STATE_QUIT:
            default:
                break;
        }
    }

    cleanup_sdl(screen);
    printf("Fermeture du jeu.\n");
    return 0;
}