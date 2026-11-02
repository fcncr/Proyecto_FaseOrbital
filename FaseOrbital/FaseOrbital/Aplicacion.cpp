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

#ifndef FASE_ORBITAL_SIN_GRAFICOS
#include "VistaAllegro.h"
#include "InterfazAllegro.h"
#include "ControladorAllegro.h"
#include "Bot.h"
#endif

//reconoce como bot cualquier alias que empiece con las tres letras bot en mayusculas
static bool esAliasBot(const string& alias) {
    return alias.size() >= 3 && alias[0] == 'B' && alias[1] == 'O' && alias[2] == 'T'; //comprueba que empiece con bot en mayusculas
}

//comprueba si se puede abrir un archivo
static bool existeArchivo(const string& ruta) {
    ifstream archivo(ruta); //intenta abrir el archivo para lectura
    return archivo.is_open(); //informa si pudo abrirlo
}

//devuelve uno mas que el mayor identificador del historial o uno si esta vacio
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

//registra los alias en orden y asigna los bots por su prefijo sin deshacer los registros si uno falla
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

//muestra las posiciones ya calculadas y anuncia al ganador o el empate que persiste tras los desempates
static void mostrarResultadoPartida(const Partida& partida) {
    cout << '\n'; //separa los resultados de la salida anterior
    cout << "=== RESULTADO ===" << '\n'; //muestra el encabezado de resultados
    cout << '\n'; //deja espacio antes de las posiciones

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
    cout << '\n'; //separa el ganador de la lista de posiciones

    if (ganador != nullptr) { //comprueba si existe un ganador definido
        cout << "Ganador: " << ganador->alias << '\n'; //muestra al ganador
    }
    else { //maneja un resultado sin ganador unico
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
    else { //maneja la falta de impactos que permitan comparar los colores
        cout << "Mejor color: no disponible" << '\n'; //informa que no se puede determinar
    }

    if (estadisticas.tipoMasDanoDisponible) { //comprueba si se pudo identificar el enemigo que mas dano causo
        cout << "Tipo que mas dano causa: " << nombreTipoEnemigoEstadistica(estadisticas.tipoMasDano) << '\n'; //muestra el tipo de enemigo
    }
    else { //maneja la falta de detalle o de dano positivo por enemigo
        cout << "Tipo que mas dano causa: no disponible" << '\n'; //informa que no hay un resultado disponible
    }

    cout << "Histograma:" << '\n'; //identifica la seccion del histograma
    imprimirHistogramaASCII(estadisticas.histograma, cout); //imprime los diez intervalos de puntajes
    cout << '\n'; //separa este reporte del siguiente jugador
}

#ifndef FASE_ORBITAL_SIN_GRAFICOS

//ejecuta el turno con controles o bot y atiende la pausa y la salida de la partida
//devuelve falso si falta el jugador y deja las interrupciones indicadas en las banderas recibidas
static bool ejecutarTurnoGrafico(Partida& partida, VistaAllegro& vista, EstadoInterfaz& estado, bool& cerrarAplicacion, bool& abandonarPartida) {
    Jugador* jugador = verJugadorActualPartida(partida); //obtiene el jugador que debe realizar el turno

    if (jugador == nullptr) { //comprueba si existe un participante activo
        return false; //informa que no se puede ejecutar el turno
    }

    if (partida.numeroRonda > jugador->oleadaAlcanzada) { //comprueba si esta comenzando otra oleada
        jugador->oleadaAlcanzada = partida.numeroRonda; //actualiza la oleada del jugador
        jugador->metricas.oleadaAlcanzada = partida.numeroRonda; //actualiza la metrica correspondiente
    }

    Motor motor; //crea el motor que ejecutara el turno
    inicializarMotor(motor, jugador, partida.numeroRonda, partida.colaOriginalRonda, &partida.bitacora); //crea la copia de apariciones del motor y conserva intacta la cola de la ronda

    EstadoControles controles; //guarda el estado de las teclas
    inicializarEstadoControles(controles); //inicializa los controles

    bool pausado = false; //indica si el juego esta en pausa
    estado.pantallaActual = PANTALLA_JUEGO; //establece la pantalla actual

    al_flush_event_queue(vista.colaEventos); //quita los eventos pendientes de otras pantallas
    al_start_timer(vista.timer); //inicia los eventos del temporizador

    dibujarMotor(vista, motor, pausado); //muestra el estado inicial del turno

    while (!cerrarAplicacion && !abandonarPartida && !motor.turnoTerminado) { //mantiene la simulacion hasta terminar o abandonar
        ALLEGRO_EVENT evento; //recibe los eventos de allegro
        al_wait_for_event(vista.colaEventos, &evento); //espera el siguiente evento

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //detecta el cierre de la ventana
            cerrarAplicacion = true; //solicita terminar la aplicacion
        }
        else { //atiende los controles y el temporizador mientras la ventana sigue abierta
            procesarEventoControlador(evento, controles); //actualiza las teclas y acciones pendientes

            if (consumirAlternarPausa(controles)) { //comprueba si se presiono el control de pausa
                pausado = !pausado; //activa o desactiva la pausa

                if (pausado) { //comprueba si el juego acaba de pausarse
                    estado.pantallaActual = PANTALLA_PAUSA; //activa la pantalla de pausa
                }
                else { //reanuda el turno despues de la pausa
                    estado.pantallaActual = PANTALLA_JUEGO; //vuelve a la pantalla de juego
                }
            }

            bool solicitarVolverMenu = consumirVolverMenu(controles); //retira la solicitud aunque se haya hecho fuera de la pausa

            if (solicitarVolverMenu && pausado) { //permite abandonar solamente desde la pausa
                abandonarPartida = true; //marca la partida como abandonada
            }

            if (evento.type == ALLEGRO_EVENT_TIMER && !pausado && !abandonarPartida) { //procesa un tick cuando el temporizador avanza
                AccionMotor accion; //guarda la accion que se aplicara

                if (jugador->esBot) { //comprueba si el jugador es automatico
                    accion = obtenerAccionBot(motor); //obtiene la accion con la heuristica del bot
                }
                else { //consulta los controles del participante humano
                    accion = obtenerAccionAllegro(controles, motor); //obtiene la accion del teclado y el raton
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

    return true; //devuelve el control incluso si el turno se interrumpio al cerrar o abandonar
}

//genera una oleada comun y ejecuta los turnos retirando a quienes pierden toda la vida
//muestra el resumen al completar los turnos y deja las interrupciones indicadas en las banderas recibidas
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

        cout << '\n'; //cierra la linea que anuncia el turno

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

//imprime por jugador el resumen de eventos de cada oleada que alcanzo
static void mostrarLineaTiempoPartida(const Partida& partida) {
    cout << '\n'; //separa la linea de tiempo de la salida anterior
    cout << "=== LINEA DE TIEMPO ===" << '\n' << '\n'; //identifica el reporte por oleadas

    for (int i = 0; i < partida.cantidadJugadores; i++) { //recorre los participantes registrados
        const Jugador& jugador = partida.jugadores[i]; //consulta el jugador actual

        imprimirLineaTiempoJugador(partida.bitacora, jugador.alias, jugador.oleadaAlcanzada, cout); //calcula el resumen a partir de la bitacora
        cout << '\n'; //separa los reportes de los jugadores
    }
}

//actualiza las metricas finales y guarda los resultados en el historial y su archivo de detalle
//solo incorpora los registros al historial en memoria si el guardado termina correctamente
static bool guardarPartidaGrafica(Partida& partida, const string& rutaHistorial, HistorialPartidas& historial) {
    HistorialPartidas nuevosResultados; //crea una lista para la partida que acaba de terminar
    inicializarHistorial(nuevosResultados); //deja la lista vacia

    int idPartida = obtenerSiguienteIdPartida(historial); //obtiene un identificador que todavia no se ha utilizado
    agregarResultadosPartidaHistorial(partida, idPartida, "grafico", nuevosResultados); //actualiza las metricas finales y crea los registros de los participantes

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

//carga el historial y mantiene el menu para jugar y consultar resultados hasta salir
//guarda las partidas finalizadas y devuelve cero al salir normalmente o uno si ocurre un error
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

    VistaAllegro vista; //guarda los recursos de allegro

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

        if (partida.finalizada && !abandonarPartida) { //guarda una partida terminada aunque se cierre la ventana desde el resumen
            bool guardadoCorrecto = guardarPartidaGrafica(partida, config.rutaHistorial, historial); //calcula las metricas y guarda antes de mostrar la pantalla final
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
#endif
//calcula porcentajes evitando divisiones entre cero
static double calcularPorcentajeReporteLotes(long long cantidad, long long total) {
    if (total <= 0) { //comprueba si no existe un total valido
        return 0.0; //evita una division incorrecta
    }

    return 100.0 * static_cast<double>(cantidad) / static_cast<double>(total); //calcula el porcentaje con decimales
}

//imprime las metricas de los registros y cuenta las bajas por color desde la bitacora
//recupera el formato y la precision de la salida cuando termina el reporte
static void imprimirReporteDetalladoLotes(const Partida& partida, const HistorialPartidas& resultados, ostream& salida) {
    ios::fmtflags formatoOriginal = salida.flags(); //guarda el formato que tenia la salida
    streamsize precisionOriginal = salida.precision(); //guarda la precision anterior

    salida << fixed << setprecision(2); //utiliza dos decimales para el reporte

    salida << '\n'; //separa el reporte de los resultados anteriores
    salida << "========================================" << '\n'; //abre el encabezado del reporte
    salida << "       REPORTE DEL MODO POR LOTES        " << '\n'; //muestra el encabezado del reporte
    salida << "========================================" << '\n'; //cierra el encabezado del reporte

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

        salida << '\n'; //separa los datos de cada participante
        salida << "----------------------------------------" << '\n'; //abre el encabezado del jugador
        salida << "JUGADOR: " << registro.jugador << '\n'; //identifica al participante
        salida << "----------------------------------------" << '\n'; //cierra el encabezado del jugador

        salida << "ID de partida: " << registro.idPartida << '\n'; //muestra el identificador
        salida << "Semilla: " << registro.semilla << '\n'; //muestra la semilla utilizada
        salida << "Modo: " << registro.modo << '\n'; //muestra el modo de ejecucion
        salida << "Es bot: " << (registro.esBot ? "Si" : "No") << '\n'; //indica si el jugador es automatico
        salida << "Posicion final: " << registro.posicion << '\n'; //muestra su posicion

        salida << '\n'; //separa los datos de la partida de las metricas
        salida << "=== RENDIMIENTO GENERAL ===" << '\n'; //presenta los resultados generales del jugador

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

        salida << '\n'; //separa las bajas de las metricas generales
        salida << "=== BAJAS POR TIPO DE ENEMIGO ===" << '\n'; //presenta el desglose por enemigo

        salida << "Dron: " << registro.bajasDron << '\n'; //muestra las bajas de drones
        salida << "Girador: " << registro.bajasGirador << '\n'; //muestra las bajas de giradores
        salida << "Divisor: " << registro.bajasDivisor << '\n'; //muestra las bajas de divisores
        salida << "Mini: " << registro.bajasMini << '\n'; //muestra las bajas de minis
        salida << "Tanque: " << registro.bajasTanque << '\n'; //muestra las bajas de tanques
        salida << "Camaleon: " << registro.bajasCamaleon << '\n'; //muestra las bajas de camaleones
        salida << "Total de bajas: " << bajasTotales << '\n'; //muestra el total de enemigos eliminados

        salida << '\n'; //separa el desglose por color del desglose por tipo
        salida << "=== BAJAS POR COLOR ===" << '\n'; //presenta las bajas contadas en la bitacora

        salida << "Cian: " << bajasPorColor[0] << '\n'; //muestra las bajas de cian
        salida << "Magenta: " << bajasPorColor[1] << '\n'; //muestra las bajas de magenta
        salida << "Ambar: " << bajasPorColor[2] << '\n'; //muestra las bajas de ambar

        if (registro.detalleDisponible) { //comprueba que exista el detalle de la partida
            salida << '\n'; //separa la precision del conteo de bajas
            salida << "=== PRECISION POR COLOR ===" << '\n'; //presenta la relacion entre aciertos e impactos de cada color

            salida << "Cian: " << calcularPorcentajeReporteLotes(registro.correctosC, registro.correctosC + registro.erroneosC) << "%" << '\n'; //muestra la precision de cian
            salida << "Magenta: " << calcularPorcentajeReporteLotes(registro.correctosM, registro.correctosM + registro.erroneosM) << "%" << '\n'; //muestra la precision de magenta
            salida << "Ambar: " << calcularPorcentajeReporteLotes(registro.correctosA, registro.correctosA + registro.erroneosA) << "%" << '\n'; //muestra la precision de ambar

            salida << '\n'; //separa el dano recibido de las precisiones
            salida << "=== DANO RECIBIDO POR TIPO ===" << '\n'; //presenta el dano causado por cada tipo de enemigo

            salida << "Dron: " << registro.danoDron << '\n'; //muestra el dano de drones
            salida << "Girador: " << registro.danoGirador << '\n'; //muestra el dano de giradores
            salida << "Divisor: " << registro.danoDivisor << '\n'; //muestra el dano de divisores
            salida << "Mini: " << registro.danoMini << '\n'; //muestra el dano de minis
            salida << "Tanque: " << registro.danoTanque << '\n'; //muestra el dano de tanques
            salida << "Camaleon: " << registro.danoCamaleon << '\n'; //muestra el dano de camaleones
        }
        else { //avisa cuando el registro no incluye las metricas de detalle
            salida << '\n'; //separa el aviso del desglose de bajas
            salida << "Detalle por color y dano por tipo: no disponible." << '\n'; //informa que el detalle no esta disponible
        }

        actual = actual->sig; //continua con el siguiente participante
    }

    salida << '\n'; //separa el cierre del ultimo participante
    salida << "========================================" << '\n'; //abre el cierre del reporte
    salida << "            FIN DEL REPORTE             " << '\n'; //marca el final del reporte
    salida << "========================================" << '\n'; //cierra visualmente el reporte

    salida.flags(formatoOriginal); //recupera el formato anterior de la salida
    salida.precision(precisionOriginal); //recupera la precision anterior
}

//simula la partida con bots o acciones del script y guarda ambos archivos antes de imprimir los reportes
//limita cada turno a diez mil ticks y devuelve cero al terminar o uno si falla algun paso
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

    cout << '\n'; //separa la linea de tiempo del reporte de metricas
    cout << "========================================" << '\n'; //abre el encabezado de la linea de tiempo
    cout << "       LINEA DE TIEMPO POR JUGADOR       " << '\n'; //identifica el reporte de eventos
    cout << "========================================" << '\n'; //cierra el encabezado de la linea de tiempo

    for (int i = 0; i < partida.cantidadJugadores; i++) { //recorre todos los participantes
        const Jugador& jugador = partida.jugadores[i]; //consulta al participante actual

        cout << '\n'; //separa los resumenes de los participantes
        cout << "----------------------------------------" << '\n'; //abre la linea de tiempo del jugador

        if (jugador.oleadaAlcanzada <= 0) { //comprueba si no llego a jugar ninguna oleada
            cout << "Jugador: " << jugador.alias << '\n'; //muestra su alias
            cout << "Sin oleadas jugadas." << '\n'; //informa que no existen turnos para resumir
        }
        else { //resume las oleadas que el participante alcanzo a jugar
            imprimirLineaTiempoJugador(partida.bitacora, jugador.alias, jugador.oleadaAlcanzada, cout); //imprime los eventos de sus oleadas
        }

        cout << "----------------------------------------" << '\n'; //cierra la linea de tiempo del jugador
    }

    cout << '\n'; //separa el ultimo resumen del cierre
    cout << "========== FIN LINEA DE TIEMPO =========" << '\n'; //indica que termino el reporte

    destruirPartida(partida); //libera las estructuras de la partida
    destruirScriptAcciones(script); //libera las acciones del script
    destruirHistorial(historialExistente); //libera los registros antiguos
    destruirHistorial(nuevosResultados); //libera los resultados de la partida

    return 0; //informa que termino correctamente
}

//carga el historial y su detalle y muestra las estadisticas de cada alias siguiendo el orden del ranking
//devuelve cero si la consulta termina incluso con un historial vacio y uno si falla la lectura
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
    cout << '\n'; //deja espacio antes de los resultados

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

//carga el historial principal y muestra hasta diez alias ordenados por mejor puntaje precision promedio y alias
//devuelve cero si la consulta termina incluso con un historial vacio y uno si falla la lectura
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
    cout << '\n'; //deja espacio antes de los puestos

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

//ejecuta el modo solicitado y devuelve su resultado al programa principal
int ejecutarAplicacion(const ConfiguracionPrograma& config) {
    switch (config.modo) { //consulta el modo indicado en los argumentos
    case MODO_GRAFICO: //selecciona la ventana con el menu y las partidas
#ifdef FASE_ORBITAL_SIN_GRAFICOS
        cerr << "Esta compilacion no incluye el modo grafico. Usa --lotes, --estadisticas, --ranking o --pruebas." << '\n'; //indica los modos disponibles en este ejecutable
        return 1; //termina porque esta compilacion no incluye la ventana
#else
        return ejecutarAplicacionGrafica(config); //abre el modo grafico
#endif

    case MODO_LOTES: //selecciona la partida sin ventana
        return ejecutarAplicacionLotes(config); //ejecuta la simulacion por lotes

    case MODO_ESTADISTICAS: //selecciona la consulta estadistica del historial
        return ejecutarAplicacionEstadisticas(config.rutaHistorial); //muestra las estadisticas por consola

    case MODO_RANKING: //selecciona los mejores resultados historicos
        return ejecutarAplicacionRanking(config.rutaHistorial); //muestra el ranking por consola

    case MODO_PRUEBAS: //selecciona las verificaciones incluidas en el proyecto
        return ejecutarTodasLasPruebas() ? 0 : 1; //ejecuta las pruebas y devuelve su resultado

    case MODO_INVALIDO: //rechaza una configuracion sin modo valido
    default: //rechaza cualquier valor que no corresponda a un modo conocido
        cerr << "Modo de programa invalido." << '\n'; //informa que el modo no existe
        return 1; //termina por argumentos invalidos
    }
}
