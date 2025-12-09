#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

// Fonction utilitaire pour enlever les espaces
void trim(char *str) {
    char *end;
    while(*str == ' ' || *str == '\t') str++;
    if(*str == 0) return;
    end = str + strlen(str) - 1;
    while(end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) end--;
    *(end+1) = 0;
}

int load_game_config(const char *filename, GameConfig *config) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur: impossible d'ouvrir %s\n", filename);
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Ignorer les commentaires et lignes vides
        if (line[0] == '#' || line[0] == '\n') continue;

        char key[64], value[64];
        if (sscanf(line, "%[^=]=%s", key, value) == 2) {
            trim(key);
            trim(value);

            if (strcmp(key, "SCREEN_WIDTH") == 0)
                config->screen_width = atoi(value);
            else if (strcmp(key, "SCREEN_HEIGHT") == 0)
                config->screen_height = atoi(value);
            else if (strcmp(key, "TILE_SIZE") == 0)
                config->tile_size = atoi(value);
            else if (strcmp(key, "FPS") == 0)
                config->fps = atoi(value);
            else if (strcmp(key, "LIVES") == 0)
                config->lives = atoi(value);
        }
    }

    fclose(file);
    printf("Config chargée: %dx%d, tile:%d, fps:%d, vies:%d\n",
           config->screen_width, config->screen_height,
           config->tile_size, config->fps, config->lives);
    return 1;
}

GhostBehavior parse_behavior(const char *str) {
    if (strcmp(str, "AGGRESSIVE") == 0) return BEHAVIOR_AGGRESSIVE;
    if (strcmp(str, "PATROL") == 0) return BEHAVIOR_PATROL;
    if (strcmp(str, "RANDOM") == 0) return BEHAVIOR_RANDOM;
    if (strcmp(str, "FLEE") == 0) return BEHAVIOR_FLEE;
    return BEHAVIOR_INACTIVE;
}

int load_ghosts_config(const char *filename, GhostConfig ghosts[4]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur: impossible d'ouvrir %s\n", filename);
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        char behavior_str[32];
        int id, active;
        if (sscanf(line, "GHOST_%d|%[^|]|%d", &id, behavior_str, &active) == 3) {
            if (id >= 0 && id < 4) {
                ghosts[id].id = id;
                ghosts[id].behavior = parse_behavior(behavior_str);
                ghosts[id].active = active;
                printf("Fantôme %d: %s, actif:%d\n", id, behavior_str, active);
            }
        }
    }

    fclose(file);
    return 1;
}

int load_modes_config(const char *filename, ModesConfig *modes) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur: impossible d'ouvrir %s\n", filename);
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        char mode[64];
        int enabled;
        if (sscanf(line, "%[^|]|%d", mode, &enabled) == 2) {
            trim(mode);
            if (strcmp(mode, "MODE_TRADITIONAL") == 0)
                modes->traditional = enabled;
            else if (strcmp(mode, "MODE_SURVIVAL") == 0)
                modes->survival = enabled;
            else if (strcmp(mode, "MODE_TIME_ATTACK") == 0)
                modes->time_attack = enabled;
        }
    }

    fclose(file);
    return 1;
}