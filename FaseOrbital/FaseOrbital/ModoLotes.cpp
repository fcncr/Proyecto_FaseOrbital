#include "ModoLotes.h"
#include "Bot.h"

AccionMotor obtenerAccionModoLotes(const Motor& motor, const ScriptAcciones& script) {
    if (motor.jugador == nullptr) {
        AccionMotor accion;
        accion.cambiarAngulo = false;
        accion.anguloObjetivo = motor.nave.anguloObjetivo;
        accion.cambiarColor = false;
        accion.colorNuevo = motor.nave.color;
        accion.solicitarPulso = false;
        return accion;
    }

    if (motor.jugador->esBot) {
        return obtenerAccionBot(motor);
    }

    return obtenerAccionScript(script, motor.jugador->alias, motor.numeroOleada, motor.tickActual);
}

bool ejecutarModoLotes(Jugador& jugador, int numeroOleada, const ColaSpawn& spawns, const ScriptAcciones& script, int limiteTicks, ResultadoModoLotes& resultado, BitacoraEventos* bitacora) {
    Motor motor;
    inicializarMotor(motor, &jugador, numeroOleada, spawns, bitacora);

    while (!motor.turnoTerminado && motor.tickActual < limiteTicks) {
        AccionMotor accion = obtenerAccionModoLotes(motor, script);
        procesarTick(motor, accion);
    }

    resultado.ticksEjecutados = motor.tickActual;
    resultado.vidaNucleo = jugador.vidaNucleo;
    resultado.puntaje = jugador.puntaje;
    resultado.combo = jugador.combo;
    resultado.cargaPulso = jugador.cargaPulso;
    resultado.oleadaTerminada = motor.oleadaTerminada;
    resultado.turnoTerminado = motor.turnoTerminado;

    bool terminoCorrectamente = motor.turnoTerminado;

    destruirMotor(motor);

    return terminoCorrectamente;
}