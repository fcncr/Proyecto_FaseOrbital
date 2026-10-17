#include "Generador.h"

void inicializarGenerador(Generador& generador, uint32_t semilla) {
    generador.estado = semilla;
}

//la u significa que las constantes son tratadas como valores "unsigned"
uint32_t siguienteNumero(Generador& generador) {
    generador.estado = (1103515245u * generador.estado + 12345u) & 0x7FFFFFFFu;
    return generador.estado;
}