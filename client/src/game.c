#include "../include/game.h"

int game_view(SDL_Surface *screen, GameConfig *config, ProgramState *state, const char *map_path) {
    printf("=== Lancement du Jeu Pacman === (map: %s)\n", map_path);

    // 1. CHARGEMENT MAP
    Map *map = load_map(map_path);
    if (!map) {
        printf("Erreur: impossible de charger la map %s\n", map_path);
        *state = STATE_MENU;
        return 1;
    }

    // 2. CHARGEMENT SPRITES
    Sprites sprites;
    if (!load_sprites(&sprites)) {
        free_map(map);
        *state = STATE_MENU;
        return 1;
    }

    // 3. SETUP DU JOUEUR (PACMAN)
    Player pacman;
    pacman.x = map->player_spawn_x * config->tile_size;
    pacman.y = map->player_spawn_y * config->tile_size;
    pacman.dx = 0;
    pacman.dy = 0;
    pacman.next_dx = 0;
    pacman.next_dy = 0;
    pacman.speed = 2; // Doit diviser tile_size pour éviter les collisions ratées

    int dots_eaten = 0;
    int running = 1;
    SDL_Event event;

    while (running && *state == STATE_GAME) {
        // --- A. ÉVÉNEMENTS ---
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                *state = STATE_QUIT;
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        *state = STATE_MENU; // retour menu
                        running = 0;
                        break;
                    case SDLK_UP:
                        pacman.next_dx = 0; pacman.next_dy = -1; break;
                    case SDLK_DOWN:
                        pacman.next_dx = 0; pacman.next_dy = 1; break;
                    case SDLK_LEFT:
                        pacman.next_dx = -1; pacman.next_dy = 0; break;
                    case SDLK_RIGHT:
                        pacman.next_dx = 1; pacman.next_dy = 0; break;
                    default:
                        break;
                }
            }
        }

        // --- B. LOGIQUE DE MOUVEMENT ---
        if (is_aligned(pacman.x, config->tile_size) && is_aligned(pacman.y, config->tile_size)) {
            int grid_x = pacman.x / config->tile_size;
            int grid_y = pacman.y / config->tile_size;

            if (pacman.next_dx != 0 || pacman.next_dy != 0) {
                if (!is_wall(map, grid_x + pacman.next_dx, grid_y + pacman.next_dy)) {
                    pacman.dx = pacman.next_dx;
                    pacman.dy = pacman.next_dy;
                }
            }

            if (is_wall(map, grid_x + pacman.dx, grid_y + pacman.dy)) {
                pacman.dx = 0;
                pacman.dy = 0;
            }
        }

        pacman.x += pacman.dx * pacman.speed;
        pacman.y += pacman.dy * pacman.speed;

        // --- C. MANGER LES GOMMES ---
        int center_x = pacman.x + config->tile_size / 2;
        int center_y = pacman.y + config->tile_size / 2;
        int grid_x = center_x / config->tile_size;
        int grid_y = center_y / config->tile_size;

        TileType tile = get_tile(map, grid_x, grid_y);
        if (tile == TILE_DOT) {
            set_tile(map, grid_x, grid_y, TILE_EMPTY);
            dots_eaten++;
            if (dots_eaten >= map->total_dots) {
                printf("VICTOIRE ! Toutes les gommes mangées.\n");
                SDL_Delay(1500);
                *state = STATE_MENU;
                running = 0;
            }
        } else if (tile == TILE_POWER_PELLET) {
            set_tile(map, grid_x, grid_y, TILE_EMPTY);
            printf("Power pellet ! (effets à implémenter)\n");
        }

        // --- D. RENDU ---
        render_map(screen, map, &sprites, config->tile_size);
        render_sprite(screen, sprites.pacman, pacman.x, pacman.y);
        update_screen(screen);

        SDL_Delay(1000 / config->fps);
    }

    free_sprites(&sprites);
    free_map(map);
    printf("Fin de la partie.\n");
    return 0;
}
