#include "Persistencia.h"
#include <fstream>
#include <sstream>
#include <limits>
#include <cstdint>

static const char* ENCABEZADO_HISTORIAL = "id_partida,semilla,modo,jugador,es_bot,posicion,puntaje,oleada,disparos,correctos,erroneos,combo_max,pulsos,dano_total,ticks,bajas_dron,bajas_girador,bajas_divisor,bajas_mini,bajas_tanque,bajas_camaleon";

const char* encabezadoHistorialCSV() {
    return ENCABEZADO_HISTORIAL;
}

static bool archivoVacioONoExiste(const std::string& ruta) {
    std::ifstream entrada(ruta, std::ios::binary | std::ios::ate);

    if (!entrada.is_open()) {
        return true;
    }

    return entrada.tellg() == 0;
}

static bool textoValidoParaCSV(const std::string& texto) {
    return texto.find('\n') == std::string::npos && texto.find('\r') == std::string::npos;
}

static std::string escaparCampoCSV(const std::string& texto) {
    bool necesitaComillas = texto.find(',') != std::string::npos || texto.find('"') != std::string::npos;

    if (!necesitaComillas) {
        return texto;
    }

    std::string resultado = "\"";

    for (char caracter : texto) {
        if (caracter == '"') {
            resultado += "\"\"";
        }
        else {
            resultado += caracter;
        }
    }

    resultado += "\"";

    return resultado;
}

static bool escribirRegistroCSV(std::ostream& salida, const RegistroPartida& registro) {
    if (!textoValidoParaCSV(registro.modo) || !textoValidoParaCSV(registro.jugador)) {
        return false;
    }

    salida << registro.idPartida << ','
        << registro.semilla << ','
        << escaparCampoCSV(registro.modo) << ','
        << escaparCampoCSV(registro.jugador) << ','
        << (registro.esBot ? 1 : 0) << ','
        << registro.posicion << ','
        << registro.puntaje << ','
        << registro.oleada << ','
        << registro.disparos << ','
        << registro.correctos << ','
        << registro.erroneos << ','
        << registro.comboMax << ','
        << registro.pulsos << ','
        << registro.danoTotal << ','
        << registro.ticks << ','
        << registro.bajasDron << ','
        << registro.bajasGirador << ','
        << registro.bajasDivisor << ','
        << registro.bajasMini << ','
        << registro.bajasTanque << ','
        << registro.bajasCamaleon << '\n';

    return salida.good();
}

bool agregarRegistroCSV(const std::string& ruta, const RegistroPartida& registro) {
    bool escribirEncabezado = archivoVacioONoExiste(ruta);

    std::ofstream salida(ruta, std::ios::app);

    if (!salida.is_open()) {
        return false;
    }

    if (escribirEncabezado) {
        salida << ENCABEZADO_HISTORIAL << '\n';
    }

    return escribirRegistroCSV(salida, registro);
}

bool agregarHistorialCSV(const std::string& ruta, const HistorialPartidas& historial) {
    bool escribirEncabezado = archivoVacioONoExiste(ruta);

    std::ofstream salida(ruta, std::ios::app);

    if (!salida.is_open()) {
        return false;
    }

    if (escribirEncabezado) {
        salida << ENCABEZADO_HISTORIAL << '\n';
    }

    const NodoRegistroPartida* actual = historial.cabeza;

    while (actual != nullptr) {
        if (!escribirRegistroCSV(salida, actual->dato)) {
            return false;
        }

        actual = actual->sig;
    }

    return salida.good();
}

static bool separarCamposCSV(const std::string& linea, std::string campos[21]) {
    int cantidad = 0;
    std::string campo;
    bool enComillas = false;
    bool comillasCerradas = false;

    for (size_t i = 0; i < linea.size(); i++) {
        char caracter = linea[i];

        if (enComillas) {
            if (caracter == '"') {
                if (i + 1 < linea.size() && linea[i + 1] == '"') {
                    campo += '"';
                    i++;
                }
                else {
                    enComillas = false;
                    comillasCerradas = true;
                }
            }
            else {
                campo += caracter;
            }

            continue;
        }

        if (comillasCerradas) {
            if (caracter != ',') {
                return false;
            }

            if (cantidad >= 21) {
                return false;
            }

            campos[cantidad] = campo;
            cantidad++;
            campo.clear();
            comillasCerradas = false;
            continue;
        }

        if (caracter == ',') {
            if (cantidad >= 21) {
                return false;
            }

            campos[cantidad] = campo;
            cantidad++;
            campo.clear();
        }
        else if (caracter == '"') {
            if (!campo.empty()) {
                return false;
            }

            enComillas = true;
        }
        else {
            campo += caracter;
        }
    }

    if (enComillas) {
        return false;
    }

    if (cantidad >= 21) {
        return false;
    }

    campos[cantidad] = campo;
    cantidad++;

    return cantidad == 21;
}

static bool convertirEntero(const std::string& texto, int& valor) {
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

    if (temporal < std::numeric_limits<int>::min() || temporal > std::numeric_limits<int>::max()) {
        return false;
    }

    valor = static_cast<int>(temporal);
    return true;
}

static bool convertirSemilla(const std::string& texto, uint32_t& valor) {
    if (texto.empty()) {
        return false;
    }

    size_t primero = texto.find_first_not_of(" \t");

    if (primero == std::string::npos || texto[primero] == '-') {
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

    if (temporal > std::numeric_limits<uint32_t>::max()) {
        return false;
    }

    valor = static_cast<uint32_t>(temporal);
    return true;
}

static bool convertirRegistroCSV(const std::string campos[21], RegistroPartida& registro) {
    int esBot;

    if (!convertirEntero(campos[0], registro.idPartida)) return false;
    if (!convertirSemilla(campos[1], registro.semilla)) return false;

    registro.modo = campos[2];
    registro.jugador = campos[3];

    if (!convertirEntero(campos[4], esBot)) return false;
    if (esBot != 0 && esBot != 1) return false;

    registro.esBot = esBot == 1;

    if (!convertirEntero(campos[5], registro.posicion)) return false;
    if (!convertirEntero(campos[6], registro.puntaje)) return false;
    if (!convertirEntero(campos[7], registro.oleada)) return false;
    if (!convertirEntero(campos[8], registro.disparos)) return false;
    if (!convertirEntero(campos[9], registro.correctos)) return false;
    if (!convertirEntero(campos[10], registro.erroneos)) return false;
    if (!convertirEntero(campos[11], registro.comboMax)) return false;
    if (!convertirEntero(campos[12], registro.pulsos)) return false;
    if (!convertirEntero(campos[13], registro.danoTotal)) return false;
    if (!convertirEntero(campos[14], registro.ticks)) return false;
    if (!convertirEntero(campos[15], registro.bajasDron)) return false;
    if (!convertirEntero(campos[16], registro.bajasGirador)) return false;
    if (!convertirEntero(campos[17], registro.bajasDivisor)) return false;
    if (!convertirEntero(campos[18], registro.bajasMini)) return false;
    if (!convertirEntero(campos[19], registro.bajasTanque)) return false;
    if (!convertirEntero(campos[20], registro.bajasCamaleon)) return false;

    return true;
}

static bool convertirLineaCSV(const std::string& linea, RegistroPartida& registro) {
    std::string campos[21];

    if (!separarCamposCSV(linea, campos)) {
        return false;
    }

    return convertirRegistroCSV(campos, registro);
}

bool cargarHistorialCSV(const std::string& ruta, HistorialPartidas& historial, std::ostream& errores) {
    std::ifstream entrada(ruta);

    if (!entrada.is_open()) {
        errores << "No se pudo abrir el archivo: " << ruta << '\n';
        return false;
    }

    std::string linea;

    if (!std::getline(entrada, linea)) {
        errores << "El archivo CSV esta vacio o no contiene encabezado." << '\n';
        return false;
    }

    if (!linea.empty() && linea.back() == '\r') {
        linea.pop_back();
    }

    if (linea != ENCABEZADO_HISTORIAL) {
        errores << "Encabezado CSV invalido." << '\n';
        return false;
    }

    HistorialPartidas temporal;
    inicializarHistorial(temporal);

    int numeroLinea = 1;

    while (std::getline(entrada, linea)) {
        numeroLinea++;

        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }

        RegistroPartida registro;

        if (!convertirLineaCSV(linea, registro)) {
            errores << "Linea " << numeroLinea << " invalida; se omite." << '\n';
            continue;
        }

        agregarRegistroFinal(temporal, registro);
    }

    destruirHistorial(historial);

    historial.cabeza = temporal.cabeza;
    historial.cola = temporal.cola;
    historial.tam = temporal.tam;

    temporal.cabeza = nullptr;
    temporal.cola = nullptr;
    temporal.tam = 0;

    return true;
}