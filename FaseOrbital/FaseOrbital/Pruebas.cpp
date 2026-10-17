#include "Pruebas.h"
#include "../FaseOrbital/ColaSpawn.h"
#include "../FaseOrbital/RondaJugadores.h"
#include "../FaseOrbital/Generador.h"

bool pruebaP05() {
    ColaSpawn cola;
    inicializarColaSpawn(cola);

    Spawn spawn1{ 10, Dron, 0, 45, 1 };
    Spawn spawn2{ 20, Girador, 1, 90, -1 };
    Spawn spawn3{ 30, Tanque, 2, 180, 1 };

    encolarSpawn(cola, spawn1);
    encolarSpawn(cola, spawn2);
    encolarSpawn(cola, spawn3);

    bool resultado = true;

    if (cola.tam != 3) {
        resultado = false;
    }

    Spawn salida;

    if (!desencolarSpawn(cola, salida) || salida.tick != 10) {
        resultado = false;
    }

    if (!desencolarSpawn(cola, salida) || salida.tick != 20) {
        resultado = false;
    }

    if (!desencolarSpawn(cola, salida) || salida.tick != 30) {
        resultado = false;
    }

    if (cola.frente != nullptr || cola.final != nullptr || cola.tam != 0) {
        resultado = false;
    }

    if (desencolarSpawn(cola, salida)) {
        resultado = false;
    }

    destruirColaSpawn(cola);

    return resultado;
}

bool pruebaP06() {
    ColaSpawn original;
    ColaSpawn copia;
    inicializarColaSpawn(original);
    inicializarColaSpawn(copia);

    Spawn spawn1{ 10, Dron, 0, 45, 1 };
    Spawn spawn2{ 20, Girador, 1, 90, -1 };
    Spawn spawn3{ 30, Tanque, 2, 180, 1 };
    Spawn viejo{ 999, Camaleon, 3, 270, -1 };

    encolarSpawn(original, spawn1);
    encolarSpawn(original, spawn2);
    encolarSpawn(original, spawn3);

    encolarSpawn(copia, viejo);

    copiarColaSpawn(original, copia);

    bool resultado = true;

    if (original.tam != 3 || copia.tam != 3) {
        resultado = false;
    }

    if (original.frente == copia.frente || original.final == copia.final) {
        resultado = false;
    }

    if (original.frente == nullptr || copia.frente == nullptr) {
        resultado = false;
    }
    else {
        if (original.frente->dato.tick != copia.frente->dato.tick) {
            resultado = false;
        }

        if (original.frente->sig == copia.frente->sig) {
            resultado = false;
        }
    }

    Spawn salida;

    if (!desencolarSpawn(copia, salida) || salida.tick != 10) {
        resultado = false;
    }

    if (!desencolarSpawn(copia, salida) || salida.tick != 20) {
        resultado = false;
    }

    if (!desencolarSpawn(copia, salida) || salida.tick != 30) {
        resultado = false;
    }

    if (!colaSpawnVacia(copia) || copia.frente != nullptr || copia.final != nullptr || copia.tam != 0) {
        resultado = false;
    }

    if (original.tam != 3 || original.frente == nullptr || original.final == nullptr) {
        resultado = false;
    }

    if (original.frente != nullptr && original.frente->dato.tick != 10) {
        resultado = false;
    }

    if (original.final != nullptr && original.final->dato.tick != 30) {
        resultado = false;
    }

    destruirColaSpawn(copia);

    if (original.tam != 3 || original.frente == nullptr) {
        resultado = false;
    }

    destruirColaSpawn(original);

    return resultado;
}

bool pruebaP07() {
    RondaJugadores ronda;
    inicializarRondaJugadores(ronda);

    Jugador jugadorA{ "A", false, 100, 0, 0, 0 };
    Jugador jugadorB{ "B", false, 100, 0, 0, 0 };
    Jugador jugadorC{ "C", false, 100, 0, 0, 0 };

    bool resultado = true;

    agregarJugador(ronda, &jugadorA);
    agregarJugador(ronda, &jugadorB);
    agregarJugador(ronda, &jugadorC);

    if (tamanoRonda(ronda) != 3) {
        resultado = false;
    }

    if (verJugadorActual(ronda) != &jugadorA) {
        resultado = false;
    }

    if (ronda.actual->sig->ant != ronda.actual || ronda.actual->ant->sig != ronda.actual) {
        resultado = false;
    }

    avanzarJugador(ronda, true);

    if (verJugadorActual(ronda) != &jugadorB) {
        resultado = false;
    }

    avanzarJugador(ronda, false);

    if (verJugadorActual(ronda) != &jugadorA) {
        resultado = false;
    }

    Jugador* eliminado = eliminarJugadorActual(ronda, true);

    if (eliminado != &jugadorA) {
        resultado = false;
    }

    if (tamanoRonda(ronda) != 2 || verJugadorActual(ronda) != &jugadorB) {
        resultado = false;
    }

    if (ronda.actual->sig->ant != ronda.actual || ronda.actual->ant->sig != ronda.actual) {
        resultado = false;
    }

    eliminado = eliminarJugadorActual(ronda, false);

    if (eliminado != &jugadorB) {
        resultado = false;
    }

    if (tamanoRonda(ronda) != 1 || verJugadorActual(ronda) != &jugadorC) {
        resultado = false;
    }

    if (ronda.actual->sig != ronda.actual || ronda.actual->ant != ronda.actual) {
        resultado = false;
    }

    eliminado = eliminarJugadorActual(ronda, true);

    if (eliminado != &jugadorC) {
        resultado = false;
    }

    if (!rondaVacia(ronda) || ronda.actual != nullptr || tamanoRonda(ronda) != 0) {
        resultado = false;
    }

    if (eliminarJugadorActual(ronda, true) != nullptr) {
        resultado = false;
    }

    destruirRondaJugadores(ronda);

    return resultado;
}

bool pruebaGeneradorDeterminista() {
    Generador generadorA;
    Generador generadorB;

    inicializarGenerador(generadorA, 42);
    inicializarGenerador(generadorB, 42);

    uint32_t primeroA = siguienteNumero(generadorA);
    uint32_t primeroB = siguienteNumero(generadorB);

    if (primeroA != 1250496027u || primeroB != 1250496027u) {
        return false;
    }

    for (int i = 0; i < 50; i++) {
        uint32_t valorA = siguienteNumero(generadorA);
        uint32_t valorB = siguienteNumero(generadorB);

        if (valorA != valorB) {
            return false;
        }
    }

    return true;
}