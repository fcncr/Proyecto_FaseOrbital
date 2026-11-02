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

static bool esAliasBot(const string& alias) {
    return alias.size() >= 3 &&
        alias[0] == 'B' &&
        alias[1] == 'O' &&
        alias[2] == 'T';
}

static bool existeArchivo(const string& ruta) {
    ifstream archivo(ruta);
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

static bool registrarJugadores( Partida& partida,const string aliasJugadores[],int cantidadJugadores){
    for (int i = 0; i < cantidadJugadores; i++) {
        const string& alias = aliasJugadores[i];
        bool esBot = esAliasBot(alias);
        if (!agregarJugadorPartida(partida, alias, esBot)) {
            return false;
        }
    }
    return true;
}

static void mostrarResultadoPartida(const Partida& partida) {
    cout << '\n';
    cout << "=== RESULTADO ===" << '\n';
    cout << '\n';

    for (int posicion = 1; posicion <= partida.cantidadJugadores; posicion++) {
        for (int i = 0; i < partida.cantidadJugadores; i++) {
            const Jugador& jugador = partida.jugadores[i];

            if (jugador.metricas.posicionFinal == posicion) {
                cout << posicion << ". "
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
    cout << '\n';

    if (ganador != nullptr) {
        cout << "Ganador: " << ganador->alias << '\n';
    }
    else {
        cout << "Ganador: empate" << '\n';
    }
}

static void mostrarEstadisticasJugador(const EstadisticasJugador& estadisticas) {
    cout << "=== " << estadisticas.alias << " ===" << '\n';
    cout << "Partidas: " << estadisticas.partidasJugadas << '\n';
    cout << "Victorias: " << estadisticas.victorias << '\n';
    cout << "Mejor puntaje: " << estadisticas.mejorPuntaje << '\n';
    cout << "Peor puntaje: " << estadisticas.peorPuntaje << '\n';

    cout << fixed << setprecision(2);

    cout << "Media: "<< estadisticas.mediaPuntaje << '\n';
    cout << "Mediana: "<< estadisticas.medianaPuntaje << '\n';
    cout << "Desviacion estandar: "<< estadisticas.desviacionEstandar << '\n';
    cout << "Precision promedio: "<< estadisticas.precisionColorPromedio * 100.0<< "%" << '\n';
    if (estadisticas.mejorColorDisponible) {
            cout << "Mejor color: " << nombreColorEstadistica(estadisticas.mejorColor) << '\n';
        }
        else {
            cout << "Mejor color: no disponible" << '\n';
        }

        if (estadisticas.tipoMasDanoDisponible) {
            cout << "Tipo que mas dano causa: " << nombreTipoEnemigoEstadistica(estadisticas.tipoMasDano) << '\n';
        }
        else {
            cout << "Tipo que mas dano causa: no disponible" << '\n';
        }
    cout << "Histograma:" << '\n';

    imprimirHistogramaASCII(estadisticas.histograma,cout);
    cout << '\n';
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

        cout << "Ronda " << partida.numeroRonda << " - Turno de " << jugador->alias;

        if (jugador->esBot) {
            cout << " [BOT]";
        }

        cout << '\n';

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

static void mostrarLineaTiempoPartida(const Partida& partida) {
    cout << '\n';
    cout<< "=== LINEA DE TIEMPO ==="<< '\n'<< '\n';

    for (int i = 0;i < partida.cantidadJugadores;i++) {
        const Jugador& jugador = partida.jugadores[i];

        imprimirLineaTiempoJugador(partida.bitacora,jugador.alias,jugador.oleadaAlcanzada,cout);
        cout << '\n';
    }
}

static bool guardarPartidaGrafica(Partida& partida, const string& rutaHistorial, HistorialPartidas& historial) {
    HistorialPartidas nuevosResultados;
    inicializarHistorial(nuevosResultados);

    int idPartida = obtenerSiguienteIdPartida(historial);

    agregarResultadosPartidaHistorial(partida, idPartida, "grafico", nuevosResultados);

    if (!agregarHistorialCSV(rutaHistorial, nuevosResultados)) {
        destruirHistorial(nuevosResultados);
        return false;
    }

    bool detalleGuardado =
        agregarDetalleHistorialCSV(rutaHistorial, nuevosResultados);

    const NodoRegistroPartida* actual =
        nuevosResultados.cabeza;

    while (actual != nullptr) {
        agregarRegistroFinal(historial, actual->dato);
        actual = actual->sig;
    }

    destruirHistorial(nuevosResultados);

    return detalleGuardado;
}

int ejecutarAplicacionGrafica(const ConfiguracionPrograma& config) {
    HistorialPartidas historial;
    inicializarHistorial(historial);

    if (existeArchivo(config.rutaHistorial)) {
        if (!cargarHistorialCSV(config.rutaHistorial, historial, cerr)) {
            cerr << "No se pudo cargar el historial al iniciar la aplicacion." << '\n';
            destruirHistorial(historial);
            return 1;
        }

        if (!cargarDetalleHistorialCSV(config.rutaHistorial, historial, cerr)) {
            cerr << "No se pudo cargar el detalle historico." << '\n';
            destruirHistorial(historial);
            return 1;
        }
    }

    VistaAllegro vista;

    if (!inicializarVistaAllegro(vista, 800, 600)) {
        cerr << "No se pudo inicializar Allegro." << '\n';
        destruirHistorial(historial);
        return 1;
    }

    EstadoInterfaz estado;
    inicializarEstadoInterfaz(estado);

    bool cerrarAplicacion = false;
    bool resultado = true;

    while (!cerrarAplicacion) {
        OpcionMenuPrincipal opcion = ejecutarMenuPrincipal(vista, estado, cerrarAplicacion);

        if (cerrarAplicacion || opcion == MENU_SALIR) {
            break;
        }

        if (opcion == MENU_ESTADISTICAS) {
            mostrarEstadisticasHistoricas(vista, estado, historial, cerrarAplicacion);
            continue;
        }

        if (opcion == MENU_RANKING) {
            mostrarRankingGrafico(vista, estado, historial, cerrarAplicacion);
            continue;
        }

        RegistroGrafico registro;

        bool registroCompletado = ejecutarRegistroJugadores(vista, estado, registro, cerrarAplicacion);

        if (cerrarAplicacion) {
            break;
        }

        if (!registroCompletado) {
            continue;
        }

        Partida partida;
        inicializarPartida(partida, config.semilla, config.oleadas);

        if (!registrarJugadores(partida, registro.aliasJugadores, registro.cantidadJugadores)) {
            cerr << "No se pudieron registrar los jugadores graficos." << '\n';

            destruirPartida(partida);
            resultado = false;
            break;
        }

        bool abandonarPartida = false;

        while (!partida.finalizada && !cerrarAplicacion && !abandonarPartida) {
            if (!ejecutarRondaGrafica(partida, vista, estado, cerrarAplicacion, abandonarPartida)) {
                resultado = false;
                break;
            }
        }

        if (!resultado) {
            destruirPartida(partida);
            break;
        }

        if (!cerrarAplicacion && partida.finalizada) {
            actualizarMetricasFinalesPartida(partida);
            mostrarFinPartida(vista, estado, partida, cerrarAplicacion);

            if (!cerrarAplicacion) {
                if (!guardarPartidaGrafica(partida, config.rutaHistorial, historial)) {
                    cerr << "No se pudo guardar la partida grafica." << '\n';
                }

                mostrarLineaTiempoPartida(partida);
            }
        }

        destruirPartida(partida);
    }

    destruirVistaAllegro(vista);
    destruirHistorial(historial);

    return resultado ? 0 : 1;
}

static double calcularPorcentajeReporteLotes(long long cantidad, long long total) {
    if (total <= 0) {
        return 0.0;
    }

    return 100.0 * static_cast<double>(cantidad) / static_cast<double>(total);
}

static void imprimirReporteDetalladoLotes(const Partida& partida, const HistorialPartidas& resultados, std::ostream& salida) {
    std::ios::fmtflags formatoOriginal = salida.flags();
    std::streamsize precisionOriginal = salida.precision();

    salida << std::fixed << std::setprecision(2);

    salida << '\n';
    salida << "========================================" << '\n';
    salida << "       REPORTE DEL MODO POR LOTES        " << '\n';
    salida << "========================================" << '\n';

    const NodoRegistroPartida* actual = resultados.cabeza;

    if (actual == nullptr) {
        salida << "No hay resultados para mostrar." << '\n';
    }

    while (actual != nullptr) {
        const RegistroPartida& registro = actual->dato;

        int bajasPorColor[3] = { 0, 0, 0 };

        const NodoEvento* eventoActual = partida.bitacora.cabeza;

        while (eventoActual != nullptr) {
            const Evento& evento = eventoActual->dato;

            if (evento.jugador == registro.jugador && evento.tipoEvento == BAJA) {
                if (evento.color >= 0 && evento.color < 3) {
                    bajasPorColor[evento.color]++;
                }
            }

            eventoActual = eventoActual->sig;
        }

        int impactosTotales = registro.correctos + registro.erroneos;

        int bajasTotales =
            registro.bajasDron +
            registro.bajasGirador +
            registro.bajasDivisor +
            registro.bajasMini +
            registro.bajasTanque +
            registro.bajasCamaleon;

        salida << '\n';
        salida << "----------------------------------------" << '\n';
        salida << "JUGADOR: " << registro.jugador << '\n';
        salida << "----------------------------------------" << '\n';

        salida << "ID de partida: " << registro.idPartida << '\n';
        salida << "Semilla: " << registro.semilla << '\n';
        salida << "Modo: " << registro.modo << '\n';
        salida << "Es bot: " << (registro.esBot ? "Si" : "No") << '\n';
        salida << "Posicion final: " << registro.posicion << '\n';

        salida << '\n';
        salida << "=== RENDIMIENTO GENERAL ===" << '\n';

        salida << "Puntaje: " << registro.puntaje << '\n';
        salida << "Oleada alcanzada: " << registro.oleada << '\n';
        salida << "Disparos: " << registro.disparos << '\n';
        salida << "Impactos correctos: " << registro.correctos << '\n';
        salida << "Impactos erroneos: " << registro.erroneos << '\n';

        salida << "Precision de color: "
            << calcularPorcentajeReporteLotes(registro.correctos, impactosTotales)
            << "%" << '\n';

        salida << "Eficiencia de disparo: "
            << calcularPorcentajeReporteLotes(registro.correctos, registro.disparos)
            << "%" << '\n';

        salida << "Combo maximo: " << registro.comboMax << '\n';
        salida << "Pulsos utilizados: " << registro.pulsos << '\n';
        salida << "Dano total recibido: " << registro.danoTotal << '\n';
        salida << "Ticks sobrevividos: " << registro.ticks << '\n';

        salida << '\n';
        salida << "=== BAJAS POR TIPO DE ENEMIGO ===" << '\n';

        salida << "Dron: " << registro.bajasDron << '\n';
        salida << "Girador: " << registro.bajasGirador << '\n';
        salida << "Divisor: " << registro.bajasDivisor << '\n';
        salida << "Mini: " << registro.bajasMini << '\n';
        salida << "Tanque: " << registro.bajasTanque << '\n';
        salida << "Camaleon: " << registro.bajasCamaleon << '\n';
        salida << "Total de bajas: " << bajasTotales << '\n';

        salida << '\n';
        salida << "=== BAJAS POR COLOR ===" << '\n';

        salida << "Cian: " << bajasPorColor[0] << '\n';
        salida << "Magenta: " << bajasPorColor[1] << '\n';
        salida << "Ambar: " << bajasPorColor[2] << '\n';

        if (registro.detalleDisponible) {
            salida << '\n';
            salida << "=== PRECISION POR COLOR ===" << '\n';

            salida << "Cian: "
                << calcularPorcentajeReporteLotes(registro.correctosC, registro.correctosC + registro.erroneosC)
                << "%" << '\n';

            salida << "Magenta: "
                << calcularPorcentajeReporteLotes(registro.correctosM, registro.correctosM + registro.erroneosM)
                << "%" << '\n';

            salida << "Ambar: "
                << calcularPorcentajeReporteLotes(registro.correctosA, registro.correctosA + registro.erroneosA)
                << "%" << '\n';

            salida << '\n';
            salida << "=== DANO RECIBIDO POR TIPO ===" << '\n';

            salida << "Dron: " << registro.danoDron << '\n';
            salida << "Girador: " << registro.danoGirador << '\n';
            salida << "Divisor: " << registro.danoDivisor << '\n';
            salida << "Mini: " << registro.danoMini << '\n';
            salida << "Tanque: " << registro.danoTanque << '\n';
            salida << "Camaleon: " << registro.danoCamaleon << '\n';
        }
        else {
            salida << '\n';
            salida << "Detalle por color y dano por tipo: no disponible." << '\n';
        }

        actual = actual->sig;
    }

    salida << '\n';
    salida << "========================================" << '\n';
    salida << "            FIN DEL REPORTE             " << '\n';
    salida << "========================================" << '\n';

    salida.flags(formatoOriginal);
    salida.precision(precisionOriginal);
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

    if (!registrarJugadores(partida, config.jugadores, config.cantidadJugadores)) {
        cerr << "No se pudieron registrar los jugadores." << '\n';

        destruirPartida(partida);
        destruirScriptAcciones(script);
        destruirHistorial(historialExistente);
        destruirHistorial(nuevosResultados);

        return 1;
    }

    if (config.tieneScript) {
        if (!cargarScriptAcciones(script, config.rutaScript)) {
            cerr << "No se pudo cargar el script: " << config.rutaScript << '\n';

            destruirPartida(partida);
            destruirScriptAcciones(script);
            destruirHistorial(historialExistente);
            destruirHistorial(nuevosResultados);

            return 1;
        }
    }

    if (existeArchivo(config.rutaHistorial)) {
        if (!cargarHistorialCSV(config.rutaHistorial, historialExistente, cerr)) {
            cerr << "No se pudo cargar el historial existente." << '\n';

            destruirPartida(partida);
            destruirScriptAcciones(script);
            destruirHistorial(historialExistente);
            destruirHistorial(nuevosResultados);

            return 1;
        }

        if (!cargarDetalleHistorialCSV(config.rutaHistorial, historialExistente, cerr)) {
            cerr << "No se pudo cargar el detalle historico." << '\n';

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
            cerr << "La partida en modo lotes no termino correctamente." << '\n';

            destruirPartida(partida);
            destruirScriptAcciones(script);
            destruirHistorial(historialExistente);
            destruirHistorial(nuevosResultados);

            return 1;
        }
    }

    agregarResultadosPartidaHistorial(partida, idPartida, "lotes", nuevosResultados);

    if (!agregarHistorialCSV(config.rutaHistorial, nuevosResultados)) {
        cerr << "No se pudo guardar el historial en: " << config.rutaHistorial << '\n';

        destruirPartida(partida);
        destruirScriptAcciones(script);
        destruirHistorial(historialExistente);
        destruirHistorial(nuevosResultados);

        return 1;
    }

    if (!agregarDetalleHistorialCSV(config.rutaHistorial, nuevosResultados)) {
        cerr << "No se pudo guardar el detalle historico." << '\n';

        destruirPartida(partida);
        destruirScriptAcciones(script);
        destruirHistorial(historialExistente);
        destruirHistorial(nuevosResultados);

        return 1;
    }

    mostrarResultadoPartida(partida);

    imprimirReporteDetalladoLotes(partida, nuevosResultados, cout);

    cout << '\n';
    cout << "========================================" << '\n';
    cout << "       LINEA DE TIEMPO POR JUGADOR       " << '\n';
    cout << "========================================" << '\n';

    for (int i = 0; i < partida.cantidadJugadores; i++) {
        const Jugador& jugador = partida.jugadores[i];

        cout << '\n';
        cout << "----------------------------------------" << '\n';

        if (jugador.oleadaAlcanzada <= 0) {
            cout << "Jugador: " << jugador.alias << '\n';
            cout << "Sin oleadas jugadas." << '\n';
        }
        else {
            imprimirLineaTiempoJugador(
                partida.bitacora,
                jugador.alias,
                jugador.oleadaAlcanzada,
                cout
            );
        }

        cout << "----------------------------------------" << '\n';
    }

    cout << '\n';
    cout << "========== FIN LINEA DE TIEMPO =========" << '\n';

    destruirPartida(partida);
    destruirScriptAcciones(script);
    destruirHistorial(historialExistente);
    destruirHistorial(nuevosResultados);

    return 0;
}

int ejecutarAplicacionEstadisticas(const string& archivo) {
    HistorialPartidas historial;
    Ranking ranking;

    inicializarHistorial(historial);
    inicializarRanking(ranking);

    if (!cargarHistorialCSV(archivo, historial, cerr)) {
        destruirRanking(ranking);
        destruirHistorial(historial);
        return 1;
    }

    if (!cargarDetalleHistorialCSV(archivo, historial, cerr)) {
        destruirRanking(ranking);
        destruirHistorial(historial);
        return 1;
    }

    construirRankingDesdeHistorial(historial, ranking);

    cout << "=== ESTADISTICAS ===" << '\n';
    cout << '\n';

    if (ranking.tam == 0) {
        cout << "El historial no contiene jugadores." << '\n';

        destruirRanking(ranking);
        destruirHistorial(historial);

        return 0;
    }

    const NodoRanking* actual = ranking.cabeza;

    while (actual != nullptr) {
        EstadisticasJugador estadisticas;

        if (calcularEstadisticasJugador(historial,actual->dato.alias, estadisticas)) {
            mostrarEstadisticasJugador(estadisticas);
        }
        actual = actual->sig;
    }

    destruirRanking(ranking);
    destruirHistorial(historial);

    return 0;
}

int ejecutarAplicacionRanking(const string& archivo) {
    HistorialPartidas historial;
    Ranking ranking;

    inicializarHistorial(historial);
    inicializarRanking(ranking);

    if (!cargarHistorialCSV(archivo, historial, cerr)) {
        destruirRanking(ranking);
        destruirHistorial(historial);
        return 1;
    }

    construirRankingDesdeHistorial(historial, ranking);

    cout << "=== TOP 10 ===" << '\n';
    cout << '\n';

    if (ranking.tam == 0) {
        cout << "El historial no contiene jugadores." << '\n';
        destruirRanking(ranking);
        destruirHistorial(historial);

        return 0;
    }

    const NodoRanking* actual = ranking.cabeza;
    int posicion = 1;

    cout << fixed << setprecision(2);

    while (actual != nullptr && posicion <= 10) {
        cout << posicion << ". "<< actual->dato.alias << "   "<< actual->dato.mejorPuntaje << " pts   "<< actual->dato.precisionColor * 100.0<< "%" << '\n';
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
        cerr << "Modo de programa invalido." << '\n';
        return 1;
    }
}



