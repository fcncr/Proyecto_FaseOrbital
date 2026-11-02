#include "Generador.h"

//guarda la semilla para que la secuencia comience siempre desde el mismo estado
void inicializarGenerador(Generador& generador, uint32_t semilla) {
    generador.estado = semilla; //guarda el valor desde el que empezaran los calculos
}

//aplica la formula congruencial conserva los treinta y un bits inferiores y devuelve el nuevo estado
uint32_t siguienteNumero(Generador& generador) {
    generador.estado = (1103515245u * generador.estado + 12345u) & 0x7FFFFFFFu; //calcula el nuevo estado con constantes sin signo y conserva los 31 bits inferiores
    return generador.estado; //devuelve el numero que se acaba de calcular
}
