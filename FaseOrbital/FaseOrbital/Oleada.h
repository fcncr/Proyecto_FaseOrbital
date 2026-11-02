#ifndef OLEADA_H
#define OLEADA_H

#include <cstdint>
#include "ColaSpawn.h"

using namespace std; //permite usar los tipos de la biblioteca estandar sin el prefijo

struct Oleada { //reune los datos necesarios para generar las apariciones de una oleada
    int numero; //numero de oleada que determina la cantidad de enemigos y las reglas de aparicion
    uint32_t semillaPartida; //semilla de la partida usada para obtener la semilla de esta oleada
};

//reemplaza el contenido de una cola ya inicializada con las apariciones calculadas para la oleada
//el numero y la semilla de partida determinan los tipos colores angulos y momentos de aparicion
void generarOleada(const Oleada& oleada, ColaSpawn& cola);

#endif
