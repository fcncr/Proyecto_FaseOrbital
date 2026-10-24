#pragma once
#ifndef HISTORIAL_PARTIDAS_H
#define HISTORIAL_PARTIDAS_H

#include <string>
#include "ModelosRegistros.h" //llama a struct "RegistroPartida"

struct NodoRegistroPartida {
    RegistroPartida dato;
    NodoRegistroPartida* sig;
};

struct HistorialPartidas {
    NodoRegistroPartida* cabeza;
    NodoRegistroPartida* cola;
    int tam;
};

void inicializarHistorial(HistorialPartidas& historial);
void agregarRegistroFinal(HistorialPartidas& historial, const RegistroPartida& registro);
void recorrerHistorial(const HistorialPartidas& historial, void (*procesar)(const RegistroPartida&));
int contarRegistros(const HistorialPartidas& historial);
void buscarRegistrosJugador(const HistorialPartidas& origen, const std::string& jugador, HistorialPartidas& destino);
void destruirHistorial(HistorialPartidas& historial);

#endif