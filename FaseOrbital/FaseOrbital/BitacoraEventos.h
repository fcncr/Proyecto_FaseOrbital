#pragma once
#ifndef BITACORA_EVENTOS_H
#define BITACORA_EVENTOS_H

#include "ModelosRegistros.h"

struct NodoEvento { //guarda un evento y permite llegar al siguiente
    Evento dato; //guarda lo que paso durante la partida
    NodoEvento* sig; //apunta al siguiente evento o a nullptr si no hay mas
};

struct BitacoraEventos { //mantiene los eventos en el orden en que se agregaron
    NodoEvento* cabeza; //permite llegar al primer evento de la lista
    NodoEvento* cola; //permite agregar eventos despues del ultimo sin recorrer la lista
    int tam; //guarda cuantos eventos hay en la bitacora
};

void inicializarBitacora(BitacoraEventos& bitacora); //prepara una bitacora para empezar a usarla sin borrar nodos anteriores
void agregarEventoFinal(BitacoraEventos& bitacora, const Evento& evento); //agrega una copia del evento al final de la lista
void recorrerBitacora(const BitacoraEventos& bitacora, void (*procesar)(const Evento&)); //usa la funcion recibida para revisar cada evento
void filtrarPorJugador(const BitacoraEventos& origen, const std::string& jugador, BitacoraEventos& destino); //deja en destino solo los eventos del jugador indicado
void filtrarPorOleada(const BitacoraEventos& origen, int oleada, BitacoraEventos& destino); //deja en destino solo los eventos de la oleada indicada
void destruirBitacora(BitacoraEventos& bitacora); //libera todos los nodos y deja la bitacora vacia

#endif