#include <iostream>
#include "Pruebas.h"

using namespace std;

void main() {
	cout << (pruebaP05() ? "P05 PASS" : "P05 FAIL") << std::endl;
	cout << (pruebaP06() ? "P06 PASS" : "P06 FAIL") << std::endl;
	cout << (pruebaP07() ? "P07 PASS" : "P07 FAIL") << std::endl;
	cout << (pruebaGeneradorDeterminista() ? "GENERADOR PASS" : "GENERADOR FAIL") << std::endl;
}
