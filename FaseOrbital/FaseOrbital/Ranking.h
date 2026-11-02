#ifndef RANKING_H
#define RANKING_H

#include <string>
#include "HistorialPartidas.h"

using namespace std; //permite usar string sin escribir el prefijo

struct RegistroRanking { //guarda los datos que determinan el puesto de un jugador
    string alias; //identifica al jugador y resuelve el ultimo desempate
    int mejorPuntaje; //mayor puntaje que obtuvo en sus partidas
    double precisionColor; //promedio de sus precisiones por partida entre cero y uno
};

struct NodoRanking { //une el resultado de un jugador con el siguiente puesto
    RegistroRanking dato; //datos del jugador que ocupa este nodo
    NodoRanking* sig; //siguiente jugador o nullptr si es el ultimo
};

struct Ranking { //mantiene una lista ordenada con un registro por alias
    NodoRanking* cabeza; //jugador que ocupa el primer puesto
    int tam; //cantidad de jugadores incluidos en la lista

    Ranking(); //crea una estructura vacia
    ~Ranking(); //libera los nodos al terminar su uso
    Ranking(const Ranking&) = delete; //se usa por referencia para no compartir nodos
    Ranking& operator=(const Ranking&) = delete; //cada estructura conserva sus propios nodos
};

//libera los registros anteriores y deja el ranking vacio
void inicializarRanking(Ranking& ranking);

//inserta un registro independiente del ranking y reemplaza el del mismo alias si ya existe
//ordena por mayor puntaje luego mayor precision y finalmente alias ascendente
void insertarOrdenadoRanking(Ranking& ranking, const RegistroRanking& registro);

//devuelve el registro del alias dentro del ranking o nullptr si no existe sin transferir su propiedad
const RegistroRanking* buscarJugadorRanking(const Ranking& ranking, const string& alias);

//devuelve el registro del puesto indicado contando desde uno o nullptr si no existe sin transferir su propiedad
const RegistroRanking* obtenerPosicionRanking(const Ranking& ranking, int posicion);

//llama a la funcion recibida para cada uno de los primeros k jugadores con su posicion desde uno
void recorrerTopRanking(const Ranking& ranking, int k, void (*procesar)(const RegistroRanking&, int posicion));

//reemplaza un ranking ya inicializado por los resultados de cada alias presente en el historial
void construirRankingDesdeHistorial(const HistorialPartidas& historial, Ranking& ranking);

//libera todos los nodos y deja el ranking vacio para reutilizarlo
void destruirRanking(Ranking& ranking);

#endif