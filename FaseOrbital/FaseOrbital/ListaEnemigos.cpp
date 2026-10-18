#include "ListaEnemigos.h"

void inicializarListaEnemigos(ListaEnemigos& lista) {
    lista.cabeza = nullptr;
    lista.cola = nullptr;
    lista.tam = 0;
}

NodoEnemigo* insertarFinal(ListaEnemigos& lista, const Enemigo& enemigo) {
    NodoEnemigo* nuevo = new NodoEnemigo{ enemigo, lista.cola, nullptr };

    if (lista.cola != nullptr) lista.cola->sig = nuevo;
    else lista.cabeza = nuevo;

    lista.cola = nuevo;
    lista.tam++;

    return nuevo;
}

NodoEnemigo* primeroEnemigo(const ListaEnemigos& lista) {
    return lista.cabeza;
}

NodoEnemigo* ultimoEnemigo(const ListaEnemigos& lista) {
    return lista.cola;
}

int tamanoListaEnemigos(const ListaEnemigos& lista) {
    return lista.tam;
}

bool listaEnemigosVacia(const ListaEnemigos& lista) {
    return lista.tam == 0;
}

void destruirListaEnemigos(ListaEnemigos& lista) {
    NodoEnemigo* actual = lista.cabeza;

    while (actual != nullptr) {
        NodoEnemigo* siguiente = actual->sig;
        delete actual;
        actual = siguiente;
    }

    lista.cabeza = nullptr;
    lista.cola = nullptr;
    lista.tam = 0;
}

NodoEnemigo* insertarDespues(ListaEnemigos& lista, NodoEnemigo* nodo, const Enemigo& enemigo) {
    if (nodo == nullptr) return nullptr;

    NodoEnemigo* nuevo = new NodoEnemigo{ enemigo, nodo, nodo->sig };

    if (nodo->sig != nullptr) nodo->sig->ant = nuevo;
    else lista.cola = nuevo;

    nodo->sig = nuevo;
    lista.tam++;

    return nuevo;
}

NodoEnemigo* eliminarEnemigo(ListaEnemigos& lista, NodoEnemigo* nodo) {
    if (nodo == nullptr) return nullptr;

    NodoEnemigo* siguiente = nodo->sig;

    if (nodo->ant != nullptr) nodo->ant->sig = nodo->sig;
    else lista.cabeza = nodo->sig;

    if (nodo->sig != nullptr) nodo->sig->ant = nodo->ant;
    else lista.cola = nodo->ant;

    delete nodo;
    lista.tam--;

    return siguiente;
}