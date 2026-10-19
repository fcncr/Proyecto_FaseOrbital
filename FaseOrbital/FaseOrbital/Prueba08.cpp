#include "Prueba08.h"
#include "Partida.h"
#include "ReportePartida.h"

static bool metricasP08Iguales(const MetricasJugador& a, const MetricasJugador& b) {
    if (a.puntaje != b.puntaje) return false;
    if (a.oleadaAlcanzada != b.oleadaAlcanzada) return false;

    if (a.disparos != b.disparos) return false;
    if (a.impactosCorrectos != b.impactosCorrectos) return false;
    if (a.impactosErroneos != b.impactosErroneos) return false;

    if (a.impactosCorrectosC != b.impactosCorrectosC) return false;
    if (a.impactosErroneosC != b.impactosErroneosC) return false;

    if (a.impactosCorrectosM != b.impactosCorrectosM) return false;
    if (a.impactosErroneosM != b.impactosErroneosM) return false;

    if (a.impactosCorrectosA != b.impactosCorrectosA) return false;
    if (a.impactosErroneosA != b.impactosErroneosA) return false;

    if (a.bajasDron != b.bajasDron) return false;
    if (a.bajasGirador != b.bajasGirador) return false;
    if (a.bajasDivisor != b.bajasDivisor) return false;
    if (a.bajasMini != b.bajasMini) return false;
    if (a.bajasTanque != b.bajasTanque) return false;
    if (a.bajasCamaleon != b.bajasCamaleon) return false;

    if (a.bajasColor0 != b.bajasColor0) return false;
    if (a.bajasColor1 != b.bajasColor1) return false;
    if (a.bajasColor2 != b.bajasColor2) return false;

    if (a.comboMaximo != b.comboMaximo) return false;

    if (a.danoTotal != b.danoTotal) return false;
    if (a.danoDron != b.danoDron) return false;
    if (a.danoGirador != b.danoGirador) return false;
    if (a.danoDivisor != b.danoDivisor) return false;
    if (a.danoMini != b.danoMini) return false;
    if (a.danoTanque != b.danoTanque) return false;
    if (a.danoCamaleon != b.danoCamaleon) return false;

    if (a.pulsosUtilizados != b.pulsosUtilizados) return false;
    if (a.ticksSobrevividos != b.ticksSobrevividos) return false;
    if (a.posicionFinal != b.posicionFinal) return false;

    if (a.precisionColor != b.precisionColor) return false;
    if (a.eficienciaDisparo != b.eficienciaDisparo) return false;
    if (a.precisionC != b.precisionC) return false;
    if (a.precisionM != b.precisionM) return false;
    if (a.precisionA != b.precisionA) return false;

    return true;
}

static bool jugadoresP08Iguales(const Jugador& a, const Jugador& b) {
    return a.alias == b.alias &&
        a.esBot == b.esBot &&
        a.vidaNucleo == b.vidaNucleo &&
        a.puntaje == b.puntaje &&
        a.combo == b.combo &&
        a.cargaPulso == b.cargaPulso &&
        a.oleadaAlcanzada == b.oleadaAlcanzada &&
        a.eliminado == b.eliminado &&
        metricasP08Iguales(a.metricas, b.metricas);
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

static bool eventosP08Iguales(const Evento& a, const Evento& b) {
    return a.tick == b.tick &&
        a.oleada == b.oleada &&
        a.jugador == b.jugador &&
        a.tipoEvento == b.tipoEvento &&
        a.tipoEnemigo == b.tipoEnemigo &&
        a.color == b.color &&
        a.valor == b.valor;
}

static bool bitacorasP08Iguales(const BitacoraEventos& a, const BitacoraEventos& b) {
    if (a.tam != b.tam) {
        return false;
    }

    const NodoEvento* nodoA = a.cabeza;
    const NodoEvento* nodoB = b.cabeza;

    while (nodoA != nullptr && nodoB != nullptr) {
        if (!eventosP08Iguales(nodoA->dato, nodoB->dato)) {
            return false;
        }

        nodoA = nodoA->sig;
        nodoB = nodoB->sig;
    }

    return nodoA == nullptr && nodoB == nullptr;
}

static bool partidasP08Iguales(const Partida& a, const Partida& b) {
    if (a.cantidadJugadores != b.cantidadJugadores) return false;
    if (a.numeroRonda != b.numeroRonda) return false;
    if (a.semillaPartida != b.semillaPartida) return false;
    if (a.limiteOleadas != b.limiteOleadas) return false;
    if (a.finalizada != b.finalizada) return false;

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

    if (!bitacorasP08Iguales(a.bitacora, b.bitacora)) {
        return false;
    }

    return true;
}

static bool ejecutarPartidaP08(Partida& partida, const ScriptAcciones& script) {
    const int limiteTicks = 10000;
    int rondasEjecutadas = 0;

    while (!partida.finalizada && rondasEjecutadas < partida.limiteOleadas + 2) {
        if (!ejecutarRondaModoLotes(partida, script, limiteTicks)) {
            return false;
        }

        rondasEjecutadas++;
    }

    if (!partida.finalizada) {
        return false;
    }

    actualizarMetricasFinalesPartida(partida);

    return true;
}

bool pruebaP08ConScript(const ScriptAcciones& script) {
    Partida partidaA;
    Partida partidaB;

    inicializarPartida(partidaA, 42u, 3);
    inicializarPartida(partidaB, 42u, 3);

    bool jugadoresAgregados = true;

    if (!agregarJugadorPartida(partidaA, "HUMANO", false)) jugadoresAgregados = false;
    if (!agregarJugadorPartida(partidaA, "BOT1", true)) jugadoresAgregados = false;
    if (!agregarJugadorPartida(partidaA, "BOT2", true)) jugadoresAgregados = false;

    if (!agregarJugadorPartida(partidaB, "HUMANO", false)) jugadoresAgregados = false;
    if (!agregarJugadorPartida(partidaB, "BOT1", true)) jugadoresAgregados = false;
    if (!agregarJugadorPartida(partidaB, "BOT2", true)) jugadoresAgregados = false;

    if (!jugadoresAgregados) {
        destruirPartida(partidaA);
        destruirPartida(partidaB);
        return false;
    }

    bool resultado = true;

    if (!partidasP08Iguales(partidaA, partidaB)) {
        resultado = false;
    }

    bool ejecucionA = ejecutarPartidaP08(partidaA, script);
    bool ejecucionB = ejecutarPartidaP08(partidaB, script);

    if (!ejecucionA || !ejecucionB) {
        resultado = false;
    }

    if (!partidasP08Iguales(partidaA, partidaB)) {
        resultado = false;
    }

    const std::string rutaA = "reporte_p08_A.txt";
    const std::string rutaB = "reporte_p08_B.txt";

    if (!guardarReportePartida(partidaA, rutaA)) {
        resultado = false;
    }

    if (!guardarReportePartida(partidaB, rutaB)) {
        resultado = false;
    }

    if (!compararArchivosExactos(rutaA, rutaB)) {
        resultado = false;
    }

    destruirPartida(partidaA);
    destruirPartida(partidaB);

    return resultado;
}

bool pruebaP08() {
    ScriptAcciones script;
    inicializarScriptAcciones(script);

    if (!cargarScriptAcciones(script, "p08_script.txt")) {
        destruirScriptAcciones(script);
        return false;
    }

    if (script.tam == 0) {
        destruirScriptAcciones(script);
        return false;
    }

    bool resultado = pruebaP08ConScript(script);

    destruirScriptAcciones(script);

    return resultado;
}