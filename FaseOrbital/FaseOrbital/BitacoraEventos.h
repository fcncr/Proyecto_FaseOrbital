#pragma once
#ifndef BITACORA_EVENTOS_H
#define BITACORA_EVENTOS_H

#include "ModelosRegistros.h" //llama a struct "Evento"

struct NodoEvento {
    Evento dato;
    NodoEvento* sig;
};

struct BitacoraEventos {
    NodoEvento* cabeza;
    NodoEvento* cola;
    int tam;
};

void inicializarBitacora(BitacoraEventos& bitacora);
void agregarEventoFinal(BitacoraEventos& bitacora, const Evento& evento);
void recorrerBitacora(const BitacoraEventos& bitacora, void (*procesar)(const Evento&));
void filtrarPorJugador(const BitacoraEventos& origen, const std::string& jugador, BitacoraEventos& destino);
void filtrarPorOleada(const BitacoraEventos& origen, int oleada, BitacoraEventos& destino);
void destruirBitacora(BitacoraEventos& bitacora);

#endif