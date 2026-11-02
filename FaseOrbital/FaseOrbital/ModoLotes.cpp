#include "ModoLotes.h"
#include "Bot.h"

AccionMotor obtenerAccionModoLotes(const Motor& motor, const ScriptAcciones& script) {
    if (motor.jugador == nullptr) { //comprueba si falta el participante que debe tomar las decisiones
        AccionMotor accion; //prepara una respuesta sin solicitudes nuevas
        accion.cambiarAngulo = false; //no solicita otro destino para la nave
        accion.anguloObjetivo = motor.nave.anguloObjetivo; //conserva el destino que ya tenia
        accion.cambiarColor = false; //no solicita cambiar la fase
        accion.colorNuevo = motor.nave.color; //mantiene la fase actual como valor de referencia
        accion.solicitarPulso = false; //no solicita activar un pulso
        return accion; // return accion
    }

    if (motor.jugador->esBot) { //revisa si el participante toma sus decisiones automaticamente
        return obtenerAccionBot(motor); //deja que el bot elija la accion a partir del estado del motor
    }

    return obtenerAccionScript(script, motor.jugador->alias, motor.numeroOleada, motor.tickActual); //consulta el script para este jugador oleada y tick
}

bool ejecutarModoLotes(Jugador& jugador, int numeroOleada, const ColaSpawn& spawns, const ScriptAcciones& script, int limiteTicks, ResultadoModoLotes& resultado, BitacoraEventos* bitacora) {
    Motor motor; //guarda el estado temporal de la simulacion de este turno
    inicializarMotor(motor, &jugador, numeroOleada, spawns, bitacora); //prepara el turno con una copia de las apariciones y reinicia el combo del jugador

    while (!motor.turnoTerminado && motor.tickActual < limiteTicks) { //continua mientras el turno siga abierto y queden ticks disponibles
        AccionMotor accion = obtenerAccionModoLotes(motor, script); //obtiene la decision del bot o la accion indicada en el script
        procesarTick(motor, accion); //ejecuta un paso completo y actualiza el contador de ticks
    }

    resultado.ticksEjecutados = motor.tickActual; //guarda cuantos pasos se alcanzaron a procesar
    resultado.vidaNucleo = jugador.vidaNucleo; //guarda la vida restante aunque se haya detenido por el limite
    resultado.puntaje = jugador.puntaje; //guarda el puntaje acumulado hasta este momento
    resultado.combo = jugador.combo; //guarda el combo con el que se detuvo el turno
    resultado.cargaPulso = jugador.cargaPulso; //guarda la carga de pulso que conserva el jugador
    resultado.oleadaTerminada = motor.oleadaTerminada; //indica si logro completar la oleada
    resultado.turnoTerminado = motor.turnoTerminado; //distingue un turno terminado de uno detenido por el limite

    bool terminoCorrectamente = motor.turnoTerminado; //conserva si el turno termino por completar la oleada o por morir

    destruirMotor(motor); //libera las listas y la copia de apariciones sin borrar al jugador ni la bitacora

    return terminoCorrectamente; //devuelve falso si se agoto el limite sin que terminara el turno
}