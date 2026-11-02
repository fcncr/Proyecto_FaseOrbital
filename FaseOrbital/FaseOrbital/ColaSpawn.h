#ifndef COLA_SPAWN_H
#define COLA_SPAWN_H

#include "ModelosCombate.h"

struct NodoSpawn { //agrupa una aparicion de enemigo y el enlace al siguiente nodo
    Spawn dato; //guarda el tick y los datos del enemigo que debe aparecer
    NodoSpawn* sig; //apunta al siguiente nodo o a nullptr si es el ultimo
};

struct ColaSpawn { //organiza las apariciones en el mismo orden en que se agregan
    NodoSpawn* frente; //apunta a la proxima aparicion que se retirara de la cola
    NodoSpawn* final; //apunta al ultimo nodo para agregar nuevas apariciones al final
    int tam; //guarda la cantidad de apariciones pendientes

    ColaSpawn(); //crea una cola vacia con sus punteros y contador inicializados
    ColaSpawn(const ColaSpawn& otra); //crea una copia con nodos independientes de la cola original
    ColaSpawn& operator=(const ColaSpawn& otra); //reemplaza el contenido por una copia independiente de otra cola
    ~ColaSpawn(); //libera automaticamente los nodos cuando se destruye el objeto
};

void inicializarColaSpawn(ColaSpawn& cola); //deja la cola vacia y libera los nodos que tuviera
void encolarSpawn(ColaSpawn& cola, const Spawn& spawn); //agrega una copia de la aparicion al final de la cola
bool desencolarSpawn(ColaSpawn& cola, Spawn& salida); //retira la primera aparicion y copia sus datos en salida
const Spawn* verFrenteSpawn(const ColaSpawn& cola); //permite consultar la primera aparicion sin retirarla ni modificarla
bool colaSpawnVacia(const ColaSpawn& cola); //indica si no quedan apariciones pendientes
int tamanoColaSpawn(const ColaSpawn& cola); //devuelve la cantidad de apariciones almacenadas
void vaciarColaSpawn(ColaSpawn& cola); //libera todos los nodos y restablece los extremos y el contador
void destruirColaSpawn(ColaSpawn& cola); //libera manualmente los nodos mediante la funcion de vaciado
void copiarColaSpawn(const ColaSpawn& origen, ColaSpawn& destino); //copia las apariciones del origen al destino sin compartir nodos

#endif