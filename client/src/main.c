#include "../include/global.h"
#include "../include/config.h"
#include "../include/map.h"
#include "../include/render.h"
#include "../include/player.h"

int main(int argc, char *argv[]) {
    printf("=== Pacman - Gameplay (Mode Cube) ===\n");

    // 1. CHARGEMENT CONFIG
    GameConfig config;
    if (!load_game_config("config/game.cfg", &config)) {
        // Valeurs par défaut si le fichier manque
        config.screen_width = 640; config.screen_height = 480;
        config.tile_size = 20; config.fps = 60; config.lives = 3;
    }

    // 2. CHARGEMENT MAP
    Map *map = load_map("maps/test.map");
    if (!map) return 1;

    // 3. INITIALISATION SDL
    SDL_Surface *screen = init_sdl(&config);
    if (!screen) { free_map(map); return 1; }

    Sprites sprites;
    // Ton render.c actuel crée déjà des carrés de couleur ici, c'est parfait
    if (!load_sprites(&sprites)) { 
        cleanup_sdl(screen); 
        free_map(map); 
        return 1; 
    }

    // 4. SETUP DU JOUEUR (PACMAN)
    Player pacman;
    // On convertit la position de spawn (grille) en pixels
    pacman.x = map->player_spawn_x * config.tile_size;
    pacman.y = map->player_spawn_y * config.tile_size;
    pacman.dx = 0; pacman.dy = 0;
    pacman.next_dx = 0; pacman.next_dy = 0;
    pacman.speed = 2; // IMPORTANT : Doit être un diviseur de TILE_SIZE (20), sinon bugs de collision !

    // 5. BOUCLE DE JEU
    int running = 1;
    SDL_Event event;

    while (running) {
        // --- A. GESTION DES ENTRÉES ---
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: running = 0; break;
                    case SDLK_UP:    pacman.next_dx = 0;  pacman.next_dy = -1; break;
                    case SDLK_DOWN:  pacman.next_dx = 0;  pacman.next_dy = 1;  break;
                    case SDLK_LEFT:  pacman.next_dx = -1; pacman.next_dy = 0;  break;
                    case SDLK_RIGHT: pacman.next_dx = 1;  pacman.next_dy = 0;  break;
                    default: break;
                }
            }
        }

        // --- B. LOGIQUE DE MOUVEMENT ---
        // On ne peut changer de direction ou s'arrêter que si on est pile sur une case
        if (is_aligned(pacman.x, config.tile_size) && is_aligned(pacman.y, config.tile_size)) {
            int grid_x = pacman.x / config.tile_size;
            int grid_y = pacman.y / config.tile_size;

            // 1. Essayer de tourner (si une nouvelle direction est demandée)
            if (pacman.next_dx != 0 || pacman.next_dy != 0) {
                if (!is_wall(map, grid_x + pacman.next_dx, grid_y + pacman.next_dy)) {
                    pacman.dx = pacman.next_dx;
                    pacman.dy = pacman.next_dy;
                }
            }

            // 2. Vérifier si on fonce dans un mur
            if (is_wall(map, grid_x + pacman.dx, grid_y + pacman.dy)) {
                pacman.dx = 0;
                pacman.dy = 0;
            }
        }

        // Appliquer le mouvement
        pacman.x += pacman.dx * pacman.speed;
        pacman.y += pacman.dy * pacman.speed;

        // --- C. MANGER LES GOMMES ---
        // On regarde la case au centre du joueur
        int center_x = pacman.x + config.tile_size / 2;
        int center_y = pacman.y + config.tile_size / 2;
        int grid_x = center_x / config.tile_size;
        int grid_y = center_y / config.tile_size;

        if (get_tile(map, grid_x, grid_y) == TILE_DOT) {
            set_tile(map, grid_x, grid_y, TILE_EMPTY); // Effacer la gomme
            // printf("Miam !\n"); // Debug
        }

        // --- D. RENDU ---
        render_map(screen, map, &sprites, config.tile_size);       // Dessine le fond
        render_sprite(screen, sprites.pacman, pacman.x, pacman.y); // Dessine le joueur (cube jaune)
        update_screen(screen);

        SDL_Delay(1000 / config.fps);
    }

    // NETTOYAGE
    free_sprites(&sprites);
    cleanup_sdl(screen);
    free_map(map);

    printf("Fin du jeu.\n");
    return 0;
}