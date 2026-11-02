#pragma once
#ifndef HISTORIAL_PARTIDAS_H
#define HISTORIAL_PARTIDAS_H

#include <string>
#include "ModelosRegistros.h"

struct NodoRegistroPartida { //guarda un resultado y permite llegar al siguiente registro
    RegistroPartida dato; //guarda los resultados de un jugador en una partida
    NodoRegistroPartida* sig; //apunta al siguiente nodo o a nullptr si no hay mas
};

struct HistorialPartidas { //mantiene los registros en el orden en que se agregaron
    NodoRegistroPartida* cabeza; //permite llegar al primer registro de la lista
    NodoRegistroPartida* cola; //permite agregar registros al final sin recorrer toda la lista
    int tam; //cuenta los registros guardados y no las partidas distintas
};

void inicializarHistorial(HistorialPartidas& historial); //prepara el historial para su primer uso sin liberar nodos anteriores
void agregarRegistroFinal(HistorialPartidas& historial, const RegistroPartida& registro); //agrega una copia del registro al final del historial
void recorrerHistorial(const HistorialPartidas& historial, void (*procesar)(const RegistroPartida&)); //usa la funcion recibida para revisar cada registro
int contarRegistros(const HistorialPartidas& historial); //devuelve cuantos registros hay en la lista
void buscarRegistrosJugador(const HistorialPartidas& origen, const std::string& jugador, HistorialPartidas& destino); //deja en destino solo los registros del jugador indicado
void destruirHistorial(HistorialPartidas& historial); //libera todos los nodos y deja el historial vacio

#endif