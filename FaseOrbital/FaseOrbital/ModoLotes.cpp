#include "ModoLotes.h"

bool ejecutarModoLotes(Jugador& jugador, int numeroOleada, const ColaSpawn& spawns, const ScriptAcciones& script, int limiteTicks, ResultadoModoLotes& resultado) {
    Motor motor;
    inicializarMotor(motor, &jugador, numeroOleada, spawns);

    while (!motor.turnoTerminado && motor.tickActual < limiteTicks) {
        AccionMotor accion = obtenerAccionScript(script, jugador.alias, numeroOleada, motor.tickActual);

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