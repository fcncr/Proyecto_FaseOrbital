#ifndef VISTA_ALLEGRO_H
#define VISTA_ALLEGRO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include "Motor.h"

//guarda un efecto visual de baja sin crear nodos nuevos
struct DestelloBajaVista {
    bool activo;
    int tickInicio;
    int color;
};

//guarda los recursos de allegro y los efectos del turno actual
struct VistaAllegro {
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* colaEventos;
    ALLEGRO_TIMER* timer;
    ALLEGRO_FONT* fuente;
    ALLEGRO_SAMPLE* sonidoBaja;
    ALLEGRO_SAMPLE* sonidoError;
    ALLEGRO_SAMPLE* sonidoDano;
    ALLEGRO_SAMPLE* sonidoPulso;
    int ancho;
    int alto;

    bool tecladoInstalado; //indica si hay que liberar el teclado
    bool mouseInstalado; //indica si el mouse quedo instalado
    bool primitivasIniciadas; //indica si se inicio el addon de dibujo
    bool fuentesIniciadas; //indica si se inicio el addon de fuentes
    bool ttfIniciado; //indica si se inicio el addon ttf
    bool audioInstalado; //indica si se inicio el sistema de audio

    const BitacoraEventos* bitacoraObservada; //identifica la bitacora del turno
    const Jugador* jugadorObservado; //identifica al jugador del turno
    int oleadaObservada; //identifica la oleada del turno
    int tickObservado; //permite detectar cuando comienza otro turno
    int eventosProcesados; //evita reproducir los mismos eventos
    int siguienteDestello; //elige la posicion del siguiente destello
    DestelloBajaVista destellos[48]; //almacena los efectos de baja
};

//crea la ventana y carga la fuente, los sonidos y los controles
bool inicializarVistaAllegro(VistaAllegro& vista, int ancho, int alto);

//libera los recursos creados por la vista
void destruirVistaAllegro(VistaAllegro& vista);

//dibuja el estado del motor y reproduce los efectos nuevos
void dibujarMotor(VistaAllegro& vista, const Motor& motor, bool pausado);

#endif