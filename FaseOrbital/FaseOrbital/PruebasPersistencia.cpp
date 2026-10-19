#include "PruebasPersistencia.h"
#include "Persistencia.h"
#include "Estadisticas.h"
#include "Ranking.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdio>

static RegistroPartida crearRegistroP13(int idPartida, uint32_t semilla, const std::string& modo, const std::string& jugador, bool esBot, int posicion, int puntaje, int oleada, int disparos, int correctos, int erroneos) {
    RegistroPartida registro;

    registro.idPartida = idPartida;
    registro.semilla = semilla;
    registro.modo = modo;
    registro.jugador = jugador;
    registro.esBot = esBot;
    registro.posicion = posicion;
    registro.puntaje = puntaje;
    registro.oleada = oleada;
    registro.disparos = disparos;
    registro.correctos = correctos;
    registro.erroneos = erroneos;
    registro.comboMax = 4;
    registro.pulsos = 1;
    registro.danoTotal = 20;
    registro.ticks = 500;
    registro.bajasDron = 2;
    registro.bajasGirador = 1;
    registro.bajasDivisor = 0;
    registro.bajasMini = 1;
    registro.bajasTanque = 0;
    registro.bajasCamaleon = 0;

    return registro;
}

static bool doublesP13Iguales(double a, double b) {
    return std::fabs(a - b) < 0.000001;
}

static bool estadisticasP13Iguales(const EstadisticasJugador& a, const EstadisticasJugador& b) {
    if (a.alias != b.alias) return false;
    if (a.partidasJugadas != b.partidasJugadas) return false;
    if (a.victorias != b.victorias) return false;
    if (a.mejorPuntaje != b.mejorPuntaje) return false;
    if (a.peorPuntaje != b.peorPuntaje) return false;
    if (!doublesP13Iguales(a.mediaPuntaje, b.mediaPuntaje)) return false;
    if (!doublesP13Iguales(a.medianaPuntaje, b.medianaPuntaje)) return false;
    if (!doublesP13Iguales(a.desviacionEstandar, b.desviacionEstandar)) return false;
    if (!doublesP13Iguales(a.precisionColorPromedio, b.precisionColorPromedio)) return false;
    if (a.mejorColorDisponible != b.mejorColorDisponible) return false;
    if (a.mejorColor != b.mejorColor) return false;
    if (a.tipoMasDanoDisponible != b.tipoMasDanoDisponible) return false;
    if (a.tipoMasDano != b.tipoMasDano) return false;
    if (!doublesP13Iguales(a.histograma.minimo, b.histograma.minimo)) return false;
    if (!doublesP13Iguales(a.histograma.maximo, b.histograma.maximo)) return false;
    if (!doublesP13Iguales(a.histograma.anchoIntervalo, b.histograma.anchoIntervalo)) return false;
    if (a.histograma.total != b.histograma.total) return false;

    for (int i = 0; i < 10; i++) {
        if (a.histograma.frecuencias[i] != b.histograma.frecuencias[i]) {
            return false;
        }
    }

    return true;
}

static bool rankingsP13Iguales(const Ranking& a, const Ranking& b) {
    if (a.tam != b.tam) {
        return false;
    }

    const NodoRanking* actualA = a.cabeza;
    const NodoRanking* actualB = b.cabeza;

    while (actualA != nullptr && actualB != nullptr) {
        if (actualA->dato.alias != actualB->dato.alias) {
            return false;
        }

        if (actualA->dato.mejorPuntaje != actualB->dato.mejorPuntaje) {
            return false;
        }

        if (!doublesP13Iguales(actualA->dato.precisionColor, actualB->dato.precisionColor)) {
            return false;
        }

        actualA = actualA->sig;
        actualB = actualB->sig;
    }

    return actualA == nullptr && actualB == nullptr;
}

static int contarEncabezadosP13(const std::string& ruta) {
    std::ifstream entrada(ruta);
    std::string linea;
    int cantidad = 0;

    while (std::getline(entrada, linea)) {
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }

        if (linea == encabezadoHistorialCSV()) {
            cantidad++;
        }
    }

    return cantidad;
}

bool pruebaP13() {
    const std::string ruta = "historial_p13_prueba.csv";

    std::remove(ruta.c_str());

    RegistroPartida registro1 = crearRegistroP13(1, 42u, "lotes", "ANA", false, 1, 1000, 5, 100, 80, 20);
    RegistroPartida registro2 = crearRegistroP13(1, 42u, "lotes", "BOB", false, 2, 1200, 5, 100, 90, 10);
    RegistroPartida registro3 = crearRegistroP13(1, 42u, "lotes", "BOT1", true, 3, 900, 4, 100, 50, 50);
    RegistroPartida registro4 = crearRegistroP13(2, 99u, "lotes", "ANA", false, 2, 1500, 7, 50, 45, 5);
    RegistroPartida registro5 = crearRegistroP13(2, 99u, "lotes", "BOB", false, 1, 1500, 7, 100, 80, 20);

    HistorialPartidas historialAntes;
    inicializarHistorial(historialAntes);

    agregarRegistroFinal(historialAntes, registro1);
    agregarRegistroFinal(historialAntes, registro2);
    agregarRegistroFinal(historialAntes, registro3);
    agregarRegistroFinal(historialAntes, registro4);
    agregarRegistroFinal(historialAntes, registro5);

    EstadisticasJugador estadisticasAntes;

    if (!calcularEstadisticasJugador(historialAntes, "ANA", estadisticasAntes)) {
        destruirHistorial(historialAntes);
        return false;
    }

    Ranking rankingAntes;
    inicializarRanking(rankingAntes);
    construirRankingDesdeHistorial(historialAntes, rankingAntes);

    HistorialPartidas primerLote;
    inicializarHistorial(primerLote);

    agregarRegistroFinal(primerLote, registro1);
    agregarRegistroFinal(primerLote, registro2);
    agregarRegistroFinal(primerLote, registro3);

    bool resultado = true;

    if (!agregarHistorialCSV(ruta, primerLote)) {
        resultado = false;
    }

    destruirHistorial(primerLote);

    {
        std::ofstream salida(ruta, std::ios::app);

        if (!salida.is_open()) {
            resultado = false;
        }
        else {
            salida << "esto,no,es,una,fila,valida" << '\n';
            salida << "99,NO_NUMERO,lotes,ERROR,0,1,100,1,1,1,0,0,0,0,10,0,0,0,0,0,0" << '\n';
        }
    }

    if (!agregarRegistroCSV(ruta, registro4)) {
        resultado = false;
    }

    if (!agregarRegistroCSV(ruta, registro5)) {
        resultado = false;
    }

    if (contarEncabezadosP13(ruta) != 1) {
        resultado = false;
    }

    destruirHistorial(historialAntes);

    HistorialPartidas historialDespues;
    inicializarHistorial(historialDespues);

    std::ostringstream errores;

    if (!cargarHistorialCSV(ruta, historialDespues, errores)) {
        resultado = false;
    }

    if (contarRegistros(historialDespues) != 5) {
        resultado = false;
    }

    std::string textoErrores = errores.str();

    if (textoErrores.find("Linea 5") == std::string::npos) {
        resultado = false;
    }

    if (textoErrores.find("Linea 6") == std::string::npos) {
        resultado = false;
    }

    EstadisticasJugador estadisticasDespues;

    if (!calcularEstadisticasJugador(historialDespues, "ANA", estadisticasDespues)) {
        resultado = false;
    }

    if (!estadisticasP13Iguales(estadisticasAntes, estadisticasDespues)) {
        resultado = false;
    }

    Ranking rankingDespues;
    inicializarRanking(rankingDespues);
    construirRankingDesdeHistorial(historialDespues, rankingDespues);

    if (!rankingsP13Iguales(rankingAntes, rankingDespues)) {
        resultado = false;
    }

    destruirRanking(rankingAntes);
    destruirRanking(rankingDespues);
    destruirHistorial(historialDespues);

    std::remove(ruta.c_str());

    return resultado;
}