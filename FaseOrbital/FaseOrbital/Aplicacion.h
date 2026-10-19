#ifndef APLICACION_H
#define APLICACION_H

#include <string>
#include "ArgumentosCLI.h"

int ejecutarAplicacion(const ConfiguracionPrograma& config);

int ejecutarAplicacionGrafica(const ConfiguracionPrograma& config);

int ejecutarAplicacionLotes(const ConfiguracionPrograma& config);

int ejecutarAplicacionEstadisticas(const std::string& archivo);

int ejecutarAplicacionRanking(const std::string& archivo);

#endif