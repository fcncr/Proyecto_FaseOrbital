#pragma once
#ifndef MODELOS_REGISTROS_H
#define MODELOS_REGISTROS_H

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

struct RegistroPartida {
    int idPartida;
    uint32_t semilla;
    std::string modo;
    std::string jugador;
    bool esBot;
    int posicion;
    int puntaje;
    int oleada;
    int disparos;
    int correctos;
    int erroneos;
    int comboMax;
    int pulsos;
    int danoTotal;
    int ticks;
    int bajasDron;
    int bajasGirador;
    int bajasDivisor;
    int bajasMini;
    int bajasTanque;
    int bajasCamaleon;
};

#endif