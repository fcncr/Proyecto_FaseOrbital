#ifndef ESTADISTICAS_H
#define ESTADISTICAS_H

#include <string>
#include <ostream>
#include "HistorialPartidas.h"
#include "BitacoraEventos.h"

using namespace std; //permite usar string y ostream sin escribir el prefijo

struct HistogramaPuntajes { //agrupa los puntajes en diez intervalos
    double minimo; //menor puntaje encontrado en el historial del jugador
    double maximo; //mayor puntaje encontrado en el historial del jugador
    double anchoIntervalo; //distancia entre los limites de cada intervalo
    int frecuencias[10]; //cantidad de puntajes que pertenecen a cada intervalo
    int total; //cantidad de puntajes usados para construir el histograma
};

struct EstadisticasJugador { //resume los resultados historicos de un jugador
    string alias; //nombre usado para buscar sus partidas en el historial
    int partidasJugadas; //cantidad de registros que pertenecen al jugador
    int victorias; //partidas en las que termino en el primer puesto
    int mejorPuntaje; //mayor puntaje entre sus partidas
    int peorPuntaje; //menor puntaje entre sus partidas
    double mediaPuntaje; //promedio de los puntajes finales
    double medianaPuntaje; //valor central de los puntajes ordenados o promedio de los dos centrales
    double desviacionEstandar; //dispersion de los puntajes respecto a la media usando todas las partidas
    double precisionColorPromedio; //promedio de las precisiones por partida entre cero y uno

    bool mejorColorDisponible; //indica si hay impactos por color para comparar
    int mejorColor; //color con mayor precision o menos uno si no se pudo calcular

    bool tipoMasDanoDisponible; //indica si se registro dano positivo por tipo de enemigo
    int tipoMasDano; //tipo que mas dano causo o menos uno si no se pudo determinar

    HistogramaPuntajes histograma; //distribucion de sus puntajes finales
};

struct ResumenOleadaJugador { //reune los resultados de una oleada a partir de la bitacora
    int oleada; //numero de la oleada consultada
    int bajas; //enemigos eliminados en esa oleada
    int danoRecibido; //vida perdida por el nucleo en esa oleada
    int puntajeOleada; //puntos obtenidos por las bajas de esa oleada
    int puntajeAcumulado; //puntos obtenidos hasta esa oleada inclusive
};

//calcula las estadisticas del alias y devuelve falso si no tiene partidas en el historial
bool calcularEstadisticasJugador(const HistorialPartidas& historial, const string& alias, EstadisticasJugador& estadisticas);

//escribe los diez intervalos con un asterisco por puntaje o avisa si no hay datos
void imprimirHistogramaASCII(const HistogramaPuntajes& histograma, ostream& salida);

//resume las bajas y el dano de una oleada junto con los puntos acumulados y devuelve si encontro eventos en ella
bool calcularResumenOleadaJugador(const BitacoraEventos& bitacora, const string& jugador, int oleada, ResumenOleadaJugador& resumen);

//escribe las bajas el dano y los puntos acumulados desde la primera oleada hasta la indicada
void imprimirLineaTiempoJugador(const BitacoraEventos& bitacora, const string& jugador, int ultimaOleada, ostream& salida);

//devuelve el nombre del color o desconocido si el numero no corresponde a una fase
const char* nombreColorEstadistica(int color);

//devuelve el nombre del tipo de enemigo o desconocido si el numero no corresponde a ninguno
const char* nombreTipoEnemigoEstadistica(int tipo);

#endif
