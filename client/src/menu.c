#include "../include/menu.h"
#include "../include/render.h"
#include <dirent.h>
#include <SDL/SDL_rotozoom.h>

#define MAX_MAPS 32
#define MAX_PATH_LEN 256

static void draw_text(SDL_Surface *screen, int x, int y, const char *text, Uint8 r, Uint8 g, Uint8 b) {
    stringRGBA(screen, x, y, text, r, g, b, 255);
}

static SDL_Surface* load_image(const char *path) {
    SDL_Surface *img = IMG_Load(path);
    if (!img) {
        printf("[MENU] Erreur IMG_Load %s : %s\n", path, IMG_GetError());
    }
    return img;
}

static SDL_Surface* scale_image(SDL_Surface *src, float factor) {
    if (!src || factor == 1.0f) return src;
    SDL_Surface *scaled = zoomSurface(src, factor, factor, 1);
    if (!scaled) {
        printf("[MENU] Erreur scale_image : %s\n", SDL_GetError());
        return src;
    }
    SDL_FreeSurface(src);
    return scaled;
}

static SDL_Surface* scale_image_to(SDL_Surface *src, int target_w, int target_h) {
    if (!src) return NULL;
    float fx = (float)target_w / (float)src->w;
    float fy = (float)target_h / (float)src->h;
    SDL_Surface *scaled = zoomSurface(src, fx, fy, 1);
    if (!scaled) {
        printf("[MENU] Erreur scale_image_to : %s\n", SDL_GetError());
        return src;
    }
    SDL_FreeSurface(src);
    return scaled;
}

// Bouton basé sur des sprites (normal/hover/click)
typedef struct {
    SDL_Rect rect;
    SDL_Surface *normal;
    SDL_Surface *hover;
    SDL_Surface *clicked;
    int hovered;
    int pressed;
} ImgButton;

static int is_mouse_over(ImgButton *btn, int mx, int my) {
    return mx >= btn->rect.x && mx <= btn->rect.x + btn->rect.w &&
           my >= btn->rect.y && my <= btn->rect.y + btn->rect.h;
}

static void draw_img_button(SDL_Surface *screen, ImgButton *btn) {
    SDL_Surface *src = btn->normal;
    if (btn->pressed && btn->clicked) src = btn->clicked;
    else if (btn->hovered && btn->hover) src = btn->hover;
    if (src) {
        SDL_BlitSurface(src, NULL, screen, &btn->rect);
    }
}

// Récupère la liste des maps disponibles dans "maps" (relatif au dossier d'exécution)
static int list_maps(const char *dir, char paths[][MAX_PATH_LEN], char names[][MAX_PATH_LEN], int max_count) {
    DIR *d = opendir(dir);
    if (!d) return 0;

    int count = 0;
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL && count < max_count) {
        const char *name = entry->d_name;
        size_t len = strlen(name);
        if (len > 4 && strcmp(name + len - 4, ".map") == 0) {
            snprintf(names[count], MAX_PATH_LEN, "%s", name);
            snprintf(paths[count], MAX_PATH_LEN, "%s/%s", dir, name);
            count++;
        }
    }
    closedir(d);
    return count;
}

static int map_select_view(SDL_Surface *screen, GameConfig *config, ProgramState *state,
                           char *selected_map, size_t selected_map_size) {
    char map_paths[MAX_MAPS][MAX_PATH_LEN];
    char map_names[MAX_MAPS][MAX_PATH_LEN];
    int map_count = list_maps("maps", map_paths, map_names, MAX_MAPS);

    if (map_count == 0) {
        printf("Aucune map trouvée dans maps/. On utilise maps/test.map\n");
        snprintf(selected_map, selected_map_size, "%s", "maps/test.map");
        *state = STATE_GAME;
        return 0;
    }

    int selected = 0;
    int running = 1;
    SDL_Event ev;

    SDL_Surface *bg = load_image("../assets/maps-settings/background.png");
    if (bg) bg = scale_image_to(bg, config->screen_width, config->screen_height);
    ImgButton back_btn = {0};
    back_btn.normal = load_image("../assets/maps-settings/back.png");
    if (back_btn.normal) {
        // réduire encore (x0.5 par rapport à l'étape précédente)
        back_btn.normal = scale_image(back_btn.normal, 0.14f);
        back_btn.hover = back_btn.normal;
        back_btn.clicked = back_btn.normal;
        back_btn.rect.w = back_btn.normal->w;
        back_btn.rect.h = back_btn.normal->h;
    } else {
        back_btn.rect.w = 90; back_btn.rect.h = 38;
    }
    // position en bas à droite, marge plus faible
    int margin = 20;
    back_btn.rect.x = config->screen_width - back_btn.rect.w - margin;
    back_btn.rect.y = config->screen_height - back_btn.rect.h - margin;

    Uint32 hilite = SDL_MapRGB(screen->format, 70, 130, 200);
    Uint32 normal = SDL_MapRGB(screen->format, 40, 40, 80);
    int start_y = 100;
    int line_h = 40;

    while (running && *state == STATE_MAP_SELECT) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                *state = STATE_QUIT;
                running = 0;
            } else if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        *state = STATE_MENU;
                        running = 0;
                        break;
                    case SDLK_UP:
                        if (selected > 0) selected--;
                        break;
                    case SDLK_DOWN:
                        if (selected < map_count - 1) selected++;
                        break;
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        snprintf(selected_map, selected_map_size, "%s", map_paths[selected]);
                        *state = STATE_GAME;
                        running = 0;
                        break;
                    default:
                        break;
                }
            } else if (ev.type == SDL_MOUSEMOTION) {
                int mx = ev.motion.x, my = ev.motion.y;
                back_btn.hovered = is_mouse_over(&back_btn, mx, my);
            } else if (ev.type == SDL_MOUSEBUTTONDOWN) {
                int mx = ev.button.x, my = ev.button.y;
                if (is_mouse_over(&back_btn, mx, my)) back_btn.pressed = 1;
            } else if (ev.type == SDL_MOUSEBUTTONUP) {
                int mx = ev.button.x, my = ev.button.y;
                if (back_btn.pressed && is_mouse_over(&back_btn, mx, my)) {
                    *state = STATE_MENU;
                    running = 0;
                }
                // clic sur une map : sélectionner et lancer
                for (int i = 0; i < map_count && running; i++) {
                    SDL_Rect r = {40, start_y + i * line_h, config->screen_width - 80, line_h - 8};
                    if (mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h) {
                        selected = i;
                        snprintf(selected_map, selected_map_size, "%s", map_paths[selected]);
                        *state = STATE_GAME;
                        running = 0;
                        break;
                    }
                }
                back_btn.pressed = 0;
            }
        }

        if (bg) SDL_BlitSurface(bg, NULL, screen, NULL);
        else SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 20, 20, 40));
        draw_text(screen, 40, 40, "Choisissez une map (Flèches + Entrée, ESC pour retour)", 255, 255, 0);

        for (int i = 0; i < map_count; i++) {
            SDL_Rect r = {40, start_y + i * line_h, config->screen_width - 80, line_h - 8};
            Uint32 color = (i == selected) ? hilite : normal;
            SDL_FillRect(screen, &r, color);
            draw_text(screen, r.x + 10, r.y + 10, map_names[i], 255, 255, 255);
        }

        draw_img_button(screen, &back_btn);

        SDL_Flip(screen);
        SDL_Delay(1000 / 60);
    }

    if (bg) SDL_FreeSurface(bg);
    if (back_btn.normal) SDL_FreeSurface(back_btn.normal);

    return 0;
}

int menu_view(SDL_Surface *screen, GameConfig *config, ProgramState *state,
              char *selected_map, size_t selected_map_size) {
    printf("=== Menu Principal ===\n");

    // Chargement assets (paths relatifs depuis output/)
    SDL_Surface *bg = load_image("../assets/menu/background.png");
    if (bg) bg = scale_image_to(bg, config->screen_width, config->screen_height);

    ImgButton play_btn = {0};
    ImgButton quit_btn = {0};

    play_btn.normal  = load_image("../assets/buttons/play_normal.png");
    play_btn.hover   = load_image("../assets/buttons/play_hovered.png");
    play_btn.clicked = load_image("../assets/buttons/play_clicked.png");

    quit_btn.normal  = load_image("../assets/buttons/quit_normal.png");
    quit_btn.hover   = load_image("../assets/buttons/quit_hovered.png");
    quit_btn.clicked = load_image("../assets/buttons/quit_clicked.png");

    // Agrandir davantage les boutons pour le menu principal
    float main_btn_scale = 1.55f;
    if (play_btn.normal) play_btn.normal = scale_image(play_btn.normal, main_btn_scale);
    if (play_btn.hover) play_btn.hover = scale_image(play_btn.hover, main_btn_scale);
    if (play_btn.clicked) play_btn.clicked = scale_image(play_btn.clicked, main_btn_scale);
    if (quit_btn.normal) quit_btn.normal = scale_image(quit_btn.normal, main_btn_scale);
    if (quit_btn.hover) quit_btn.hover = scale_image(quit_btn.hover, main_btn_scale);
    if (quit_btn.clicked) quit_btn.clicked = scale_image(quit_btn.clicked, main_btn_scale);

    // Positionnement
    int center_x = config->screen_width / 2;
    int spacing = 190;
    int start_y = config->screen_height / 2 - 110;

    // Utiliser la taille réelle des sprites si dispo
    if (play_btn.normal) {
        play_btn.rect.w = play_btn.normal->w;
        play_btn.rect.h = play_btn.normal->h;
    } else { play_btn.rect.w = 340; play_btn.rect.h = 96; }
    if (quit_btn.normal) {
        quit_btn.rect.w = quit_btn.normal->w;
        quit_btn.rect.h = quit_btn.normal->h;
    } else { quit_btn.rect.w = 340; quit_btn.rect.h = 96; }

    play_btn.rect.x = center_x - play_btn.rect.w / 2;
    play_btn.rect.y = start_y;
    quit_btn.rect.x = center_x - quit_btn.rect.w / 2;
    quit_btn.rect.y = start_y + spacing;

    int running = 1;
    SDL_Event ev;

    while (running && *state == STATE_MENU) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                *state = STATE_QUIT;
                running = 0;
            } else if (ev.type == SDL_MOUSEMOTION) {
                int mx = ev.motion.x, my = ev.motion.y;
                play_btn.hovered = is_mouse_over(&play_btn, mx, my);
                quit_btn.hovered = is_mouse_over(&quit_btn, mx, my);
            } else if (ev.type == SDL_MOUSEBUTTONDOWN) {
                int mx = ev.button.x, my = ev.button.y;
                if (is_mouse_over(&play_btn, mx, my)) play_btn.pressed = 1;
                if (is_mouse_over(&quit_btn, mx, my)) quit_btn.pressed = 1;
            } else if (ev.type == SDL_MOUSEBUTTONUP) {
                int mx = ev.button.x, my = ev.button.y;
                if (play_btn.pressed && is_mouse_over(&play_btn, mx, my)) {
                    *state = STATE_MAP_SELECT;
                    running = 0;
                } else if (quit_btn.pressed && is_mouse_over(&quit_btn, mx, my)) {
                    *state = STATE_QUIT;
                    running = 0;
                }
                play_btn.pressed = quit_btn.pressed = 0;
            } else if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_ESCAPE) {
                    *state = STATE_QUIT;
                    running = 0;
                } else if (ev.key.keysym.sym == SDLK_RETURN || ev.key.keysym.sym == SDLK_KP_ENTER) {
                    *state = STATE_MAP_SELECT;
                    running = 0;
                }
            }
        }

        // Fond
        if (bg) SDL_BlitSurface(bg, NULL, screen, NULL);
        else SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 15, 15, 30));

        // Titre
        draw_text(screen, center_x - 120, 80, "PACMAN SDL - MENU", 255, 255, 0);

        // Boutons
        draw_img_button(screen, &play_btn);
        draw_img_button(screen, &quit_btn);

        SDL_Flip(screen);
        SDL_Delay(1000 / 60);
    }

    // Sélection de map
    if (*state == STATE_MAP_SELECT) {
        map_select_view(screen, config, state, selected_map, selected_map_size);
    }

    // Cleanup
    if (bg) SDL_FreeSurface(bg);
    if (play_btn.normal) SDL_FreeSurface(play_btn.normal);
    if (play_btn.hover) SDL_FreeSurface(play_btn.hover);
    if (play_btn.clicked) SDL_FreeSurface(play_btn.clicked);
    if (quit_btn.normal) SDL_FreeSurface(quit_btn.normal);
    if (quit_btn.hover) SDL_FreeSurface(quit_btn.hover);
    if (quit_btn.clicked) SDL_FreeSurface(quit_btn.clicked);

    return 0;
}
