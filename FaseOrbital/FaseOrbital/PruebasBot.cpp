#include "PruebasBot.h"
#include "Bot.h"

static Enemigo crearEnemigoPrueba(int id, int color, double r, double theta) {
    Enemigo enemigo;
    enemigo.id = id;
    enemigo.tipo = Dron;
    enemigo.color = color;
    enemigo.r = r;
    enemigo.theta = theta;
    enemigo.omega = 0.0;
    enemigo.vida = 1;
    enemigo.radio = 10;
    enemigo.ticksColor = 0;
    return enemigo;
}

static bool pruebaBotSeleccionObjetivo() {
    Jugador jugador{ "BOT1", true, 100, 0, 0, 0, 0, false };

    Motor motor{};
    motor.jugador = &jugador;
    motor.nave.anguloActual = 0.0;
    motor.nave.anguloObjetivo = 0.0;
    motor.nave.color = 0;
    inicializarListaEnemigos(motor.enemigos);

    insertarFinal(motor.enemigos, crearEnemigoPrueba(1, 0, 90.0, 20.0));
    insertarFinal(motor.enemigos, crearEnemigoPrueba(2, 2, 130.0, 45.0));
    insertarFinal(motor.enemigos, crearEnemigoPrueba(3, 1, 120.0, 90.0));
    insertarFinal(motor.enemigos, crearEnemigoPrueba(4, 2, 120.0, 180.0));

    AccionMotor accion = obtenerAccionBot(motor);

    bool resultado = true;

    if (!accion.cambiarAngulo || accion.anguloObjetivo != 90.0) {
        resultado = false;
    }

    if (!accion.cambiarColor || accion.colorNuevo != 1) {
        resultado = false;
    }

    if (accion.solicitarPulso) {
        resultado = false;
    }

    destruirListaEnemigos(motor.enemigos);

    return resultado;
}

static bool pruebaBotSinObjetivo() {
    Jugador jugador{ "BOT1", true, 100, 0, 0, 0, 0, false };

    Motor motor{};
    motor.jugador = &jugador;
    motor.nave.anguloActual = 25.0;
    motor.nave.anguloObjetivo = 25.0;
    motor.nave.color = 2;
    inicializarListaEnemigos(motor.enemigos);

    insertarFinal(motor.enemigos, crearEnemigoPrueba(1, 0, 100.0, 50.0));
    insertarFinal(motor.enemigos, crearEnemigoPrueba(2, 1, 80.0, 90.0));

    AccionMotor accion = obtenerAccionBot(motor);

    bool resultado = true;

    if (accion.cambiarAngulo) {
        resultado = false;
    }

    if (accion.cambiarColor) {
        resultado = false;
    }

    if (accion.solicitarPulso) {
        resultado = false;
    }

    destruirListaEnemigos(motor.enemigos);

    return resultado;
}

static bool pruebaBotMismoColor() {
    Jugador jugador{ "BOT1", true, 100, 0, 0, 0, 0, false };

    Motor motor{};
    motor.jugador = &jugador;
    motor.nave.anguloActual = 0.0;
    motor.nave.anguloObjetivo = 0.0;
    motor.nave.color = 1;
    inicializarListaEnemigos(motor.enemigos);

    insertarFinal(motor.enemigos, crearEnemigoPrueba(1, 1, 120.0, 75.0));

    AccionMotor accion = obtenerAccionBot(motor);

    bool resultado = true;

    if (!accion.cambiarAngulo || accion.anguloObjetivo != 75.0) {
        resultado = false;
    }

    if (accion.cambiarColor) {
        resultado = false;
    }

    destruirListaEnemigos(motor.enemigos);

    return resultado;
}

static bool pruebaBotPulso() {
    Jugador jugador{ "BOT1", true, 100, 0, 0, 100, 0, false };

    Motor motor{};
    motor.jugador = &jugador;
    motor.nave.anguloActual = 0.0;
    motor.nave.anguloObjetivo = 0.0;
    motor.nave.color = 0;
    inicializarListaEnemigos(motor.enemigos);

    insertarFinal(motor.enemigos, crearEnemigoPrueba(1, 0, 120.0, 10.0));
    insertarFinal(motor.enemigos, crearEnemigoPrueba(2, 0, 130.0, 20.0));
    insertarFinal(motor.enemigos, crearEnemigoPrueba(3, 0, 140.0, 30.0));
    insertarFinal(motor.enemigos, crearEnemigoPrueba(4, 0, 150.0, 40.0));

    AccionMotor accion = obtenerAccionBot(motor);

    bool resultado = true;

    if (accion.solicitarPulso) {
        resultado = false;
    }

    insertarFinal(motor.enemigos, crearEnemigoPrueba(5, 0, 159.0, 50.0));

    accion = obtenerAccionBot(motor);

    if (!accion.solicitarPulso) {
        resultado = false;
    }

    insertarFinal(motor.enemigos, crearEnemigoPrueba(6, 0, 160.0, 60.0));

    accion = obtenerAccionBot(motor);

    if (!accion.solicitarPulso) {
        resultado = false;
    }

    jugador.cargaPulso = 99;

    accion = obtenerAccionBot(motor);

    if (accion.solicitarPulso) {
        resultado = false;
    }

    destruirListaEnemigos(motor.enemigos);

    return resultado;
}

static bool pruebaBotNoModificaEstado() {
    Jugador jugador{ "BOT1", true, 100, 250, 3, 100, 2, false };

    Motor motor{};
    motor.jugador = &jugador;
    motor.nave.anguloActual = 30.0;
    motor.nave.anguloObjetivo = 40.0;
    motor.nave.color = 2;
    inicializarListaEnemigos(motor.enemigos);

    insertarFinal(motor.enemigos, crearEnemigoPrueba(1, 1, 120.0, 90.0));
    insertarFinal(motor.enemigos, crearEnemigoPrueba(2, 2, 130.0, 180.0));

    NodoEnemigo* cabezaAntes = motor.enemigos.cabeza;
    NodoEnemigo* segundoAntes = cabezaAntes->sig;
    int tamAntes = motor.enemigos.tam;
    int puntajeAntes = jugador.puntaje;
    int cargaAntes = jugador.cargaPulso;
    double anguloActualAntes = motor.nave.anguloActual;
    double anguloObjetivoAntes = motor.nave.anguloObjetivo;
    int colorAntes = motor.nave.color;

    obtenerAccionBot(motor);

    bool resultado = true;

    if (motor.enemigos.cabeza != cabezaAntes || motor.enemigos.cabeza->sig != segundoAntes || motor.enemigos.tam != tamAntes) {
        resultado = false;
    }

    if (jugador.puntaje != puntajeAntes || jugador.cargaPulso != cargaAntes) {
        resultado = false;
    }

    if (motor.nave.anguloActual != anguloActualAntes || motor.nave.anguloObjetivo != anguloObjetivoAntes || motor.nave.color != colorAntes) {
        resultado = false;
    }

    destruirListaEnemigos(motor.enemigos);

    return resultado;
}

bool pruebaBot() {
    if (!pruebaBotSeleccionObjetivo()) {
        return false;
    }

    if (!pruebaBotSinObjetivo()) {
        return false;
    }

    if (!pruebaBotMismoColor()) {
        return false;
    }

    if (!pruebaBotPulso()) {
        return false;
    }

    if (!pruebaBotNoModificaEstado()) {
        return false;
    }

    return true;
}