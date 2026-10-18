#include <iostream>
#include "PruebasListaEnemigos.h"
#include "PruebasListaBalas.h"
#include "Pruebas.h"
#include "PruebasMotor.h"
#include "PruebasOleada.h"
#include "PruebasBot.h"
#include "Prueba08.h"

using namespace std;

int main() {
    cout << "=== PRUEBAS SPRINT 1 ===" << endl;

    cout << (pruebaP01() ? "P01 PASS" : "P01 FAIL") << endl;
    cout << (pruebaP02() ? "P02 PASS" : "P02 FAIL") << endl;
    cout << (pruebaP03() ? "P03 PASS" : "P03 FAIL") << endl;
    cout << (pruebaP04() ? "P04 PASS" : "P04 FAIL") << endl;

    cout << (pruebaP05() ? "P05 PASS" : "P05 FAIL") << endl;
    cout << (pruebaP06() ? "P06 PASS" : "P06 FAIL") << endl;
    cout << (pruebaP07() ? "P07 PASS" : "P07 FAIL") << endl;
    cout << (pruebaP08() ? "P08 PASS" : "P08 FAIL") << endl;

    cout << (pruebaGeneradorDeterminista() ? "GENERADOR PASS" : "GENERADOR FAIL") << endl;
    cout << (pruebaMotorBase() ? "MOTOR BASE PASS" : "MOTOR BASE FAIL") << endl;
    cout << (pruebaOleadasDeterministas() ? "OLEADA PASS" : "OLEADA FAIL") << std::endl;
    cout << (pruebaCopiasIndependientesPartida() ? "PARTIDA COPIAS PASS" : "PARTIDA COPIAS FAIL") << endl;
    cout << (pruebaOrdenSerpenteantePartida() ? "PARTIDA SERPIENTE PASS" : "PARTIDA SERPIENTE FAIL") << endl;
    cout << (pruebaEliminacionesPartida() ? "PARTIDA ELIMINACIONES PASS" : "PARTIDA ELIMINACIONES FAIL") << endl;
    cout << (pruebaGanadorPartida() ? "PARTIDA GANADOR PASS" : "PARTIDA GANADOR FAIL") << endl;
    cout << (pruebaBot() ? "BOT PASS" : "BOT FAIL") << endl;
    cout << (pruebaIntegracionModoLotes() ? "MODO LOTES INTEGRACION PASS" : "MODO LOTES INTEGRACION FAIL") << endl;
    return 0;
}