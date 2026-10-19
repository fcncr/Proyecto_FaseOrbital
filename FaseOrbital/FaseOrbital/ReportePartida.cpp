#include "ReportePartida.h"
#include <fstream>

static void imprimirJugadorReporte(const Jugador& jugador, int posicionRegistro, std::ostream& salida) {
    const MetricasJugador& metricas = jugador.metricas;

    salida << "[Jugador " << posicionRegistro << "]" << '\n';

    salida << "alias=" << jugador.alias << '\n';
    salida << "esBot=" << (jugador.esBot ? 1 : 0) << '\n';

    salida << "posicion=" << metricas.posicionFinal << '\n';
    salida << "puntaje=" << jugador.puntaje << '\n';
    salida << "oleadaAlcanzada=" << jugador.oleadaAlcanzada << '\n';
    salida << "vidaFinal=" << jugador.vidaNucleo << '\n';

    salida << "disparos=" << metricas.disparos << '\n';
    salida << "correctos=" << metricas.impactosCorrectos << '\n';
    salida << "erroneos=" << metricas.impactosErroneos << '\n';

    salida << "comboMaximo=" << metricas.comboMaximo << '\n';
    salida << "pulsos=" << metricas.pulsosUtilizados << '\n';
    salida << "danoTotal=" << metricas.danoTotal << '\n';
    salida << "ticks=" << metricas.ticksSobrevividos << '\n';

    salida << "bajasDron=" << metricas.bajasDron << '\n';
    salida << "bajasGirador=" << metricas.bajasGirador << '\n';
    salida << "bajasDivisor=" << metricas.bajasDivisor << '\n';
    salida << "bajasMini=" << metricas.bajasMini << '\n';
    salida << "bajasTanque=" << metricas.bajasTanque << '\n';
    salida << "bajasCamaleon=" << metricas.bajasCamaleon << '\n';
}

void imprimirReportePartida(const Partida& partida, std::ostream& salida) {
    salida << "FASE_ORBITAL_REPORTE_V1" << '\n';

    salida << "semilla=" << partida.semillaPartida << '\n';
    salida << "numeroRonda=" << partida.numeroRonda << '\n';
    salida << "limiteOleadas=" << partida.limiteOleadas << '\n';
    salida << "cantidadJugadores=" << partida.cantidadJugadores << '\n';
    salida << "finalizada=" << (partida.finalizada ? 1 : 0) << '\n';

    for (int i = 0; i < partida.cantidadJugadores; i++) {
        salida << '\n';
        imprimirJugadorReporte(partida.jugadores[i], i + 1, salida);
    }
}

bool guardarReportePartida(const Partida& partida, const std::string& ruta) {
    std::ofstream archivo(ruta, std::ios::binary);

    if (!archivo.is_open()) {
        return false;
    }

    imprimirReportePartida(partida, archivo);

    bool resultado = archivo.good();

    archivo.close();

    return resultado;
}

bool compararArchivosExactos(const std::string& rutaA, const std::string& rutaB) {
    std::ifstream archivoA(rutaA, std::ios::binary);
    std::ifstream archivoB(rutaB, std::ios::binary);

    if (!archivoA.is_open() || !archivoB.is_open()) {
        return false;
    }

    char caracterA;
    char caracterB;

    while (true) {
        bool leyoA = static_cast<bool>(archivoA.get(caracterA));
        bool leyoB = static_cast<bool>(archivoB.get(caracterB));

        if (leyoA != leyoB) {
            return false;
        }

        if (!leyoA) {
            return true;
        }

        if (caracterA != caracterB) {
            return false;
        }
    }
}