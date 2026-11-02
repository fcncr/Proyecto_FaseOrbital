#ifndef RONDA_JUGADORES_H
#define RONDA_JUGADORES_H

#include "ModelosJugador.h"

struct NodoJugador { //enlaza un jugador con sus vecinos dentro de la ronda circular
    Jugador* jugador; //apunta a un jugador almacenado fuera de la ronda
    NodoJugador* ant; //apunta al nodo anterior en el recorrido circular
    NodoJugador* sig; //apunta al siguiente nodo en el recorrido circular
};

struct RondaJugadores { //organiza los jugadores mediante una lista circular doble
    NodoJugador* actual; //apunta al nodo del turno actual o a nullptr si no hay jugadores
    int tam; //guarda la cantidad de jugadores enlazados en la ronda
};

void inicializarRondaJugadores(RondaJugadores& ronda); //establece los valores iniciales sin liberar nodos previos
void agregarJugador(RondaJugadores& ronda, Jugador* jugador); //inserta un nodo antes del actual sin cambiar el turno existente
void avanzarJugador(RondaJugadores& ronda, bool horario); //cambia el turno al siguiente o al anterior segun el sentido recibido
Jugador* eliminarJugadorActual(RondaJugadores& ronda, bool horario); //retira el nodo actual y devuelve su jugador sin destruirlo
Jugador* verJugadorActual(const RondaJugadores& ronda); //devuelve el jugador del turno actual o nullptr si la ronda esta vacia
int tamanoRonda(const RondaJugadores& ronda); //devuelve la cantidad de jugadores de la ronda
bool rondaVacia(const RondaJugadores& ronda); //indica si la ronda no contiene jugadores
void recorrerRondaJugadores(const RondaJugadores& ronda, bool horario, void (*procesar)(Jugador*)); //aplica una funcion a cada jugador sin mover el turno actual
void destruirRondaJugadores(RondaJugadores& ronda); //libera los nodos de la ronda y conserva los objetos de los jugadores

#endif