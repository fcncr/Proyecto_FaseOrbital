#ifndef ESTADISTICAS_H
#define ESTADISTICAS_H

#include <string>
#include <ostream>
#include "HistorialPartidas.h"
#include "BitacoraEventos.h"

struct HistogramaPuntajes {
    double minimo;
    double maximo;
    double anchoIntervalo;
    int frecuencias[10];
    int total;
};

struct EstadisticasJugador {
    std::string alias;
    int partidasJugadas;
    int victorias;
    int mejorPuntaje;
    int peorPuntaje;
    double mediaPuntaje;
    double medianaPuntaje;
    double desviacionEstandar;
    double precisionColorPromedio;

    bool mejorColorDisponible;
    int mejorColor;

    bool tipoMasDanoDisponible;
    int tipoMasDano;

    HistogramaPuntajes histograma;
};

struct ResumenOleadaJugador {
    int oleada;
    int bajas;
    int danoRecibido;
    int puntajeOleada;
    int puntajeAcumulado;
};

bool calcularEstadisticasJugador(const HistorialPartidas& historial, const std::string& alias, EstadisticasJugador& estadisticas);
void imprimirHistogramaASCII(const HistogramaPuntajes& histograma, std::ostream& salida);
bool calcularResumenOleadaJugador(const BitacoraEventos& bitacora, const std::string& jugador, int oleada, ResumenOleadaJugador& resumen);
void imprimirLineaTiempoJugador(const BitacoraEventos& bitacora, const std::string& jugador, int ultimaOleada, std::ostream& salida);

#endif