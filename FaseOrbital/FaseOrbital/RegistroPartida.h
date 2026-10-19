#ifndef REGISTRO_PARTIDA_H
#define REGISTRO_PARTIDA_H

#include <cstdint>
#include <string>

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