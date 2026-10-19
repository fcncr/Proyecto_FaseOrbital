#include "ArgumentosCLI.h"

#include <sstream>
#include <limits>

static std::string recortarTexto(const std::string& texto) {
    size_t inicio = 0;

    while (inicio < texto.size() &&
        (texto[inicio] == ' ' || texto[inicio] == '\t' ||
            texto[inicio] == '\r' || texto[inicio] == '\n')) {
        inicio++;
    }

    size_t fin = texto.size();

    while (fin > inicio &&
        (texto[fin - 1] == ' ' || texto[fin - 1] == '\t' ||
            texto[fin - 1] == '\r' || texto[fin - 1] == '\n')) {
        fin--;
    }

    return texto.substr(inicio, fin - inicio);
}

static bool pareceOpcion(const std::string& texto) {
    return texto.size() >= 2 &&
        texto[0] == '-' &&
        texto[1] == '-';
}

static bool hayValorDespues(int indice, int argc, char* argv[]) {
    if (indice + 1 >= argc) {
        return false;
    }

    std::string siguiente = argv[indice + 1];

    if (siguiente.empty()) {
        return false;
    }

    if (pareceOpcion(siguiente)) {
        return false;
    }

    return true;
}

static bool convertirSemilla(const std::string& texto, unsigned int& valor) {
    if (texto.empty()) {
        return false;
    }

    if (texto[0] == '-') {
        return false;
    }

    std::istringstream entrada(texto);

    unsigned long long temporal;
    char extra;

    if (!(entrada >> temporal)) {
        return false;
    }

    if (entrada >> extra) {
        return false;
    }

    if (temporal > std::numeric_limits<unsigned int>::max()) {
        return false;
    }

    valor = static_cast<unsigned int>(temporal);

    return true;
}

static bool convertirOleadas(const std::string& texto, int& valor) {
    if (texto.empty()) {
        return false;
    }

    std::istringstream entrada(texto);

    long long temporal;
    char extra;

    if (!(entrada >> temporal)) {
        return false;
    }

    if (entrada >> extra) {
        return false;
    }

    if (temporal <= 0 ||
        temporal > std::numeric_limits<int>::max()) {
        return false;
    }

    valor = static_cast<int>(temporal);

    return true;
}

static bool parsearListaJugadores(const std::string& texto, ConfiguracionPrograma& config) {
    config.cantidadJugadores = 0;

    if (texto.empty()) {
        return false;
    }

    size_t inicio = 0;

    while (inicio <= texto.size()) {
        size_t coma = texto.find(',', inicio);

        std::string alias;

        if (coma == std::string::npos) {
            alias = texto.substr(inicio);
        }
        else {
            alias = texto.substr(inicio, coma - inicio);
        }

        alias = recortarTexto(alias);

        if (alias.empty()) {
            return false;
        }

        if (config.cantidadJugadores >= 6) {
            return false;
        }

        config.jugadores[config.cantidadJugadores] = alias;
        config.cantidadJugadores++;

        if (coma == std::string::npos) {
            break;
        }

        inicio = coma + 1;
    }

    return config.cantidadJugadores > 0;
}

void inicializarConfiguracionPrograma(ConfiguracionPrograma& config) {
    config.modo = MODO_INVALIDO;

    for (int i = 0; i < 6; i++) {
        config.jugadores[i].clear();
    }

    config.cantidadJugadores = 0;

    config.semilla = 0;
    config.oleadas = 10;

    config.rutaScript.clear();
    config.tieneScript = false;

    config.rutaHistorial = "historial.csv";
}

static bool parsearModoLotes(int argc, char* argv[], ConfiguracionPrograma& config) {
    bool tieneJugadores = false;
    bool tieneSemilla = false;
    bool tieneOleadas = false;
    bool tieneScript = false;

    config.modo = MODO_LOTES;

    for (int i = 2; i < argc; i++) {
        std::string opcion = argv[i];

        if (opcion == "--jugadores") {
            if (tieneJugadores) {
                return false;
            }

            if (!hayValorDespues(i, argc, argv)) {
                return false;
            }

            if (!parsearListaJugadores(argv[i + 1], config)) {
                return false;
            }

            tieneJugadores = true;
            i++;
        }
        else if (opcion == "--semilla") {
            if (tieneSemilla) {
                return false;
            }

            if (!hayValorDespues(i, argc, argv)) {
                return false;
            }

            if (!convertirSemilla(argv[i + 1], config.semilla)) {
                return false;
            }

            tieneSemilla = true;
            i++;
        }
        else if (opcion == "--oleadas") {
            if (tieneOleadas) {
                return false;
            }

            if (!hayValorDespues(i, argc, argv)) {
                return false;
            }

            if (!convertirOleadas(argv[i + 1], config.oleadas)) {
                return false;
            }

            tieneOleadas = true;
            i++;
        }
        else if (opcion == "--script") {
            if (tieneScript) {
                return false;
            }

            if (!hayValorDespues(i, argc, argv)) {
                return false;
            }

            config.rutaScript = argv[i + 1];

            if (config.rutaScript.empty()) {
                return false;
            }

            config.tieneScript = true;
            tieneScript = true;

            i++;
        }
        else {
            return false;
        }
    }

    if (!tieneJugadores) {
        return false;
    }

    if (!tieneSemilla) {
        return false;
    }

    return true;
}

bool parsearArgumentos(int argc, char* argv[], ConfiguracionPrograma& config) {
    inicializarConfiguracionPrograma(config);

    if (argc <= 0 || argv == nullptr) {
        return false;
    }

    if (argc == 1) {
        config.modo = MODO_GRAFICO;
        return true;
    }

    std::string modo = argv[1];

    if (modo == "--lotes") {
        if (!parsearModoLotes(argc, argv, config)) {
            config.modo = MODO_INVALIDO;
            return false;
        }

        return true;
    }

    if (modo == "--estadisticas") {
        if (argc != 3) {
            return false;
        }

        config.rutaHistorial = argv[2];

        if (config.rutaHistorial.empty()) {
            return false;
        }

        config.modo = MODO_ESTADISTICAS;
        return true;
    }

    if (modo == "--ranking") {
        if (argc != 3) {
            return false;
        }

        config.rutaHistorial = argv[2];

        if (config.rutaHistorial.empty()) {
            return false;
        }

        config.modo = MODO_RANKING;
        return true;
    }

    if (modo == "--pruebas") {
        if (argc != 2) {
            return false;
        }

        config.modo = MODO_PRUEBAS;
        return true;
    }

    return false;
}

void imprimirUso(std::ostream& salida) {
    salida << "Uso:" << '\n';
    salida << '\n';

    salida << "faseorbital" << '\n';

    salida << "faseorbital --lotes --jugadores A,B,BOT1 "
        << "--semilla 42 [--oleadas 10] [--script partida.txt]" << '\n';

    salida << "faseorbital --estadisticas historial.csv" << '\n';
    salida << "faseorbital --ranking historial.csv" << '\n';
    salida << "faseorbital --pruebas" << '\n';
}