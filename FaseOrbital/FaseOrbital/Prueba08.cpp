#include "Prueba08.h"
#include "Partida.h"
#include "ReportePartida.h"

//compara todos los campos de las metricas y exige igualdad exacta tambien en las proporciones calculadas
static bool metricasP08Iguales(const MetricasJugador& a, const MetricasJugador& b) {
    if (a.puntaje != b.puntaje) return false; //detecta una diferencia en los puntos acumulados
    if (a.oleadaAlcanzada != b.oleadaAlcanzada) return false; //comprueba que ambos jugadores hayan llegado a la misma oleada

    if (a.disparos != b.disparos) return false; //compara la cantidad de balas disparadas
    if (a.impactosCorrectos != b.impactosCorrectos) return false; //revisa el total de impactos del color correcto
    if (a.impactosErroneos != b.impactosErroneos) return false; //revisa el total de impactos de otro color

    if (a.impactosCorrectosC != b.impactosCorrectosC) return false; //compara los aciertos de las balas cian
    if (a.impactosErroneosC != b.impactosErroneosC) return false; //compara los errores de las balas cian

    if (a.impactosCorrectosM != b.impactosCorrectosM) return false; //compara los aciertos de las balas magenta
    if (a.impactosErroneosM != b.impactosErroneosM) return false; //compara los errores de las balas magenta

    if (a.impactosCorrectosA != b.impactosCorrectosA) return false; //compara los aciertos de las balas ambar
    if (a.impactosErroneosA != b.impactosErroneosA) return false; //compara los errores de las balas ambar

    if (a.bajasDron != b.bajasDron) return false; //revisa las bajas de drones
    if (a.bajasGirador != b.bajasGirador) return false; //revisa las bajas de giradores
    if (a.bajasDivisor != b.bajasDivisor) return false; //revisa las bajas de divisores
    if (a.bajasMini != b.bajasMini) return false; //revisa las bajas de minis
    if (a.bajasTanque != b.bajasTanque) return false; //revisa las bajas de tanques
    if (a.bajasCamaleon != b.bajasCamaleon) return false; //revisa las bajas de camaleones

    if (a.bajasColor0 != b.bajasColor0) return false; //compara las bajas de enemigos cian
    if (a.bajasColor1 != b.bajasColor1) return false; //compara las bajas de enemigos magenta
    if (a.bajasColor2 != b.bajasColor2) return false; //compara las bajas de enemigos ambar

    if (a.comboMaximo != b.comboMaximo) return false; //comprueba que la mayor racha coincida

    if (a.danoTotal != b.danoTotal) return false; //compara toda la vida perdida por el nucleo
    if (a.danoDron != b.danoDron) return false; //revisa el dano causado por drones
    if (a.danoGirador != b.danoGirador) return false; //revisa el dano causado por giradores
    if (a.danoDivisor != b.danoDivisor) return false; //revisa el dano causado por divisores
    if (a.danoMini != b.danoMini) return false; //revisa el dano causado por minis
    if (a.danoTanque != b.danoTanque) return false; //revisa el dano causado por tanques
    if (a.danoCamaleon != b.danoCamaleon) return false; //revisa el dano causado por camaleones

    if (a.pulsosUtilizados != b.pulsosUtilizados) return false; //compara las activaciones del pulso
    if (a.ticksSobrevividos != b.ticksSobrevividos) return false; //comprueba que se haya jugado la misma cantidad de ticks
    if (a.posicionFinal != b.posicionFinal) return false; //revisa el puesto obtenido al terminar

    if (a.precisionColor != b.precisionColor) return false; //exige la misma precision general sin aproximaciones
    if (a.eficienciaDisparo != b.eficienciaDisparo) return false; //compara la relacion entre aciertos y disparos
    if (a.precisionC != b.precisionC) return false; //revisa la precision calculada para cian
    if (a.precisionM != b.precisionM) return false; //revisa la precision calculada para magenta
    if (a.precisionA != b.precisionA) return false; //revisa la precision calculada para ambar

    return true; //confirma que ninguna metrica difiere
}

//compara el estado del jugador y sus metricas sin confundir los datos con la direccion del objeto
static bool jugadoresP08Iguales(const Jugador& a, const Jugador& b) {
    return a.alias == b.alias && a.esBot == b.esBot && a.vidaNucleo == b.vidaNucleo && a.puntaje == b.puntaje && a.combo == b.combo && a.cargaPulso == b.cargaPulso && a.oleadaAlcanzada == b.oleadaAlcanzada && a.eliminado == b.eliminado && metricasP08Iguales(a.metricas, b.metricas); //exige que todos los datos del participante coincidan
}

//comprueba que dos apariciones tengan los mismos datos de tiempo tipo fase angulo y sentido
static bool spawnsP08Iguales(const Spawn& a, const Spawn& b) {
    return a.tick == b.tick && a.tipo == b.tipo && a.color == b.color && a.angulo == b.angulo && a.sentido == b.sentido; //compara todos los campos que definen la aparicion
}

//recorre dos colas en paralelo y exige el mismo contenido en el mismo orden
static bool colasP08Iguales(const ColaSpawn& a, const ColaSpawn& b) {
    if (a.tam != b.tam) { //descarta colas que registran cantidades diferentes
        return false; //informa la diferencia de tamano
    }

    const NodoSpawn* nodoA = a.frente; //empieza por la primera aparicion de la cola inicial
    const NodoSpawn* nodoB = b.frente; //empieza por la posicion equivalente de la otra cola

    while (nodoA != nullptr && nodoB != nullptr) { //compara mientras ambas colas tengan nodos pendientes
        if (!spawnsP08Iguales(nodoA->dato, nodoB->dato)) { //revisa los datos de las apariciones que ocupan la misma posicion
            return false; //detiene la comparacion al encontrar datos distintos
        }

        nodoA = nodoA->sig; //avanza en la primera cola
        nodoB = nodoB->sig; //avanza en la segunda cola
    }

    return nodoA == nullptr && nodoB == nullptr; //exige que las dos colas se hayan recorrido por completo
}

//compara todos los datos de un evento para detectar diferencias entre las dos simulaciones
static bool eventosP08Iguales(const Evento& a, const Evento& b) {
    return a.tick == b.tick && a.oleada == b.oleada && a.jugador == b.jugador && a.tipoEvento == b.tipoEvento && a.tipoEnemigo == b.tipoEnemigo && a.color == b.color && a.valor == b.valor; //exige el mismo suceso con el mismo momento participante y valor
}

//revisa que dos bitacoras contengan los mismos eventos en el mismo orden de registro
static bool bitacorasP08Iguales(const BitacoraEventos& a, const BitacoraEventos& b) {
    if (a.tam != b.tam) { //compara la cantidad de eventos registrada
        return false; //rechaza bitacoras de distinto tamano
    }

    const NodoEvento* nodoA = a.cabeza; //consulta el primer evento de la primera partida
    const NodoEvento* nodoB = b.cabeza; //consulta el primer evento de la segunda partida

    while (nodoA != nullptr && nodoB != nullptr) { //recorre los eventos de ambas partidas en paralelo
        if (!eventosP08Iguales(nodoA->dato, nodoB->dato)) { //busca una diferencia en el evento de esta posicion
            return false; //informa que las bitacoras no coinciden
        }

        nodoA = nodoA->sig; //avanza al siguiente evento de la primera partida
        nodoB = nodoB->sig; //avanza al siguiente evento de la segunda partida
    }

    return nodoA == nullptr && nodoB == nullptr; //comprueba que ninguna bitacora tenga eventos adicionales
}

//compara los datos generales los jugadores el turno actual la cola y la bitacora de dos partidas
static bool partidasP08Iguales(const Partida& a, const Partida& b) {
    if (a.cantidadJugadores != b.cantidadJugadores) return false; //revisa el total de participantes registrados
    if (a.numeroRonda != b.numeroRonda) return false; //comprueba que las partidas esten en la misma ronda
    if (a.semillaPartida != b.semillaPartida) return false; //compara la semilla usada para generar las oleadas
    if (a.limiteOleadas != b.limiteOleadas) return false; //revisa el limite configurado para ambas partidas
    if (a.finalizada != b.finalizada) return false; //detecta si solo una partida ha terminado

    if (cantidadJugadoresActivos(a) != cantidadJugadoresActivos(b)) { //compara cuantos jugadores siguen en la ronda
        return false; //informa que las eliminaciones no dejaron el mismo numero de activos
    }

    for (int i = 0; i < a.cantidadJugadores; i++) { //revisa los participantes en su orden de registro
        if (!jugadoresP08Iguales(a.jugadores[i], b.jugadores[i])) { //compara el estado y las metricas del mismo participante
            return false; //detiene la revision si encuentra un jugador diferente
        }
    }

    const Jugador* actualA = verJugadorActual(a.ronda); //obtiene al participante que tiene el turno en la primera partida
    const Jugador* actualB = verJugadorActual(b.ronda); //obtiene al participante actual de la segunda partida

    if ((actualA == nullptr) != (actualB == nullptr)) { //detecta si solo una ronda tiene jugador actual
        return false; //rechaza ese estado diferente de la ronda
    }

    if (actualA != nullptr && actualB != nullptr && actualA->alias != actualB->alias) { //compara los alias cuando ambas rondas tienen un jugador actual
        return false; //informa que el turno corresponde a jugadores distintos
    }

    if (!colasP08Iguales(a.colaOriginalRonda, b.colaOriginalRonda)) { //compara las apariciones originales de las dos rondas
        return false; //detecta una diferencia en las colas generadas
    }

    if (!bitacorasP08Iguales(a.bitacora, b.bitacora)) { //revisa los eventos acumulados en ambas partidas
        return false; //detecta una diferencia en el registro de lo ocurrido
    }

    return true; //confirma que coinciden todos los datos revisados
}

//ejecuta la partida por lotes con un limite de ticks por turno y de rondas para evitar un ciclo sin fin
//actualiza las metricas finales si la partida termina y devuelve falso si no puede completarla
static bool ejecutarPartidaP08(Partida& partida, const ScriptAcciones& script) {
    const int limiteTicks = 10000; //limita el tiempo de simulacion permitido para cada turno
    int rondasEjecutadas = 0; //cuenta cuantas rondas se han completado

    while (!partida.finalizada && rondasEjecutadas < partida.limiteOleadas + 2) { //continua mientras la partida este abierta y no agote el margen de rondas
        if (!ejecutarRondaModoLotes(partida, script, limiteTicks)) { //intenta completar la ronda con las acciones del script y de los bots
            return false; //informa que una ronda no pudo completarse
        }

        rondasEjecutadas++; //registra la ronda ejecutada antes de volver a comprobar el limite
    }

    if (!partida.finalizada) { //detecta si se agoto el margen sin terminar la partida
        return false; //rechaza una simulacion que quedo incompleta
    }

    actualizarMetricasFinalesPartida(partida); //calcula las proporciones y posiciones que se compararan

    return true; //confirma que la partida termino y tiene sus metricas actualizadas
}

//prepara dos partidas con semilla cuarenta y dos un humano dos bots y un limite de tres oleadas
//ejecuta ambas con el script recibido y compara sus estados y los reportes escritos byte por byte
bool pruebaP08ConScript(const ScriptAcciones& script) {
    Partida partidaA; //guarda la primera simulacion
    Partida partidaB; //guarda la segunda simulacion independiente

    inicializarPartida(partidaA, 42u, 3); //establece la semilla y el limite de la primera partida
    inicializarPartida(partidaB, 42u, 3); //repite la configuracion en la segunda partida

    bool jugadoresAgregados = true; //recuerda si falla alguno de los seis registros

    if (!agregarJugadorPartida(partidaA, "HUMANO", false)) jugadoresAgregados = false; //agrega al participante que seguira el script en la primera partida
    if (!agregarJugadorPartida(partidaA, "BOT1", true)) jugadoresAgregados = false; //registra al primer bot de la primera partida
    if (!agregarJugadorPartida(partidaA, "BOT2", true)) jugadoresAgregados = false; //registra al segundo bot de la primera partida

    if (!agregarJugadorPartida(partidaB, "HUMANO", false)) jugadoresAgregados = false; //registra al mismo humano en la segunda partida
    if (!agregarJugadorPartida(partidaB, "BOT1", true)) jugadoresAgregados = false; //mantiene al primer bot en la misma posicion de registro
    if (!agregarJugadorPartida(partidaB, "BOT2", true)) jugadoresAgregados = false; //completa la segunda partida con el mismo segundo bot

    if (!jugadoresAgregados) { //comprueba si alguna partida quedo sin todos sus participantes
        destruirPartida(partidaA); //libera lo registrado en la primera partida
        destruirPartida(partidaB); //libera lo registrado en la segunda partida
        return false; //rechaza una preparacion incompleta
    }

    bool resultado = true; //conserva los fallos sin saltarse las comparaciones posteriores

    if (!partidasP08Iguales(partidaA, partidaB)) { //comprueba que ambas partidas empiecen con los mismos datos
        resultado = false; //marca una diferencia en el estado inicial
    }

    bool ejecucionA = ejecutarPartidaP08(partidaA, script); //ejecuta la primera partida hasta terminar o fallar
    bool ejecucionB = ejecutarPartidaP08(partidaB, script); //ejecuta la segunda aunque la primera no haya terminado correctamente

    if (!ejecucionA || !ejecucionB) { //revisa que las dos simulaciones hayan terminado
        resultado = false; //conserva el fallo de cualquiera de las ejecuciones
    }

    if (!partidasP08Iguales(partidaA, partidaB)) { //compara los datos obtenidos despues de ejecutar ambas partidas
        resultado = false; //marca una diferencia entre los resultados de las simulaciones
    }

    const string rutaA = "reporte_p08_A.txt"; //define el archivo que recibira el primer reporte
    const string rutaB = "reporte_p08_B.txt"; //usa otro archivo para conservar el segundo reporte

    if (!guardarReportePartida(partidaA, rutaA)) { //escribe el reporte de la primera partida
        resultado = false; //marca el fallo si no se pudo guardar por completo
    }

    if (!guardarReportePartida(partidaB, rutaB)) { //escribe el reporte de la segunda partida
        resultado = false; //marca el fallo aunque el primer reporte se haya guardado
    }

    if (!compararArchivosExactos(rutaA, rutaB)) { //lee los dos reportes y exige que coincidan byte por byte
        resultado = false; //rechaza una diferencia de contenido o un fallo de lectura
    }

    destruirPartida(partidaA); //libera los nodos y eventos de la primera simulacion
    destruirPartida(partidaB); //libera los nodos y eventos de la segunda simulacion

    return resultado; //devuelve si la preparacion las ejecuciones y las comparaciones fueron correctas
}

//carga el archivo de acciones de p08 y exige que contenga acciones antes de ejecutar la prueba de repeticion
bool pruebaP08() {
    ScriptAcciones script; //guarda las acciones que compartiran ambas partidas
    inicializarScriptAcciones(script); //prepara la lista vacia antes de cargar el archivo

    if (!cargarScriptAcciones(script, "p08_script.txt")) { //lee el archivo de acciones desde el directorio de trabajo
        destruirScriptAcciones(script); //libera cualquier accion que haya quedado cargada
        return false; //informa que no se pudo abrir el archivo necesario
    }

    if (script.tam == 0) { //comprueba que el archivo haya aportado al menos una accion valida
        destruirScriptAcciones(script); //deja el script liberado antes de salir
        return false; //rechaza la prueba sin acciones cargadas
    }

    bool resultado = pruebaP08ConScript(script); //ejecuta y compara las partidas usando el mismo script

    destruirScriptAcciones(script); //libera las acciones despues de completar la prueba

    return resultado; //devuelve el resultado de la comparacion de ambas partidas
}