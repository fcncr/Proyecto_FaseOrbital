#ifndef VISTA_ALLEGRO_H
#define VISTA_ALLEGRO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include "Motor.h"

struct DestelloBajaVista { //guarda un efecto de baja en un espacio fijo del arreglo
    bool activo; //indica que el espacio contiene un efecto aunque su duracion ya haya terminado
    int tickInicio; //tick en que la vista registro el efecto para comenzar a dibujarlo
    int color; //fase de la baja usada para colorear el destello
    double r; //distancia al centro donde murio el enemigo
    double theta; //angulo donde murio el enemigo
};

struct VistaAllegro { //reune los recursos que libera la vista y el seguimiento de efectos del turno
    ALLEGRO_DISPLAY* display; //ventana donde se presentan las pantallas y el juego
    ALLEGRO_EVENT_QUEUE* colaEventos; //cola que recibe eventos de ventana temporizador teclado y mouse disponible
    ALLEGRO_TIMER* timer; //temporizador de treinta eventos por segundo que la aplicacion inicia al jugar
    ALLEGRO_FONT* fuente; //fuente compartida por los textos de todas las pantallas
    ALLEGRO_SAMPLE* sonidoBaja; //muestra de audio para una eliminacion de enemigo
    ALLEGRO_SAMPLE* sonidoError; //muestra de audio para un impacto de color incorrecto
    ALLEGRO_SAMPLE* sonidoDano; //muestra de audio para el dano recibido por el nucleo
    ALLEGRO_SAMPLE* sonidoPulso; //muestra de audio para la activacion del pulso
    int ancho; //ancho guardado de la ventana en pixeles
    int alto; //alto guardado de la ventana en pixeles

    bool tecladoInstalado; //indica si hay que liberar el teclado
    bool mouseInstalado; //indica si el mouse quedo instalado
    bool primitivasIniciadas; //indica si se inicio el addon de dibujo
    bool fuentesIniciadas; //indica si se inicio el addon de fuentes
    bool ttfIniciado; //indica si se inicio el addon ttf
    bool audioInstalado; //indica si se inicio el sistema de audio

    const BitacoraEventos* bitacoraObservada; //referencia prestada a la bitacora que la vista no debe liberar
    const Jugador* jugadorObservado; //referencia prestada al jugador cuyos efectos se estan mostrando
    int oleadaObservada; //oleada usada para detectar cambios de turno
    int tickObservado; //ultimo tick revisado para detectar un reinicio de la simulacion
    int eventosProcesados; //cantidad de nodos revisados incluidos los de otros jugadores u oleadas
    int siguienteDestello; //posicion que se reutilizara al agregar el siguiente efecto
    DestelloBajaVista destellos[48]; //espacios reutilizables para las explosiones de bajas recientes
};

//inicializa una vista sin recursos activos y crea la ventana la fuente los controles y los sonidos cuando estan disponibles
//con addresssanitizer omite el audio y permite continuar sin mouse liberando los recursos si ocurre otro fallo
bool inicializarVistaAllegro(VistaAllegro& vista, int ancho, int alto);

//libera los recursos de una vista inicializada aunque la carga haya quedado incompleta y deja sus punteros en nulo
void destruirVistaAllegro(VistaAllegro& vista);

//actualiza los efectos y dibuja el motor con los recursos cargados sin avanzar la simulacion
//el parametro de pausa agrega el aviso visual y la aplicacion decide cuando detener los ticks
void dibujarMotor(VistaAllegro& vista, const Motor& motor, bool pausado);

#endif
