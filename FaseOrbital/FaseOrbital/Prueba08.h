#ifndef PRUEBA_08_H
#define PRUEBA_08_H

#include "ScriptAcciones.h"

//carga el archivo de acciones de p08 y exige al menos una accion antes de comprobar la repeticion de la partida
bool pruebaP08();

//ejecuta dos partidas con la misma semilla y el script recibido y compara sus estados y reportes
bool pruebaP08ConScript(const ScriptAcciones& script);

#endif