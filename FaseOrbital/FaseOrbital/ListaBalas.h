#ifndef LISTA_BALAS_H
#define LISTA_BALAS_H

#include "ModelosCombate.h"

struct NodoBala { //agrupa una bala y el enlace al siguiente nodo
    Bala dato; //guarda la posicion velocidad color y duenio de la bala
    NodoBala* sig; //apunta al siguiente nodo o a nullptr si es el ultimo
};

struct ListaBalas { //mantiene el inicio y la cantidad de nodos de la lista
    NodoBala* cabeza; //apunta al primer nodo o a nullptr si la lista esta vacia
    int tam; //guarda la cantidad de balas almacenadas

    ListaBalas(); //crea la lista vacia con sus campos inicializados
    ~ListaBalas(); //libera los nodos propios al terminar la vida del objeto
    ListaBalas(const ListaBalas& otra) = delete; //se usa por referencia porque sus nodos tienen un unico propietario
    ListaBalas& operator=(const ListaBalas& otra) = delete; //evita compartir nodos mediante una asignacion
};

void inicializarListaBalas(ListaBalas& lista); //libera los nodos previos y deja la lista vacia
void insertarInicio(ListaBalas& lista, const Bala& bala); //agrega una copia de la bala al principio de la lista
int eliminarBalasSi(ListaBalas& lista, bool (*predicado)(const Bala&)); //elimina las balas que cumplen la condicion recibida y devuelve su cantidad
int tamanoListaBalas(const ListaBalas& lista); //devuelve la cantidad de balas almacenadas
bool listaBalasVacia(const ListaBalas& lista); //indica si la lista tiene cero balas
void destruirListaBalas(ListaBalas& lista); //libera todos los nodos y deja la lista vacia

#endif