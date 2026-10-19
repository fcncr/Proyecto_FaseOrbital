#include "BitacoraEventos.h"

void inicializarBitacora(BitacoraEventos& bitacora) {
    bitacora.cabeza = nullptr;
    bitacora.cola = nullptr;
    bitacora.tam = 0;
}

void agregarEventoFinal(BitacoraEventos& bitacora, const Evento& evento) {
    NodoEvento* nuevo = new NodoEvento;
    nuevo->dato = evento;
    nuevo->sig = nullptr;

    if (bitacora.cola == nullptr) {
        bitacora.cabeza = nuevo;
        bitacora.cola = nuevo;
    }
    else {
        bitacora.cola->sig = nuevo;
        bitacora.cola = nuevo;
    }

    bitacora.tam++;
}

void recorrerBitacora(const BitacoraEventos& bitacora, void (*procesar)(const Evento&)) {
    if (procesar == nullptr) {
        return;
    }

    const NodoEvento* actual = bitacora.cabeza;

    while (actual != nullptr) {
        procesar(actual->dato);
        actual = actual->sig;
    }
}

void filtrarPorJugador(const BitacoraEventos& origen, const std::string& jugador, BitacoraEventos& destino) {
    BitacoraEventos temporal;
    inicializarBitacora(temporal);

    const NodoEvento* actual = origen.cabeza;

    while (actual != nullptr) {
        if (actual->dato.jugador == jugador) {
            agregarEventoFinal(temporal, actual->dato);
        }

        actual = actual->sig;
    }

    destruirBitacora(destino);

    destino.cabeza = temporal.cabeza;
    destino.cola = temporal.cola;
    destino.tam = temporal.tam;

    temporal.cabeza = nullptr;
    temporal.cola = nullptr;
    temporal.tam = 0;
}

void filtrarPorOleada(const BitacoraEventos& origen, int oleada, BitacoraEventos& destino) {
    BitacoraEventos temporal;
    inicializarBitacora(temporal);

    const NodoEvento* actual = origen.cabeza;

    while (actual != nullptr) {
        if (actual->dato.oleada == oleada) {
            agregarEventoFinal(temporal, actual->dato);
        }

        actual = actual->sig;
    }

    destruirBitacora(destino);

    destino.cabeza = temporal.cabeza;
    destino.cola = temporal.cola;
    destino.tam = temporal.tam;

    temporal.cabeza = nullptr;
    temporal.cola = nullptr;
    temporal.tam = 0;
}

void destruirBitacora(BitacoraEventos& bitacora) {
    NodoEvento* actual = bitacora.cabeza;

    while (actual != nullptr) {
        NodoEvento* siguiente = actual->sig;
        delete actual;
        actual = siguiente;
    }

    bitacora.cabeza = nullptr;
    bitacora.cola = nullptr;
    bitacora.tam = 0;
}