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

using namespace std; //permite usar cout sin escribir el prefijo

//muestra el nombre y el resultado de una prueba y devuelve ese resultado para acumular los fallos
static bool mostrarResultado(const char* nombre, bool resultado) {
    cout << nombre << (resultado ? " PASS" : " FAIL") << '\n'; //indica en consola si la prueba paso o fallo
    return resultado; //permite actualizar el resultado general sin repetir la prueba
}

//ejecuta todas las pruebas aunque alguna falle y devuelve si todas terminaron correctamente
//reutiliza los resultados de ciclos y estres de p14 al mostrarlos entre las pruebas auxiliares
bool ejecutarTodasLasPruebas() {
    bool todoCorrecto = true; //mantiene el resultado general hasta que alguna prueba falle

    cout << "=== PRUEBAS OFICIALES ===" << '\n'; //identifica el grupo de pruebas numeradas

    if (!mostrarResultado("P01", pruebaP01())) todoCorrecto = false; //comprueba que al quitar los enemigos pares se conserven los demas nodos y sus enlaces
    if (!mostrarResultado("P02", pruebaP02())) todoCorrecto = false; //revisa la eliminacion de cabeza cola y ultimo enemigo
    if (!mostrarResultado("P03", pruebaP03())) todoCorrecto = false; //marca un fallo si insertar despues del ultimo enemigo deja enlaces incorrectos
    if (!mostrarResultado("P04", pruebaP04())) todoCorrecto = false; //comprueba que al eliminar balas por color se conserven las demas
    if (!mostrarResultado("P05", pruebaP05())) todoCorrecto = false; //verifica el orden de llegada y que la cola termine vacia
    if (!mostrarResultado("P06", pruebaP06())) todoCorrecto = false; //revisa que las copias de la cola sean independientes del original
    if (!mostrarResultado("P07", pruebaP07())) todoCorrecto = false; //prueba el recorrido y las eliminaciones en ambos sentidos de la ronda
    if (!mostrarResultado("P08", pruebaP08())) todoCorrecto = false; //revisa que repetir la semilla y el script produzca el mismo estado y los mismos reportes
    if (!mostrarResultado("P09", pruebaP09())) todoCorrecto = false; //comprueba el efecto de un impacto de color incorrecto
    if (!mostrarResultado("P10", pruebaP10())) todoCorrecto = false; //revisa la creacion de minis y los resultados al abatir un divisor
    if (!mostrarResultado("P11", pruebaP11())) todoCorrecto = false; //comprueba la carga necesaria para el pulso y su efecto sobre bajas puntos y combo
    if (!mostrarResultado("P12", pruebaP12())) todoCorrecto = false; //verifica el ordenamiento de puntajes y el calculo de la mediana
    if (!mostrarResultado("P13", pruebaP13())) todoCorrecto = false; //revisa la lectura y el guardado del historial junto con su detalle

    bool escenarioP14 = prepararEscenarioP14(); //comprueba el escenario base de la prueba de estructuras
    bool ciclosP14 = pruebaCiclosMemoriaP14(); //comprueba los ciclos de creacion uso y vaciado de las estructuras
    bool estresP14 = pruebaEstresDiezOleadasP14(); //comprueba el estado de las estructuras al simular diez oleadas

    bool p14LogicoCorrecto = escenarioP14 && ciclosP14 && estresP14; //combina los tres resultados despues de haber ejecutado todas las comprobaciones

    if (p14LogicoCorrecto) { //acepta p14 solamente si sus tres comprobaciones pasaron
        cout << "P14 PASS" << '\n'; //informa que las comprobaciones logicas de p14 pasaron
    }
    else { //maneja el fallo de cualquiera de las tres comprobaciones
        cout << "P14 FAIL LOGICO" << '\n'; //distingue el fallo de las comprobaciones logicas
        todoCorrecto = false; //conserva el fallo aunque las pruebas siguientes pasen
    }

    cout << '\n'; //separa las pruebas oficiales de las auxiliares
    cout << "=== PRUEBAS AUXILIARES ===" << '\n'; //identifica las comprobaciones adicionales

    if (!mostrarResultado("GENERADOR", pruebaGeneradorDeterminista())) todoCorrecto = false; //comprueba que la misma semilla reproduzca la secuencia del generador
    if (!mostrarResultado("MOTOR BASE", pruebaMotorBase())) todoCorrecto = false; //revisa las operaciones basicas de la simulacion
    if (!mostrarResultado("OLEADA", pruebaOleadasDeterministas())) todoCorrecto = false; //comprueba que las apariciones respeten las reglas de generacion
    if (!mostrarResultado("PARTIDA COPIAS", pruebaCopiasIndependientesPartida())) todoCorrecto = false; //verifica que cada turno reciba una cola independiente
    if (!mostrarResultado("PARTIDA SERPIENTE", pruebaOrdenSerpenteantePartida())) todoCorrecto = false; //revisa que los turnos alternen el sentido entre rondas
    if (!mostrarResultado("PARTIDA ELIMINACIONES", pruebaEliminacionesPartida())) todoCorrecto = false; //comprueba el avance y el fin de partida al retirar jugadores
    if (!mostrarResultado("PARTIDA GANADOR", pruebaGanadorPartida())) todoCorrecto = false; //revisa la eleccion del ganador y sus desempates
    if (!mostrarResultado("BOT", pruebaBot())) todoCorrecto = false; //comprueba las decisiones automaticas del bot
    if (!mostrarResultado("MODO LOTES INTEGRACION", pruebaIntegracionModoLotes())) todoCorrecto = false; //revisa la ejecucion de rondas en modo lotes
    if (!mostrarResultado("BITACORA", pruebaBitacoraEventos())) todoCorrecto = false; //verifica el almacenamiento y la consulta de eventos
    if (!mostrarResultado("ESTADISTICAS", pruebaEstadisticasHistoricas())) todoCorrecto = false; //revisa los calculos realizados sobre el historial
    if (!mostrarResultado("RANKING", pruebaRanking())) todoCorrecto = false; //comprueba la clasificacion de los jugadores
    if (!mostrarResultado("LINEA TIEMPO", pruebaLineaTiempo())) todoCorrecto = false; //revisa los resumenes de eventos por oleada
    if (!mostrarResultado("INTEGRACION ESTADISTICAS", pruebaIntegracionEstadisticasMotor())) todoCorrecto = false; //comprueba la relacion entre las metricas del motor y las estadisticas
    if (!mostrarResultado("MEMORIA CICLOS", ciclosP14)) todoCorrecto = false; //muestra el resultado de ciclos ya obtenido sin ejecutar la prueba otra vez
    if (!mostrarResultado("P14 ESTRES 10 OLEADAS", estresP14)) todoCorrecto = false; //muestra el resultado de estres ya obtenido sin repetir las diez oleadas

    return todoCorrecto; //informa si todas las pruebas pasaron conservando cualquier fallo anterior
}
