#include "Prueba08.h"
#include "Partida.h"

static bool jugadoresP08Iguales(const Jugador& a, const Jugador& b) {
    return a.alias == b.alias &&
        a.esBot == b.esBot &&
        a.vidaNucleo == b.vidaNucleo &&
        a.puntaje == b.puntaje &&
        a.combo == b.combo &&
        a.cargaPulso == b.cargaPulso &&
        a.oleadaAlcanzada == b.oleadaAlcanzada &&
        a.eliminado == b.eliminado;
}

static bool spawnsP08Iguales(const Spawn& a, const Spawn& b) {
    return a.tick == b.tick &&
        a.tipo == b.tipo &&
        a.color == b.color &&
        a.angulo == b.angulo &&
        a.sentido == b.sentido;
}

static bool colasP08Iguales(const ColaSpawn& a, const ColaSpawn& b) {
    if (a.tam != b.tam) {
        return false;
    }

    const NodoSpawn* nodoA = a.frente;
    const NodoSpawn* nodoB = b.frente;

    while (nodoA != nullptr && nodoB != nullptr) {
        if (!spawnsP08Iguales(nodoA->dato, nodoB->dato)) {
            return false;
        }

        nodoA = nodoA->sig;
        nodoB = nodoB->sig;
    }

    return nodoA == nullptr && nodoB == nullptr;
}

static bool partidasP08Iguales(const Partida& a, const Partida& b) {
    if (a.cantidadJugadores != b.cantidadJugadores) {
        return false;
    }

    if (a.numeroRonda != b.numeroRonda) {
        return false;
    }

    if (a.semillaPartida != b.semillaPartida) {
        return false;
    }

    if (a.limiteOleadas != b.limiteOleadas) {
        return false;
    }

    if (a.finalizada != b.finalizada) {
        return false;
    }

    if (cantidadJugadoresActivos(a) != cantidadJugadoresActivos(b)) {
        return false;
    }

    for (int i = 0; i < a.cantidadJugadores; i++) {
        if (!jugadoresP08Iguales(a.jugadores[i], b.jugadores[i])) {
            return false;
        }
    }

    const Jugador* actualA = verJugadorActual(a.ronda);
    const Jugador* actualB = verJugadorActual(b.ronda);

    if ((actualA == nullptr) != (actualB == nullptr)) {
        return false;
    }

    if (actualA != nullptr && actualB != nullptr && actualA->alias != actualB->alias) {
        return false;
    }

    if (!colasP08Iguales(a.colaOriginalRonda, b.colaOriginalRonda)) {
        return false;
    }

    return true;
}

bool pruebaP08ConScript(const ScriptAcciones& script) {
    Partida partidaA;
    Partida partidaB;

    inicializarPartida(partidaA, 42u, 3);
    inicializarPartida(partidaB, 42u, 3);

    agregarJugadorPartida(partidaA, "HUMANO", false);
    agregarJugadorPartida(partidaA, "BOT1", true);
    agregarJugadorPartida(partidaA, "BOT2", true);

    agregarJugadorPartida(partidaB, "HUMANO", false);
    agregarJugadorPartida(partidaB, "BOT1", true);
    agregarJugadorPartida(partidaB, "BOT2", true);

    bool resultado = true;
    const int limiteTicks = 10000;

    if (!partidasP08Iguales(partidaA, partidaB)) {
        resultado = false;
    }

    while (resultado && !partidaA.finalizada && !partidaB.finalizada) {
        bool ejecucionA = ejecutarRondaModoLotes(partidaA, script, limiteTicks);
        bool ejecucionB = ejecutarRondaModoLotes(partidaB, script, limiteTicks);

        if (ejecucionA != ejecucionB) {
            resultado = false;
            break;
        }

        if (!ejecucionA || !ejecucionB) {
            resultado = false;
            break;
        }

        if (!partidasP08Iguales(partidaA, partidaB)) {
            resultado = false;
            break;
        }
    }

    if (partidaA.finalizada != partidaB.finalizada) {
        resultado = false;
    }

    if (!partidasP08Iguales(partidaA, partidaB)) {
        resultado = false;
    }

    destruirPartida(partidaA);
    destruirPartida(partidaB);

    return resultado;
}

bool pruebaP08() {
    ScriptAcciones script;
    inicializarScriptAcciones(script);

    bool resultado = pruebaP08ConScript(script);

    destruirScriptAcciones(script);

    return resultado;
}