#pragma once
#ifndef EVENTO_H
#define EVENTO_H

#include <string>

enum TipoEvento {
    BAJA,
    IMPACTO_ERRONEO,
    DANO_NUCLEO,
    CHOQUE_NAVE,
    PULSO,
    FIN_OLEADA,
    ELIMINACION
};

struct Evento {
    int tick;
    int oleada;
    std::string jugador;
    TipoEvento tipoEvento;
    int tipoEnemigo;
    int color;
    int valor;
};

#endif