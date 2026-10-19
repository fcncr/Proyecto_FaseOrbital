#ifndef RANKING_H
#define RANKING_H

#include <string>
#include "HistorialPartidas.h"

struct RegistroRanking {
    std::string alias;
    int mejorPuntaje;
    double precisionColor;
};

struct NodoRanking {
    RegistroRanking dato;
    NodoRanking* sig;
};

struct Ranking {
    NodoRanking* cabeza;
    int tam;
};

void inicializarRanking(Ranking& ranking);
void insertarOrdenadoRanking(Ranking& ranking, const RegistroRanking& registro);
const RegistroRanking* buscarJugadorRanking(const Ranking& ranking, const std::string& alias);
const RegistroRanking* obtenerPosicionRanking(const Ranking& ranking, int posicion);
void recorrerTopRanking(const Ranking& ranking, int k, void (*procesar)(const RegistroRanking&, int posicion));
void construirRankingDesdeHistorial(const HistorialPartidas& historial, Ranking& ranking);
void destruirRanking(Ranking& ranking);

#endif