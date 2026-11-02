#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <string>
#include <ostream>
#include "HistorialPartidas.h"

using namespace std; //permite usar string y ostream sin escribir el prefijo

//devuelve el encabezado que debe tener el historial principal
const char* encabezadoHistorialCSV();

//agrega un registro al archivo principal y comprueba la escritura y el cierre sin crear respaldos
bool agregarRegistroCSV(const string& ruta, const RegistroPartida& registro);

//agrega los registros en el orden de la lista y puede dejar filas ya escritas si una posterior falla
bool agregarHistorialCSV(const string& ruta, const HistorialPartidas& historial);

//reemplaza un historial inicializado por las filas validas e informa las que omite
//conserva el historial anterior si no puede abrir el archivo o el encabezado no es valido
bool cargarHistorialCSV(const string& ruta, HistorialPartidas& historial, ostream& errores);

//obtiene el nombre del archivo que guarda las metricas adicionales
string rutaDetalleHistorialCSV(const string& rutaHistorial);

//agrega solo los registros con detalle y no crea el archivo auxiliar si ninguno lo tiene
bool agregarDetalleHistorialCSV(const string& rutaHistorial, const HistorialPartidas& historial);

//completa los registros sin detalle buscando coincidencia de partida alias y posicion
//acepta que el archivo auxiliar no se pueda abrir para permitir historiales sin detalle
bool cargarDetalleHistorialCSV(const string& rutaHistorial, HistorialPartidas& historial, ostream& errores);

//prepara ambos archivos y utiliza respaldos para intentar recuperar los anteriores si falla el reemplazo
//exige resultados con detalle de una misma partida nueva e informa los errores de guardado o recuperacion
bool guardarPartidaCompletaCSV(const string& rutaHistorial, const HistorialPartidas& nuevosResultados, ostream& errores);

#endif
