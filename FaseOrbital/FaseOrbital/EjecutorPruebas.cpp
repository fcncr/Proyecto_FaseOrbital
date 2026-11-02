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

using namespace std; //permite utilizar los elementos de la biblioteca estandar sin escribir el prefijo

//muestra el resultado de una prueba y lo devuelve para acumular los fallos
static bool mostrarResultado(const char* nombre, bool resultado) {
    cout << nombre << (resultado ? " PASS" : " FAIL") << '\n'; //muestra si la prueba paso o fallo
    return resultado; //devuelve el resultado recibido
}

//ejecuta las pruebas oficiales y auxiliares conservando cualquier fallo encontrado
bool ejecutarTodasLasPruebas() {
    bool todoCorrecto = true; //mantiene el resultado general de las pruebas

    cout << " PRUEBAS OFICIALES " << '\n'; //identifica el grupo de pruebas numeradas

    if (!mostrarResultado("P01", pruebaP01())) todoCorrecto = false; //comprueba la eliminacion durante el recorrido de enemigos
    if (!mostrarResultado("P02", pruebaP02())) todoCorrecto = false; //comprueba la eliminacion de cabeza cola y nodo unico
    if (!mostrarResultado("P03", pruebaP03())) todoCorrecto = false; //comprueba la insercion despues del ultimo enemigo
    if (!mostrarResultado("P04", pruebaP04())) todoCorrecto = false; //comprueba la eliminacion de balas consecutivas
    if (!mostrarResultado("P05", pruebaP05())) todoCorrecto = false; //comprueba el orden y vaciado de la cola
    if (!mostrarResultado("P06", pruebaP06())) todoCorrecto = false; //comprueba la copia profunda de la cola
    if (!mostrarResultado("P07", pruebaP07())) todoCorrecto = false; //comprueba la ronda circular de jugadores
    if (!mostrarResultado("P08", pruebaP08())) todoCorrecto = false; //comprueba la reproducibilidad con semilla y script
    if (!mostrarResultado("P09", pruebaP09())) todoCorrecto = false; //comprueba los impactos de color incorrecto
    if (!mostrarResultado("P10", pruebaP10())) todoCorrecto = false; //comprueba la division y aparicion de los minis
    if (!mostrarResultado("P11", pruebaP11())) todoCorrecto = false; //comprueba la activacion y el efecto del pulso
    if (!mostrarResultado("P12", pruebaP12())) todoCorrecto = false; //comprueba el ordenamiento y la mediana
    if (!mostrarResultado("P13", pruebaP13())) todoCorrecto = false; //comprueba el guardado y la recarga del historial

    bool escenarioP14 = prepararEscenarioP14(); //ejecuta el escenario principal de la prueba de memoria
    bool ciclosP14 = pruebaCiclosMemoriaP14(); //comprueba ciclos de creacion uso y destruccion
    bool estresP14 = pruebaEstresDiezOleadasP14(); //comprueba el escenario de estres de diez oleadas
    bool p14LogicoCorrecto = escenarioP14 && ciclosP14 && estresP14; //combina las comprobaciones logicas de p14

    if (p14LogicoCorrecto) { //comprueba si todos los escenarios terminaron correctamente
        cout << "P14 PASS" << '\n'; //muestra que las comprobaciones logicas pasaron
    }
    else { //atiende el fallo de alguno de los escenarios
        cout << "P14 FAIL LOGICO" << '\n'; //muestra el fallo de las comprobaciones
        todoCorrecto = false; //conserva el fallo en el resultado general
    }

    cout << '\n'; //separa los grupos de pruebas
    cout << " PRUEBAS AUXILIARES " << '\n'; //identifica las comprobaciones adicionales

    if (!mostrarResultado("GENERADOR", pruebaGeneradorDeterminista())) todoCorrecto = false; //comprueba la secuencia del generador
    if (!mostrarResultado("MOTOR BASE", pruebaMotorBase())) todoCorrecto = false; //comprueba las operaciones basicas del motor
    if (!mostrarResultado("OLEADA", pruebaOleadasDeterministas())) todoCorrecto = false; //comprueba las apariciones de enemigos
    if (!mostrarResultado("PARTIDA COPIAS", pruebaCopiasIndependientesPartida())) todoCorrecto = false; //comprueba las copias independientes por jugador
    if (!mostrarResultado("PARTIDA SERPIENTE", pruebaOrdenSerpenteantePartida())) todoCorrecto = false; //comprueba la alternancia del orden de los turnos
    if (!mostrarResultado("PARTIDA ELIMINACIONES", pruebaEliminacionesPartida())) todoCorrecto = false; //comprueba los turnos despues de eliminar jugadores
    if (!mostrarResultado("PARTIDA GANADOR", pruebaGanadorPartida())) todoCorrecto = false; //comprueba la eleccion del ganador
    if (!mostrarResultado("REGISTRO ALIAS UNICOS", pruebaRegistroAliasUnicos())) todoCorrecto = false; //comprueba que los duplicados no alteren el registro
    if (!mostrarResultado("ESTADISTICAS ALIAS UNICOS", pruebaEstadisticasAliasUnicos())) todoCorrecto = false; //comprueba la separacion de resultados y la continuidad historica
    if (!mostrarResultado("BOT", pruebaBot())) todoCorrecto = false; //comprueba las decisiones del jugador automatico
    if (!mostrarResultado("MODO LOTES INTEGRACION", pruebaIntegracionModoLotes())) todoCorrecto = false; //comprueba las acciones utilizadas por el modo por lotes
    if (!mostrarResultado("BITACORA", pruebaBitacoraEventos())) todoCorrecto = false; //comprueba el registro y la consulta de eventos
    if (!mostrarResultado("ESTADISTICAS", pruebaEstadisticasHistoricas())) todoCorrecto = false; //comprueba los calculos historicos
    if (!mostrarResultado("RANKING", pruebaRanking())) todoCorrecto = false; //comprueba el orden de la clasificacion
    if (!mostrarResultado("LINEA TIEMPO", pruebaLineaTiempo())) todoCorrecto = false; //comprueba los resultados por oleada
    if (!mostrarResultado("INTEGRACION ESTADISTICAS", pruebaIntegracionEstadisticasMotor())) todoCorrecto = false; //comprueba las estadisticas producidas por el motor
    if (!mostrarResultado("MEMORIA CICLOS", ciclosP14)) todoCorrecto = false; //muestra el resultado de los ciclos ya ejecutados
    if (!mostrarResultado("P14 ESTRES 10 OLEADAS", estresP14)) todoCorrecto = false; //muestra el resultado del estres ya ejecutado

    return todoCorrecto; //indica si todas las pruebas terminaron correctamente
}