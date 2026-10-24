#pragma once
#ifndef MODELOS_JUGADOR_H
#define MODELOS_JUGADOR_H

#include <string>

using namespace std;

struct MetricasJugador {
    int puntaje;
    int oleadaAlcanzada;

    int disparos;
    int impactosCorrectos;
    int impactosErroneos;

    int impactosCorrectosC;
    int impactosErroneosC;
    int impactosCorrectosM;
    int impactosErroneosM;
    int impactosCorrectosA;
    int impactosErroneosA;

    int bajasDron;
    int bajasGirador;
    int bajasDivisor;
    int bajasMini;
    int bajasTanque;
    int bajasCamaleon;

    int bajasColor0;
    int bajasColor1;
    int bajasColor2;

    int comboMaximo;

    int danoTotal;
    int danoDron;
    int danoGirador;
    int danoDivisor;
    int danoMini;
    int danoTanque;
    int danoCamaleon;

    int pulsosUtilizados;
    int ticksSobrevividos;
    int posicionFinal;

    double precisionColor;
    double eficienciaDisparo;
    double precisionC;
    double precisionM;
    double precisionA;
};

//el inline es necesario. Estamos creando una funcion en un header, y si se llama en otros cpp, podria causar definicion multiple
inline void inicializarMetricasJugador(MetricasJugador& metricas) {
    metricas.puntaje = 0;
    metricas.oleadaAlcanzada = 0;
    metricas.disparos = 0;
    metricas.impactosCorrectos = 0;
    metricas.impactosErroneos = 0;
    metricas.impactosCorrectosC = 0;
    metricas.impactosErroneosC = 0;
    metricas.impactosCorrectosM = 0;
    metricas.impactosErroneosM = 0;
    metricas.impactosCorrectosA = 0;
    metricas.impactosErroneosA = 0;
    metricas.bajasDron = 0;
    metricas.bajasGirador = 0;
    metricas.bajasDivisor = 0;
    metricas.bajasMini = 0;
    metricas.bajasTanque = 0;
    metricas.bajasCamaleon = 0;
    metricas.bajasColor0 = 0;
    metricas.bajasColor1 = 0;
    metricas.bajasColor2 = 0;
    metricas.comboMaximo = 0;
    metricas.danoTotal = 0;
    metricas.danoDron = 0;
    metricas.danoGirador = 0;
    metricas.danoDivisor = 0;
    metricas.danoMini = 0;
    metricas.danoTanque = 0;
    metricas.danoCamaleon = 0;
    metricas.pulsosUtilizados = 0;
    metricas.ticksSobrevividos = 0;
    metricas.posicionFinal = 0;
    metricas.precisionColor = 0.0;
    metricas.eficienciaDisparo = 0.0;
    metricas.precisionC = 0.0;
    metricas.precisionM = 0.0;
    metricas.precisionA = 0.0;
}

inline double divisionSeguraMetricas(int numerador, int denominador) {
    if (denominador <= 0) {
        return 0.0;
    }

    return static_cast<double>(numerador) / static_cast<double>(denominador);
}

inline void actualizarMetricasCalculadas(MetricasJugador& metricas) {
    metricas.precisionColor = divisionSeguraMetricas(metricas.impactosCorrectos, metricas.impactosCorrectos + metricas.impactosErroneos);
    metricas.eficienciaDisparo = divisionSeguraMetricas(metricas.impactosCorrectos, metricas.disparos);
    metricas.precisionC = divisionSeguraMetricas(metricas.impactosCorrectosC, metricas.impactosCorrectosC + metricas.impactosErroneosC);
    metricas.precisionM = divisionSeguraMetricas(metricas.impactosCorrectosM, metricas.impactosCorrectosM + metricas.impactosErroneosM);
    metricas.precisionA = divisionSeguraMetricas(metricas.impactosCorrectosA, metricas.impactosCorrectosA + metricas.impactosErroneosA);
}


struct Jugador {
    string alias;
    bool esBot;
    int vidaNucleo;
    int puntaje;
    int combo;
    int cargaPulso;
    int oleadaAlcanzada;
    bool eliminado;
    MetricasJugador metricas;
};

#endif