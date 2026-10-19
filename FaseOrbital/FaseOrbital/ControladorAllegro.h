#ifndef CONTROLADOR_ALLEGRO_H
#define CONTROLADOR_ALLEGRO_H

#include <allegro5/allegro.h>

#include "AccionMotor.h"
#include "Motor.h"

struct EstadoControles {
    bool izquierda;
    bool derecha;

    bool cambiarC;
    bool cambiarM;
    bool cambiarA;

    bool solicitarPulso;
    bool alternarPausa;
};

void inicializarEstadoControles(EstadoControles& controles);

void procesarEventoControlador(
    const ALLEGRO_EVENT& evento,
    EstadoControles& controles
);

AccionMotor obtenerAccionAllegro(
    EstadoControles& controles,
    const Motor& motor
);

bool consumirAlternarPausa(EstadoControles& controles);

#endif