#include "ListaBalas.h"

void inicializarListaBalas(ListaBalas& lista) {
    lista.cabeza = nullptr;
    lista.tam = 0;
}

void insertarInicio(ListaBalas& lista, const Bala& bala) {
    NodoBala* nuevo = new NodoBala{ bala, lista.cabeza };
    lista.cabeza = nuevo;
    lista.tam++;
}

int tamanoListaBalas(const ListaBalas& lista) {
    return lista.tam;
}

bool listaBalasVacia(const ListaBalas& lista) {
    return lista.tam == 0;
}

void destruirListaBalas(ListaBalas& lista) {
    NodoBala* actual = lista.cabeza;

    while (actual != nullptr) {
        NodoBala* siguiente = actual->sig;
        delete actual;
        actual = siguiente;
    }

    lista.cabeza = nullptr;
    lista.tam = 0;
}

int eliminarBalasSi(ListaBalas& lista, int colorEliminar) {
    int eliminadas = 0;
    NodoBala** enlace = &lista.cabeza;

    while (*enlace != nullptr) {
        NodoBala* actual = *enlace;

        if (actual->dato.color == colorEliminar) {
            *enlace = actual->sig;
            delete actual;
            lista.tam--;
            eliminadas++;
        }
        else enlace = &actual->sig;
    }

    return eliminadas;
}