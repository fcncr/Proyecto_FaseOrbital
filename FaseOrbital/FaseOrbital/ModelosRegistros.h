#pragma once
#ifndef MODELOS_REGISTROS_H
#define MODELOS_REGISTROS_H
#include <cstdint>
#include <string>

using namespace std; //permite usar string y uint32_t sin escribir el prefijo

enum TipoEvento { //situaciones que se pueden guardar en la bitacora
    BAJA, //un enemigo fue eliminado
    IMPACTO_ERRONEO, //una bala golpeo un enemigo de otro color
    DANO_NUCLEO, //un enemigo llego al nucleo y le causo dano
    CHOQUE_NAVE, //un enemigo choco con la nave y causo dano al nucleo
    PULSO, //el jugador activo un pulso
    FIN_OLEADA, //se termino la oleada del jugador
    ELIMINACION //el jugador quedo fuera de la partida
};

struct Evento { //guarda lo que ocurrio en un momento de la partida
    int tick; //tick del turno en que ocurrio el evento
    int oleada; //numero de la oleada que se estaba jugando
    string jugador; //alias del jugador relacionado con el evento
    TipoEvento tipoEvento; //indica que fue lo que ocurrio
    int tipoEnemigo; //tipo del enemigo o menos uno cuando no corresponde
    int color; //color relacionado con el evento o menos uno cuando no corresponde
    int valor; //puntos ganados o dano recibido y cero para los otros eventos
    double r = -1.0; //distancia al centro donde ocurrio una baja o menos uno si no corresponde
    double theta = 0.0; //angulo donde ocurrio una baja o cero si no corresponde
};

struct RegistroPartida { //guarda el resultado de un jugador en una partida para el historial
    int idPartida; //identificador de la partida a la que pertenece el resultado
    uint32_t semilla; //valor inicial usado para generar las oleadas de esa partida
    string modo; //modo en que se jugo la partida
    string jugador; //alias del jugador al que pertenece el resultado
    bool esBot; //indica si el jugador fue controlado por el programa
    int posicion; //puesto final del jugador dentro de la partida
    int puntaje; //puntos que acumulo al terminar la partida
    int oleada; //ultima oleada que llego a iniciar
    int disparos; //cantidad de balas que disparo
    int correctos; //impactos de balas del mismo color que el enemigo
    int erroneos; //impactos de balas de un color distinto al enemigo
    int comboMax; //mayor combo que alcanzo durante la partida
    int pulsos; //cantidad de pulsos que activo
    int danoTotal; //vida que perdio el nucleo por los choques recibidos
    int ticks; //ticks jugados sumando todos sus turnos
    int bajasDron; //drones eliminados durante la partida
    int bajasGirador; //giradores eliminados durante la partida
    int bajasDivisor; //divisores eliminados durante la partida
    int bajasMini; //minis eliminados durante la partida
    int bajasTanque; //tanques eliminados durante la partida
    int bajasCamaleon; //camaleones eliminados durante la partida

    bool detalleDisponible; //indica si se cuenta con impactos por color y dano por tipo

    int correctosC; //impactos correctos de balas cian
    int erroneosC; //impactos erroneos de balas cian
    int correctosM; //impactos correctos de balas magenta
    int erroneosM; //impactos erroneos de balas magenta
    int correctosA; //impactos correctos de balas ambar
    int erroneosA; //impactos erroneos de balas ambar

    int danoDron; //vida perdida por choques causados por drones
    int danoGirador; //vida perdida por choques causados por giradores
    int danoDivisor; //vida perdida por choques causados por divisores
    int danoMini; //vida perdida por choques causados por minis
    int danoTanque; //vida perdida por choques causados por tanques
    int danoCamaleon; //vida perdida por choques causados por camaleones
};

#endif
