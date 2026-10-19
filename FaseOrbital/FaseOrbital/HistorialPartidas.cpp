#include "HistorialPartidas.h"

void inicializarHistorial(HistorialPartidas& historial) {
    historial.cabeza = nullptr;
    historial.cola = nullptr;
    historial.tam = 0;
}

void agregarRegistroFinal(HistorialPartidas& historial, const RegistroPartida& registro) {
    NodoRegistroPartida* nuevo = new NodoRegistroPartida;
    nuevo->dato = registro;
    nuevo->sig = nullptr;

    if (historial.cola == nullptr) {
        historial.cabeza = nuevo;
        historial.cola = nuevo;
    }
    else {
        historial.cola->sig = nuevo;
        historial.cola = nuevo;
    }

    historial.tam++;
}

void recorrerHistorial(const HistorialPartidas& historial, void (*procesar)(const RegistroPartida&)) {
    if (procesar == nullptr) {
        return;
    }

    const NodoRegistroPartida* actual = historial.cabeza;

    while (actual != nullptr) {
        procesar(actual->dato);
        actual = actual->sig;
    }
}

int contarRegistros(const HistorialPartidas& historial) {
    return historial.tam;
}

void buscarRegistrosJugador(const HistorialPartidas& origen, const std::string& jugador, HistorialPartidas& destino) {
    HistorialPartidas temporal;
    inicializarHistorial(temporal);

    const NodoRegistroPartida* actual = origen.cabeza;

    while (actual != nullptr) {
        if (actual->dato.jugador == jugador) {
            agregarRegistroFinal(temporal, actual->dato);
        }

        actual = actual->sig;
    }

    destruirHistorial(destino);

    destino.cabeza = temporal.cabeza;
    destino.cola = temporal.cola;
    destino.tam = temporal.tam;

    temporal.cabeza = nullptr;
    temporal.cola = nullptr;
    temporal.tam = 0;
}

void destruirHistorial(HistorialPartidas& historial) {
    NodoRegistroPartida* actual = historial.cabeza;

    while (actual != nullptr) {
        NodoRegistroPartida* siguiente = actual->sig;
        delete actual;
        actual = siguiente;
    }

    historial.cabeza = nullptr;
    historial.cola = nullptr;
    historial.tam = 0;
}