#include "EjecutorPruebas.h"

#include <iostream>

#include "PruebasListaEnemigos.h"
#include "PruebasListaBalas.h"
#include "Pruebas.h"
#include "PruebasMotor.h"
#include "PruebasOleada.h"
#include "PruebasBot.h"
#include "Prueba08.h"
#include "PruebasEstadisticas.h"
#include "PruebasPersistencia.h"
#include "Prueba14.h"

static bool mostrarResultado(const char* nombre, bool resultado) {
    std::cout << nombre << (resultado ? " PASS" : " FAIL") << '\n';
    return resultado;
}

bool ejecutarTodasLasPruebas() {
    bool todoCorrecto = true;

    std::cout << "=== PRUEBAS OFICIALES ===" << '\n';

    if (!mostrarResultado("P01", pruebaP01())) todoCorrecto = false;
    if (!mostrarResultado("P02", pruebaP02())) todoCorrecto = false;
    if (!mostrarResultado("P03", pruebaP03())) todoCorrecto = false;
    if (!mostrarResultado("P04", pruebaP04())) todoCorrecto = false;
    if (!mostrarResultado("P05", pruebaP05())) todoCorrecto = false;
    if (!mostrarResultado("P06", pruebaP06())) todoCorrecto = false;
    if (!mostrarResultado("P07", pruebaP07())) todoCorrecto = false;
    if (!mostrarResultado("P08", pruebaP08())) todoCorrecto = false;
    if (!mostrarResultado("P09", pruebaP09())) todoCorrecto = false;
    if (!mostrarResultado("P10", pruebaP10())) todoCorrecto = false;
    if (!mostrarResultado("P11", pruebaP11())) todoCorrecto = false;
    if (!mostrarResultado("P12", pruebaP12())) todoCorrecto = false;
    if (!mostrarResultado("P13", pruebaP13())) todoCorrecto = false;

    bool escenarioP14 = prepararEscenarioP14();
    bool ciclosP14 = pruebaCiclosMemoriaP14();
    bool estresP14 = pruebaEstresDiezOleadasP14();

    bool p14LogicoCorrecto =
        escenarioP14 &&
        ciclosP14 &&
        estresP14;

    if (p14LogicoCorrecto) {
        std::cout << "P14 PASS" << '\n';
    }
    else {
        std::cout << "P14 FAIL LOGICO" << '\n';
        todoCorrecto = false;
    }

    std::cout << '\n';
    std::cout << "=== PRUEBAS AUXILIARES ===" << '\n';

    if (!mostrarResultado("GENERADOR", pruebaGeneradorDeterminista())) todoCorrecto = false;
    if (!mostrarResultado("MOTOR BASE", pruebaMotorBase())) todoCorrecto = false;
    if (!mostrarResultado("OLEADA", pruebaOleadasDeterministas())) todoCorrecto = false;
    if (!mostrarResultado("PARTIDA COPIAS", pruebaCopiasIndependientesPartida())) todoCorrecto = false;
    if (!mostrarResultado("PARTIDA SERPIENTE", pruebaOrdenSerpenteantePartida())) todoCorrecto = false;
    if (!mostrarResultado("PARTIDA ELIMINACIONES", pruebaEliminacionesPartida())) todoCorrecto = false;
    if (!mostrarResultado("PARTIDA GANADOR", pruebaGanadorPartida())) todoCorrecto = false;
    if (!mostrarResultado("BOT", pruebaBot())) todoCorrecto = false;
    if (!mostrarResultado("MODO LOTES INTEGRACION", pruebaIntegracionModoLotes())) todoCorrecto = false;
    if (!mostrarResultado("BITACORA", pruebaBitacoraEventos())) todoCorrecto = false;
    if (!mostrarResultado("ESTADISTICAS", pruebaEstadisticasHistoricas())) todoCorrecto = false;
    if (!mostrarResultado("RANKING", pruebaRanking())) todoCorrecto = false;
    if (!mostrarResultado("LINEA TIEMPO", pruebaLineaTiempo())) todoCorrecto = false;
    if (!mostrarResultado("INTEGRACION ESTADISTICAS", pruebaIntegracionEstadisticasMotor())) todoCorrecto = false;
    if (!mostrarResultado("MEMORIA CICLOS", ciclosP14)) todoCorrecto = false;
    if (!mostrarResultado("P14 ESTRES 10 OLEADAS", estresP14)) todoCorrecto = false;

    return todoCorrecto;
}