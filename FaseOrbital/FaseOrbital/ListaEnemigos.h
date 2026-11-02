#ifndef LISTA_ENEMIGOS_H
#define LISTA_ENEMIGOS_H

#include "ModelosCombate.h"

struct NodoEnemigo { //Agrupa un enemigo y los enlaces a sus nodos vecinos
    Enemigo dato; //Guarda los datos del enemigo almacenado en este nodo
    NodoEnemigo* ant; //Apunta al nodo anterior o a nullptr si es el primero
    NodoEnemigo* sig; //Apunta al siguiente nodo o a nullptr si es el ultimo
};

struct ListaEnemigos { //Mantiene los extremos y la cantidad de nodos de la lista doble
    NodoEnemigo* cabeza; //Apunta al primer nodo o a nullptr si la lista esta vacia
    NodoEnemigo* cola; //Apunta al ultimo nodo o a nullptr si la lista esta vacia
    int tam; //Guarda la cantidad de enemigos almacenados
};

void inicializarListaEnemigos(ListaEnemigos& lista); //Establece los valores iniciales sin liberar nodos previos
NodoEnemigo* insertarFinal(ListaEnemigos& lista, const Enemigo& enemigo); //Agrega un enemigo al final y devuelve el nodo creado
NodoEnemigo* insertarDespues(ListaEnemigos& lista, NodoEnemigo* nodo, const Enemigo& enemigo); //Inserta despues del nodo recibido y devuelve el nuevo nodo
NodoEnemigo* eliminarEnemigo(ListaEnemigos& lista, NodoEnemigo* nodo); //Libera el nodo recibido y devuelve el que estaba despues
NodoEnemigo* primeroEnemigo(const ListaEnemigos& lista); //Devuelve el primer nodo de la lista
NodoEnemigo* ultimoEnemigo(const ListaEnemigos& lista); //Devuelve el ultimo nodo de la lista
int tamanoListaEnemigos(const ListaEnemigos& lista); //Devuelve la cantidad de enemigos almacenados
bool listaEnemigosVacia(const ListaEnemigos& lista); //Indica si la lista tiene cero enemigos
void destruirListaEnemigos(ListaEnemigos& lista); //Libera todos los nodos y deja la lista vacia

#endif