#include <iostream>

#include "ArgumentosCLI.h"
#include "Aplicacion.h"

int main(int argc, char* argv[]) {
    ConfiguracionPrograma config;

    if (!parsearArgumentos(argc, argv, config)) {
        std::cerr << "Argumentos invalidos." << '\n';
        std::cerr << '\n';

        imprimirUso(std::cerr);

        return 1;
    }

    return ejecutarAplicacion(config);
}