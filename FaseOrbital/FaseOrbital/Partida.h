#ifndef PARTIDA_H
#define PARTIDA_H

#include <cstdint>
#include <string>
#include "Jugador.h"
#include "RondaJugadores.h"
#include "ColaSpawn.h"
#include "ModoLotes.h"
#include "BitacoraEventos.h"
#include "HistorialPartidas.h"

struct Partida {
    Jugador jugadores[6];
    int cantidadJugadores;
    RondaJugadores ronda;
    int numeroRonda;
    uint32_t semillaPartida;
    int limiteOleadas;
    bool finalizada;
    ColaSpawn colaOriginalRonda;
    BitacoraEventos bitacora;
};

void inicializarPartida(Partida& partida, uint32_t semillaPartida, int limiteOleadas);
bool agregarJugadorPartida(Partida& partida, const std::string& alias, bool esBot);
Jugador* verJugadorPartida(Partida& partida, int indice);
const Jugador* verJugadorPartida(const Partida& partida, int indice);
Jugador* verJugadorActualPartida(Partida& partida);
int cantidadJugadoresPartida(const Partida& partida);
int cantidadJugadoresActivos(const Partida& partida);
void destruirPartida(Partida& partida);

void generarColaOriginalRonda(Partida& partida);
void prepararColaJugador(const Partida& partida, ColaSpawn& colaJugador);

bool sentidoHorarioRondaPartida(const Partida& partida);
void avanzarTurnoPartida(Partida& partida);
void finalizarRondaPartida(Partida& partida);

bool retirarJugadorActualSiEliminado(Partida& partida);
const Jugador* determinarGanadorPartida(const Partida& partida, const double* precisiones);

bool ejecutarTurnoActualModoLotes(Partida& partida, const ScriptAcciones& script, int limiteTicks, ResultadoModoLotes& resultado);
bool ejecutarRondaModoLotes(Partida& partida, const ScriptAcciones& script, int limiteTicks);

void actualizarMetricasFinalesPartida(Partida& partida);
bool crearRegistroJugadorPartida(const Partida& partida, int indice, int idPartida, const std::string& modo, RegistroPartida& registro);
void agregarResultadosPartidaHistorial(Partida& partida, int idPartida, const std::string& modo, HistorialPartidas& historial);

#endif