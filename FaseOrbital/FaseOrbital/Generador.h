#ifndef GENERADOR_H
#define GENERADOR_H

#include <cstdint>

struct Generador {
    uint32_t estado;
};

void inicializarGenerador(Generador& generador, uint32_t semilla);
uint32_t siguienteNumero(Generador& generador);

#endif