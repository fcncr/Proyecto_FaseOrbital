#include "Aplicacion.h"
#include <iomanip>
#include <iostream>
#include <fstream>

#include "EjecutorPruebas.h"
#include "Partida.h"
#include "ScriptAcciones.h"
#include "HistorialPartidas.h"
#include "Persistencia.h"
#include "Estadisticas.h"
#include "Ranking.h"
#include "VistaAllegro.h"
#include "InterfazAllegro.h"
#include "ControladorAllegro.h"
#include "Bot.h"

using namespace std;

//revisa si el alias corresponde a un bot
static bool esAliasBot(const string& alias) {
    return alias.size() >= 3 && alias[0] == 'B' && alias[1] == 'O' && alias[2] == 'T'; //comprueba que empiece con bot en mayusculas
}

//comprueba si se puede abrir un archivo
static bool existeArchivo(const string& ruta) {
    ifstream archivo(ruta); //intenta abrir el archivo para lectura
    return archivo.is_open(); //informa si pudo abrirlo
}

//busca el ultimo identificador de partida utilizado
static int obtenerSiguienteIdPartida(const HistorialPartidas& historial) {
    int mayorId = 0; //guarda el identificador mas grande encontrado
    const NodoRegistroPartida* actual = historial.cabeza; //empieza desde el primer registro

    while (actual != nullptr) { //recorre todo el historial
        if (actual->dato.idPartida > mayorId) { //comprueba si encontro un identificador mayor
            mayorId = actual->dato.idPartida; //actualiza el mayor identificador
        }

        actual = actual->sig; //avanza al siguiente registro
    }

    return mayorId + 1; //devuelve el identificador para la partida nueva
}

//agrega los participantes recibidos a la partida
static bool registrarJugadores(Partida& partida, const string aliasJugadores[], int cantidadJugadores) {
    for (int i = 0; i < cantidadJugadores; i++) { //recorre los participantes en el orden indicado
        const string& alias = aliasJugadores[i]; //obtiene el alias sin copiarlo
        bool esBot = esAliasBot(alias); //determina si el participante sera automatico

        if (!agregarJugadorPartida(partida, alias, esBot)) { //intenta registrar el participante
            return false; //detiene el registro si ocurre un error
        }
    }

    return true; //confirma que todos fueron registrados
}

//muestra las posiciones finales y determina el ganador
static void mostrarResultadoPartida(const Partida& partida) {
    cout << '\n';
    cout << "=== RESULTADO ===" << '\n'; //muestra el encabezado de resultados
    cout << '\n';

    for (int posicion = 1; posicion <= partida.cantidadJugadores; posicion++) { //recorre las posiciones posibles
        for (int i = 0; i < partida.cantidadJugadores; i++) { //busca los jugadores de cada posicion
            const Jugador& jugador = partida.jugadores[i]; //consulta los datos del participante

            if (jugador.metricas.posicionFinal == posicion) { //comprueba si ocupa la posicion actual
                cout << posicion << ". " << jugador.alias << "   " << jugador.puntaje << " pts" << '\n'; //muestra sus resultados
            }
        }
    }

    double precisiones[6]; //guarda las precisiones necesarias para el desempate

    for (int i = 0; i < partida.cantidadJugadores; i++) { //recorre los jugadores registrados
        precisiones[i] = partida.jugadores[i].metricas.precisionColor; //copia la precision del jugador
    }

    const Jugador* ganador = determinarGanadorPartida(partida, precisiones); //aplica las reglas de desempate
    cout << '\n';

    if (ganador != nullptr) { //comprueba si existe un ganador definido
        cout << "Ganador: " << ganador->alias << '\n'; //muestra al ganador
    }
    else {
        cout << "Ganador: empate" << '\n'; //informa que el empate no se pudo resolver
    }
}

//muestra las estadisticas historicas de un jugador
static void mostrarEstadisticasJugador(const EstadisticasJugador& estadisticas) {
    cout << "=== " << estadisticas.alias << " ===" << '\n'; //identifica al jugador consultado
    cout << "Partidas: " << estadisticas.partidasJugadas << '\n'; //muestra las partidas jugadas
    cout << "Victorias: " << estadisticas.victorias << '\n'; //muestra las victorias
    cout << "Mejor puntaje: " << estadisticas.mejorPuntaje << '\n'; //muestra el mayor puntaje
    cout << "Peor puntaje: " << estadisticas.peorPuntaje << '\n'; //muestra el menor puntaje

    cout << fixed << setprecision(2); //utiliza dos decimales para los resultados estadisticos
    cout << "Media: " << estadisticas.mediaPuntaje << '\n'; //muestra el promedio
    cout << "Mediana: " << estadisticas.medianaPuntaje << '\n'; //muestra la mediana
    cout << "Desviacion estandar: " << estadisticas.desviacionEstandar << '\n'; //muestra la desviacion poblacional
    cout << "Precision promedio: " << estadisticas.precisionColorPromedio * 100.0 << "%" << '\n'; //convierte la precision en porcentaje

    if (estadisticas.mejorColorDisponible) { //comprueba si hay impactos suficientes para calcular el mejor color
        cout << "Mejor color: " << nombreColorEstadistica(estadisticas.mejorColor) << '\n'; //muestra el nombre del color
    }
    else {
        cout << "Mejor color: no disponible" << '\n'; //informa que no se puede determinar
    }

    if (estadisticas.tipoMasDanoDisponible) { //comprueba si se pudo identificar el enemigo que mas dano causo
        cout << "Tipo que mas dano causa: " << nombreTipoEnemigoEstadistica(estadisticas.tipoMasDano) << '\n'; //muestra el tipo de enemigo
    }
    else {
        cout << "Tipo que mas dano causa: no disponible" << '\n'; //informa que no hay un resultado disponible
    }

    cout << "Histograma:" << '\n'; //identifica la seccion del histograma
    imprimirHistogramaASCII(estadisticas.histograma, cout); //imprime los diez intervalos de puntajes
    cout << '\n';
}

//ejecuta un turno usando acciones del jugador o del bot
static bool ejecutarTurnoGrafico(Partida& partida, VistaAllegro& vista, EstadoInterfaz& estado, bool& cerrarAplicacion, bool& abandonarPartida) {
    Jugador* jugador = verJugadorActualPartida(partida); //obtiene el jugador que debe realizar el turno

    if (jugador == nullptr) { //comprueba si existe un participante activo
        return false; //informa que no se puede ejecutar el turno
    }

    if (partida.numeroRonda > jugador->oleadaAlcanzada) { //comprueba si esta comenzando otra oleada
        jugador->oleadaAlcanzada = partida.numeroRonda; //actualiza la oleada del jugador
        jugador->metricas.oleadaAlcanzada = partida.numeroRonda; //actualiza la metrica correspondiente
    }

    //cada participante recibe una copia independiente de las apariciones
    ColaSpawn colaJugador; //crea la cola que utilizara este turno
    inicializarColaSpawn(colaJugador); //inicializa la cola
    prepararColaJugador(partida, colaJugador); //copia la oleada sin compartir nodos con la original

    Motor motor; //crea el motor que ejecutara el turno
    inicializarMotor(motor, jugador, partida.numeroRonda, colaJugador, &partida.bitacora); //relaciona el motor con el jugador y la bitacora

    EstadoControles controles; //guarda el estado de las teclas
    inicializarEstadoControles(controles); //inicializa los controles

    bool pausado = false; //indica si el juego esta en pausa
    estado.pantallaActual = PANTALLA_JUEGO; //establece la pantalla actual

    al_flush_event_queue(vista.colaEventos); //quita los eventos pendientes de otras pantallas
    al_start_timer(vista.timer); //inicia los eventos del temporizador

    dibujarMotor(vista, motor, pausado); //muestra el estado inicial del turno

    while (!cerrarAplicacion && !abandonarPartida && !motor.turnoTerminado) { //mantiene la simulacion hasta terminar o abandonar
        ALLEGRO_EVENT evento; //recibe los eventos de Allegro
        al_wait_for_event(vista.colaEventos, &evento); //espera el siguiente evento

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //detecta el cierre de la ventana
            cerrarAplicacion = true; //solicita terminar la aplicacion
        }
        else {
            procesarEventoControlador(evento, controles); //actualiza las teclas y acciones pendientes

            if (consumirAlternarPausa(controles)) { //comprueba si se presiono el control de pausa
                pausado = !pausado; //activa o desactiva la pausa

                if (pausado) { //comprueba si el juego acaba de pausarse
                    estado.pantallaActual = PANTALLA_PAUSA; //activa la pantalla de pausa
                }
                else {
                    estado.pantallaActual = PANTALLA_JUEGO; //vuelve a la pantalla de juego
                }
            }

            bool solicitarVolverMenu = consumirVolverMenu(controles); //consulta si el jugador solicito regresar al menu

            if (solicitarVolverMenu && pausado) { //permite abandonar solamente desde la pausa
                abandonarPartida = true; //marca la partida como abandonada
            }

            if (evento.type == ALLEGRO_EVENT_TIMER && !pausado && !abandonarPartida) { //procesa un tick cuando el temporizador avanza
                AccionMotor accion; //guarda la accion que se aplicara

                if (jugador->esBot) { //comprueba si el jugador es automatico
                    accion = obtenerAccionBot(motor); //obtiene la accion con la heuristica del bot
                }
                else {
                    accion = obtenerAccionAllegro(controles, motor); //obtiene la accion del teclado
                }

                procesarTick(motor, accion); //ejecuta las reglas de un tick completo
            }
        }

        if (!cerrarAplicacion && !abandonarPartida && al_is_event_queue_empty(vista.colaEventos)) { //dibuja cuando ya no quedan eventos pendientes
            dibujarMotor(vista, motor, pausado); //actualiza los elementos visibles
        }
    }

    al_stop_timer(vista.timer); //detiene los eventos de tiempo al salir del turno
    destruirMotor(motor); //libera las listas y la cola interna del motor
    destruirColaSpawn(colaJugador); //libera la copia de la oleada utilizada

    return true; //devuelve el control a la ronda
}

//ejecuta los turnos de todos los participantes activos de una ronda
static bool ejecutarRondaGrafica(Partida& partida, VistaAllegro& vista, EstadoInterfaz& estado, bool& cerrarAplicacion, bool& abandonarPartida) {
    if (partida.finalizada || cantidadJugadoresActivos(partida) <= 0) { //comprueba si la ronda puede comenzar
        return false; //rechaza una ronda sin participantes activos
    }

    generarColaOriginalRonda(partida); //genera una sola oleada para todos los jugadores
    int turnosDeLaRonda = cantidadJugadoresActivos(partida); //guarda los turnos que corresponden a esta ronda

    for (int i = 0; i < turnosDeLaRonda; i++) { //recorre los turnos previstos
        Jugador* jugador = verJugadorActualPartida(partida); //consulta el participante actual

        if (jugador == nullptr) { //comprueba que exista un jugador activo
            return false; //informa que no se puede continuar
        }

        mostrarAnuncioTurno(vista, estado, *jugador, partida.numeroRonda, cerrarAplicacion); //muestra a quien le corresponde jugar

        if (cerrarAplicacion) { //comprueba si se cerro la ventana durante el anuncio
            return true; //deja la ronda sin continuar los turnos
        }

        cout << "Ronda " << partida.numeroRonda << " - Turno de " << jugador->alias; //muestra el turno actual en consola

        if (jugador->esBot) { //comprueba si el participante es automatico
            cout << " [BOT]"; //identifica el turno del bot
        }

        cout << '\n';

        if (!ejecutarTurnoGrafico(partida, vista, estado, cerrarAplicacion, abandonarPartida)) { //intenta ejecutar el turno
            return false; //informa si fallo la ejecucion
        }

        if (cerrarAplicacion || abandonarPartida) { //comprueba si se interrumpio el turno
            return true; //sale sin finalizar una ronda incompleta
        }

        bool eliminado = retirarJugadorActualSiEliminado(partida); //retira al jugador que haya perdido toda su vida

        if (partida.finalizada) { //comprueba si ya terminaron todos los participantes
            break; //deja de ejecutar los turnos restantes
        }

        if (i < turnosDeLaRonda - 1 && !eliminado) { //comprueba si quedan turnos y el jugador no fue retirado
            avanzarTurnoPartida(partida); //avanza en el sentido que corresponde a esta ronda
        }
    }

    if (!cerrarAplicacion && !abandonarPartida) { //comprueba si la ronda termino normalmente
        mostrarResumenOleada(vista, estado, partida, cerrarAplicacion); //muestra los resultados de la oleada
    }

    if (!partida.finalizada) { //comprueba si todavia no se marco el fin de partida
        finalizarRondaPartida(partida); //decide si termina o avanza a la siguiente ronda
    }

    return true; //informa que termino la ejecucion de esta ronda
}

//imprime una linea de tiempo usando los eventos de la partida
static void mostrarLineaTiempoPartida(const Partida& partida) {
    cout << '\n';
    cout << "=== LINEA DE TIEMPO ===" << '\n' << '\n'; //identifica el reporte por oleadas

    for (int i = 0; i < partida.cantidadJugadores; i++) { //recorre los participantes registrados
        const Jugador& jugador = partida.jugadores[i]; //consulta el jugador actual

        imprimirLineaTiempoJugador(partida.bitacora, jugador.alias, jugador.oleadaAlcanzada, cout); //calcula el resumen a partir de la bitacora
        cout << '\n'; //separa los reportes de los jugadores
    }
}

//guarda los resultados completos y despues actualiza el historial en memoria
static bool guardarPartidaGrafica(Partida& partida, const string& rutaHistorial, HistorialPartidas& historial) {
    HistorialPartidas nuevosResultados; //crea una lista para la partida que acaba de terminar
    inicializarHistorial(nuevosResultados); //deja la lista vacia

    int idPartida = obtenerSiguienteIdPartida(historial); //obtiene un identificador que todavia no se ha utilizado
    agregarResultadosPartidaHistorial(partida, idPartida, "grafico", nuevosResultados); //crea los registros de los participantes

    if (!guardarPartidaCompletaCSV(rutaHistorial, nuevosResultados, cerr)) { //intenta guardar el historial principal y su detalle
        destruirHistorial(nuevosResultados); //libera los registros temporales
        return false; //informa que el guardado no pudo completarse
    }

    const NodoRegistroPartida* actual = nuevosResultados.cabeza; //empieza a recorrer los resultados nuevos

    while (actual != nullptr) { //agrega los nuevos resultados al historial en memoria
        agregarRegistroFinal(historial, actual->dato); //copia los datos del participante
        actual = actual->sig; //avanza al siguiente registro
    }

    destruirHistorial(nuevosResultados); //libera la lista temporal porque sus datos ya fueron copiados
    return true; //confirma que el guardado termino correctamente
}

//administra el modo grafico manteniendo el historial en memoria
int ejecutarAplicacionGrafica(const ConfiguracionPrograma& config) {
    HistorialPartidas historial; //guarda los resultados historicos durante toda la sesion
    inicializarHistorial(historial); //inicializa la lista vacia

    if (existeArchivo(config.rutaHistorial)) { //comprueba si hay un historial anterior
        if (!cargarHistorialCSV(config.rutaHistorial, historial, cerr)) { //intenta cargar los registros principales
            cerr << "No se pudo cargar el historial al iniciar la aplicacion." << '\n'; //informa el error de lectura
            destruirHistorial(historial); //libera los registros que se hayan cargado
            return 1; //termina la aplicacion con error
        }

        if (!cargarDetalleHistorialCSV(config.rutaHistorial, historial, cerr)) { //completa los registros con su detalle historico
            cerr << "No se pudo cargar el detalle historico." << '\n'; //informa el error del archivo auxiliar
            destruirHistorial(historial); //libera los registros del historial
            return 1; //termina la aplicacion con error
        }
    }

    VistaAllegro vista; //guarda los recursos de Allegro

    if (!inicializarVistaAllegro(vista, 800, 600)) { //crea la ventana, la fuente, los eventos y el temporizador
        cerr << "No se pudo inicializar Allegro." << '\n'; //informa que fallo la inicializacion
        destruirHistorial(historial); //libera el historial antes de salir
        return 1; //termina la aplicacion con error
    }

    EstadoInterfaz estado; //guarda la pantalla actual y la opcion seleccionada
    inicializarEstadoInterfaz(estado); //prepara el menu inicial

    bool cerrarAplicacion = false; //indica si el usuario solicito cerrar el programa
    bool resultado = true; //guarda el resultado general de la ejecucion

    while (!cerrarAplicacion) { //mantiene disponible el menu hasta que se solicite salir
        OpcionMenuPrincipal opcion = ejecutarMenuPrincipal(vista, estado, cerrarAplicacion); //muestra las opciones del menu

        if (cerrarAplicacion || opcion == MENU_SALIR) { //comprueba si el usuario quiere terminar
            break; //sale del menu para liberar los recursos
        }

        if (opcion == MENU_ESTADISTICAS) { //comprueba si se seleccionaron las estadisticas
            mostrarEstadisticasHistoricas(vista, estado, historial, cerrarAplicacion); //muestra las estadisticas del historial en memoria
            continue; //vuelve al menu principal
        }

        if (opcion == MENU_RANKING) { //comprueba si se selecciono el ranking
            mostrarRankingGrafico(vista, estado, historial, cerrarAplicacion); //muestra los diez primeros jugadores
            continue; //vuelve al menu principal
        }

        RegistroGrafico registro; //guarda los alias y la cantidad de jugadores

        bool registroCompletado = ejecutarRegistroJugadores(vista, estado, registro, cerrarAplicacion); //solicita los datos para iniciar la partida

        if (cerrarAplicacion) { //comprueba si el usuario cerro el registro
            break; //termina el programa
        }

        if (!registroCompletado) { //comprueba si se cancelo el registro
            continue; //vuelve al menu sin crear una partida
        }

        Partida partida; //crea el estado completo de la partida
        inicializarPartida(partida, config.semilla, config.oleadas); //utiliza la semilla y el limite configurado

        if (!registrarJugadores(partida, registro.aliasJugadores, registro.cantidadJugadores)) { //intenta agregar los participantes
            cerr << "No se pudieron registrar los jugadores graficos." << '\n'; //informa el error de registro
            destruirPartida(partida); //libera los datos creados
            resultado = false; //marca el error general
            break; //termina la sesion grafica
        }

        bool abandonarPartida = false; //indica si el jugador decidio abandonar

        while (!partida.finalizada && !cerrarAplicacion && !abandonarPartida) { //ejecuta las rondas hasta terminar o salir
            if (!ejecutarRondaGrafica(partida, vista, estado, cerrarAplicacion, abandonarPartida)) { //intenta ejecutar la ronda actual
                resultado = false; //marca el error si la ronda fallo
                break; //sale del ciclo de rondas
            }
        }

        if (!resultado) { //comprueba si fallo la ejecucion
            destruirPartida(partida); //libera la partida incompleta
            break; //termina el ciclo principal
        }

        if (partida.finalizada && !abandonarPartida) { //procesa solamente las partidas terminadas
            actualizarMetricasFinalesPartida(partida); //actualiza el resultado y las posiciones finales

            bool guardadoCorrecto = guardarPartidaGrafica(partida, config.rutaHistorial, historial); //guarda antes de mostrar la pantalla final
            mostrarLineaTiempoPartida(partida); //imprime los resultados de la bitacora por oleada

            if (!guardadoCorrecto) { //comprueba si los archivos no pudieron guardarse
                resultado = false; //marca el error de la sesion

                if (!cerrarAplicacion) { //comprueba si puede mostrar una pantalla de error
                    mostrarPantallaInformativa(vista, estado, PANTALLA_GAME_OVER, "ERROR DE GUARDADO", "No se completo el guardado. Revise la consola.", cerrarAplicacion); //informa que fallo el guardado
                }
            }
            else if (!cerrarAplicacion) { //muestra los resultados cuando el guardado funciona
                mostrarFinPartida(vista, estado, partida, cerrarAplicacion); //abre la pantalla final con las posiciones
            }
        }

        destruirPartida(partida); //libera la lista de turnos, la cola y la bitacora

        if (!resultado) { //comprueba si ocurrio algun error
            break; //evita iniciar otra partida en una sesion incorrecta
        }
    }

    destruirVistaAllegro(vista); //libera los recursos de la ventana
    destruirHistorial(historial); //libera todos los registros historicos en memoria

    return resultado ? 0 : 1; //indica si la ejecucion termino correctamente
}

//calcula porcentajes evitando divisiones entre cero
static double calcularPorcentajeReporteLotes(long long cantidad, long long total) {
    if (total <= 0) { //comprueba si no existe un total valido
        return 0.0; //evita una division incorrecta
    }

    return 100.0 * static_cast<double>(cantidad) / static_cast<double>(total); //calcula el porcentaje con decimales
}

//muestra todas las metricas de una partida ejecutada por lotes
static void imprimirReporteDetalladoLotes(const Partida& partida, const HistorialPartidas& resultados, ostream& salida) {
    ios::fmtflags formatoOriginal = salida.flags(); //guarda el formato que tenia la salida
    streamsize precisionOriginal = salida.precision(); //guarda la precision anterior

    salida << fixed << setprecision(2); //utiliza dos decimales para el reporte

    salida << '\n';
    salida << "========================================" << '\n';
    salida << "       REPORTE DEL MODO POR LOTES        " << '\n'; //muestra el encabezado del reporte
    salida << "========================================" << '\n';

    const NodoRegistroPartida* actual = resultados.cabeza; //comienza con los registros de la partida

    if (actual == nullptr) { //comprueba si no se generaron resultados
        salida << "No hay resultados para mostrar." << '\n'; //informa que no hay participantes
    }

    while (actual != nullptr) { //recorre todos los registros de la partida
        const RegistroPartida& registro = actual->dato; //consulta los datos del participante

        int bajasPorColor[3] = { 0, 0, 0 }; //guarda las bajas de cian, magenta y ambar

        const NodoEvento* eventoActual = partida.bitacora.cabeza; //empieza a recorrer los eventos de la partida

        while (eventoActual != nullptr) { //revisa todas las bajas registradas
            const Evento& evento = eventoActual->dato; //consulta el evento actual

            if (evento.jugador == registro.jugador && evento.tipoEvento == BAJA) { //selecciona las bajas del participante
                if (evento.color >= 0 && evento.color < 3) { //comprueba que el color sea valido
                    bajasPorColor[evento.color]++; //suma la baja al color correspondiente
                }
            }

            eventoActual = eventoActual->sig; //avanza al siguiente evento
        }

        int impactosTotales = registro.correctos + registro.erroneos; //suma todos los impactos registrados

        int bajasTotales = registro.bajasDron + registro.bajasGirador + registro.bajasDivisor + registro.bajasMini + registro.bajasTanque + registro.bajasCamaleon; //suma las bajas de todos los enemigos

        salida << '\n';
        salida << "----------------------------------------" << '\n';
        salida << "JUGADOR: " << registro.jugador << '\n'; //identifica al participante
        salida << "----------------------------------------" << '\n';

        salida << "ID de partida: " << registro.idPartida << '\n'; //muestra el identificador
        salida << "Semilla: " << registro.semilla << '\n'; //muestra la semilla utilizada
        salida << "Modo: " << registro.modo << '\n'; //muestra el modo de ejecucion
        salida << "Es bot: " << (registro.esBot ? "Si" : "No") << '\n'; //indica si el jugador es automatico
        salida << "Posicion final: " << registro.posicion << '\n'; //muestra su posicion

        salida << '\n';
        salida << "=== RENDIMIENTO GENERAL ===" << '\n';

        salida << "Puntaje: " << registro.puntaje << '\n'; //muestra el puntaje final
        salida << "Oleada alcanzada: " << registro.oleada << '\n'; //muestra la ultima oleada alcanzada
        salida << "Disparos: " << registro.disparos << '\n'; //muestra los disparos realizados
        salida << "Impactos correctos: " << registro.correctos << '\n'; //muestra los aciertos
        salida << "Impactos erroneos: " << registro.erroneos << '\n'; //muestra los errores

        salida << "Precision de color: " << calcularPorcentajeReporteLotes(registro.correctos, impactosTotales) << "%" << '\n'; //calcula la precision por impactos
        salida << "Eficiencia de disparo: " << calcularPorcentajeReporteLotes(registro.correctos, registro.disparos) << "%" << '\n'; //calcula los aciertos respecto a disparos
        salida << "Combo maximo: " << registro.comboMax << '\n'; //muestra el mayor combo
        salida << "Pulsos utilizados: " << registro.pulsos << '\n'; //muestra las activaciones del pulso
        salida << "Dano total recibido: " << registro.danoTotal << '\n'; //muestra el dano recibido
        salida << "Ticks sobrevividos: " << registro.ticks << '\n'; //muestra el tiempo de simulacion jugado

        salida << '\n';
        salida << "=== BAJAS POR TIPO DE ENEMIGO ===" << '\n';

        salida << "Dron: " << registro.bajasDron << '\n'; //muestra las bajas de drones
        salida << "Girador: " << registro.bajasGirador << '\n'; //muestra las bajas de giradores
        salida << "Divisor: " << registro.bajasDivisor << '\n'; //muestra las bajas de divisores
        salida << "Mini: " << registro.bajasMini << '\n'; //muestra las bajas de minis
        salida << "Tanque: " << registro.bajasTanque << '\n'; //muestra las bajas de tanques
        salida << "Camaleon: " << registro.bajasCamaleon << '\n'; //muestra las bajas de camaleones
        salida << "Total de bajas: " << bajasTotales << '\n'; //muestra el total de enemigos eliminados

        salida << '\n';
        salida << "=== BAJAS POR COLOR ===" << '\n';

        salida << "Cian: " << bajasPorColor[0] << '\n'; //muestra las bajas de cian
        salida << "Magenta: " << bajasPorColor[1] << '\n'; //muestra las bajas de magenta
        salida << "Ambar: " << bajasPorColor[2] << '\n'; //muestra las bajas de ambar

        if (registro.detalleDisponible) { //comprueba que exista el detalle de la partida
            salida << '\n';
            salida << "=== PRECISION POR COLOR ===" << '\n';

            salida << "Cian: " << calcularPorcentajeReporteLotes(registro.correctosC, registro.correctosC + registro.erroneosC) << "%" << '\n'; //muestra la precision de cian
            salida << "Magenta: " << calcularPorcentajeReporteLotes(registro.correctosM, registro.correctosM + registro.erroneosM) << "%" << '\n'; //muestra la precision de magenta
            salida << "Ambar: " << calcularPorcentajeReporteLotes(registro.correctosA, registro.correctosA + registro.erroneosA) << "%" << '\n'; //muestra la precision de ambar

            salida << '\n';
            salida << "=== DANO RECIBIDO POR TIPO ===" << '\n';

            salida << "Dron: " << registro.danoDron << '\n'; //muestra el dano de drones
            salida << "Girador: " << registro.danoGirador << '\n'; //muestra el dano de giradores
            salida << "Divisor: " << registro.danoDivisor << '\n'; //muestra el dano de divisores
            salida << "Mini: " << registro.danoMini << '\n'; //muestra el dano de minis
            salida << "Tanque: " << registro.danoTanque << '\n'; //muestra el dano de tanques
            salida << "Camaleon: " << registro.danoCamaleon << '\n'; //muestra el dano de camaleones
        }
        else {
            salida << '\n';
            salida << "Detalle por color y dano por tipo: no disponible." << '\n'; //informa que el detalle no esta disponible
        }

        actual = actual->sig; //continua con el siguiente participante
    }

    salida << '\n';
    salida << "========================================" << '\n';
    salida << "            FIN DEL REPORTE             " << '\n'; //marca el final del reporte
    salida << "========================================" << '\n';

    salida.flags(formatoOriginal); //recupera el formato anterior de la salida
    salida.precision(precisionOriginal); //recupera la precision anterior
}

//ejecuta una partida por lotes y guarda todos sus resultados
int ejecutarAplicacionLotes(const ConfiguracionPrograma& config) {
    Partida partida; //guarda el estado de la partida
    ScriptAcciones script; //guarda las acciones del archivo de script
    HistorialPartidas historialExistente; //guarda los registros de partidas anteriores
    HistorialPartidas nuevosResultados; //guarda los registros de esta partida

    inicializarPartida(partida, config.semilla, config.oleadas); //inicializa la partida con la semilla indicada
    inicializarScriptAcciones(script); //inicializa la lista de acciones
    inicializarHistorial(historialExistente); //inicializa el historial anterior
    inicializarHistorial(nuevosResultados); //inicializa la lista de resultados nuevos

    if (!registrarJugadores(partida, config.jugadores, config.cantidadJugadores)) { //intenta registrar a todos los participantes
        cerr << "No se pudieron registrar los jugadores." << '\n'; //informa el error de registro

        destruirPartida(partida); //libera las estructuras de la partida
        destruirScriptAcciones(script); //libera los nodos de acciones
        destruirHistorial(historialExistente); //libera los registros anteriores
        destruirHistorial(nuevosResultados); //libera los resultados nuevos

        return 1; //termina la ejecucion con error
    }

    if (config.tieneScript) { //comprueba si se proporciono un script
        if (!cargarScriptAcciones(script, config.rutaScript)) { //intenta leer las acciones del archivo
            cerr << "No se pudo cargar el script: " << config.rutaScript << '\n'; //informa la ruta que produjo el error

            destruirPartida(partida); //libera las estructuras de la partida
            destruirScriptAcciones(script); //libera los nodos del script
            destruirHistorial(historialExistente); //libera los registros anteriores
            destruirHistorial(nuevosResultados); //libera los resultados nuevos

            return 1; //termina si no puede cargar el script
        }
    }

    if (existeArchivo(config.rutaHistorial)) { //comprueba si existen registros historicos
        if (!cargarHistorialCSV(config.rutaHistorial, historialExistente, cerr)) { //intenta cargar el archivo principal
            cerr << "No se pudo cargar el historial existente." << '\n'; //informa el error de lectura

            destruirPartida(partida); //libera la partida
            destruirScriptAcciones(script); //libera el script
            destruirHistorial(historialExistente); //libera el historial anterior
            destruirHistorial(nuevosResultados); //libera los resultados nuevos

            return 1; //termina si falla la carga
        }

        if (!cargarDetalleHistorialCSV(config.rutaHistorial, historialExistente, cerr)) { //intenta completar el detalle historico
            cerr << "No se pudo cargar el detalle historico." << '\n'; //informa el error del archivo auxiliar

            destruirPartida(partida); //libera la partida
            destruirScriptAcciones(script); //libera el script
            destruirHistorial(historialExistente); //libera el historial anterior
            destruirHistorial(nuevosResultados); //libera los resultados nuevos

            return 1; //termina si falla la carga del detalle
        }
    }

    int idPartida = obtenerSiguienteIdPartida(historialExistente); //calcula el identificador de la partida nueva
    const int limiteTicks = 10000; //establece un limite de seguridad para cada turno

    while (!partida.finalizada) { //ejecuta rondas hasta terminar la partida
        if (!ejecutarRondaModoLotes(partida, script, limiteTicks)) { //intenta ejecutar la ronda actual
            cerr << "La partida en modo lotes no termino correctamente." << '\n'; //informa el error de simulacion

            destruirPartida(partida); //libera la partida
            destruirScriptAcciones(script); //libera el script
            destruirHistorial(historialExistente); //libera el historial anterior
            destruirHistorial(nuevosResultados); //libera los resultados nuevos

            return 1; //termina si la partida no pudo completarse
        }
    }

    agregarResultadosPartidaHistorial(partida, idPartida, "lotes", nuevosResultados); //crea una fila por participante con sus resultados

    if (!guardarPartidaCompletaCSV(config.rutaHistorial, nuevosResultados, cerr)) { //guarda ambos archivos con el mismo identificador
        cerr << "No se pudo completar el guardado de la partida en modo lotes." << '\n'; //informa el error de persistencia

        destruirPartida(partida); //libera la partida
        destruirScriptAcciones(script); //libera el script
        destruirHistorial(historialExistente); //libera el historial anterior
        destruirHistorial(nuevosResultados); //libera los resultados nuevos

        return 1; //termina si el guardado falla
    }

    mostrarResultadoPartida(partida); //muestra las posiciones y el ganador
    imprimirReporteDetalladoLotes(partida, nuevosResultados, cout); //muestra las metricas de todos los participantes

    cout << '\n';
    cout << "========================================" << '\n';
    cout << "       LINEA DE TIEMPO POR JUGADOR       " << '\n'; //identifica el reporte de eventos
    cout << "========================================" << '\n';

    for (int i = 0; i < partida.cantidadJugadores; i++) { //recorre todos los participantes
        const Jugador& jugador = partida.jugadores[i]; //consulta al participante actual

        cout << '\n';
        cout << "----------------------------------------" << '\n';

        if (jugador.oleadaAlcanzada <= 0) { //comprueba si no llego a jugar ninguna oleada
            cout << "Jugador: " << jugador.alias << '\n'; //muestra su alias
            cout << "Sin oleadas jugadas." << '\n'; //informa que no existen turnos para resumir
        }
        else {
            imprimirLineaTiempoJugador(partida.bitacora, jugador.alias, jugador.oleadaAlcanzada, cout); //imprime los eventos de sus oleadas
        }

        cout << "----------------------------------------" << '\n';
    }

    cout << '\n';
    cout << "========== FIN LINEA DE TIEMPO =========" << '\n'; //indica que termino el reporte

    destruirPartida(partida); //libera las estructuras de la partida
    destruirScriptAcciones(script); //libera las acciones del script
    destruirHistorial(historialExistente); //libera los registros antiguos
    destruirHistorial(nuevosResultados); //libera los resultados de la partida

    return 0; //informa que termino correctamente
}

//carga el historial y muestra las estadisticas por consola
int ejecutarAplicacionEstadisticas(const string& archivo) {
    HistorialPartidas historial; //guarda los registros cargados
    Ranking ranking; //mantiene una entrada ordenada por jugador

    inicializarHistorial(historial); //inicializa el historial vacio
    inicializarRanking(ranking); //inicializa el ranking vacio

    if (!cargarHistorialCSV(archivo, historial, cerr)) { //intenta leer el archivo principal
        destruirRanking(ranking); //libera el ranking
        destruirHistorial(historial); //libera los registros
        return 1; //termina si falla la lectura
    }

    if (!cargarDetalleHistorialCSV(archivo, historial, cerr)) { //carga las metricas adicionales
        destruirRanking(ranking); //libera el ranking
        destruirHistorial(historial); //libera el historial
        return 1; //termina si falla el detalle
    }

    construirRankingDesdeHistorial(historial, ranking); //crea una entrada por cada alias

    cout << "=== ESTADISTICAS ===" << '\n'; //identifica el reporte de estadisticas
    cout << '\n';

    if (ranking.tam == 0) { //comprueba si no hay jugadores en el historial
        cout << "El historial no contiene jugadores." << '\n'; //informa que no hay resultados

        destruirRanking(ranking); //libera el ranking vacio
        destruirHistorial(historial); //libera el historial vacio
        return 0; //termina sin error
    }

    const NodoRanking* actual = ranking.cabeza; //empieza con el primer jugador

    while (actual != nullptr) { //recorre los jugadores del historial
        EstadisticasJugador estadisticas; //guarda los calculos del jugador actual

        if (calcularEstadisticasJugador(historial, actual->dato.alias, estadisticas)) { //calcula las metricas historicas
            mostrarEstadisticasJugador(estadisticas); //imprime los resultados obtenidos
        }

        actual = actual->sig; //avanza al siguiente jugador
    }

    destruirRanking(ranking); //libera los nodos del ranking
    destruirHistorial(historial); //libera los registros cargados

    return 0; //termina correctamente
}

//carga el historial y muestra el ranking por consola
int ejecutarAplicacionRanking(const string& archivo) {
    HistorialPartidas historial; //guarda los registros cargados
    Ranking ranking; //guarda los jugadores en orden de clasificacion

    inicializarHistorial(historial); //inicializa el historial vacio
    inicializarRanking(ranking); //inicializa el ranking vacio

    if (!cargarHistorialCSV(archivo, historial, cerr)) { //intenta leer el historial
        destruirRanking(ranking); //libera los nodos del ranking
        destruirHistorial(historial); //libera el historial
        return 1; //termina si falla la carga
    }

    construirRankingDesdeHistorial(historial, ranking); //ordena los jugadores por puntaje, precision y alias

    cout << "=== TOP 10 ===" << '\n'; //identifica la clasificacion historica
    cout << '\n';

    if (ranking.tam == 0) { //comprueba si no hay jugadores
        cout << "El historial no contiene jugadores." << '\n'; //informa que la clasificacion esta vacia
        destruirRanking(ranking); //libera el ranking
        destruirHistorial(historial); //libera los registros
        return 0; //termina sin error
    }

    const NodoRanking* actual = ranking.cabeza; //empieza desde la primera posicion
    int posicion = 1; //numera los lugares del ranking

    cout << fixed << setprecision(2); //muestra la precision con dos decimales

    while (actual != nullptr && posicion <= 10) { //muestra como maximo diez jugadores
        cout << posicion << ". " << actual->dato.alias << "   " << actual->dato.mejorPuntaje << " pts   " << actual->dato.precisionColor * 100.0 << "%" << '\n'; //imprime posicion, alias, puntaje y precision
        actual = actual->sig; //avanza al siguiente puesto
        posicion++; //actualiza la posicion que se mostrara
    }

    destruirRanking(ranking); //libera los nodos del ranking
    destruirHistorial(historial); //libera los registros cargados

    return 0; //informa que termino correctamente
}

//selecciona la forma en que se ejecutara el programa
int ejecutarAplicacion(const ConfiguracionPrograma& config) {
    switch (config.modo) { //consulta el modo indicado en los argumentos
    case MODO_GRAFICO:
        return ejecutarAplicacionGrafica(config); //abre el modo grafico

    case MODO_LOTES:
        return ejecutarAplicacionLotes(config); //ejecuta la simulacion por lotes

    case MODO_ESTADISTICAS:
        return ejecutarAplicacionEstadisticas(config.rutaHistorial); //muestra las estadisticas por consola

    case MODO_RANKING:
        return ejecutarAplicacionRanking(config.rutaHistorial); //muestra el ranking por consola

    case MODO_PRUEBAS:
        return ejecutarTodasLasPruebas() ? 0 : 1; //ejecuta las pruebas y devuelve su resultado

    case MODO_INVALIDO:
    default:
        cerr << "Modo de programa invalido." << '\n'; //informa que el modo no existe
        return 1; //termina por argumentos invalidos
    }
}