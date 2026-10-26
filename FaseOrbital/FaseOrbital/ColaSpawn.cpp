#include "ColaSpawn.h"

ColaSpawn::ColaSpawn() {
    frente = nullptr;
    final = nullptr;
    tam = 0;
}

ColaSpawn::ColaSpawn(const ColaSpawn& otra) {
    frente = nullptr;
    final = nullptr;
    tam = 0;

    NodoSpawn* actual = otra.frente;

    while (actual != nullptr) {
        encolarSpawn(*this, actual->dato);
        actual = actual->sig;
    }
}

ColaSpawn& ColaSpawn::operator=(const ColaSpawn& otra) {
    if (this == &otra) {
        return *this;
    }

    vaciarColaSpawn(*this);

    NodoSpawn* actual = otra.frente;

    while (actual != nullptr) {
        encolarSpawn(*this, actual->dato);
        actual = actual->sig;
    }

    return *this;
}

ColaSpawn::~ColaSpawn() {
    vaciarColaSpawn(*this);
}

void inicializarColaSpawn(ColaSpawn& cola) {
    vaciarColaSpawn(cola);
}

void encolarSpawn(ColaSpawn& cola, const Spawn& spawn) {
    NodoSpawn* nuevo = new NodoSpawn;
    nuevo->dato = spawn;
    nuevo->sig = nullptr;

    if (cola.final == nullptr) {
        cola.frente = nuevo;
        cola.final = nuevo;
    }
    else {
        cola.final->sig = nuevo;
        cola.final = nuevo;
    }

    cola.tam++;
}

bool desencolarSpawn(ColaSpawn& cola, Spawn& salida) {
    if (cola.frente == nullptr) {
        return false;
    }

    NodoSpawn* eliminado = cola.frente;
    salida = eliminado->dato;
    cola.frente = eliminado->sig;

    delete eliminado;

    cola.tam--;

    if (cola.frente == nullptr) {
        cola.final = nullptr;
        cola.tam = 0;
    }

    return true;
}

const Spawn* verFrenteSpawn(const ColaSpawn& cola) {
    if (cola.frente == nullptr) {
        return nullptr;
    }

    return &cola.frente->dato;
}

bool colaSpawnVacia(const ColaSpawn& cola) {
    return cola.frente == nullptr;
}

int tamanoColaSpawn(const ColaSpawn& cola) {
    return cola.tam;
}

void vaciarColaSpawn(ColaSpawn& cola) {
    NodoSpawn* actual = cola.frente;

    while (actual != nullptr) {
        NodoSpawn* siguiente = actual->sig;
        delete actual;
        actual = siguiente;
    }

    cola.frente = nullptr;
    cola.final = nullptr;
    cola.tam = 0;
}

void destruirColaSpawn(ColaSpawn& cola) {
    vaciarColaSpawn(cola);
}

void copiarColaSpawn(const ColaSpawn& origen, ColaSpawn& destino) {
    destino = origen;
}