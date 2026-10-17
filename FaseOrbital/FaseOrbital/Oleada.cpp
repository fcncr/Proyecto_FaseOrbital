#include "Oleada.h"
#include "Generador.h"

void generarOleada(const Oleada& oleada, ColaSpawn& cola) {
    uint32_t semillaOleada = oleada.semillaPartida * 1000u + static_cast<uint32_t>(oleada.numero);
    int cantidadApariciones = 6 + 3 * oleada.numero;

    Generador generador;
    inicializarGenerador(generador, semillaOleada);

    (void)cola; //variabes preparadas, pero aun no son utilizadas
    (void)cantidadApariciones;
}