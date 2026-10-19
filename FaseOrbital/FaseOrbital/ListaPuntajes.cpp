#include "ListaPuntajes.h"

static void dividirListaPuntajes(NodoPuntaje* cabeza, NodoPuntaje*& izquierda, NodoPuntaje*& derecha) {
    if (cabeza == nullptr || cabeza->sig == nullptr) {
        izquierda = cabeza;
        derecha = nullptr;
        return;
    }

    NodoPuntaje* lento = cabeza;
    NodoPuntaje* rapido = cabeza->sig;

    while (rapido != nullptr) {
        rapido = rapido->sig;

        if (rapido != nullptr) {
            lento = lento->sig;
            rapido = rapido->sig;
        }
    }

    izquierda = cabeza;
    derecha = lento->sig;
    lento->sig = nullptr;
}

static NodoPuntaje* fusionarListasPuntajes(NodoPuntaje* izquierda, NodoPuntaje* derecha) {
    if (izquierda == nullptr) {
        return derecha;
    }

    if (derecha == nullptr) {
        return izquierda;
    }

    NodoPuntaje* resultado = nullptr;

    if (izquierda->puntaje <= derecha->puntaje) {
        resultado = izquierda;
        resultado->sig = fusionarListasPuntajes(izquierda->sig, derecha);
    }
    else {
        resultado = derecha;
        resultado->sig = fusionarListasPuntajes(izquierda, derecha->sig);
    }

    return resultado;
}

static NodoPuntaje* mergeSortPuntajes(NodoPuntaje* cabeza) {
    if (cabeza == nullptr || cabeza->sig == nullptr) {
        return cabeza;
    }

    NodoPuntaje* izquierda = nullptr;
    NodoPuntaje* derecha = nullptr;

    dividirListaPuntajes(cabeza, izquierda, derecha);

    izquierda = mergeSortPuntajes(izquierda);
    derecha = mergeSortPuntajes(derecha);

    return fusionarListasPuntajes(izquierda, derecha);
}

void inicializarListaPuntajes(ListaPuntajes& lista) {
    lista.cabeza = nullptr;
    lista.cola = nullptr;
    lista.tam = 0;
}

void agregarPuntajeFinal(ListaPuntajes& lista, int puntaje) {
    NodoPuntaje* nuevo = new NodoPuntaje;
    nuevo->puntaje = puntaje;
    nuevo->sig = nullptr;

    if (lista.cola == nullptr) {
        lista.cabeza = nuevo;
        lista.cola = nuevo;
    }
    else {
        lista.cola->sig = nuevo;
        lista.cola = nuevo;
    }

    lista.tam++;
}

void ordenarListaPuntajesMergeSort(ListaPuntajes& lista) {
    lista.cabeza = mergeSortPuntajes(lista.cabeza);

    lista.cola = lista.cabeza;

    if (lista.cola != nullptr) {
        while (lista.cola->sig != nullptr) {
            lista.cola = lista.cola->sig;
        }
    }
}

double calcularMedianaPuntajes(ListaPuntajes& lista) {
    if (lista.tam == 0) {
        return 0.0;
    }

    ordenarListaPuntajesMergeSort(lista);

    int indiceIzquierdo = (lista.tam - 1) / 2;
    int indiceDerecho = lista.tam / 2;

    const NodoPuntaje* actual = lista.cabeza;
    int indice = 0;
    int valorIzquierdo = 0;
    int valorDerecho = 0;

    while (actual != nullptr && indice <= indiceDerecho) {
        if (indice == indiceIzquierdo) {
            valorIzquierdo = actual->puntaje;
        }

        if (indice == indiceDerecho) {
            valorDerecho = actual->puntaje;
        }

        actual = actual->sig;
        indice++;
    }

    return (static_cast<double>(valorIzquierdo) + static_cast<double>(valorDerecho)) / 2.0;
}

void destruirListaPuntajes(ListaPuntajes& lista) {
    NodoPuntaje* actual = lista.cabeza;

    while (actual != nullptr) {
        NodoPuntaje* siguiente = actual->sig;
        delete actual;
        actual = siguiente;
    }

    lista.cabeza = nullptr;
    lista.cola = nullptr;
    lista.tam = 0;
}