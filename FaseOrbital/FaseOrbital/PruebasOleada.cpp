#include "PruebasOleada.h"
#include "../FaseOrbital/Oleada.h"

static bool spawnsIguales(const Spawn& a, const Spawn& b) {
    return a.tick == b.tick && a.tipo == b.tipo && a.color == b.color && a.angulo == b.angulo && a.sentido == b.sentido;
}

static bool colasMismosDatos(const ColaSpawn& a, const ColaSpawn& b) {
    if (a.tam != b.tam) {
        return false;
    }

    const NodoSpawn* nodoA = a.frente;
    const NodoSpawn* nodoB = b.frente;

    while (nodoA != nullptr && nodoB != nullptr) {
        if (!spawnsIguales(nodoA->dato, nodoB->dato)) {
            return false;
        }

        nodoA = nodoA->sig;
        nodoB = nodoB->sig;
    }

    return nodoA == nullptr && nodoB == nullptr;
}

static bool nodosIndependientes(const ColaSpawn& a, const ColaSpawn& b) {
    if (a.tam != b.tam) {
        return false;
    }

    const NodoSpawn* nodoA = a.frente;
    const NodoSpawn* nodoB = b.frente;

    while (nodoA != nullptr && nodoB != nullptr) {
        if (nodoA == nodoB) {
            return false;
        }

        nodoA = nodoA->sig;
        nodoB = nodoB->sig;
    }

    return nodoA == nullptr && nodoB == nullptr;
}

static bool tipoPermitido(int numeroOleada, TipoEnemigo tipo) {
    if (numeroOleada == 1) {
        return tipo == Dron;
    }

    if (numeroOleada == 2) {
        return tipo == Dron || tipo == Girador;
    }

    if (numeroOleada == 3) {
        return tipo == Dron || tipo == Girador || tipo == Divisor;
    }

    if (numeroOleada == 4) {
        return tipo == Dron || tipo == Girador || tipo == Divisor || tipo == Tanque;
    }

    return tipo == Dron || tipo == Girador || tipo == Divisor || tipo == Tanque || tipo == Camaleon;
}

static bool validarContenidoOleada(const ColaSpawn& cola, int numeroOleada) {
    int cantidadEsperada = 6 + 3 * numeroOleada;

    if (cola.tam != cantidadEsperada) {
        return false;
    }

    if (cola.frente == nullptr || cola.final == nullptr) {
        return false;
    }

    if (cola.frente->dato.tick != 60) {
        return false;
    }

    int intervaloBase = 35 - 2 * numeroOleada;

    if (intervaloBase < 9) {
        intervaloBase = 9;
    }

    int intervaloMinimo = intervaloBase * 60 / 100;
    int intervaloMaximo = intervaloBase * 140 / 100;

    const NodoSpawn* actual = cola.frente;
    const NodoSpawn* anterior = nullptr;
    const NodoSpawn* ultimo = nullptr;
    int contador = 0;

    while (actual != nullptr) {
        const Spawn& spawn = actual->dato;

        if (!tipoPermitido(numeroOleada, spawn.tipo)) {
            return false;
        }

        if (spawn.color < 0 || spawn.color > 2) {
            return false;
        }

        if (spawn.angulo < 0 || spawn.angulo > 359) {
            return false;
        }

        if (spawn.tipo == Girador) {
            if (spawn.sentido != 0 && spawn.sentido != 1) {
                return false;
            }
        }
        else {
            if (spawn.sentido != 0) {
                return false;
            }
        }

        if (anterior != nullptr) {
            int diferencia = spawn.tick - anterior->dato.tick;

            if (numeroOleada < 7 && diferencia <= 0) {
                return false;
            }

            if (numeroOleada >= 7 && diferencia < 0) {
                return false;
            }

            if (diferencia != 0 && (diferencia < intervaloMinimo || diferencia > intervaloMaximo)) {
                return false;
            }
        }

        anterior = actual;
        ultimo = actual;
        actual = actual->sig;
        contador++;
    }

    if (contador != cantidadEsperada) {
        return false;
    }

    if (ultimo != cola.final) {
        return false;
    }

    return true;
}

static bool tieneTickCompartido(const ColaSpawn& cola) {
    const NodoSpawn* actual = cola.frente;

    while (actual != nullptr && actual->sig != nullptr) {
        if (actual->dato.tick == actual->sig->dato.tick) {
            return true;
        }

        actual = actual->sig;
    }

    return false;
}

static bool probarOleada(int numeroOleada, uint32_t semilla, bool exigirTickCompartido) {
    ColaSpawn colaA;
    ColaSpawn colaB;
    inicializarColaSpawn(colaA);
    inicializarColaSpawn(colaB);

    Oleada oleada{ numeroOleada, semilla };

    generarOleada(oleada, colaA);
    generarOleada(oleada, colaB);

    bool resultado = true;

    if (!validarContenidoOleada(colaA, numeroOleada) || !validarContenidoOleada(colaB, numeroOleada)) {
        resultado = false;
    }

    if (!colasMismosDatos(colaA, colaB)) {
        resultado = false;
    }

    if (!nodosIndependientes(colaA, colaB)) {
        resultado = false;
    }

    if (exigirTickCompartido && !tieneTickCompartido(colaA)) {
        resultado = false;
    }

    destruirColaSpawn(colaA);

    if (!validarContenidoOleada(colaB, numeroOleada)) {
        resultado = false;
    }

    destruirColaSpawn(colaB);

    return resultado;
}

static bool probarSemillaDiferente() {
    ColaSpawn colaA;
    ColaSpawn colaB;
    inicializarColaSpawn(colaA);
    inicializarColaSpawn(colaB);

    Oleada oleadaA{ 5, 42u };
    Oleada oleadaB{ 5, 99u };

    generarOleada(oleadaA, colaA);
    generarOleada(oleadaB, colaB);

    bool resultado = true;

    if (!validarContenidoOleada(colaA, 5) || !validarContenidoOleada(colaB, 5)) {
        resultado = false;
    }

    if (colasMismosDatos(colaA, colaB)) {
        resultado = false;
    }

    if (!nodosIndependientes(colaA, colaB)) {
        resultado = false;
    }

    destruirColaSpawn(colaA);

    if (!validarContenidoOleada(colaB, 5)) {
        resultado = false;
    }

    destruirColaSpawn(colaB);

    return resultado;
}

bool pruebaOleadasDeterministas() {
    if (!probarOleada(1, 42u, false)) {
        return false;
    }

    if (!probarOleada(2, 42u, false)) {
        return false;
    }

    if (!probarOleada(5, 42u, false)) {
        return false;
    }

    if (!probarOleada(7, 42u, true)) {
        return false;
    }

    if (!probarSemillaDiferente()) {
        return false;
    }

    return true;
}