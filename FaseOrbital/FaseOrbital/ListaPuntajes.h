#ifndef LISTA_PUNTAJES_H
#define LISTA_PUNTAJES_H

struct NodoPuntaje {
    int puntaje;
    NodoPuntaje* sig;
};

struct ListaPuntajes {
    NodoPuntaje* cabeza;
    NodoPuntaje* cola;
    int tam;
};

void inicializarListaPuntajes(ListaPuntajes& lista);
void agregarPuntajeFinal(ListaPuntajes& lista, int puntaje);
void ordenarListaPuntajesMergeSort(ListaPuntajes& lista);
double calcularMedianaPuntajes(ListaPuntajes& lista);
void destruirListaPuntajes(ListaPuntajes& lista);

#endif