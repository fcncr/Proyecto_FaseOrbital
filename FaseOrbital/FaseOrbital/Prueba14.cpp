#include "Prueba14.h"
#include "Partida.h"
#include "ScriptAcciones.h"

static bool agregarBotsP14(Partida& partida) {
    if (!agregarJugadorPartida(partida, "BOT1", true)) return false;
    if (!agregarJugadorPartida(partida, "BOT2", true)) return false;
    if (!agregarJugadorPartida(partida, "BOT3", true)) return false;
    if (!agregarJugadorPartida(partida, "BOT4", true)) return false;

    return true;
}

static bool partidaDestruidaCorrectamenteP14(const Partida& partida) {
    if (partida.cantidadJugadores != 0) return false;

    if (partida.ronda.actual != nullptr) return false;
    if (partida.ronda.tam != 0) return false;

    if (partida.colaOriginalRonda.frente != nullptr) return false;
    if (partida.colaOriginalRonda.final != nullptr) return false;
    if (partida.colaOriginalRonda.tam != 0) return false;

    if (partida.bitacora.cabeza != nullptr) return false;
    if (partida.bitacora.cola != nullptr) return false;
    if (partida.bitacora.tam != 0) return false;

    if (!partida.finalizada) return false;

    return true;
}

static bool scriptDestruidoCorrectamenteP14(const ScriptAcciones& script) {
    return script.cabeza == nullptr &&
        script.cola == nullptr &&
        script.tam == 0;
}

bool prepararEscenarioP14() {
    Partida partida;
    ScriptAcciones script;

    inicializarPartida(partida, 42u, 10);
    inicializarScriptAcciones(script);

    bool resultado = true;

    if (!agregarBotsP14(partida)) {
        resultado = false;
    }

    const int limiteTicks = 10000;
    int rondasEjecutadas = 0;

    while (resultado && !partida.finalizada && rondasEjecutadas < 10) {
        if (!ejecutarRondaModoLotes(partida, script, limiteTicks)) {
            resultado = false;
        }
        else {
            rondasEjecutadas++;
        }
    }

    if (!partida.finalizada) {
        resultado = false;
    }

    if (partida.limiteOleadas != 10) {
        resultado = false;
    }

    if (partida.cantidadJugadores != 4) {
        resultado = false;
    }

    if (rondasEjecutadas <= 0 || rondasEjecutadas > 10) {
        resultado = false;
    }

    if (resultado) {
        actualizarMetricasFinalesPartida(partida);
    }

    destruirPartida(partida);
    destruirScriptAcciones(script);

    if (!partidaDestruidaCorrectamenteP14(partida)) {
        resultado = false;
    }

    if (!scriptDestruidoCorrectamenteP14(script)) {
        resultado = false;
    }

    return resultado;
}

bool pruebaCiclosMemoriaP14() {
    const int cantidadCiclos = 30;

    for (int i = 0; i < cantidadCiclos; i++) {
        Partida partida;
        ScriptAcciones script;

        inicializarPartida(partida, 100u + static_cast<unsigned int>(i), 1);
        inicializarScriptAcciones(script);

        bool resultado = true;

        if (!agregarBotsP14(partida)) {
            resultado = false;
        }

        if (resultado) {
            if (!ejecutarRondaModoLotes(partida, script, 10000)) {
                resultado = false;
            }
        }

        if (resultado && !partida.finalizada) {
            resultado = false;
        }

        if (resultado) {
            actualizarMetricasFinalesPartida(partida);
        }

        destruirPartida(partida);
        destruirScriptAcciones(script);

        if (!partidaDestruidaCorrectamenteP14(partida)) {
            resultado = false;
        }

        if (!scriptDestruidoCorrectamenteP14(script)) {
            resultado = false;
        }

        if (!resultado) {
            return false;
        }
    }

    return true;
}

bool pruebaEstresDiezOleadasP14() {
    ScriptAcciones script;
    inicializarScriptAcciones(script);

    bool resultado = true;

    const unsigned int semilla = 42u;
    const int limiteTicks = 10000;

    for (int numeroOleada = 1; numeroOleada <= 10 && resultado; numeroOleada++) {
        Partida partida;

        inicializarPartida(partida, semilla, 10);

        if (!agregarBotsP14(partida)) {
            resultado = false;
        }

        partida.numeroRonda = numeroOleada;

        if (resultado) {
            generarColaOriginalRonda(partida);
        }

        if (resultado && partida.colaOriginalRonda.tam <= 0) {
            resultado = false;
        }

        for (int i = 0; i < 4 && resultado; i++) {
            ColaSpawn colaJugador;
            inicializarColaSpawn(colaJugador);

            prepararColaJugador(partida, colaJugador);

            ResultadoModoLotes resultadoTurno;

            if (!ejecutarModoLotes(
                partida.jugadores[i],
                numeroOleada,
                colaJugador,
                script,
                limiteTicks,
                resultadoTurno,
                &partida.bitacora
            )) {
                resultado = false;
            }

            destruirColaSpawn(colaJugador);
        }

        destruirPartida(partida);

        if (!partidaDestruidaCorrectamenteP14(partida)) {
            resultado = false;
        }
    }

    destruirScriptAcciones(script);

    if (!scriptDestruidoCorrectamenteP14(script)) {
        resultado = false;
    }

    return resultado;
}