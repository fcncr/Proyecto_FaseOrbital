#ifndef SPAWN_H
#define SPAWN_H

#include "TipoEnemigo.h"

struct Spawn {
    int tick;
    TipoEnemigo tipo;
    int color;
    int angulo;
    int sentido;
};

#endif