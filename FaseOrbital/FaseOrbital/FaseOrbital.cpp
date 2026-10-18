#include <iostream>
#include "PruebasListaEnemigos.h"
#include "PruebasListaBalas.h"
#include "Pruebas.h"
#include "PruebasMotor.h"

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
    cout << (pruebaGeneradorDeterminista() ? "GENERADOR PASS" : "GENERADOR FAIL") << endl;
    cout << (pruebaMotorBase() ? "MOTOR BASE PASS" : "MOTOR BASE FAIL") << endl;

    return 0;
}