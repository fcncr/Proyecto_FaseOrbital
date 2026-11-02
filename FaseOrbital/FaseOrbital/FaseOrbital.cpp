#include <iostream>

#include "ArgumentosCLI.h"
#include "Aplicacion.h"

using namespace std;

int main(int argc, char* argv[]) {
    ConfiguracionPrograma config;

    if (!parsearArgumentos(argc, argv, config)) {
        cerr << "Argumentos invalidos." << '\n';
        cerr << '\n';

        imprimirUso(cerr);

        return 1;
    }

    return ejecutarAplicacion(config);
}