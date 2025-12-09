#ifndef MAP_H
#define MAP_H

// Types de cases
typedef enum {
    TILE_EMPTY,
    TILE_WALL,
    TILE_DOT,           // Pac-gomme normale
    TILE_POWER_PELLET,  // Power pellet
    TILE_SPAWN_PLAYER,  // Spawn du joueur
    TILE_SPAWN_GHOST    // Spawn des fantômes
} TileType;

// Structure d'une carte
typedef struct {
    int width;
    int height;
    TileType **tiles;   // Tableau 2D dynamique
    int player_spawn_x;
    int player_spawn_y;
    int ghost_spawn_x[4];
    int ghost_spawn_y[4];
    int total_dots;     // Nombre total de pac-gommes
} Map;

// Créer une map vide
Map* create_map(int width, int height);

// Charger une map depuis un fichier
Map* load_map(const char *filename);

// Libérer la mémoire d'une map
void free_map(Map *map);

// Obtenir le type d'une case
TileType get_tile(Map *map, int x, int y);

// Modifier une case
void set_tile(Map *map, int x, int y, TileType type);

// Afficher la map en mode texte (debug)
void print_map(Map *map);

#endif