#ifndef REPORTE_PARTIDA_H
#define REPORTE_PARTIDA_H

#include <ostream>
#include <string>
#include "Partida.h"

void imprimirReportePartida(const Partida& partida, std::ostream& salida);
bool guardarReportePartida(const Partida& partida, const std::string& ruta);
bool compararArchivosExactos(const std::string& rutaA, const std::string& rutaB);

#endif