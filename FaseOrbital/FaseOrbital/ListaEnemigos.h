#ifndef LISTA_ENEMIGOS_H
#define LISTA_ENEMIGOS_H

#include "ModelosCombate.h" //llama al struct "Enemigo"

struct NodoEnemigo {
    Enemigo dato;
    NodoEnemigo* ant;
    NodoEnemigo* sig;
};

struct ListaEnemigos {
    NodoEnemigo* cabeza;
    NodoEnemigo* cola;
    int tam;
};

void inicializarListaEnemigos(ListaEnemigos& lista);
NodoEnemigo* insertarFinal(ListaEnemigos& lista, const Enemigo& enemigo);
NodoEnemigo* insertarDespues(ListaEnemigos& lista, NodoEnemigo* nodo, const Enemigo& enemigo);
NodoEnemigo* eliminarEnemigo(ListaEnemigos& lista, NodoEnemigo* nodo);
NodoEnemigo* primeroEnemigo(const ListaEnemigos& lista);
NodoEnemigo* ultimoEnemigo(const ListaEnemigos& lista);
int tamanoListaEnemigos(const ListaEnemigos& lista);
bool listaEnemigosVacia(const ListaEnemigos& lista);
void destruirListaEnemigos(ListaEnemigos& lista);

#endif