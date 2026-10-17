#ifndef RONDA_JUGADORES_H
#define RONDA_JUGADORES_H

#include "../FaseOrbital/Jugador.h"

struct NodoJugador {
    Jugador* jugador;
    NodoJugador* ant;
    NodoJugador* sig;
};

struct RondaJugadores {
    NodoJugador* actual;
    int tam;
};

void inicializarRondaJugadores(RondaJugadores& ronda);
void agregarJugador(RondaJugadores& ronda, Jugador* jugador);
void avanzarJugador(RondaJugadores& ronda, bool horario);
Jugador* eliminarJugadorActual(RondaJugadores& ronda, bool horario);
Jugador* verJugadorActual(const RondaJugadores& ronda);
int tamanoRonda(const RondaJugadores& ronda);
bool rondaVacia(const RondaJugadores& ronda);
void destruirRondaJugadores(RondaJugadores& ronda);

#endif