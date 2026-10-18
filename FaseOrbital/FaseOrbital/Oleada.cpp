#include "Oleada.h"
#include "Generador.h"

void generarOleada(const Oleada& oleada, ColaSpawn& cola) {
    vaciarColaSpawn(cola);

    uint32_t semillaOleada = oleada.semillaPartida * 1000u + static_cast<uint32_t>(oleada.numero);
    int cantidadApariciones = 6 + 3 * oleada.numero;
    int intervaloBase = 35 - 2 * oleada.numero;

    if (intervaloBase < 9) {
        intervaloBase = 9;
    }

    TipoEnemigo tiposDisponibles[7] = { Dron, Dron, Girador, Divisor, Tanque, Camaleon, Girador };
    int cantidadTipos = 2;

    if (oleada.numero == 2) {
        cantidadTipos = 3;
    }
    else if (oleada.numero == 3) {
        cantidadTipos = 4;
    }
    else if (oleada.numero == 4) {
        cantidadTipos = 5;
    }
    else if (oleada.numero >= 5) {
        cantidadTipos = 7;
    }

    Generador generador;
    inicializarGenerador(generador, semillaOleada);

    int tickActual = 60;

    for (int i = 0; i < cantidadApariciones; i++) {
        Spawn spawn;
        spawn.tick = tickActual;

        uint32_t x = siguienteNumero(generador);
        spawn.tipo = tiposDisponibles[x % cantidadTipos];

        x = siguienteNumero(generador);
        spawn.color = static_cast<int>(x % 3u);

        x = siguienteNumero(generador);
        spawn.angulo = static_cast<int>(x % 360u);

        if (spawn.tipo == Girador) {
            x = siguienteNumero(generador);
            spawn.sentido = static_cast<int>(x % 2u);
        }
        else {
            spawn.sentido = 0;
        }

        encolarSpawn(cola, spawn);

        if (i < cantidadApariciones - 1) {
            x = siguienteNumero(generador);
            int intervaloReal = intervaloBase * (60 + static_cast<int>(x % 81u)) / 100;

            if (!(oleada.numero >= 7 && x % 4u == 0u)) {
                tickActual += intervaloReal;
            }
        }
    }
}