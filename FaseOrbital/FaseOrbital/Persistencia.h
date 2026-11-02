#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <string>
#include <ostream>
#include "HistorialPartidas.h"

using namespace std;

//devuelve el encabezado que debe tener el historial principal
const char* encabezadoHistorialCSV();

//agrega un solo registro al archivo principal
bool agregarRegistroCSV(const string& ruta, const RegistroPartida& registro);

//agrega todos los registros recibidos al archivo principal
bool agregarHistorialCSV(const string& ruta, const HistorialPartidas& historial);

//carga los registros validos e informa las filas con errores
bool cargarHistorialCSV(const string& ruta, HistorialPartidas& historial, ostream& errores);

//obtiene el nombre del archivo que guarda las metricas adicionales
string rutaDetalleHistorialCSV(const string& rutaHistorial);

//agrega el detalle de los registros recibidos
bool agregarDetalleHistorialCSV(const string& rutaHistorial, const HistorialPartidas& historial);

//carga el detalle y lo relaciona con el historial principal
bool cargarDetalleHistorialCSV(const string& rutaHistorial, HistorialPartidas& historial, ostream& errores);

//guarda los dos archivos y conserva los anteriores si ocurre un error controlado
bool guardarPartidaCompletaCSV(const string& rutaHistorial, const HistorialPartidas& nuevosResultados, ostream& errores);

#endif