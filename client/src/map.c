#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

Map* create_map(int width, int height) {
    Map *map = (Map*)malloc(sizeof(Map));
    map->width = width;
    map->height = height;
    map->total_dots = 0;
    
    // Allouer le tableau 2D
    map->tiles = (TileType**)malloc(height * sizeof(TileType*));
    for (int i = 0; i < height; i++) {
        map->tiles[i] = (TileType*)malloc(width * sizeof(TileType));
        for (int j = 0; j < width; j++) {
            map->tiles[i][j] = TILE_EMPTY;
        }
    }
    
    // Initialiser les spawns
    for (int i = 0; i < 4; i++) {
        map->ghost_spawn_x[i] = -1;
        map->ghost_spawn_y[i] = -1;
    }
    
    return map;
}

Map* load_map(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur: impossible d'ouvrir %s\n", filename);
        return NULL;
    }

    int width = 0, height = 0;
    char line[256];
    
    // Lire WIDTH et HEIGHT
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        if (strncmp(line, "WIDTH=", 6) == 0) {
            width = atoi(line + 6);
        } else if (strncmp(line, "HEIGHT=", 7) == 0) {
            height = atoi(line + 7);
            break; // On a tout ce qu'il faut
        }
    }

    if (width == 0 || height == 0) {
        printf("Erreur: dimensions invalides\n");
        fclose(file);
        return NULL;
    }

    Map *map = create_map(width, height);
    int ghost_count = 0;
    int row = 0;
    int in_grid = 0;  // Flag pour savoir si on lit la grille

    // Lire la grille
    while (fgets(line, sizeof(line), file) && row < height) {
        // Si la ligne commence par '#' et on n'a pas encore commencé la grille, c'est un commentaire
        if (line[0] == '#' && !in_grid && (line[1] == ' ' || line[1] == '\n' || line[1] == '\r')) {
            continue; // Ignorer les commentaires
        }
        
        // Si on trouve une ligne qui commence par '#' mais c'est la grille (pleine de #)
        if (line[0] == '#' || line[0] == '.') {
            in_grid = 1;  // On est dans la grille maintenant
        }
        
        if (!in_grid) continue;  // Ignorer tout avant la grille
        
        int len = strlen(line);
        for (int col = 0; col < len && col < width; col++) {
            TileType tile = TILE_EMPTY;
            
            switch (line[col]) {
                case '#':
                    tile = TILE_WALL;
                    break;
                case '.':
                    tile = TILE_DOT;
                    map->total_dots++;
                    break;
                case 'o':
                case 'O':
                    tile = TILE_POWER_PELLET;
                    break;
                case 'P':
                case 'p':
                    tile = TILE_SPAWN_PLAYER;
                    map->player_spawn_x = col;
                    map->player_spawn_y = row;
                    break;
                case 'G':
                case 'g':
                    tile = TILE_SPAWN_GHOST;
                    if (ghost_count < 4) {
                        map->ghost_spawn_x[ghost_count] = col;
                        map->ghost_spawn_y[ghost_count] = row;
                        ghost_count++;
                    }
                    break;
                case ' ':
                case '\n':
                case '\r':
                    tile = TILE_EMPTY;
                    break;
            }
            
            map->tiles[row][col] = tile;
        }
        row++;
    }

    fclose(file);
    printf("Map chargée: %dx%d, %d pac-gommes, %d fantômes\n", 
           width, height, map->total_dots, ghost_count);
    
    // Debug: afficher la map en console
    #ifdef DEBUG
    print_map(map);
    #endif
    
    return map;
}


int is_wall(Map *map, int grid_x, int grid_y) {
    // Si on est hors de la map, c'est comme un mur
    if (grid_x < 0 || grid_x >= map->width || grid_y < 0 || grid_y >= map->height)
        return 1;
    return get_tile(map, grid_x, grid_y) == TILE_WALL;
}

// Vérifie si le joueur est parfaitement aligné sur la grille (nécessaire pour tourner)
int is_aligned(int pos, int tile_size) {
    return (pos % tile_size) == 0;
}



void free_map(Map *map) {
    if (!map) return;
    
    for (int i = 0; i < map->height; i++) {
        free(map->tiles[i]);
    }
    free(map->tiles);
    free(map);
}

TileType get_tile(Map *map, int x, int y) {
    if (x < 0 || x >= map->width || y < 0 || y >= map->height)
        return TILE_WALL; // Hors limites = mur
    return map->tiles[y][x];
}

void set_tile(Map *map, int x, int y, TileType type) {
    if (x >= 0 && x < map->width && y >= 0 && y < map->height) {
        map->tiles[y][x] = type;
    }
}

void print_map(Map *map) {
    printf("\n=== MAP DEBUG ===\n");
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            switch (map->tiles[y][x]) {
                case TILE_WALL: printf("#"); break;
                case TILE_DOT: printf("."); break;
                case TILE_POWER_PELLET: printf("O"); break;
                case TILE_SPAWN_PLAYER: printf("P"); break;
                case TILE_SPAWN_GHOST: printf("G"); break;
                default: printf(" "); break;
            }
        }
        printf("\n");
    }
    printf("=================\n");
}