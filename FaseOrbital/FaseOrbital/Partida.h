#ifndef PARTIDA_H
#define PARTIDA_H

#include <cstdint>
#include <string>
#include "ModelosJugador.h"
#include "RondaJugadores.h"
#include "ColaSpawn.h"
#include "ModoLotes.h"
#include "BitacoraEventos.h"
#include "HistorialPartidas.h"

using namespace std; //permite usar string y los tipos estandar sin el prefijo

struct Partida { //reune los jugadores y la informacion compartida durante el juego
    Jugador jugadores[6]; //conserva los datos de hasta seis participantes incluidos los eliminados
    int cantidadJugadores; //cuenta todos los participantes registrados en la partida
    RondaJugadores ronda; //enlaza los jugadores activos y guarda a quien le toca jugar
    int numeroRonda; //indica la oleada que les corresponde jugar en esta ronda
    uint32_t semillaPartida; //permite reproducir las mismas oleadas al usar otra vez este valor
    int limiteOleadas; //cantidad maxima de rondas que puede alcanzar la partida
    bool finalizada; //indica si la partida ya termino
    ColaSpawn colaOriginalRonda; //guarda las apariciones que se copiaran para cada jugador
    BitacoraEventos bitacora; //reune los eventos registrados durante los turnos
};

//prepara una partida para su primer uso sin liberar los nodos de una partida anterior
void inicializarPartida(Partida& partida, uint32_t semillaPartida, int limiteOleadas);

//registra un jugador con sus valores iniciales y devuelve falso si ya hay seis participantes
bool agregarJugadorPartida(Partida& partida, const string& alias, bool esBot);

//devuelve el jugador de la posicion indicada para poder modificarlo o nullptr si el indice no es valido
Jugador* verJugadorPartida(Partida& partida, int indice);

//permite consultar un jugador sin modificarlo y devuelve nullptr si el indice no es valido
const Jugador* verJugadorPartida(const Partida& partida, int indice);

//devuelve el jugador que tiene el turno o nullptr cuando la ronda esta vacia
Jugador* verJugadorActualPartida(Partida& partida);

//devuelve cuantos participantes se registraron incluidos los que ya fueron eliminados
int cantidadJugadoresPartida(const Partida& partida);

//devuelve cuantos jugadores siguen enlazados en la ronda de turnos
int cantidadJugadoresActivos(const Partida& partida);

//libera los nodos de la ronda la cola y la bitacora y marca la partida como finalizada
void destruirPartida(Partida& partida);

//genera las apariciones de la ronda actual usando la semilla de la partida
void generarColaOriginalRonda(Partida& partida);

//reemplaza la cola del jugador por una copia de la original con nodos independientes
void prepararColaJugador(const Partida& partida, ColaSpawn& colaJugador);

//indica que las rondas impares avanzan al siguiente jugador y las pares al anterior
bool sentidoHorarioRondaPartida(const Partida& partida);

//pasa el turno al jugador vecino segun el sentido de la ronda
void avanzarTurnoPartida(Partida& partida);

//cierra la partida si queda como maximo un jugador o se alcanzo el limite y en otro caso aumenta la ronda
void finalizarRondaPartida(Partida& partida);

//retira al jugador actual si no tiene vida y devuelve verdadero cuando lo retiro de la ronda
bool retirarJugadorActualSiEliminado(Partida& partida);

//busca entre todos los participantes por puntaje oleada y precision opcional y devuelve nullptr si no hay ganador unico
const Jugador* determinarGanadorPartida(const Partida& partida, const double* precisiones);

//simula el turno con una copia de las apariciones y devuelve falso si no puede jugar o no termina dentro del limite
bool ejecutarTurnoActualModoLotes(Partida& partida, const ScriptAcciones& script, int limiteTicks, ResultadoModoLotes& resultado);

//ejecuta los turnos de la ronda y su cierre o devuelve falso si no puede comenzar o algun turno no termina
bool ejecutarRondaModoLotes(Partida& partida, const ScriptAcciones& script, int limiteTicks);

//actualiza las proporciones y asigna puestos compartidos cuando los resultados estan empatados
void actualizarMetricasFinalesPartida(Partida& partida);

//copia el resultado de un jugador al registro sin recalcular sus metricas y devuelve falso si el indice no es valido
bool crearRegistroJugadorPartida(const Partida& partida, int indice, int idPartida, const string& modo, RegistroPartida& registro);

//actualiza las metricas finales y agrega al historial un registro por cada participante
void agregarResultadosPartidaHistorial(Partida& partida, int idPartida, const string& modo, HistorialPartidas& historial);

#endif