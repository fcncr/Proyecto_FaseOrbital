#include "ControladorAllegro.h"

static AccionMotor crearAccionVaciaAllegro(const Motor& motor) {
    AccionMotor accion;

    accion.cambiarAngulo = false;
    accion.anguloObjetivo = motor.nave.anguloObjetivo;

    accion.cambiarColor = false;
    accion.colorNuevo = motor.nave.color;

    accion.solicitarPulso = false;

    return accion;
}

void inicializarEstadoControles(EstadoControles& controles) {
    controles.izquierda = false;
    controles.derecha = false;

    controles.cambiarC = false;
    controles.cambiarM = false;
    controles.cambiarA = false;

    controles.solicitarPulso = false;
    controles.alternarPausa = false;
    controles.volverMenu = false;
}

void procesarEventoControlador(const ALLEGRO_EVENT& evento,EstadoControles& controles) {
    if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (evento.keyboard.keycode) {
        case ALLEGRO_KEY_LEFT:
            controles.izquierda = true;
            break;

        case ALLEGRO_KEY_RIGHT:
            controles.derecha = true;
            break;

        case ALLEGRO_KEY_1:
            controles.cambiarC = true;
            break;

        case ALLEGRO_KEY_2:
            controles.cambiarM = true;
            break;

        case ALLEGRO_KEY_3:
            controles.cambiarA = true;
            break;

        case ALLEGRO_KEY_SPACE:
            controles.solicitarPulso = true;
            break;

        case ALLEGRO_KEY_P:
        case ALLEGRO_KEY_ESCAPE:
            controles.alternarPausa = true;
            break;
        case ALLEGRO_KEY_M:
            controles.volverMenu = true;
            break;
        }
 
    }

    if (evento.type == ALLEGRO_EVENT_KEY_UP) {
        switch (evento.keyboard.keycode) {
        case ALLEGRO_KEY_LEFT:
            controles.izquierda = false;
            break;

        case ALLEGRO_KEY_RIGHT:
            controles.derecha = false;
            break;
        }
    }
}

AccionMotor obtenerAccionAllegro(EstadoControles& controles,const Motor& motor) {
    AccionMotor accion = crearAccionVaciaAllegro(motor);

    if (controles.izquierda && !controles.derecha) {
        accion.cambiarAngulo = true;
        accion.anguloObjetivo = motor.nave.anguloActual - 3.0;
    }
    else if (controles.derecha && !controles.izquierda) {
        accion.cambiarAngulo = true;
        accion.anguloObjetivo = motor.nave.anguloActual + 3.0;
    }

    if (controles.cambiarC) {
        accion.cambiarColor = true;
        accion.colorNuevo = 0;
    }
    else if (controles.cambiarM) {
        accion.cambiarColor = true;
        accion.colorNuevo = 1;
    }
    else if (controles.cambiarA) {
        accion.cambiarColor = true;
        accion.colorNuevo = 2;
    }

    if (controles.solicitarPulso) {
        accion.solicitarPulso = true;
    }

    controles.cambiarC = false;
    controles.cambiarM = false;
    controles.cambiarA = false;
    controles.solicitarPulso = false;

    return accion;
}

bool consumirAlternarPausa(EstadoControles& controles) {
    if (!controles.alternarPausa) {
        return false;
    }

    controles.alternarPausa = false;
    return true;
}

bool consumirVolverMenu(EstadoControles& controles) {
    if (!controles.volverMenu) {
        return false;
    }

    controles.volverMenu = false;
    return true;
}