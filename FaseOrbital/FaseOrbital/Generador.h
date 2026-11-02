#ifndef GENERADOR_H
#define GENERADOR_H

#include <cstdint>

using namespace std; //permite usar los tipos de la biblioteca estandar sin el prefijo

struct Generador { //guarda el estado necesario para producir la secuencia de numeros
    uint32_t estado; //entero sin signo de 32 bits usado para calcular el siguiente numero
};

//prepara el generador con la semilla desde la que comenzara la secuencia
void inicializarGenerador(Generador& generador, uint32_t semilla);

//aplica la formula congruencial conserva los treinta y un bits inferiores y devuelve el nuevo estado
uint32_t siguienteNumero(Generador& generador);

#endif
