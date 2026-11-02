#pragma once
#ifndef MODELOS_JUGADOR_H
#define MODELOS_JUGADOR_H

#include <string>

using namespace std; //permite usar string sin escribir std delante

struct MetricasJugador { //reune los resultados acumulados del jugador durante la partida
    int puntaje; //copia del puntaje del jugador para las estadisticas
    int oleadaAlcanzada; //ultima oleada que el jugador llego a iniciar

    int disparos; //cantidad de balas que ha disparado
    int impactosCorrectos; //impactos de balas del mismo color que el enemigo
    int impactosErroneos; //impactos de balas de un color distinto al enemigo

    int impactosCorrectosC; //impactos correctos de balas cian
    int impactosErroneosC; //impactos erroneos de balas cian
    int impactosCorrectosM; //impactos correctos de balas magenta
    int impactosErroneosM; //impactos erroneos de balas magenta
    int impactosCorrectosA; //impactos correctos de balas ambar
    int impactosErroneosA; //impactos erroneos de balas ambar

    int bajasDron; //drones eliminados con balas o pulsos
    int bajasGirador; //giradores eliminados con balas o pulsos
    int bajasDivisor; //divisores eliminados con balas o pulsos
    int bajasMini; //minis eliminados con balas o pulsos
    int bajasTanque; //tanques eliminados con balas o pulsos
    int bajasCamaleon; //camaleones eliminados con balas o pulsos

    int bajasColor0; //enemigos que eran cian cuando fueron eliminados
    int bajasColor1; //enemigos que eran magenta cuando fueron eliminados
    int bajasColor2; //enemigos que eran ambar cuando fueron eliminados

    int comboMaximo; //mayor combo que ha alcanzado el jugador

    int danoTotal; //vida que ha perdido el nucleo por los choques recibidos
    int danoDron; //vida perdida por choques causados por drones
    int danoGirador; //vida perdida por choques causados por giradores
    int danoDivisor; //vida perdida por choques causados por divisores
    int danoMini; //vida perdida por choques causados por minis
    int danoTanque; //vida perdida por choques causados por tanques
    int danoCamaleon; //vida perdida por choques causados por camaleones

    int pulsosUtilizados; //cantidad de pulsos que ha activado
    int ticksSobrevividos; //ticks jugados sumando todos sus turnos
    int posicionFinal; //puesto obtenido al terminar la partida

    double precisionColor; //proporcion de impactos correctos entre todos los impactos
    double eficienciaDisparo; //proporcion de impactos correctos respecto a las balas disparadas
    double precisionC; //proporcion de impactos correctos entre los impactos de balas cian
    double precisionM; //proporcion de impactos correctos entre los impactos de balas magenta
    double precisionA; //proporcion de impactos correctos entre los impactos de balas ambar
};

//reinicia todos los contadores y proporciones de las metricas recibidas
inline void inicializarMetricasJugador(MetricasJugador& metricas) {
    metricas = {}; //deja en cero todos los campos enteros y decimales de las metricas
}

//calcula una proporcion con decimales y devuelve cero si el total no es positivo
inline double divisionSeguraMetricas(int numerador, int denominador) {
    if (denominador <= 0) { //evita dividir entre cero o entre un total negativo
        return 0.0; //deja la proporcion en cero cuando no se puede calcular
    }

    return static_cast<double>(numerador) / static_cast<double>(denominador); //convierte a double para conservar los decimales de la division
}

//actualiza la precision general la eficiencia y las precisiones por color a partir de los conteos actuales
inline void actualizarMetricasCalculadas(MetricasJugador& metricas) {
    metricas.precisionColor = divisionSeguraMetricas(metricas.impactosCorrectos, metricas.impactosCorrectos + metricas.impactosErroneos); //compara los aciertos con todos los impactos
    metricas.eficienciaDisparo = divisionSeguraMetricas(metricas.impactosCorrectos, metricas.disparos); //compara los aciertos con todas las balas disparadas
    metricas.precisionC = divisionSeguraMetricas(metricas.impactosCorrectosC, metricas.impactosCorrectosC + metricas.impactosErroneosC); //calcula la precision usando solo impactos de balas cian
    metricas.precisionM = divisionSeguraMetricas(metricas.impactosCorrectosM, metricas.impactosCorrectosM + metricas.impactosErroneosM); //calcula la precision usando solo impactos de balas magenta
    metricas.precisionA = divisionSeguraMetricas(metricas.impactosCorrectosA, metricas.impactosCorrectosA + metricas.impactosErroneosA); //calcula la precision usando solo impactos de balas ambar
}


struct Jugador { //guarda el estado del participante entre sus turnos
    string alias; //nombre con el que aparece en la partida y en el historial
    bool esBot; //indica si sus acciones las decide el programa
    int vidaNucleo; //vida que le queda al nucleo de este jugador
    int puntaje; //puntos que lleva acumulados
    int combo; //combo que tiene en este momento
    int cargaPulso; //carga reunida para poder activar el pulso
    int oleadaAlcanzada; //ultima oleada que ha iniciado
    bool eliminado; //indica si ya quedo fuera de la partida
    MetricasJugador metricas; //conteos y resultados usados para las estadisticas
};

#endif
