#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <string>
#include <ostream>
#include "HistorialPartidas.h"

const char* encabezadoHistorialCSV();

bool agregarRegistroCSV(const std::string& ruta, const RegistroPartida& registro);
bool agregarHistorialCSV(const std::string& ruta, const HistorialPartidas& historial);
bool cargarHistorialCSV(const std::string& ruta, HistorialPartidas& historial, std::ostream& errores);

std::string rutaDetalleHistorialCSV(const std::string& rutaHistorial);
bool agregarDetalleHistorialCSV(const std::string& rutaHistorial, const HistorialPartidas& historial);
bool cargarDetalleHistorialCSV(const std::string& rutaHistorial, HistorialPartidas& historial, std::ostream& errores);

#endif