#include "Aplicacion.h"

#include <iostream>
#include <fstream>

#include "EjecutorPruebas.h"
#include "Partida.h"
#include "ScriptAcciones.h"
#include "HistorialPartidas.h"
#include "Persistencia.h"

#include <iomanip>

#include "Estadisticas.h"
#include "Ranking.h"
#include "VistaAllegro.h"
#include "InterfazAllegro.h"

#include "ControladorAllegro.h"
#include "Bot.h"

static bool esAliasBot(const std::string& alias) {
    return alias.size() >= 3 &&
        alias[0] == 'B' &&
        alias[1] == 'O' &&
        alias[2] == 'T';
}

static bool existeArchivo(const std::string& ruta) {
    std::ifstream archivo(ruta);
    return archivo.is_open();
}

static int obtenerSiguienteIdPartida(const HistorialPartidas& historial) {
    int mayorId = 0;

    const NodoRegistroPartida* actual = historial.cabeza;

    while (actual != nullptr) {
        if (actual->dato.idPartida > mayorId) {
            mayorId = actual->dato.idPartida;
        }

        actual = actual->sig;
    }

    return mayorId + 1;
}

static bool registrarJugadoresLotes(Partida& partida, const ConfiguracionPrograma& config) {
    for (int i = 0; i < config.cantidadJugadores; i++) {
        bool esBot = esAliasBot(config.jugadores[i]);

        if (!agregarJugadorPartida(partida, config.jugadores[i], esBot)) {
            return false;
        }
    }

    return true;
}

static void mostrarResultadoPartida(const Partida& partida) {
    std::cout << '\n';
    std::cout << "=== RESULTADO ===" << '\n';
    std::cout << '\n';

    for (int posicion = 1; posicion <= partida.cantidadJugadores; posicion++) {
        for (int i = 0; i < partida.cantidadJugadores; i++) {
            const Jugador& jugador = partida.jugadores[i];

            if (jugador.metricas.posicionFinal == posicion) {
                std::cout << posicion << ". "
                    << jugador.alias << "   "
                    << jugador.puntaje << " pts" << '\n';
            }
        }
    }

    double precisiones[6];

    for (int i = 0; i < partida.cantidadJugadores; i++) {
        precisiones[i] = partida.jugadores[i].metricas.precisionColor;
    }

    const Jugador* ganador = determinarGanadorPartida(partida, precisiones);

    std::cout << '\n';

    if (ganador != nullptr) {
        std::cout << "Ganador: " << ganador->alias << '\n';
    }
    else {
        std::cout << "Ganador: empate" << '\n';
    }
}

static void mostrarEstadisticasJugador(const EstadisticasJugador& estadisticas) {
    std::cout << "=== " << estadisticas.alias << " ===" << '\n';

    std::cout << "Partidas: " << estadisticas.partidasJugadas << '\n';
    std::cout << "Victorias: " << estadisticas.victorias << '\n';
    std::cout << "Mejor puntaje: " << estadisticas.mejorPuntaje << '\n';
    std::cout << "Peor puntaje: " << estadisticas.peorPuntaje << '\n';

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Media: "
        << estadisticas.mediaPuntaje << '\n';

    std::cout << "Mediana: "
        << estadisticas.medianaPuntaje << '\n';

    std::cout << "Desviacion estandar: "
        << estadisticas.desviacionEstandar << '\n';

    std::cout << "Precision promedio: "
        << estadisticas.precisionColorPromedio * 100.0
        << "%" << '\n';

    std::cout << "Histograma:" << '\n';

    imprimirHistogramaASCII(
        estadisticas.histograma,
        std::cout
    );

    std::cout << '\n';
}

static bool registrarJugadoresGraficos(Partida& partida, const RegistroGrafico& registro
) {
    for (int i = 0; i < registro.cantidadJugadores; i++) {

        const std::string& alias = registro.aliasJugadores[i];

        bool esBot = esAliasBot(alias);

        if (!agregarJugadorPartida( partida, alias, esBot )) {
            return false;
        }
    }

    return true;
}

static bool ejecutarTurnoGrafico(Partida& partida, VistaAllegro& vista, EstadoInterfaz& estado, bool& cerrarAplicacion, bool& abandonarPartida) {
    Jugador* jugador = verJugadorActualPartida(partida);

    if (jugador == nullptr) {
        return false;
    }

    if (partida.numeroRonda > jugador->oleadaAlcanzada) {
        jugador->oleadaAlcanzada = partida.numeroRonda;
        jugador->metricas.oleadaAlcanzada = partida.numeroRonda;
    }

    ColaSpawn colaJugador;
    inicializarColaSpawn(colaJugador);
    prepararColaJugador(partida, colaJugador);

    Motor motor;
    inicializarMotor(motor, jugador, partida.numeroRonda, colaJugador, &partida.bitacora);

    EstadoControles controles;
    inicializarEstadoControles(controles);

    bool pausado = false;
    estado.pantallaActual = PANTALLA_JUEGO;

    al_flush_event_queue(vista.colaEventos);
    al_start_timer(vista.timer);

    dibujarMotor(vista, motor, pausado);

    while (!cerrarAplicacion && !abandonarPartida && !motor.turnoTerminado) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(vista.colaEventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            cerrarAplicacion = true;
        }
        else {
            procesarEventoControlador(evento, controles);

            if (consumirAlternarPausa(controles)) {
                pausado = !pausado;

                if (pausado) {
                    estado.pantallaActual = PANTALLA_PAUSA;
                }
                else {
                    estado.pantallaActual = PANTALLA_JUEGO;
                }
            }

            bool solicitarVolverMenu = consumirVolverMenu(controles);

            if (solicitarVolverMenu && pausado) {
                abandonarPartida = true;
            }

            if (evento.type == ALLEGRO_EVENT_TIMER && !pausado && !abandonarPartida) {
                AccionMotor accion;

                if (jugador->esBot) {
                    accion = obtenerAccionBot(motor);
                }
                else {
                    accion = obtenerAccionAllegro(controles, motor);
                }

                procesarTick(motor, accion);
            }
        }

        if (!cerrarAplicacion && !abandonarPartida && al_is_event_queue_empty(vista.colaEventos)) {
            dibujarMotor(vista, motor, pausado);
        }
    }

    al_stop_timer(vista.timer);

    destruirMotor(motor);
    destruirColaSpawn(colaJugador);

    return true;
}

static bool ejecutarRondaGrafica(Partida& partida, VistaAllegro& vista, EstadoInterfaz& estado, bool& cerrarAplicacion, bool& abandonarPartida) {
    if (partida.finalizada || cantidadJugadoresActivos(partida) <= 0) {
        return false;
    }

    generarColaOriginalRonda(partida);

    int turnosDeLaRonda = cantidadJugadoresActivos(partida);

    for (int i = 0; i < turnosDeLaRonda; i++) {
        Jugador* jugador = verJugadorActualPartida(partida);

        if (jugador == nullptr) {
            return false;
        }

        mostrarAnuncioTurno(vista, estado, *jugador, partida.numeroRonda, cerrarAplicacion);

        if (cerrarAplicacion) {
            return true;
        }

        std::cout << "Ronda " << partida.numeroRonda << " - Turno de " << jugador->alias;

        if (jugador->esBot) {
            std::cout << " [BOT]";
        }

        std::cout << '\n';

        if (!ejecutarTurnoGrafico(partida, vista, estado, cerrarAplicacion, abandonarPartida)) {
            return false;
        }

        if (cerrarAplicacion || abandonarPartida) {
            return true;
        }

        bool eliminado = retirarJugadorActualSiEliminado(partida);

        if (partida.finalizada) {
            break;
        }

        if (i < turnosDeLaRonda - 1 && !eliminado) {
            avanzarTurnoPartida(partida);
        }
    }

    if (!cerrarAplicacion && !abandonarPartida) {
        mostrarResumenOleada(vista, estado, partida, cerrarAplicacion);
    }

    if (!partida.finalizada && !cerrarAplicacion && !abandonarPartida) {
        finalizarRondaPartida(partida);
    }

    return true;
}

static void mostrarLineaTiempoPartida(const Partida& partida
) {
    std::cout << '\n';
    std::cout
        << "=== LINEA DE TIEMPO ==="
        << '\n'
        << '\n';

    for (int i = 0;
        i < partida.cantidadJugadores;
        i++) {

        const Jugador& jugador =
            partida.jugadores[i];

        imprimirLineaTiempoJugador(
            partida.bitacora,
            jugador.alias,
            jugador.oleadaAlcanzada,
            std::cout
        );

        std::cout << '\n';
    }
}

static bool guardarPartidaGrafica(Partida& partida, const std::string& rutaHistorial
) {
    HistorialPartidas historialExistente;
    HistorialPartidas nuevosResultados;

    inicializarHistorial(historialExistente);

    inicializarHistorial(nuevosResultados);

    if (existeArchivo(rutaHistorial)) {
        if (!cargarHistorialCSV(
            rutaHistorial,
            historialExistente,
            std::cerr
        )) {
            destruirHistorial(
                historialExistente
            );

            destruirHistorial(
                nuevosResultados
            );

            return false;
        }
    }

    int idPartida = obtenerSiguienteIdPartida(historialExistente);

    agregarResultadosPartidaHistorial(partida, idPartida, "grafico", nuevosResultados);

    bool guardado = agregarHistorialCSV(rutaHistorial, nuevosResultados);

    destruirHistorial(historialExistente);

    destruirHistorial(nuevosResultados);

    return guardado;
}

int ejecutarAplicacionGrafica(
    const ConfiguracionPrograma& config
) {
    VistaAllegro vista;

    if (!inicializarVistaAllegro(
        vista,
        800,
        600
    )) {
        std::cerr
            << "No se pudo inicializar Allegro."
            << '\n';

        return 1;
    }

    EstadoInterfaz estado;
    inicializarEstadoInterfaz(
        estado
    );

    bool cerrarAplicacion = false;
    bool resultado = true;

    while (!cerrarAplicacion) {
        OpcionMenuPrincipal opcion =
            ejecutarMenuPrincipal(
                vista,
                estado,
                cerrarAplicacion
            );

        if (cerrarAplicacion ||
            opcion == MENU_SALIR) {

            break;
        }

        if (opcion ==
            MENU_ESTADISTICAS) {

            mostrarPantallaInformativa(
                vista,
                estado,
                PANTALLA_ESTADISTICAS,
                "ESTADISTICAS",
                "La pantalla historica se integra en F5.5.",
                cerrarAplicacion
            );

            continue;
        }

        if (opcion ==
            MENU_RANKING) {

            mostrarPantallaInformativa(
                vista,
                estado,
                PANTALLA_RANKING,
                "RANKING TOP 10",
                "La pantalla de ranking se integra en F5.5.",
                cerrarAplicacion
            );

            continue;
        }

        RegistroGrafico registro;

        bool registroCompletado =
            ejecutarRegistroJugadores(
                vista,
                estado,
                registro,
                cerrarAplicacion
            );

        if (cerrarAplicacion) {
            break;
        }

        if (!registroCompletado) {
            continue;
        }

        Partida partida;

        inicializarPartida(
            partida,
            config.semilla,
            config.oleadas
        );

        if (!registrarJugadoresGraficos(
            partida,
            registro
        )) {
            std::cerr
                << "No se pudieron registrar "
                << "los jugadores graficos."
                << '\n';

            destruirPartida(
                partida
            );

            resultado = false;
            break;
        }

        bool abandonarPartida = false;

        while (!partida.finalizada && !cerrarAplicacion && !abandonarPartida) {
            if (!ejecutarRondaGrafica(
                partida,
                vista,
                estado,
                cerrarAplicacion,
                abandonarPartida
            )) {
                resultado = false;
                break;
            }
        }

        if (!resultado) {
            destruirPartida(
                partida
            );

            break;
        }

        if (!cerrarAplicacion &&
            partida.finalizada) {

            actualizarMetricasFinalesPartida(
                partida
            );

            mostrarFinPartida(
                vista,
                estado,
                partida,
                cerrarAplicacion
            );

            if (!cerrarAplicacion) {
                if (!guardarPartidaGrafica(
                    partida,
                    config.rutaHistorial
                )) {
                    std::cerr
                        << "No se pudo guardar "
                        << "la partida grafica."
                        << '\n';
                }

                mostrarLineaTiempoPartida(
                    partida
                );
            }
        }

        destruirPartida(
            partida
        );
    }

    destruirVistaAllegro(
        vista
    );

    return resultado ? 0 : 1;
}

int ejecutarAplicacionLotes(const ConfiguracionPrograma& config) {
    Partida partida;
    ScriptAcciones script;
    HistorialPartidas historialExistente;
    HistorialPartidas nuevosResultados;

    inicializarPartida(partida, config.semilla, config.oleadas);
    inicializarScriptAcciones(script);
    inicializarHistorial(historialExistente);
    inicializarHistorial(nuevosResultados);

    if (!registrarJugadoresLotes(partida, config)) {
        std::cerr << "No se pudieron registrar los jugadores." << '\n';

        destruirPartida(partida);
        destruirScriptAcciones(script);
        destruirHistorial(historialExistente);
        destruirHistorial(nuevosResultados);

        return 1;
    }

    if (config.tieneScript) {
        if (!cargarScriptAcciones(script, config.rutaScript)) {
            std::cerr << "No se pudo cargar el script: "
                << config.rutaScript << '\n';

            destruirPartida(partida);
            destruirScriptAcciones(script);
            destruirHistorial(historialExistente);
            destruirHistorial(nuevosResultados);

            return 1;
        }
    }

    if (existeArchivo(config.rutaHistorial)) {
        if (!cargarHistorialCSV(config.rutaHistorial, historialExistente, std::cerr)) {
            std::cerr << "No se pudo cargar el historial existente." << '\n';

            destruirPartida(partida);
            destruirScriptAcciones(script);
            destruirHistorial(historialExistente);
            destruirHistorial(nuevosResultados);

            return 1;
        }
    }

    int idPartida = obtenerSiguienteIdPartida(historialExistente);

    const int limiteTicks = 10000;

    while (!partida.finalizada) {
        if (!ejecutarRondaModoLotes(partida, script, limiteTicks)) {
            std::cerr << "La partida en modo lotes no termino correctamente." << '\n';

            destruirPartida(partida);
            destruirScriptAcciones(script);
            destruirHistorial(historialExistente);
            destruirHistorial(nuevosResultados);

            return 1;
        }
    }

    agregarResultadosPartidaHistorial(
        partida,
        idPartida,
        "lotes",
        nuevosResultados
    );

    if (!agregarHistorialCSV(config.rutaHistorial, nuevosResultados)) {
        std::cerr << "No se pudo guardar el historial en: "
            << config.rutaHistorial << '\n';

        destruirPartida(partida);
        destruirScriptAcciones(script);
        destruirHistorial(historialExistente);
        destruirHistorial(nuevosResultados);

        return 1;
    }

    mostrarResultadoPartida(partida);

    destruirPartida(partida);
    destruirScriptAcciones(script);
    destruirHistorial(historialExistente);
    destruirHistorial(nuevosResultados);

    return 0;
}

int ejecutarAplicacionEstadisticas(const std::string& archivo) {
    HistorialPartidas historial;
    Ranking ranking;

    inicializarHistorial(historial);
    inicializarRanking(ranking);

    if (!cargarHistorialCSV(archivo, historial, std::cerr)) {
        destruirRanking(ranking);
        destruirHistorial(historial);
        return 1;
    }

    construirRankingDesdeHistorial(historial, ranking);

    std::cout << "=== ESTADISTICAS ===" << '\n';
    std::cout << '\n';

    if (ranking.tam == 0) {
        std::cout << "El historial no contiene jugadores." << '\n';

        destruirRanking(ranking);
        destruirHistorial(historial);

        return 0;
    }

    const NodoRanking* actual = ranking.cabeza;

    while (actual != nullptr) {
        EstadisticasJugador estadisticas;

        if (calcularEstadisticasJugador(
            historial,
            actual->dato.alias,
            estadisticas
        )) {
            mostrarEstadisticasJugador(estadisticas);
        }

        actual = actual->sig;
    }

    destruirRanking(ranking);
    destruirHistorial(historial);

    return 0;
}

int ejecutarAplicacionRanking(const std::string& archivo) {
    HistorialPartidas historial;
    Ranking ranking;

    inicializarHistorial(historial);
    inicializarRanking(ranking);

    if (!cargarHistorialCSV(archivo, historial, std::cerr)) {
        destruirRanking(ranking);
        destruirHistorial(historial);
        return 1;
    }

    construirRankingDesdeHistorial(historial, ranking);

    std::cout << "=== TOP 10 ===" << '\n';
    std::cout << '\n';

    if (ranking.tam == 0) {
        std::cout << "El historial no contiene jugadores." << '\n';

        destruirRanking(ranking);
        destruirHistorial(historial);

        return 0;
    }

    const NodoRanking* actual = ranking.cabeza;
    int posicion = 1;

    std::cout << std::fixed << std::setprecision(2);

    while (actual != nullptr && posicion <= 10) {
        std::cout << posicion << ". "
            << actual->dato.alias << "   "
            << actual->dato.mejorPuntaje << " pts   "
            << actual->dato.precisionColor * 100.0
            << "%" << '\n';

        actual = actual->sig;
        posicion++;
    }

    destruirRanking(ranking);
    destruirHistorial(historial);

    return 0;
}

int ejecutarAplicacion(const ConfiguracionPrograma& config) {
    switch (config.modo) {
    case MODO_GRAFICO:
        return ejecutarAplicacionGrafica(config);

    case MODO_LOTES:
        return ejecutarAplicacionLotes(config);

    case MODO_ESTADISTICAS:
        return ejecutarAplicacionEstadisticas(config.rutaHistorial);

    case MODO_RANKING:
        return ejecutarAplicacionRanking(config.rutaHistorial);

    case MODO_PRUEBAS:
        return ejecutarTodasLasPruebas() ? 0 : 1;

    case MODO_INVALIDO:
    default:
        std::cerr << "Modo de programa invalido." << '\n';
        return 1;
    }
}



