#ifndef MENU_H
#define MENU_H

#include "global.h"
#include "config.h"

// États principaux du programme
typedef enum {
    STATE_MENU,
    STATE_MAP_SELECT,
    STATE_GAME,
    STATE_QUIT
} ProgramState;

// Affiche le menu principal puis la sélection de map
// - selected_map doit contenir un chemin valide vers une map (par défaut)
// - selected_map_size est la taille du buffer selected_map
int menu_view(SDL_Surface *screen, GameConfig *config, ProgramState *state,
              char *selected_map, size_t selected_map_size);

#endif
