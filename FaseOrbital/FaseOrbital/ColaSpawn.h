#ifndef COLA_SPAWN_H
#define COLA_SPAWN_H

#include "ModelosCombate.h" //llama a struct "Spawn"

struct NodoSpawn {
    Spawn dato;
    NodoSpawn* sig;
};

struct ColaSpawn {
    NodoSpawn* frente;
    NodoSpawn* final;
    int tam;
};

void inicializarColaSpawn(ColaSpawn& cola);
void encolarSpawn(ColaSpawn& cola, const Spawn& spawn);
bool desencolarSpawn(ColaSpawn& cola, Spawn& salida);
const Spawn* verFrenteSpawn(const ColaSpawn& cola);
bool colaSpawnVacia(const ColaSpawn& cola);
int tamanoColaSpawn(const ColaSpawn& cola);
void vaciarColaSpawn(ColaSpawn& cola);
void destruirColaSpawn(ColaSpawn& cola);
void copiarColaSpawn(const ColaSpawn& origen, ColaSpawn& destino);

#endif