#pragma once
#ifndef PRUEBAS_H
#define PRUEBAS_H

//comprueba el orden de salida de una cola y su estado al retirar todas las apariciones
bool pruebaP05();

//revisa las copias por constructor y asignacion su independencia y la autoasignacion
bool pruebaP06();

//comprueba los recorridos y las eliminaciones de la ronda circular
bool pruebaP07();

//verifica los valores y la repetibilidad del generador
bool pruebaGeneradorDeterminista();

//comprueba que cada jugador reciba una copia independiente de la cola
bool pruebaCopiasIndependientesPartida();

//comprueba el orden de los jugadores entre rondas consecutivas
bool pruebaOrdenSerpenteantePartida();

//comprueba la eliminacion de jugadores y las condiciones de finalizacion
bool pruebaEliminacionesPartida();

//comprueba la eleccion del ganador y sus criterios de desempate
bool pruebaGanadorPartida();

//comprueba las acciones del script y del bot en modo por lotes
bool pruebaIntegracionModoLotes();

//comprueba el almacenamiento los filtros y el vaciado de la bitacora
bool pruebaBitacoraEventos();

//comprueba los alias repetidos el limite de jugadores y su reutilizacion entre partidas
bool pruebaRegistroAliasUnicos();

//comprueba que los resultados individuales se separen y conserven su historial
bool pruebaEstadisticasAliasUnicos();

#endif