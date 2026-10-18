#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "TipoEnemigo.h"

struct Enemigo {
    int id;
    TipoEnemigo tipo;
    int color;
    double r;
    double theta;
    double omega;
    int vida;
    int radio;
    int ticksColor;
};

#endif