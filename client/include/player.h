#ifndef PLAYER_H
#define PLAYER_H


typedef struct {
    int x, y;             // Position précise en pixels
    int dx, dy;           // Direction actuelle (-1, 0 ou 1)
    int next_dx, next_dy; // Prochaine direction voulue par le joueur
    int speed;            // Vitesse de déplacement (pixels par frame)
} Player;

#endif 