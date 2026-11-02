#ifndef APLICACION_H
#define APLICACION_H

#include <string>
#include "ArgumentosCLI.h"

using namespace std;

//selecciona el modo de ejecucion indicado en la configuracion
int ejecutarAplicacion(const ConfiguracionPrograma& config);

//administra las partidas y el historial del modo grafico
int ejecutarAplicacionGrafica(const ConfiguracionPrograma& config);

//ejecuta una partida completa sin abrir una ventana
int ejecutarAplicacionLotes(const ConfiguracionPrograma& config);

//muestra las estadisticas del historial indicado
int ejecutarAplicacionEstadisticas(const string& archivo);

//muestra los primeros diez jugadores del ranking
int ejecutarAplicacionRanking(const string& archivo);

#endif