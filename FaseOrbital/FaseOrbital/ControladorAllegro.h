#ifndef CONTROLADOR_ALLEGRO_H
#define CONTROLADOR_ALLEGRO_H

#include <allegro5/allegro.h>
#include "ModelosCombate.h"
#include "Motor.h"

//guarda los controles que utiliza el jugador durante su turno
struct EstadoControles {
    bool izquierda; //indica si se mantiene presionada la flecha izquierda
    bool derecha; //indica si se mantiene presionada la flecha derecha
    bool usarMouse; //indica si la ultima direccion elegida fue con el mouse
    double anguloMouse; //guarda el angulo al que apunta el cursor
    bool cambiarC; //guarda la solicitud de cambiar a cian
    bool cambiarM; //guarda la solicitud de cambiar a magenta
    bool cambiarA; //guarda la solicitud de cambiar a ambar
    bool solicitarPulso; //guarda la solicitud de activar el pulso
    bool alternarPausa; //guarda la solicitud de pausar o reanudar
    bool volverMenu; //guarda la solicitud de salir desde la pausa
};

//deja los controles sin acciones pendientes al iniciar cada turno
void inicializarEstadoControles(EstadoControles& controles);

//recibe eventos de teclado y mouse sin modificar directamente el motor
void procesarEventoControlador(const ALLEGRO_EVENT& evento, EstadoControles& controles);

//convierte el estado de los controles en una accion para el siguiente tick
AccionMotor obtenerAccionAllegro(EstadoControles& controles, const Motor& motor);

//consulta una solicitud de pausa y la retira para no repetirla
bool consumirAlternarPausa(EstadoControles& controles);

//consulta una solicitud de volver al menu y la retira
bool consumirVolverMenu(EstadoControles& controles);

#endif