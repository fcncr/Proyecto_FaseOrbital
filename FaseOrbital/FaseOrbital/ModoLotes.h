#ifndef MODO_LOTES_H
#define MODO_LOTES_H

#include "Motor.h"
#include "ScriptAcciones.h"

struct ResultadoModoLotes {
    int ticksEjecutados;
    int vidaNucleo;
    int puntaje;
    int combo;
    int cargaPulso;
    bool oleadaTerminada;
    bool turnoTerminado;
};

bool ejecutarModoLotes(Jugador& jugador, int numeroOleada, const ColaSpawn& spawns, const ScriptAcciones& script, int limiteTicks, ResultadoModoLotes& resultado);

#endif