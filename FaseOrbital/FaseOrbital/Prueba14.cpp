#include "Prueba14.h"
#include "Partida.h"
#include "ScriptAcciones.h"

//registra cuatro bots en orden y devuelve falso si alguno no puede agregarse
static bool agregarBotsP14(Partida& partida) {
    if (!agregarJugadorPartida(partida, "BOT1", true)) return false; //registra al primer bot y detiene la preparacion si falla
    if (!agregarJugadorPartida(partida, "BOT2", true)) return false; //agrega al segundo participante automatico
    if (!agregarJugadorPartida(partida, "BOT3", true)) return false; //incorpora al tercer bot en el mismo orden de registro
    if (!agregarJugadorPartida(partida, "BOT4", true)) return false; //completa los cuatro participantes del escenario

    return true; //confirma que los cuatro bots quedaron registrados
}

//comprueba los punteros contadores y bandera final que debe dejar la destruccion de una partida
//estas comprobaciones del estado no sustituyen la revision con un detector de fugas
static bool partidaDestruidaCorrectamenteP14(const Partida& partida) {
    if (partida.cantidadJugadores != 0) return false; //revisa que no se sigan contando jugadores registrados

    if (partida.ronda.actual != nullptr) return false; //comprueba que la ronda ya no apunte a un turno
    if (partida.ronda.tam != 0) return false; //revisa el conteo de nodos de la ronda vacia

    if (partida.colaOriginalRonda.frente != nullptr) return false; //exige que la cola original no conserve un primer nodo
    if (partida.colaOriginalRonda.final != nullptr) return false; //exige que tampoco conserve un ultimo nodo
    if (partida.colaOriginalRonda.tam != 0) return false; //comprueba que la cola no registre apariciones pendientes

    if (partida.bitacora.cabeza != nullptr) return false; //revisa que se haya restablecido el inicio de la bitacora
    if (partida.bitacora.cola != nullptr) return false; //revisa que se haya restablecido el final de la bitacora
    if (partida.bitacora.tam != 0) return false; //comprueba que ya no se cuenten eventos guardados

    if (!partida.finalizada) return false; //exige que la partida destruida quede marcada como terminada

    return true; //confirma que todos los campos revisados tienen el estado esperado
}

//revisa que el script destruido no conserve extremos ni acciones registradas
static bool scriptDestruidoCorrectamenteP14(const ScriptAcciones& script) {
    return script.cabeza == nullptr && script.cola == nullptr && script.tam == 0; //comprueba juntos los dos extremos y el conteo de acciones
}

//ejecuta una partida normal de cuatro bots con semilla fija y limite de diez oleadas
//revisa que termine dentro del limite y que destruir la partida y el script deje sus estructuras vacias
bool prepararEscenarioP14() {
    Partida partida; //guarda el escenario que se ejecutara por rondas
    ScriptAcciones script; //mantiene el script vacio porque los participantes son bots

    inicializarPartida(partida, 42u, 10); //establece la semilla y el limite de oleadas de la partida
    inicializarScriptAcciones(script); //prepara el script sin acciones

    bool resultado = true; //conserva cualquier fallo de preparacion ejecucion o limpieza

    if (!agregarBotsP14(partida)) { //intenta registrar a los cuatro participantes automaticos
        resultado = false; //impide ejecutar una partida con la preparacion incompleta
    }

    const int limiteTicks = 10000; //limita los ticks que puede consumir cada turno
    int rondasEjecutadas = 0; //cuenta las rondas que se completan correctamente

    while (resultado && !partida.finalizada && rondasEjecutadas < 10) { //continua hasta terminar la partida encontrar un fallo o alcanzar diez rondas
        if (!ejecutarRondaModoLotes(partida, script, limiteTicks)) { //simula los turnos de la ronda actual
            resultado = false; //detiene las rondas si alguna no pudo completarse
        }
        else { //cuenta solamente las rondas que terminaron correctamente
            rondasEjecutadas++; //registra la ronda completada
        }
    }

    if (!partida.finalizada) { //revisa que la simulacion no haya quedado abierta al salir del ciclo
        resultado = false; //marca una partida que no termino
    }

    if (partida.limiteOleadas != 10) { //comprueba que la configuracion conserve el limite inicial
        resultado = false; //detecta una modificacion inesperada del limite
    }

    if (partida.cantidadJugadores != 4) { //revisa que sigan registrados los cuatro bots incluidos los eliminados
        resultado = false; //marca una cantidad de participantes incorrecta
    }

    if (rondasEjecutadas <= 0 || rondasEjecutadas > 10) { //exige que se haya completado al menos una ronda sin superar diez
        resultado = false; //rechaza un conteo fuera del intervalo esperado
    }

    if (resultado) { //actualiza los resultados finales solo si el escenario fue correcto
        actualizarMetricasFinalesPartida(partida); //calcula las proporciones y las posiciones de los participantes
    }

    destruirPartida(partida); //libera los nodos de la ronda las apariciones y los eventos
    destruirScriptAcciones(script); //restablece el script vacio usado durante la simulacion

    if (!partidaDestruidaCorrectamenteP14(partida)) { //revisa los campos de la partida despues de destruirla
        resultado = false; //marca un estado de partida que no quedo restablecido
    }

    if (!scriptDestruidoCorrectamenteP14(script)) { //revisa los extremos y el conteo del script destruido
        resultado = false; //marca un estado de script que no quedo vacio
    }

    return resultado; //devuelve el resultado conjunto del escenario y sus comprobaciones de estado
}

//crea ejecuta y destruye treinta partidas de una oleada cambiando la semilla en cada ciclo
//comprueba el estado vacio despues de cada destruccion y se detiene al encontrar un fallo
bool pruebaCiclosMemoriaP14() {
    const int cantidadCiclos = 30; //establece cuantas veces se repetira la creacion y destruccion

    for (int i = 0; i < cantidadCiclos; i++) { //prepara un escenario independiente en cada repeticion
        Partida partida; //guarda la partida de este ciclo
        ScriptAcciones script; //acompana la simulacion con un script vacio

        inicializarPartida(partida, 100u + static_cast<unsigned int>(i), 1); //usa una semilla distinta y limita la partida a una oleada
        inicializarScriptAcciones(script); //deja el script sin acciones para los turnos de bots

        bool resultado = true; //acumula los fallos de este ciclo antes de liberar sus estructuras

        if (!agregarBotsP14(partida)) { //intenta agregar los cuatro bots del ciclo
            resultado = false; //marca una preparacion incompleta
        }

        if (resultado) { //ejecuta la ronda solo si todos los bots quedaron registrados
            if (!ejecutarRondaModoLotes(partida, script, 10000)) { //simula la unica ronda con un limite de diez mil ticks por turno
                resultado = false; //marca el fallo de la simulacion
            }
        }

        if (resultado && !partida.finalizada) { //comprueba que la ronda haya cerrado la partida limitada a una oleada
            resultado = false; //detecta que el ciclo quedo incompleto
        }

        if (resultado) { //completa los resultados solo en un escenario que termino correctamente
            actualizarMetricasFinalesPartida(partida); //calcula las metricas finales antes de destruir el estado
        }

        destruirPartida(partida); //libera las estructuras creadas por el ciclo aunque haya fallado
        destruirScriptAcciones(script); //deja el script del ciclo restablecido

        if (!partidaDestruidaCorrectamenteP14(partida)) { //comprueba el estado de la partida despues de liberarla
            resultado = false; //registra un contador o puntero que no quedo restablecido
        }

        if (!scriptDestruidoCorrectamenteP14(script)) { //revisa el estado vacio del script despues de destruirlo
            resultado = false; //conserva el fallo del script en el resultado del ciclo
        }

        if (!resultado) { //detiene la prueba si este ciclo no cumplio las comprobaciones
            return false; //informa el primer fallo sin iniciar otra partida
        }
    }

    return true; //confirma que los treinta ciclos terminaron con el estado esperado
}

//simula un turno por cada uno de cuatro bots en las oleadas del uno al diez usando una partida nueva por oleada
//comprueba que los turnos terminen y que las estructuras queden restablecidas al destruirlas
bool pruebaEstresDiezOleadasP14() {
    ScriptAcciones script; //mantiene el script vacio compartido por los escenarios de bots
    inicializarScriptAcciones(script); //prepara el script antes de recorrer las oleadas

    bool resultado = true; //conserva cualquier fallo para detener los escenarios siguientes

    const unsigned int semilla = 42u; //usa la misma semilla en los escenarios de las diez oleadas
    const int limiteTicks = 10000; //limita el tiempo de simulacion permitido para cada turno

    for (int numeroOleada = 1; numeroOleada <= 10 && resultado; numeroOleada++) { //prueba cada oleada hasta completar diez o encontrar un fallo
        Partida partida; //crea participantes nuevos para el escenario de esta oleada

        inicializarPartida(partida, semilla, 10); //restablece la partida con la semilla comun y limite de diez oleadas

        if (!agregarBotsP14(partida)) { //registra los cuatro bots nuevos del escenario
            resultado = false; //impide ejecutar turnos con participantes incompletos
        }

        partida.numeroRonda = numeroOleada; //selecciona directamente la oleada que corresponde a esta vuelta

        if (resultado) { //genera apariciones solo si los participantes se registraron correctamente
            generarColaOriginalRonda(partida); //prepara la cola que se copiara para cada bot
        }

        if (resultado && partida.colaOriginalRonda.tam <= 0) { //revisa que la oleada generada tenga enemigos programados
            resultado = false; //rechaza un escenario sin apariciones
        }

        for (int i = 0; i < 4 && resultado; i++) { //ejecuta un turno por bot mientras no se detecte un fallo
            ColaSpawn colaJugador; //guarda la copia independiente que se entregara a este turno
            inicializarColaSpawn(colaJugador); //prepara la cola antes de copiar las apariciones

            prepararColaJugador(partida, colaJugador); //copia las apariciones de la ronda para el jugador actual del arreglo

            ResultadoModoLotes resultadoTurno; //recibe los datos finales que devuelve la simulacion del turno

            if (!ejecutarModoLotes(partida.jugadores[i], numeroOleada, colaJugador, script, limiteTicks, resultadoTurno, &partida.bitacora)) { //simula el turno y exige que termine dentro del limite de ticks
                resultado = false; //marca el fallo aunque el jugador haya quedado con cambios parciales
            }

            destruirColaSpawn(colaJugador); //libera la copia del turno incluso si la simulacion fallo
        }

        destruirPartida(partida); //libera la ronda las apariciones originales y los eventos del escenario

        if (!partidaDestruidaCorrectamenteP14(partida)) { //revisa el estado de la partida despues de destruirla
            resultado = false; //detiene los escenarios si quedaron campos sin restablecer
        }
    }

    destruirScriptAcciones(script); //restablece el script compartido al terminar los escenarios

    if (!scriptDestruidoCorrectamenteP14(script)) { //comprueba que el script conserve el estado vacio esperado
        resultado = false; //incluye un fallo de limpieza del script en el resultado final
    }

    return resultado; //indica si todos los turnos y las comprobaciones de estado fueron correctos
}
