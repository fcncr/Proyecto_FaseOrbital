#ifndef REPORTE_PARTIDA_H
#define REPORTE_PARTIDA_H

#include <ostream>
#include <string>
#include "Partida.h"

using namespace std; //permite usar ostream y string sin escribir el prefijo

//escribe el estado de la partida y sus jugadores en el orden de registro sin recalcular las metricas
void imprimirReportePartida(const Partida& partida, ostream& salida);

//crea o reemplaza el archivo del reporte y devuelve falso si falla al abrir escribir o cerrar
bool guardarReportePartida(const Partida& partida, const string& ruta);

//compara los archivos byte por byte y devuelve falso si difieren o no se pueden leer por completo
bool compararArchivosExactos(const string& rutaA, const string& rutaB);

#endif
