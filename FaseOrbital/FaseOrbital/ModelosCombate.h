#pragma once
#ifndef MODELOS_COMBATE_H
#define MODELOS_COMBATE_H

enum TipoEnemigo { Dron, Girador, Divisor, Mini, Tanque, Camaleon }; //tipos de enemigos que puede crear el juego

struct Spawn { //guarda los datos de un enemigo que aparecera mas adelante
    int tick; //momento de la oleada en que debe aparecer
    TipoEnemigo tipo; //tipo de enemigo que se va a crear
    int color; //fase del enemigo con cero para cian uno para magenta y dos para ambar
    int angulo; //angulo de aparicion medido en grados
    int sentido; //sentido de giro del girador representado con cero o uno
};

struct Enemigo { //guarda el estado de un enemigo que ya esta en la partida
    int id; //identificador que permite distinguirlo de los demas enemigos
    TipoEnemigo tipo; //define las reglas que usa este enemigo
    int color; //fase actual del enemigo entre cian magenta y ambar
    double r; //distancia del enemigo al centro del escenario
    double theta; //angulo actual alrededor del centro medido en grados
    double omega; //grados que gira por tick con el signo indicando el sentido
    int vida; //vida que le queda antes de ser eliminado
    int radio; //tamano del enemigo usado al revisar choques
    int ticksColor; //ticks que lleva el camaleon desde su ultimo cambio de color
};

struct Bala { //guarda la posicion y el movimiento de un disparo
    double x; //posicion horizontal respecto al centro del escenario
    double y; //posicion vertical respecto al centro del escenario
    double vx; //avance horizontal de la bala en cada tick
    double vy; //avance vertical de la bala en cada tick
    int color; //fase que tenia la nave cuando disparo
    int duenio; //identificador reservado para el jugador que dispara
};

struct Nave { //guarda la ubicacion de la nave en su orbita y su fase
    double anguloActual; //angulo en grados donde esta la nave
    double anguloObjetivo; //angulo en grados hacia donde debe moverse
    int color; //fase activa de la nave entre cian magenta y ambar
};

struct Pulso { //guarda el avance de la onda que se expande desde el nucleo
    bool activo; //indica si el pulso esta en funcionamiento
    double radioActual; //distancia que ha alcanzado el pulso desde el centro
};

struct AccionMotor { //reune las acciones que el jugador o el bot le pide al motor
    bool cambiarAngulo; //indica si se debe cambiar el destino de la nave
    double anguloObjetivo; //nuevo destino de la nave medido en grados
    bool cambiarColor; //indica si se debe cambiar la fase de la nave
    int colorNuevo; //fase que se usara cuando se pida el cambio
    bool solicitarPulso; //pide activar el pulso si el jugador tiene carga suficiente
};

#endif