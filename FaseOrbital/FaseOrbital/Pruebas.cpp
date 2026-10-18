#include "Pruebas.h"
#include "../FaseOrbital/ColaSpawn.h"
#include "../FaseOrbital/RondaJugadores.h"
#include "../FaseOrbital/Generador.h"
#include "../FaseOrbital/Partida.h"
#include "ModoLotes.h"
#include "Bot.h"

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
/*
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
*/
bool pruebaP07() {
    RondaJugadores ronda;
    inicializarRondaJugadores(ronda);

    Jugador jugadorA{ "A", false, 100, 0, 0, 0, 0, false };
    Jugador jugadorB{ "B", false, 100, 0, 0, 0, 0, false };
    Jugador jugadorC{ "C", false, 100, 0, 0, 0, 0, false };

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

    if (ronda.actual == nullptr) {
        resultado = false;
    }
    else if (ronda.actual->sig->ant != ronda.actual || ronda.actual->ant->sig != ronda.actual) {
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

    if (ronda.actual == nullptr) {
        resultado = false;
    }
    else if (ronda.actual->sig->ant != ronda.actual || ronda.actual->ant->sig != ronda.actual) {
        resultado = false;
    }

    eliminado = eliminarJugadorActual(ronda, false);

    if (eliminado != &jugadorB) {
        resultado = false;
    }

    if (tamanoRonda(ronda) != 1 || verJugadorActual(ronda) != &jugadorC) {
        resultado = false;
    }

    if (ronda.actual == nullptr) {
        resultado = false;
    }
    else if (ronda.actual->sig != ronda.actual || ronda.actual->ant != ronda.actual) {
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

bool pruebaCopiasIndependientesPartida() {
    Partida partida;
    inicializarPartida(partida, 42u, 10);

    agregarJugadorPartida(partida, "A", false);
    agregarJugadorPartida(partida, "B", false);

    generarColaOriginalRonda(partida);

    ColaSpawn colaA;
    ColaSpawn colaB;
    inicializarColaSpawn(colaA);
    inicializarColaSpawn(colaB);

    prepararColaJugador(partida, colaA);
    prepararColaJugador(partida, colaB);

    bool resultado = true;

    if (partida.colaOriginalRonda.tam <= 0) {
        resultado = false;
    }

    if (colaA.tam != partida.colaOriginalRonda.tam || colaB.tam != partida.colaOriginalRonda.tam) {
        resultado = false;
    }

    if (colaA.frente == partida.colaOriginalRonda.frente || colaB.frente == partida.colaOriginalRonda.frente) {
        resultado = false;
    }

    if (colaA.frente == colaB.frente) {
        resultado = false;
    }

    int tamOriginal = partida.colaOriginalRonda.tam;
    int tamB = colaB.tam;

    Spawn salida;

    while (desencolarSpawn(colaA, salida)) {
    }

    if (!colaSpawnVacia(colaA)) {
        resultado = false;
    }

    if (partida.colaOriginalRonda.tam != tamOriginal) {
        resultado = false;
    }

    if (colaB.tam != tamB) {
        resultado = false;
    }

    if (partida.colaOriginalRonda.frente == nullptr || colaB.frente == nullptr) {
        resultado = false;
    }

    destruirColaSpawn(colaA);
    destruirColaSpawn(colaB);
    destruirPartida(partida);

    return resultado;
}

bool pruebaOrdenSerpenteantePartida() {
    Partida partida;
    inicializarPartida(partida, 42u, 10);

    agregarJugadorPartida(partida, "A", false);
    agregarJugadorPartida(partida, "B", false);
    agregarJugadorPartida(partida, "C", false);

    bool resultado = true;

    if (partida.numeroRonda != 1 || !sentidoHorarioRondaPartida(partida)) {
        resultado = false;
    }

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "A") {
        resultado = false;
    }

    avanzarTurnoPartida(partida);

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "B") {
        resultado = false;
    }

    avanzarTurnoPartida(partida);

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "C") {
        resultado = false;
    }

    finalizarRondaPartida(partida);

    if (partida.numeroRonda != 2 || sentidoHorarioRondaPartida(partida)) {
        resultado = false;
    }

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "C") {
        resultado = false;
    }

    avanzarTurnoPartida(partida);

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "B") {
        resultado = false;
    }

    avanzarTurnoPartida(partida);

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "A") {
        resultado = false;
    }

    finalizarRondaPartida(partida);

    if (partida.numeroRonda != 3 || !sentidoHorarioRondaPartida(partida)) {
        resultado = false;
    }

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "A") {
        resultado = false;
    }

    destruirPartida(partida);

    return resultado;
}

bool pruebaEliminacionesPartida() {
    Partida partida;
    inicializarPartida(partida, 42u, 10);

    agregarJugadorPartida(partida, "A", false);
    agregarJugadorPartida(partida, "B", false);
    agregarJugadorPartida(partida, "C", false);

    bool resultado = true;

    avanzarTurnoPartida(partida);

    Jugador* jugadorB = verJugadorActualPartida(partida);

    if (jugadorB == nullptr || jugadorB->alias != "B") {
        resultado = false;
    }
    else {
        jugadorB->vidaNucleo = 0;
        jugadorB->puntaje = 500;
        jugadorB->oleadaAlcanzada = 1;
    }

    if (!retirarJugadorActualSiEliminado(partida)) {
        resultado = false;
    }

    if (!partida.jugadores[1].eliminado || partida.jugadores[1].puntaje != 500) {
        resultado = false;
    }

    if (cantidadJugadoresPartida(partida) != 3 || cantidadJugadoresActivos(partida) != 2) {
        resultado = false;
    }

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "C") {
        resultado = false;
    }

    Jugador* jugadorC = verJugadorActualPartida(partida);

    if (jugadorC != nullptr) {
        jugadorC->vidaNucleo = 0;
    }

    if (!retirarJugadorActualSiEliminado(partida)) {
        resultado = false;
    }

    if (cantidadJugadoresActivos(partida) != 1) {
        resultado = false;
    }

    if (partida.finalizada) {
        resultado = false;
    }

    finalizarRondaPartida(partida);

    if (!partida.finalizada) {
        resultado = false;
    }

    destruirPartida(partida);

    Partida partidaSinActivos;
    inicializarPartida(partidaSinActivos, 42u, 10);
    agregarJugadorPartida(partidaSinActivos, "SOLO", false);

    partidaSinActivos.jugadores[0].vidaNucleo = 0;

    if (!retirarJugadorActualSiEliminado(partidaSinActivos)) {
        resultado = false;
    }

    if (!partidaSinActivos.finalizada || cantidadJugadoresActivos(partidaSinActivos) != 0) {
        resultado = false;
    }

    destruirPartida(partidaSinActivos);

    Partida partidaLimite;
    inicializarPartida(partidaLimite, 42u, 1);
    agregarJugadorPartida(partidaLimite, "A", false);
    agregarJugadorPartida(partidaLimite, "B", false);

    finalizarRondaPartida(partidaLimite);

    if (!partidaLimite.finalizada || partidaLimite.numeroRonda != 1) {
        resultado = false;
    }

    destruirPartida(partidaLimite);

    return resultado;
}
/*
Esta prueba cubre las tres condiciones pedidas:

queda 1 después de ronda → finaliza
quedan 0 → finaliza
se alcanza límite → finaliza
*/
bool pruebaGanadorPartida() {
    Partida partida;
    inicializarPartida(partida, 42u, 10);

    agregarJugadorPartida(partida, "A", false);
    agregarJugadorPartida(partida, "B", false);
    agregarJugadorPartida(partida, "C", false);

    bool resultado = true;

    partida.jugadores[0].puntaje = 1000;
    partida.jugadores[1].puntaje = 1500;
    partida.jugadores[2].puntaje = 1200;

    const Jugador* ganador = determinarGanadorPartida(partida, nullptr);

    if (ganador == nullptr || ganador->alias != "B") {
        resultado = false;
    }

    partida.jugadores[0].puntaje = 1500;
    partida.jugadores[1].puntaje = 1500;
    partida.jugadores[0].oleadaAlcanzada = 4;
    partida.jugadores[1].oleadaAlcanzada = 5;

    ganador = determinarGanadorPartida(partida, nullptr);

    if (ganador == nullptr || ganador->alias != "B") {
        resultado = false;
    }

    partida.jugadores[0].oleadaAlcanzada = 5;
    partida.jugadores[1].oleadaAlcanzada = 5;

    ganador = determinarGanadorPartida(partida, nullptr);

    if (ganador != nullptr) {
        resultado = false;
    }

    double precisiones[6] = { 90.0, 95.0, 80.0, 0.0, 0.0, 0.0 };

    ganador = determinarGanadorPartida(partida, precisiones);

    if (ganador == nullptr || ganador->alias != "B") {
        resultado = false;
    }

    destruirPartida(partida);

    return resultado;
}
/*
puntaje:
B gana

puntaje empatado:
B gana por oleada

puntaje + oleada empatados:
sin precisión → nullptr

con precisión:
B gana
*/

static bool accionesMotorIguales(const AccionMotor& a, const AccionMotor& b) {
    return a.cambiarAngulo == b.cambiarAngulo && a.anguloObjetivo == b.anguloObjetivo && a.cambiarColor == b.cambiarColor && a.colorNuevo == b.colorNuevo && a.solicitarPulso == b.solicitarPulso;
}

bool pruebaIntegracionModoLotes() {
    ScriptAcciones script;
    inicializarScriptAcciones(script);

    bool resultado = true;

    Jugador humano{ "HUMANO", false, 100, 0, 0, 0, 0, false };

    Motor motorHumano{};
    motorHumano.jugador = &humano;
    motorHumano.numeroOleada = 1;
    motorHumano.tickActual = 0;
    motorHumano.nave.anguloActual = 270.0;
    motorHumano.nave.anguloObjetivo = 270.0;
    motorHumano.nave.color = 0;

    AccionMotor accionScript = obtenerAccionScript(script, humano.alias, 1, 0);
    AccionMotor accionHumano = obtenerAccionModoLotes(motorHumano, script);

    if (!accionesMotorIguales(accionScript, accionHumano)) {
        resultado = false;
    }

    Jugador bot{ "BOT1", true, 100, 0, 0, 0, 0, false };

    Motor motorBot{};
    motorBot.jugador = &bot;
    motorBot.numeroOleada = 1;
    motorBot.tickActual = 0;
    motorBot.nave.anguloActual = 270.0;
    motorBot.nave.anguloObjetivo = 270.0;
    motorBot.nave.color = 0;
    inicializarListaEnemigos(motorBot.enemigos);

    Enemigo enemigo;
    enemigo.id = 1;
    enemigo.tipo = Dron;
    enemigo.color = 2;
    enemigo.r = 120.0;
    enemigo.theta = 90.0;
    enemigo.omega = 0.0;
    enemigo.vida = 1;
    enemigo.radio = 6;
    enemigo.ticksColor = 0;

    insertarFinal(motorBot.enemigos, enemigo);

    AccionMotor accionBotDirecta = obtenerAccionBot(motorBot);
    AccionMotor accionBotIntegrada = obtenerAccionModoLotes(motorBot, script);

    if (!accionesMotorIguales(accionBotDirecta, accionBotIntegrada)) {
        resultado = false;
    }

    destruirListaEnemigos(motorBot.enemigos);
    destruirScriptAcciones(script);

    return resultado;
}