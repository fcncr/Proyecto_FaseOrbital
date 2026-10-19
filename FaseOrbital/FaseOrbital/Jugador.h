#ifndef JUGADOR_H
#define JUGADOR_H

#include <string>
#include "MetricasJugador.h"

struct Jugador {
    std::string alias;
    bool esBot;
    int vidaNucleo;
    int puntaje;
    int combo;
    int cargaPulso;
    int oleadaAlcanzada;
    bool eliminado;
    MetricasJugador metricas;
};

#endif