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

using namespace std;

int main() {
    cout << "=== PRUEBAS OFICIALES ===" << endl;

    cout << (pruebaP01() ? "P01 PASS" : "P01 FAIL") << endl;
    cout << (pruebaP02() ? "P02 PASS" : "P02 FAIL") << endl;
    cout << (pruebaP03() ? "P03 PASS" : "P03 FAIL") << endl;
    cout << (pruebaP04() ? "P04 PASS" : "P04 FAIL") << endl;
    cout << (pruebaP05() ? "P05 PASS" : "P05 FAIL") << endl;
    cout << (pruebaP06() ? "P06 PASS" : "P06 FAIL") << endl;
    cout << (pruebaP07() ? "P07 PASS" : "P07 FAIL") << endl;
    cout << (pruebaP08() ? "P08 PASS" : "P08 FAIL") << endl;
    cout << (pruebaP09() ? "P09 PASS" : "P09 FAIL") << endl;
    cout << (pruebaP10() ? "P10 PASS" : "P10 FAIL") << endl;
    cout << (pruebaP11() ? "P11 PASS" : "P11 FAIL") << endl;
    cout << (pruebaP12() ? "P12 PASS" : "P12 FAIL") << endl;
    cout << (pruebaP13() ? "P13 PASS" : "P13 FAIL") << endl;

    bool escenarioP14 = prepararEscenarioP14();
    bool ciclosP14 = pruebaCiclosMemoriaP14();

    cout << (escenarioP14 && ciclosP14 ? "P14 PREPARADO" : "P14 FAIL LOGICO") << endl;

    cout << endl;
    cout << "=== PRUEBAS AUXILIARES ===" << endl;

    cout << (pruebaGeneradorDeterminista() ? "GENERADOR PASS" : "GENERADOR FAIL") << endl;
    cout << (pruebaMotorBase() ? "MOTOR BASE PASS" : "MOTOR BASE FAIL") << endl;
    cout << (pruebaOleadasDeterministas() ? "OLEADA PASS" : "OLEADA FAIL") << endl;
    cout << (pruebaCopiasIndependientesPartida() ? "PARTIDA COPIAS PASS" : "PARTIDA COPIAS FAIL") << endl;
    cout << (pruebaOrdenSerpenteantePartida() ? "PARTIDA SERPIENTE PASS" : "PARTIDA SERPIENTE FAIL") << endl;
    cout << (pruebaEliminacionesPartida() ? "PARTIDA ELIMINACIONES PASS" : "PARTIDA ELIMINACIONES FAIL") << endl;
    cout << (pruebaGanadorPartida() ? "PARTIDA GANADOR PASS" : "PARTIDA GANADOR FAIL") << endl;
    cout << (pruebaBot() ? "BOT PASS" : "BOT FAIL") << endl;
    cout << (pruebaIntegracionModoLotes() ? "MODO LOTES INTEGRACION PASS" : "MODO LOTES INTEGRACION FAIL") << endl;
    cout << (pruebaBitacoraEventos() ? "BITACORA PASS" : "BITACORA FAIL") << endl;
    cout << (pruebaEstadisticasHistoricas() ? "ESTADISTICAS PASS" : "ESTADISTICAS FAIL") << endl;
    cout << (pruebaRanking() ? "RANKING PASS" : "RANKING FAIL") << endl;
    cout << (pruebaLineaTiempo() ? "LINEA TIEMPO PASS" : "LINEA TIEMPO FAIL") << endl;
    cout << (pruebaIntegracionEstadisticasMotor() ? "INTEGRACION ESTADISTICAS PASS" : "INTEGRACION ESTADISTICAS FAIL") << endl;
    cout << (ciclosP14 ? "MEMORIA CICLOS PASS" : "MEMORIA CICLOS FAIL") << endl;

    return 0;
}