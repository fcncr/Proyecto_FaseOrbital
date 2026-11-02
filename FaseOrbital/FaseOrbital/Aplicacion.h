#ifndef APLICACION_H
#define APLICACION_H

#include <string>
#include "ArgumentosCLI.h"

using namespace std; //permite usar string sin escribir el prefijo

//ejecuta el modo de una configuracion ya validada y devuelve cero si termina bien o uno si falla
int ejecutarAplicacion(const ConfiguracionPrograma& config);

//administra el menu las partidas y el historial del modo grafico hasta que se solicite salir
//guarda las partidas finalizadas y devuelve cero al salir normalmente o uno si ocurre un error
int ejecutarAplicacionGrafica(const ConfiguracionPrograma& config);

//simula una partida sin ventana guarda sus resultados y muestra los reportes por consola
//devuelve cero al terminar o uno si falla el registro la carga la simulacion o el guardado
int ejecutarAplicacionLotes(const ConfiguracionPrograma& config);

//carga el historial y su detalle y muestra las estadisticas por alias siguiendo el orden del ranking
//devuelve cero si termina la consulta o uno si falla la lectura
int ejecutarAplicacionEstadisticas(const string& archivo);

//carga el historial principal y muestra hasta diez alias ordenados por mejor puntaje precision promedio y alias
//devuelve cero si termina la consulta o uno si falla la lectura
int ejecutarAplicacionRanking(const string& archivo);

#endif
