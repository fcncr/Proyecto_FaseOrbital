#ifndef LISTA_BALAS_H
#define LISTA_BALAS_H

#include "Bala.h"

struct NodoBala {
    Bala dato;
    NodoBala* sig;
};

struct ListaBalas {
    NodoBala* cabeza;
    int tam;
};

void inicializarListaBalas(ListaBalas& lista);
void insertarInicio(ListaBalas& lista, const Bala& bala);
int eliminarBalasSi(ListaBalas& lista, int colorEliminar);
int tamanoListaBalas(const ListaBalas& lista);
bool listaBalasVacia(const ListaBalas& lista);
void destruirListaBalas(ListaBalas& lista);

#endif