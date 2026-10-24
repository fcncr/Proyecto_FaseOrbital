#pragma once
#ifndef MODELOS_COMBATE_H
#define MODELOS_COMBATE_H

//TIPO ENEMIGO
enum TipoEnemigo { Dron, Girador, Divisor, Mini, Tanque, Camaleon };

//SPAWN
struct Spawn {
    int tick;
    TipoEnemigo tipo;
    int color;
    int angulo;
    int sentido;
};

//ENEMIGO
struct Enemigo {
    int id;
    TipoEnemigo tipo;
    int color;
    double r;
    double theta;
    double omega;
    int vida;
    int radio;
    int ticksColor;
};

//BALA
struct Bala {
    double x;
    double y;
    double vx;
    double vy;
    int color;
    int duenio;
};

//NAVE
struct Nave {
    double anguloActual;
    double anguloObjetivo;
    int color;
};

//PULSO
struct Pulso {
    bool activo;
    double radioActual;
};

//AccionMotor
struct AccionMotor {
    bool cambiarAngulo;
    double anguloObjetivo;
    bool cambiarColor;
    int colorNuevo;
    bool solicitarPulso;
};

#endif