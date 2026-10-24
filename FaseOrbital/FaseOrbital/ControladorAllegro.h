#ifndef CONTROLADOR_ALLEGRO_H
#define CONTROLADOR_ALLEGRO_H

#include <allegro5/allegro.h>

#include "ModelosCombate.h" //llama struct "AccionMotor"
#include "Motor.h"

struct EstadoControles {
    bool izquierda;
    bool derecha;

    bool cambiarC;
    bool cambiarM;
    bool cambiarA;

    bool solicitarPulso;
    bool alternarPausa;

    bool volverMenu;
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
bool consumirVolverMenu(EstadoControles& controles);

#endif