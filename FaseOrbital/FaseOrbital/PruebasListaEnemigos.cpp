#include "PruebasListaEnemigos.h"
#include "ListaEnemigos.h"

bool pruebaP01() {
    ListaEnemigos lista;
    inicializarListaEnemigos(lista);

    for (int i = 0; i < 1000; i++) {
        Enemigo enemigo{ i, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 };
        insertarFinal(lista, enemigo);
    }

    NodoEnemigo* actual = lista.cabeza;

    while (actual != nullptr) {
        if (actual->dato.id % 2 == 0) actual = eliminarEnemigo(lista, actual);
        else actual = actual->sig;
    }

    bool resultado = true;

    if (lista.tam != 500) resultado = false;

    int esperado = 1;
    actual = lista.cabeza;

    while (actual != nullptr) {
        if (actual->dato.id != esperado) resultado = false;
        esperado += 2;
        actual = actual->sig;
    }

    esperado = 999;
    actual = lista.cola;

    while (actual != nullptr) {
        if (actual->dato.id != esperado) resultado = false;
        esperado -= 2;
        actual = actual->ant;
    }

    if (lista.cabeza != nullptr && lista.cabeza->ant != nullptr) resultado = false;
    if (lista.cola != nullptr && lista.cola->sig != nullptr) resultado = false;

    destruirListaEnemigos(lista);

    return resultado;
}

bool pruebaP02() {
    bool resultado = true;

    ListaEnemigos lista;
    inicializarListaEnemigos(lista);

    Enemigo enemigo1{ 1, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 };
    Enemigo enemigo2{ 2, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 };
    Enemigo enemigo3{ 3, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 };

    insertarFinal(lista, enemigo1);
    insertarFinal(lista, enemigo2);
    insertarFinal(lista, enemigo3);

    eliminarEnemigo(lista, lista.cabeza);

    if (lista.cabeza == nullptr || lista.cabeza->dato.id != 2 || lista.cabeza->ant != nullptr || lista.tam != 2) resultado = false;

    eliminarEnemigo(lista, lista.cola);

    if (lista.cola == nullptr || lista.cola->dato.id != 2 || lista.cola->sig != nullptr || lista.tam != 1) resultado = false;

    eliminarEnemigo(lista, lista.cabeza);

    if (lista.cabeza != nullptr || lista.cola != nullptr || lista.tam != 0) resultado = false;

    destruirListaEnemigos(lista);

    return resultado;
}

bool pruebaP03() {
    ListaEnemigos lista;
    inicializarListaEnemigos(lista);

    Enemigo enemigo1{ 1, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 };
    Enemigo enemigo2{ 2, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 };
    Enemigo enemigo3{ 3, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 };

    insertarFinal(lista, enemigo1);
    NodoEnemigo* ultimo = insertarFinal(lista, enemigo2);
    NodoEnemigo* nuevo = insertarDespues(lista, ultimo, enemigo3);

    bool resultado = true;

    if (lista.cola != nuevo) resultado = false;
    if (nuevo == nullptr || nuevo->ant != ultimo || nuevo->sig != nullptr) resultado = false;
    if (ultimo->sig != nuevo) resultado = false;
    if (lista.tam != 3) resultado = false;

    destruirListaEnemigos(lista);

    return resultado;
}