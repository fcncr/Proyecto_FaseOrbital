#include "RondaJugadores.h"

void inicializarRondaJugadores(RondaJugadores& ronda) {
    ronda.actual = nullptr;
    ronda.tam = 0;
}

void agregarJugador(RondaJugadores& ronda, Jugador* jugador) {
    if (jugador == nullptr) {
        return;
    }

    NodoJugador* nuevo = new NodoJugador;
    nuevo->jugador = jugador;

    if (ronda.actual == nullptr) {
        nuevo->sig = nuevo;
        nuevo->ant = nuevo;
        ronda.actual = nuevo;
        ronda.tam = 1;
        return;
    }

    NodoJugador* ultimo = ronda.actual->ant;
    nuevo->sig = ronda.actual;
    nuevo->ant = ultimo;
    ultimo->sig = nuevo;
    ronda.actual->ant = nuevo;
    ronda.tam++;
}

void avanzarJugador(RondaJugadores& ronda, bool horario) {
    if (ronda.actual == nullptr) {
        return;
    }
    if (horario) {
        ronda.actual = ronda.actual->sig;
    }
    else {
        ronda.actual = ronda.actual->ant;
    }
}

Jugador* eliminarJugadorActual(RondaJugadores& ronda, bool horario) {
    if (ronda.actual == nullptr) {
        return nullptr;
    }

    NodoJugador* eliminado = ronda.actual;
    Jugador* jugadorEliminado = eliminado->jugador;

    if (ronda.tam == 1) {
        delete eliminado;
        ronda.actual = nullptr;
        ronda.tam = 0;
        return jugadorEliminado;
    }

    NodoJugador* anterior = eliminado->ant;
    NodoJugador* siguiente = eliminado->sig;
    anterior->sig = siguiente;
    siguiente->ant = anterior;

    if (horario) {
        ronda.actual = siguiente;
    }
    else {
        ronda.actual = anterior;
    }

    delete eliminado;
    ronda.tam--;

    return jugadorEliminado;
}

Jugador* verJugadorActual(const RondaJugadores& ronda) {
    if (ronda.actual == nullptr) {
        return nullptr;
    }
    return ronda.actual->jugador;
}

int tamanoRonda(const RondaJugadores& ronda) {
    return ronda.tam;
}

bool rondaVacia(const RondaJugadores& ronda) {
    return ronda.actual == nullptr;
}

void destruirRondaJugadores(RondaJugadores& ronda) {
    while (ronda.actual != nullptr) {
        eliminarJugadorActual(ronda, true);
    }
}