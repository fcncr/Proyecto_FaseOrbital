#ifndef LISTA_ENEMIGOS_H
#define LISTA_ENEMIGOS_H

#include "ModelosCombate.h"

struct NodoEnemigo { //agrupa un enemigo y los enlaces a sus nodos vecinos
    Enemigo dato; //guarda los datos del enemigo almacenado en este nodo
    NodoEnemigo* ant; //apunta al nodo anterior o a nullptr si es el primero
    NodoEnemigo* sig; //apunta al siguiente nodo o a nullptr si es el ultimo
};

struct ListaEnemigos { //mantiene los extremos y la cantidad de nodos de la lista doble
    NodoEnemigo* cabeza; //apunta al primer nodo o a nullptr si la lista esta vacia
    NodoEnemigo* cola; //apunta al ultimo nodo o a nullptr si la lista esta vacia
    int tam; //guarda la cantidad de enemigos almacenados

    ListaEnemigos(); //crea la lista vacia con sus campos inicializados
    ~ListaEnemigos(); //libera los nodos propios al terminar la vida del objeto
    ListaEnemigos(const ListaEnemigos& otra) = delete; //se usa por referencia porque sus nodos tienen un unico propietario
    ListaEnemigos& operator=(const ListaEnemigos& otra) = delete; //evita compartir nodos mediante una asignacion
};

void inicializarListaEnemigos(ListaEnemigos& lista); //libera los nodos previos y deja la lista vacia
NodoEnemigo* insertarFinal(ListaEnemigos& lista, const Enemigo& enemigo); //agrega un enemigo al final y devuelve el nodo creado
NodoEnemigo* insertarDespues(ListaEnemigos& lista, NodoEnemigo* nodo, const Enemigo& enemigo); //inserta despues del nodo recibido y devuelve el nuevo nodo
NodoEnemigo* eliminarEnemigo(ListaEnemigos& lista, NodoEnemigo* nodo); //libera el nodo recibido y devuelve el que estaba despues
NodoEnemigo* primeroEnemigo(const ListaEnemigos& lista); //devuelve el primer nodo de la lista
NodoEnemigo* ultimoEnemigo(const ListaEnemigos& lista); //devuelve el ultimo nodo de la lista
int tamanoListaEnemigos(const ListaEnemigos& lista); //devuelve la cantidad de enemigos almacenados
bool listaEnemigosVacia(const ListaEnemigos& lista); //indica si la lista tiene cero enemigos
void destruirListaEnemigos(ListaEnemigos& lista); //libera todos los nodos y deja la lista vacia

#endif