#ifndef MOTOR_H
#define MOTOR_H

#include "Nave.h"
#include "Pulso.h"
#include "AccionMotor.h"
#include "Jugador.h"
#include "ListaEnemigos.h"
#include "ListaBalas.h"
#include "ColaSpawn.h"

struct Motor {
    int tickActual;
    int numeroOleada;

    Nave nave;
    Pulso pulso;

    Jugador* jugador;

    ListaEnemigos enemigos;
    ListaBalas balas;
    ColaSpawn spawns;

    int siguienteIdEnemigo;

    bool oleadaTerminada;
    bool turnoTerminado;
};

void inicializarMotor(Motor& motor, Jugador* jugador, int numeroOleada, const ColaSpawn& spawns);
void destruirMotor(Motor& motor);

double normalizarAngulo(double angulo);
int calcularMultiplicador(int combo);
int puntosBaseEnemigo(TipoEnemigo tipo);

void aplicarAccion(Motor& motor, const AccionMotor& accion);
void actualizarNave(Motor& motor);

Enemigo crearEnemigoDesdeSpawn(Motor& motor, const Spawn& spawn);
void procesarSpawns(Motor& motor);
void moverEnemigos(Motor& motor);

void dispararAutomaticamente(Motor& motor);
void moverBalasYResolverImpactos(Motor& motor);

void resolverChoquesEnemigos(Motor& motor);

void actualizarPulso(Motor& motor);

void verificarFinMotor(Motor& motor);

void procesarTick(Motor& motor, const AccionMotor& accion);

#endif