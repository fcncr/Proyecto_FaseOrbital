#ifndef ARGUMENTOS_CLI_H
#define ARGUMENTOS_CLI_H

#include <string>
#include <ostream>

enum ModoPrograma {
    MODO_GRAFICO,
    MODO_LOTES,
    MODO_ESTADISTICAS,
    MODO_RANKING,
    MODO_PRUEBAS,
    MODO_INVALIDO
};

struct ConfiguracionPrograma {
    ModoPrograma modo;

    std::string jugadores[6];
    int cantidadJugadores;

    unsigned int semilla;
    int oleadas;

    std::string rutaScript;
    bool tieneScript;

    std::string rutaHistorial;
};

void inicializarConfiguracionPrograma(ConfiguracionPrograma& config);
bool parsearArgumentos(int argc, char* argv[], ConfiguracionPrograma& config);
void imprimirUso(std::ostream& salida);

#endif