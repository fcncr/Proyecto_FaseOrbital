#include "Bot.h"

AccionMotor obtenerAccionBot(const Motor& motor) {
    AccionMotor accion; //reune las decisiones que el bot le entregara al motor
    accion.cambiarAngulo = true; //actualiza el destino para evitar que continue un giro anterior
    accion.anguloObjetivo = motor.nave.anguloActual; //mantiene el angulo actual si no encuentra un objetivo
    accion.cambiarColor = false; //empieza sin pedir un cambio de fase
    accion.colorNuevo = motor.nave.color; //conserva la fase actual como valor inicial
    accion.solicitarPulso = false; //no pide el pulso hasta comprobar sus condiciones

    const NodoEnemigo* objetivo = nullptr; //todavia no ha elegido un enemigo al que apuntar
    const NodoEnemigo* actual = motor.enemigos.cabeza; //empieza a revisar desde el primer enemigo de la lista
    int enemigosCerca = 0; //cuenta los enemigos que estan a menos de ciento sesenta unidades del centro

    while (actual != nullptr) { //recorre todos los enemigos para elegir objetivo y contar los cercanos
        if (actual->dato.r > 100.0) { //considera como objetivo solo a los que siguen fuera de la orbita de la nave
            if (objetivo == nullptr || actual->dato.r < objetivo->dato.r) { //elige el menor radio y conserva al primero si hay empate
                objetivo = actual; //recuerda este enemigo sin modificarlo ni copiarlo
            }
        }

        if (actual->dato.r < 160.0) { //cuenta los cercanos incluso si ya pasaron hacia dentro de la orbita
            enemigosCerca++; //suma un enemigo para decidir si conviene pedir el pulso
        }

        actual = actual->sig; //continua con el siguiente enemigo
    }

    if (objetivo != nullptr) { //prepara los cambios cuando encontro un enemigo al que apuntar
        accion.anguloObjetivo = objetivo->dato.theta; //usa el angulo actual del objetivo

        if (objetivo->dato.color != motor.nave.color) { //comprueba si necesita otra fase para acertarle
            accion.cambiarColor = true; //pide cambiar la fase de la nave
            accion.colorNuevo = objetivo->dato.color; //elige el mismo color que tiene el enemigo
        }
    }

    if (motor.jugador != nullptr && motor.jugador->cargaPulso == 100 && enemigosCerca >= 5) { //pide el pulso con carga completa y al menos cinco enemigos cerca
        accion.solicitarPulso = true; //deja que el motor compruebe si puede activar la onda
    }

    return accion; //entrega las decisiones sin mover la nave ni gastar carga aqui
}