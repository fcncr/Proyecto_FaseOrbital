#include "Bot.h"

AccionMotor obtenerAccionBot(const Motor& motor) {
    AccionMotor accion;
    accion.cambiarAngulo = false;
    accion.anguloObjetivo = motor.nave.anguloObjetivo;
    accion.cambiarColor = false;
    accion.colorNuevo = motor.nave.color;
    accion.solicitarPulso = false;

    const NodoEnemigo* objetivo = nullptr;
    const NodoEnemigo* actual = motor.enemigos.cabeza;
    int enemigosCerca = 0;

    while (actual != nullptr) {
        if (actual->dato.r > 100.0) {
            if (objetivo == nullptr || actual->dato.r < objetivo->dato.r) {
                objetivo = actual;
            }
        }

        if (actual->dato.r < 160.0) {
            enemigosCerca++;
        }

        actual = actual->sig;
    }

    if (objetivo != nullptr) {
        accion.cambiarAngulo = true;
        accion.anguloObjetivo = objetivo->dato.theta;

        if (objetivo->dato.color != motor.nave.color) {
            accion.cambiarColor = true;
            accion.colorNuevo = objetivo->dato.color;
        }
    }

    if (motor.jugador != nullptr && motor.jugador->cargaPulso == 100 && enemigosCerca >= 5) {
        accion.solicitarPulso = true;
    }

    return accion;
}