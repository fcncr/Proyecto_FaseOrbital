#pragma once
#ifndef PRUEBAS_ESTADISTICAS_H
#define PRUEBAS_ESTADISTICAS_H

//comprueba el ordenamiento de puntajes y la mediana con listas vacias cortas ordenadas inversas y con repetidos
bool pruebaP12();

//revisa las estadisticas y el histograma por jugador con y sin detalle e incluye alias ausentes y puntajes iguales
bool pruebaEstadisticasHistoricas();

//comprueba el orden del ranking sus desempates las consultas y el reemplazo de un alias existente
//revisa tambien la clasificacion construida desde varias partidas de un historial
bool pruebaRanking();

//comprueba los resumenes por oleada y jugador y que la linea de tiempo incluya los puntajes acumulados
bool pruebaLineaTiempo();

//genera disparos impactos pulso y dano para comprobar las metricas los eventos y el registro final del jugador
//revisa que el historial conserve los resultados despues de destruir la partida
bool pruebaIntegracionEstadisticasMotor();

#endif
