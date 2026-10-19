#include "Partida.h"
#include "Oleada.h"

void inicializarPartida(Partida& partida, uint32_t semillaPartida, int limiteOleadas) {
    partida.cantidadJugadores = 0;
    inicializarRondaJugadores(partida.ronda);
    inicializarColaSpawn(partida.colaOriginalRonda);
    inicializarBitacora(partida.bitacora);
    partida.numeroRonda = 1;
    partida.semillaPartida = semillaPartida;
    partida.limiteOleadas = limiteOleadas;
    partida.finalizada = false;
}

bool agregarJugadorPartida(Partida& partida, const std::string& alias, bool esBot) {
    if (partida.cantidadJugadores >= 6) {
        return false;
    }

    Jugador& jugador = partida.jugadores[partida.cantidadJugadores];
    jugador.alias = alias;
    jugador.esBot = esBot;
    jugador.vidaNucleo = 100;
    jugador.puntaje = 0;
    jugador.combo = 0;
    jugador.cargaPulso = 0;
    jugador.oleadaAlcanzada = 0;
    jugador.eliminado = false;
    inicializarMetricasJugador(jugador.metricas); //las metricas se inicializan una sola vez al registrar el jugador

    agregarJugador(partida.ronda, &jugador);
    partida.cantidadJugadores++;

    return true;
}

Jugador* verJugadorPartida(Partida& partida, int indice) {
    if (indice < 0 || indice >= partida.cantidadJugadores) {
        return nullptr;
    }

    return &partida.jugadores[indice];
}

const Jugador* verJugadorPartida(const Partida& partida, int indice) {
    if (indice < 0 || indice >= partida.cantidadJugadores) {
        return nullptr;
    }

    return &partida.jugadores[indice];
}

Jugador* verJugadorActualPartida(Partida& partida) {
    return verJugadorActual(partida.ronda);
}

int cantidadJugadoresPartida(const Partida& partida) {
    return partida.cantidadJugadores;
}

int cantidadJugadoresActivos(const Partida& partida) {
    return tamanoRonda(partida.ronda);
}

void destruirPartida(Partida& partida) {
    destruirRondaJugadores(partida.ronda);
    destruirColaSpawn(partida.colaOriginalRonda);
    destruirBitacora(partida.bitacora);
    partida.cantidadJugadores = 0;
    partida.finalizada = true;
}

void generarColaOriginalRonda(Partida& partida) {
    Oleada oleada;
    oleada.numero = partida.numeroRonda;
    oleada.semillaPartida = partida.semillaPartida;
    generarOleada(oleada, partida.colaOriginalRonda);
}

void prepararColaJugador(const Partida& partida, ColaSpawn& colaJugador) {
    copiarColaSpawn(partida.colaOriginalRonda, colaJugador);
}

bool sentidoHorarioRondaPartida(const Partida& partida) {
    return partida.numeroRonda % 2 != 0;
}

void avanzarTurnoPartida(Partida& partida) {
    if (rondaVacia(partida.ronda)) {
        return;
    }
    avanzarJugador(partida.ronda, sentidoHorarioRondaPartida(partida));
}

void finalizarRondaPartida(Partida& partida) {
    if (partida.finalizada) {
        return;
    }

    if (cantidadJugadoresActivos(partida) <= 1) {
        partida.finalizada = true;
        return;
    }

    if (partida.numeroRonda >= partida.limiteOleadas) {
        partida.finalizada = true;
        return;
    }

    partida.numeroRonda++;
}

bool retirarJugadorActualSiEliminado(Partida& partida) {
    Jugador* jugador = verJugadorActualPartida(partida);

    if (jugador == nullptr) {
        partida.finalizada = true;
        return false;
    }

    if (jugador->vidaNucleo > 0) {
        return false;
    }

    jugador->eliminado = true;
    eliminarJugadorActual(partida.ronda, sentidoHorarioRondaPartida(partida));

    if (rondaVacia(partida.ronda)) {
        partida.finalizada = true;
    }

    return true;
}

const Jugador* determinarGanadorPartida(const Partida& partida, const double* precisiones) {
    if (partida.cantidadJugadores <= 0) {
        return nullptr;
    }

    int mejor = 0;
    bool empateSinResolver = false;

    for (int i = 1; i < partida.cantidadJugadores; i++) {
        const Jugador& candidato = partida.jugadores[i];
        const Jugador& actualMejor = partida.jugadores[mejor];

        if (candidato.puntaje > actualMejor.puntaje) {
            mejor = i;
            empateSinResolver = false;
            continue;
        }

        if (candidato.puntaje < actualMejor.puntaje) {
            continue;
        }

        if (candidato.oleadaAlcanzada > actualMejor.oleadaAlcanzada) {
            mejor = i;
            empateSinResolver = false;
            continue;
        }

        if (candidato.oleadaAlcanzada < actualMejor.oleadaAlcanzada) {
            continue;
        }

        if (precisiones == nullptr) {
            empateSinResolver = true;
            continue;
        }

        if (precisiones[i] > precisiones[mejor]) {
            mejor = i;
            empateSinResolver = false;
            continue;
        }

        if (precisiones[i] == precisiones[mejor]) {
            empateSinResolver = true;
        }
    }

    if (empateSinResolver) {
        return nullptr;
    }

    return &partida.jugadores[mejor];
}

bool ejecutarTurnoActualModoLotes(Partida& partida, const ScriptAcciones& script, int limiteTicks, ResultadoModoLotes& resultado) {
    if (partida.finalizada || rondaVacia(partida.ronda)) {
        return false;
    }

    Jugador* jugador = verJugadorActualPartida(partida);

    if (jugador == nullptr) {
        return false;
    }

    if (partida.numeroRonda > jugador->oleadaAlcanzada) {
        jugador->oleadaAlcanzada = partida.numeroRonda;
        jugador->metricas.oleadaAlcanzada = jugador->oleadaAlcanzada;
    }

    ColaSpawn colaJugador;
    inicializarColaSpawn(colaJugador);
    prepararColaJugador(partida, colaJugador);

    bool terminoCorrectamente = ejecutarModoLotes(*jugador, partida.numeroRonda, colaJugador, script, limiteTicks, resultado, &partida.bitacora);
    destruirColaSpawn(colaJugador);

    return terminoCorrectamente;
}

bool ejecutarRondaModoLotes(Partida& partida, const ScriptAcciones& script, int limiteTicks) {
    if (partida.finalizada || rondaVacia(partida.ronda)) {
        return false;
    }

    generarColaOriginalRonda(partida);

    int turnosDeLaRonda = cantidadJugadoresActivos(partida);

    for (int i = 0; i < turnosDeLaRonda; i++) {
        ResultadoModoLotes resultado;

        if (!ejecutarTurnoActualModoLotes(partida, script, limiteTicks, resultado)) {
            return false;
        }

        bool eliminado = retirarJugadorActualSiEliminado(partida);

        if (partida.finalizada) {
            break;
        }

        if (i < turnosDeLaRonda - 1 && !eliminado) {
            avanzarTurnoPartida(partida);
        }
    }

    if (!partida.finalizada) {
        finalizarRondaPartida(partida);
    }

    return true;
}


static int compararResultadoFinal(const Jugador& a, const Jugador& b) {
    if (a.puntaje > b.puntaje) return 1;
    if (a.puntaje < b.puntaje) return -1;

    if (a.oleadaAlcanzada > b.oleadaAlcanzada) return 1;
    if (a.oleadaAlcanzada < b.oleadaAlcanzada) return -1;

    if (a.metricas.precisionColor > b.metricas.precisionColor) return 1;
    if (a.metricas.precisionColor < b.metricas.precisionColor) return -1;

    return 0;
}

bool crearRegistroJugadorPartida(const Partida& partida, int indice, int idPartida, const std::string& modo, RegistroPartida& registro) {
    if (indice < 0 || indice >= partida.cantidadJugadores) {
        return false;
    }

    const Jugador& jugador = partida.jugadores[indice];
    const MetricasJugador& metricas = jugador.metricas;

    registro.idPartida = idPartida;
    registro.semilla = partida.semillaPartida;
    registro.modo = modo;
    registro.jugador = jugador.alias;
    registro.esBot = jugador.esBot;
    registro.posicion = metricas.posicionFinal;
    registro.puntaje = jugador.puntaje;
    registro.oleada = jugador.oleadaAlcanzada;
    registro.disparos = metricas.disparos;
    registro.correctos = metricas.impactosCorrectos;
    registro.erroneos = metricas.impactosErroneos;
    registro.comboMax = metricas.comboMaximo;
    registro.pulsos = metricas.pulsosUtilizados;
    registro.danoTotal = metricas.danoTotal;
    registro.ticks = metricas.ticksSobrevividos;
    registro.bajasDron = metricas.bajasDron;
    registro.bajasGirador = metricas.bajasGirador;
    registro.bajasDivisor = metricas.bajasDivisor;
    registro.bajasMini = metricas.bajasMini;
    registro.bajasTanque = metricas.bajasTanque;
    registro.bajasCamaleon = metricas.bajasCamaleon;

    return true;
}

void actualizarMetricasFinalesPartida(Partida& partida) {
    for (int i = 0; i < partida.cantidadJugadores; i++) {
        Jugador& jugador = partida.jugadores[i];

        jugador.metricas.puntaje = jugador.puntaje;
        jugador.metricas.oleadaAlcanzada = jugador.oleadaAlcanzada;

        actualizarMetricasCalculadas(jugador.metricas);
    }

    for (int i = 0; i < partida.cantidadJugadores; i++) {
        int posicion = 1;

        for (int j = 0; j < partida.cantidadJugadores; j++) {
            if (i != j && compararResultadoFinal(partida.jugadores[j], partida.jugadores[i]) > 0) {
                posicion++;
            }
        }

        partida.jugadores[i].metricas.posicionFinal = posicion;
    }
}

void agregarResultadosPartidaHistorial(Partida& partida, int idPartida, const std::string& modo, HistorialPartidas& historial) {
    actualizarMetricasFinalesPartida(partida);

    for (int i = 0; i < partida.cantidadJugadores; i++) {
        RegistroPartida registro;

        if (crearRegistroJugadorPartida(partida, i, idPartida, modo, registro)) {
            agregarRegistroFinal(historial, registro);

        }
    }
}