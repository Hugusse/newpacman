#ifndef CONFIG_H
#define CONFIG_H

// Configuration générale du jeu
typedef struct {
    int screen_width;
    int screen_height;
    int tile_size;
    int fps;
    int lives;
} GameConfig;

// Comportements possibles pour les fantômes
typedef enum {
    BEHAVIOR_INACTIVE,
    BEHAVIOR_AGGRESSIVE,
    BEHAVIOR_PATROL,
    BEHAVIOR_RANDOM,
    BEHAVIOR_FLEE
} GhostBehavior;

// Configuration d'un fantôme
typedef struct {
    int id;
    GhostBehavior behavior;
    int active;
} GhostConfig;

// Configuration des modes de jeu
typedef struct {
    int traditional;
    int survival;
    int time_attack;
} ModesConfig;

// Fonction pour charger la config générale
int load_game_config(const char *filename, GameConfig *config);

// Fonction pour charger la config des fantômes
int load_ghosts_config(const char *filename, GhostConfig ghosts[4]);

// Fonction pour charger les modes
int load_modes_config(const char *filename, ModesConfig *modes);

#endif