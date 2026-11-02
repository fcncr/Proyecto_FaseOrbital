#pragma once
#ifndef PRUEBAS_H
#define PRUEBAS_H

//comprueba el orden de salida de una cola y su estado al retirar todas las apariciones
bool pruebaP05();

//revisa las copias por constructor y asignacion su independencia y la autoasignacion de una cola
bool pruebaP06();

//comprueba los recorridos y las eliminaciones de una ronda circular en ambos sentidos hasta dejarla vacia
bool pruebaP07();

//verifica el primer valor conocido de la semilla cuarenta y dos y compara las siguientes cincuenta salidas
bool pruebaGeneradorDeterminista();

//vacia la cola de un turno y revisa que la cola original y la del otro turno se conserven
bool pruebaCopiasIndependientesPartida();

//comprueba el orden de los turnos en las dos primeras rondas y el inicio de la tercera
bool pruebaOrdenSerpenteantePartida();

//revisa que los eliminados conserven sus resultados y que la partida termine al quedar cero activos o uno al cerrar la ronda
//tambien comprueba que alcanzar el limite de oleadas termine la partida
bool pruebaEliminacionesPartida();

//comprueba la eleccion por puntaje los desempates por oleada y precision y el empate sin precision disponible
bool pruebaGanadorPartida();

//compara la accion del modo por lotes con la del script vacio para un humano y con la del bot ante un enemigo
bool pruebaIntegracionModoLotes();

//comprueba el orden de insercion los filtros por jugador y oleada y el vaciado de la bitacora
bool pruebaBitacoraEventos();

#endif
