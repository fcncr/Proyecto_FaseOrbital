#include "ColaSpawn.h"

void inicializarColaSpawn(ColaSpawn& cola) {
    cola.frente = nullptr;
    cola.final = nullptr;
    cola.tam = 0;
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
    if (&origen == &destino) {
        return;
    }
    vaciarColaSpawn(destino);
    NodoSpawn* actual = origen.frente;
    while (actual != nullptr) {
        encolarSpawn(destino, actual->dato);
        actual = actual->sig;
    }
}