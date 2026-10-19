#ifndef VISTA_ALLEGRO_H
#define VISTA_ALLEGRO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "Motor.h"

struct VistaAllegro {
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* colaEventos;
    ALLEGRO_TIMER* timer;
    ALLEGRO_FONT* fuente;

    int ancho;
    int alto;
};

bool inicializarVistaAllegro(VistaAllegro& vista, int ancho, int alto);
void destruirVistaAllegro(VistaAllegro& vista);

void dibujarMotor(
    const VistaAllegro& vista,
    const Motor& motor,
    bool pausado
);

#endif