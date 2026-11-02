#ifndef MODO_LOTES_H
#define MODO_LOTES_H

#include "Motor.h"
#include "ScriptAcciones.h"

struct ResultadoModoLotes { //guarda como quedo el turno al detener la simulacion
    int ticksEjecutados; //cantidad de ticks procesados durante esta ejecucion
    int vidaNucleo; //vida que le queda al nucleo del jugador
    int puntaje; //puntaje acumulado del jugador hasta este momento
    int combo; //combo que conserva al detenerse la simulacion
    int cargaPulso; //carga que le queda disponible para activar un pulso
    bool oleadaTerminada; //indica si completo la oleada con el nucleo vivo
    bool turnoTerminado; //indica si el turno termino por completar la oleada o perder la vida
};

//elige las acciones del bot o del script segun el jugador y no pide cambios cuando falta el jugador
AccionMotor obtenerAccionModoLotes(const Motor& motor, const ScriptAcciones& script);

//simula un turno sin interfaz grafica y devuelve verdadero cuando termina incluso si el jugador pierde
//si alcanza el limite sin terminar devuelve falso y conserva los cambios ya realizados en el jugador y la bitacora
bool ejecutarModoLotes(Jugador& jugador, int numeroOleada, const ColaSpawn& spawns, const ScriptAcciones& script, int limiteTicks, ResultadoModoLotes& resultado, BitacoraEventos* bitacora = nullptr);

#endif