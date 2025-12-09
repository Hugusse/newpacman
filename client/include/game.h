#ifndef GAME_H
#define GAME_H

#include "global.h"
#include "config.h"
#include "map.h"
#include "render.h"
#include "player.h"
#include "menu.h"

// Lance une partie en utilisant la map fournie (chemin relatif depuis le dossier d'exécution)
int game_view(SDL_Surface *screen, GameConfig *config, ProgramState *state, const char *map_path);

#endif
