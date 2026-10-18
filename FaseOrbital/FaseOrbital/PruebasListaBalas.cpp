#include "PruebasListaBalas.h"
#include "ListaBalas.h"

bool pruebaP04() {
    ListaBalas lista;
    inicializarListaBalas(lista);

    Bala bala1{ 0.0, 0.0, 1.0, 0.0, 0, 1 };
    Bala bala2{ 0.0, 0.0, 1.0, 0.0, 1, 1 };
    Bala bala3{ 0.0, 0.0, 1.0, 0.0, 1, 1 };
    Bala bala4{ 0.0, 0.0, 1.0, 0.0, 0, 1 };
    Bala bala5{ 0.0, 0.0, 1.0, 0.0, 1, 1 };

    insertarInicio(lista, bala1);
    insertarInicio(lista, bala2);
    insertarInicio(lista, bala3);
    insertarInicio(lista, bala4);
    insertarInicio(lista, bala5);

    int eliminadas = eliminarBalasSi(lista, 1);
    bool resultado = true;

    if (eliminadas != 3) resultado = false;
    if (lista.tam != 2) resultado = false;

    NodoBala* actual = lista.cabeza;

    while (actual != nullptr) {
        if (actual->dato.color == 1) resultado = false;
        actual = actual->sig;
    }

    destruirListaBalas(lista);

    if (lista.cabeza != nullptr || lista.tam != 0) resultado = false;

    return resultado;
}