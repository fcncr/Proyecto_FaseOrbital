#ifndef OLEADA_H
#define OLEADA_H

#include <cstdint>
#include "../FaseOrbital/ColaSpawn.h"

struct Oleada {
    int numero;
    uint32_t semillaPartida;
};

void generarOleada(const Oleada& oleada, ColaSpawn& cola);

#endif