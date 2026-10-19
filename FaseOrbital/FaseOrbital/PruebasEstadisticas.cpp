#include "PruebasEstadisticas.h"
#include "ListaPuntajes.h"
#include "Estadisticas.h"
#include "Ranking.h"
#include "Motor.h"
#include "Partida.h"
#include <cmath>
#include <sstream>


static bool listaOrdenadaAscendente(const ListaPuntajes& lista) {
    const NodoPuntaje* actual = lista.cabeza;

    while (actual != nullptr && actual->sig != nullptr) {
        if (actual->puntaje > actual->sig->puntaje) {
            return false;
        }

        actual = actual->sig;
    }

    return true;
}

static int contarValorPuntaje(const ListaPuntajes& lista, int valor) {
    int cantidad = 0;
    const NodoPuntaje* actual = lista.cabeza;

    while (actual != nullptr) {
        if (actual->puntaje == valor) {
            cantidad++;
        }

        actual = actual->sig;
    }

    return cantidad;
}

bool pruebaP12() {
    bool resultado = true;

    ListaPuntajes vacia;
    inicializarListaPuntajes(vacia);
    ordenarListaPuntajesMergeSort(vacia);

    if (vacia.cabeza != nullptr || vacia.cola != nullptr || vacia.tam != 0) {
        resultado = false;
    }

    if (calcularMedianaPuntajes(vacia) != 0.0) {
        resultado = false;
    }

    destruirListaPuntajes(vacia);

    ListaPuntajes unNodo;
    inicializarListaPuntajes(unNodo);
    agregarPuntajeFinal(unNodo, 500);
    ordenarListaPuntajesMergeSort(unNodo);

    if (unNodo.cabeza == nullptr || unNodo.cabeza != unNodo.cola || unNodo.cabeza->puntaje != 500 || unNodo.tam != 1) {
        resultado = false;
    }

    if (calcularMedianaPuntajes(unNodo) != 500.0) {
        resultado = false;
    }

    destruirListaPuntajes(unNodo);

    ListaPuntajes dosNodos;
    inicializarListaPuntajes(dosNodos);
    agregarPuntajeFinal(dosNodos, 900);
    agregarPuntajeFinal(dosNodos, 300);
    ordenarListaPuntajesMergeSort(dosNodos);

    if (!listaOrdenadaAscendente(dosNodos) || dosNodos.cabeza == nullptr || dosNodos.cola == nullptr) {
        resultado = false;
    }

    if (dosNodos.cabeza != nullptr && dosNodos.cabeza->puntaje != 300) {
        resultado = false;
    }

    if (dosNodos.cola != nullptr && dosNodos.cola->puntaje != 900) {
        resultado = false;
    }

    destruirListaPuntajes(dosNodos);

    ListaPuntajes ordenada;
    inicializarListaPuntajes(ordenada);
    agregarPuntajeFinal(ordenada, 100);
    agregarPuntajeFinal(ordenada, 200);
    agregarPuntajeFinal(ordenada, 300);
    agregarPuntajeFinal(ordenada, 400);
    ordenarListaPuntajesMergeSort(ordenada);

    if (!listaOrdenadaAscendente(ordenada) || ordenada.tam != 4) {
        resultado = false;
    }

    destruirListaPuntajes(ordenada);

    ListaPuntajes inversa;
    inicializarListaPuntajes(inversa);
    agregarPuntajeFinal(inversa, 500);
    agregarPuntajeFinal(inversa, 400);
    agregarPuntajeFinal(inversa, 300);
    agregarPuntajeFinal(inversa, 200);
    agregarPuntajeFinal(inversa, 100);
    ordenarListaPuntajesMergeSort(inversa);

    if (!listaOrdenadaAscendente(inversa) || inversa.cabeza == nullptr || inversa.cola == nullptr) {
        resultado = false;
    }

    if (inversa.cabeza != nullptr && inversa.cabeza->puntaje != 100) {
        resultado = false;
    }

    if (inversa.cola != nullptr && inversa.cola->puntaje != 500) {
        resultado = false;
    }

    destruirListaPuntajes(inversa);

    ListaPuntajes repetidos;
    inicializarListaPuntajes(repetidos);
    agregarPuntajeFinal(repetidos, 500);
    agregarPuntajeFinal(repetidos, 200);
    agregarPuntajeFinal(repetidos, 500);
    agregarPuntajeFinal(repetidos, 100);
    agregarPuntajeFinal(repetidos, 500);
    ordenarListaPuntajesMergeSort(repetidos);

    if (!listaOrdenadaAscendente(repetidos) || repetidos.tam != 5) {
        resultado = false;
    }

    if (contarValorPuntaje(repetidos, 500) != 3) {
        resultado = false;
    }

    destruirListaPuntajes(repetidos);

    ListaPuntajes impar;
    inicializarListaPuntajes(impar);
    agregarPuntajeFinal(impar, 9);
    agregarPuntajeFinal(impar, 1);
    agregarPuntajeFinal(impar, 5);
    agregarPuntajeFinal(impar, 3);
    agregarPuntajeFinal(impar, 7);

    if (calcularMedianaPuntajes(impar) != 5.0) {
        resultado = false;
    }

    destruirListaPuntajes(impar);

    ListaPuntajes par;
    inicializarListaPuntajes(par);
    agregarPuntajeFinal(par, 7);
    agregarPuntajeFinal(par, 1);
    agregarPuntajeFinal(par, 5);
    agregarPuntajeFinal(par, 3);

    if (calcularMedianaPuntajes(par) != 4.0) {
        resultado = false;
    }

    destruirListaPuntajes(par);

    return resultado;
}

static RegistroPartida crearRegistroEstadisticas(int idPartida, const std::string& jugador, int posicion, int puntaje, int correctos, int erroneos) {
    RegistroPartida registro;
    registro.idPartida = idPartida;
    registro.semilla = 42u;
    registro.modo = "lotes";
    registro.jugador = jugador;
    registro.esBot = false;
    registro.posicion = posicion;
    registro.puntaje = puntaje;
    registro.oleada = 3;
    registro.disparos = correctos + erroneos;
    registro.correctos = correctos;
    registro.erroneos = erroneos;
    registro.comboMax = 0;
    registro.pulsos = 0;
    registro.danoTotal = 0;
    registro.ticks = 100;
    registro.bajasDron = 0;
    registro.bajasGirador = 0;
    registro.bajasDivisor = 0;
    registro.bajasMini = 0;
    registro.bajasTanque = 0;
    registro.bajasCamaleon = 0;
    return registro;
}

static bool casiIguales(double a, double b) {
    return std::fabs(a - b) < 0.000001;
}

bool pruebaEstadisticasHistoricas() {
    HistorialPartidas historial;
    inicializarHistorial(historial);

    agregarRegistroFinal(historial, crearRegistroEstadisticas(1, "ANA", 1, 100, 1, 1));
    agregarRegistroFinal(historial, crearRegistroEstadisticas(2, "ANA", 2, 200, 2, 0));
    agregarRegistroFinal(historial, crearRegistroEstadisticas(3, "ANA", 1, 300, 3, 3));
    agregarRegistroFinal(historial, crearRegistroEstadisticas(4, "ANA", 3, 400, 0, 0));
    agregarRegistroFinal(historial, crearRegistroEstadisticas(5, "OTRO", 1, 999, 10, 0));

    bool resultado = true;

    EstadisticasJugador estadisticas;

    if (!calcularEstadisticasJugador(historial, "ANA", estadisticas)) {
        resultado = false;
    }

    if (estadisticas.alias != "ANA") {
        resultado = false;
    }

    if (estadisticas.partidasJugadas != 4) {
        resultado = false;
    }

    if (estadisticas.victorias != 2) {
        resultado = false;
    }

    if (estadisticas.mejorPuntaje != 400) {
        resultado = false;
    }

    if (estadisticas.peorPuntaje != 100) {
        resultado = false;
    }

    if (!casiIguales(estadisticas.mediaPuntaje, 250.0)) {
        resultado = false;
    }

    if (!casiIguales(estadisticas.medianaPuntaje, 250.0)) {
        resultado = false;
    }

    if (!casiIguales(estadisticas.desviacionEstandar, 111.80339887498948)) {
        resultado = false;
    }

    if (!casiIguales(estadisticas.precisionColorPromedio, 0.5)) {
        resultado = false;
    }

    if (estadisticas.mejorColorDisponible || estadisticas.mejorColor != -1) {
        resultado = false;
    }

    if (estadisticas.tipoMasDanoDisponible || estadisticas.tipoMasDano != -1) {
        resultado = false;
    }

    if (!casiIguales(estadisticas.histograma.minimo, 100.0)) {
        resultado = false;
    }

    if (!casiIguales(estadisticas.histograma.maximo, 400.0)) {
        resultado = false;
    }

    if (!casiIguales(estadisticas.histograma.anchoIntervalo, 30.0)) {
        resultado = false;
    }

    if (estadisticas.histograma.frecuencias[0] != 1) {
        resultado = false;
    }

    if (estadisticas.histograma.frecuencias[3] != 1) {
        resultado = false;
    }

    if (estadisticas.histograma.frecuencias[6] != 1) {
        resultado = false;
    }

    if (estadisticas.histograma.frecuencias[9] != 1) {
        resultado = false;
    }

    EstadisticasJugador inexistente;

    if (calcularEstadisticasJugador(historial, "NO_EXISTE", inexistente)) {
        resultado = false;
    }

    HistorialPartidas repetidos;
    inicializarHistorial(repetidos);

    agregarRegistroFinal(repetidos, crearRegistroEstadisticas(10, "IGUAL", 1, 500, 0, 0));
    agregarRegistroFinal(repetidos, crearRegistroEstadisticas(11, "IGUAL", 2, 500, 0, 0));
    agregarRegistroFinal(repetidos, crearRegistroEstadisticas(12, "IGUAL", 3, 500, 0, 0));

    EstadisticasJugador iguales;

    if (!calcularEstadisticasJugador(repetidos, "IGUAL", iguales)) {
        resultado = false;
    }

    if (!casiIguales(iguales.mediaPuntaje, 500.0) || !casiIguales(iguales.medianaPuntaje, 500.0) || !casiIguales(iguales.desviacionEstandar, 0.0)) {
        resultado = false;
    }

    if (iguales.histograma.frecuencias[0] != 3) {
        resultado = false;
    }

    for (int i = 1; i < 10; i++) {
        if (iguales.histograma.frecuencias[i] != 0) {
            resultado = false;
        }
    }

    std::ostringstream salida;
    imprimirHistogramaASCII(estadisticas.histograma, salida);

    if (salida.str().empty()) {
        resultado = false;
    }

    destruirHistorial(repetidos);
    destruirHistorial(historial);

    return resultado;
}

bool pruebaRanking() {
    Ranking ranking;
    inicializarRanking(ranking);

    bool resultado = true;

    if (ranking.cabeza != nullptr || ranking.tam != 0) {
        resultado = false;
    }

    RegistroRanking luis{ "LUIS", 500, 0.60 };
    RegistroRanking ana{ "ANA", 700, 0.50 };
    RegistroRanking mario{ "MARIO", 300, 0.90 };
    RegistroRanking bea{ "BEA", 500, 0.80 };
    RegistroRanking alan{ "ALAN", 500, 0.80 };

    insertarOrdenadoRanking(ranking, luis);

    if (ranking.cabeza == nullptr || ranking.cabeza->dato.alias != "LUIS" || ranking.tam != 1) {
        resultado = false;
    }

    insertarOrdenadoRanking(ranking, ana);
    insertarOrdenadoRanking(ranking, mario);
    insertarOrdenadoRanking(ranking, bea);
    insertarOrdenadoRanking(ranking, alan);

    const RegistroRanking* posicion1 = obtenerPosicionRanking(ranking, 1);
    const RegistroRanking* posicion2 = obtenerPosicionRanking(ranking, 2);
    const RegistroRanking* posicion3 = obtenerPosicionRanking(ranking, 3);
    const RegistroRanking* posicion4 = obtenerPosicionRanking(ranking, 4);
    const RegistroRanking* posicion5 = obtenerPosicionRanking(ranking, 5);

    if (posicion1 == nullptr || posicion1->alias != "ANA") {
        resultado = false;
    }

    if (posicion2 == nullptr || posicion2->alias != "ALAN") {
        resultado = false;
    }

    if (posicion3 == nullptr || posicion3->alias != "BEA") {
        resultado = false;
    }

    if (posicion4 == nullptr || posicion4->alias != "LUIS") {
        resultado = false;
    }

    if (posicion5 == nullptr || posicion5->alias != "MARIO") {
        resultado = false;
    }

    RegistroRanking marioActualizado{ "MARIO", 800, 0.40 };
    insertarOrdenadoRanking(ranking, marioActualizado);

    if (ranking.tam != 5) {
        resultado = false;
    }

    posicion1 = obtenerPosicionRanking(ranking, 1);

    if (posicion1 == nullptr || posicion1->alias != "MARIO" || posicion1->mejorPuntaje != 800) {
        resultado = false;
    }

    const RegistroRanking* marioBuscado = buscarJugadorRanking(ranking, "MARIO");

    if (marioBuscado == nullptr || marioBuscado->mejorPuntaje != 800) {
        resultado = false;
    }

    if (buscarJugadorRanking(ranking, "NO_EXISTE") != nullptr) {
        resultado = false;
    }

    if (obtenerPosicionRanking(ranking, 0) != nullptr) {
        resultado = false;
    }

    if (obtenerPosicionRanking(ranking, 100) != nullptr) {
        resultado = false;
    }

    destruirRanking(ranking);

    if (ranking.cabeza != nullptr || ranking.tam != 0) {
        resultado = false;
    }

    HistorialPartidas historial;
    inicializarHistorial(historial);

    agregarRegistroFinal(historial, crearRegistroEstadisticas(1, "ANA", 1, 100, 1, 0));
    agregarRegistroFinal(historial, crearRegistroEstadisticas(2, "ANA", 2, 900, 1, 1));
    agregarRegistroFinal(historial, crearRegistroEstadisticas(3, "BOB", 1, 900, 4, 1));
    agregarRegistroFinal(historial, crearRegistroEstadisticas(4, "CARLOS", 2, 700, 3, 1));

    inicializarRanking(ranking);
    construirRankingDesdeHistorial(historial, ranking);

    if (ranking.tam != 3) {
        resultado = false;
    }

    posicion1 = obtenerPosicionRanking(ranking, 1);
    posicion2 = obtenerPosicionRanking(ranking, 2);
    posicion3 = obtenerPosicionRanking(ranking, 3);

    if (posicion1 == nullptr || posicion1->alias != "BOB") {
        resultado = false;
    }

    if (posicion2 == nullptr || posicion2->alias != "ANA") {
        resultado = false;
    }

    if (posicion3 == nullptr || posicion3->alias != "CARLOS") {
        resultado = false;
    }

    destruirRanking(ranking);
    destruirHistorial(historial);

    return resultado;
}

bool pruebaLineaTiempo() {
    BitacoraEventos bitacora;
    inicializarBitacora(bitacora);

    Evento evento1{ 10, 1, "ANA", BAJA, 0, 0, 100 };
    Evento evento2{ 20, 1, "ANA", BAJA, 1, 1, 150 };
    Evento evento3{ 30, 1, "ANA", DANO_NUCLEO, 0, 0, 10 };
    Evento evento4{ 40, 1, "BOB", BAJA, 0, 0, 500 };
    Evento evento5{ 50, 2, "ANA", BAJA, 4, 2, 400 };
    Evento evento6{ 60, 2, "ANA", CHOQUE_NAVE, 4, 2, 12 };
    Evento evento7{ 70, 2, "ANA", FIN_OLEADA, -1, -1, 0 };

    agregarEventoFinal(bitacora, evento1);
    agregarEventoFinal(bitacora, evento2);
    agregarEventoFinal(bitacora, evento3);
    agregarEventoFinal(bitacora, evento4);
    agregarEventoFinal(bitacora, evento5);
    agregarEventoFinal(bitacora, evento6);
    agregarEventoFinal(bitacora, evento7);

    bool resultado = true;

    ResumenOleadaJugador ronda1;

    if (!calcularResumenOleadaJugador(bitacora, "ANA", 1, ronda1)) {
        resultado = false;
    }

    if (ronda1.oleada != 1) {
        resultado = false;
    }

    if (ronda1.bajas != 2) {
        resultado = false;
    }

    if (ronda1.danoRecibido != 10) {
        resultado = false;
    }

    if (ronda1.puntajeOleada != 250) {
        resultado = false;
    }

    if (ronda1.puntajeAcumulado != 250) {
        resultado = false;
    }

    ResumenOleadaJugador ronda2;

    if (!calcularResumenOleadaJugador(bitacora, "ANA", 2, ronda2)) {
        resultado = false;
    }

    if (ronda2.bajas != 1) {
        resultado = false;
    }

    if (ronda2.danoRecibido != 12) {
        resultado = false;
    }

    if (ronda2.puntajeOleada != 400) {
        resultado = false;
    }

    if (ronda2.puntajeAcumulado != 650) {
        resultado = false;
    }

    ResumenOleadaJugador bob;

    if (!calcularResumenOleadaJugador(bitacora, "BOB", 1, bob)) {
        resultado = false;
    }

    if (bob.bajas != 1 || bob.puntajeAcumulado != 500) {
        resultado = false;
    }

    std::ostringstream salida;
    imprimirLineaTiempoJugador(bitacora, "ANA", 2, salida);

    std::string texto = salida.str();

    if (texto.find("Oleada 1") == std::string::npos) {
        resultado = false;
    }

    if (texto.find("Oleada 2") == std::string::npos) {
        resultado = false;
    }

    if (texto.find("Puntaje acumulado: 650") == std::string::npos) {
        resultado = false;
    }

    destruirBitacora(bitacora);

    return resultado;
}

bool pruebaIntegracionEstadisticasMotor() {
    Partida partida;
    inicializarPartida(partida, 42u, 1);

    if (!agregarJugadorPartida(partida, "ANA", false)) {
        destruirPartida(partida);
        return false;
    }

    Jugador* jugador = verJugadorPartida(partida, 0);

    if (jugador == nullptr) {
        destruirPartida(partida);
        return false;
    }

    jugador->oleadaAlcanzada = 1;
    jugador->metricas.oleadaAlcanzada = 1;

    ColaSpawn cola;
    inicializarColaSpawn(cola);

    Spawn pendiente;
    pendiente.tick = 1000;
    pendiente.tipo = Dron;
    pendiente.color = 0;
    pendiente.angulo = 0;
    pendiente.sentido = 0;
    encolarSpawn(cola, pendiente);

    Motor motor;
    inicializarMotor(motor, jugador, 1, cola, &partida.bitacora);

    AccionMotor sinAccion;
    sinAccion.cambiarAngulo = false;
    sinAccion.anguloObjetivo = 270.0;
    sinAccion.cambiarColor = false;
    sinAccion.colorNuevo = 0;
    sinAccion.solicitarPulso = false;

    procesarTick(motor, sinAccion);

    Enemigo enemigoIncorrecto;
    enemigoIncorrecto.id = 1;
    enemigoIncorrecto.tipo = Dron;
    enemigoIncorrecto.color = 1;
    enemigoIncorrecto.r = 100.0;
    enemigoIncorrecto.theta = 0.0;
    enemigoIncorrecto.omega = 0.0;
    enemigoIncorrecto.vida = 1;
    enemigoIncorrecto.radio = 6;
    enemigoIncorrecto.ticksColor = 0;

    insertarFinal(motor.enemigos, enemigoIncorrecto);

    Bala balaIncorrecta{ 100.0, 0.0, 0.0, 0.0, 0, 0 };
    insertarInicio(motor.balas, balaIncorrecta);

    moverBalasYResolverImpactos(motor);

    destruirListaEnemigos(motor.enemigos);
    inicializarListaEnemigos(motor.enemigos);

    Enemigo enemigoCorrecto;
    enemigoCorrecto.id = 2;
    enemigoCorrecto.tipo = Dron;
    enemigoCorrecto.color = 0;
    enemigoCorrecto.r = 100.0;
    enemigoCorrecto.theta = 0.0;
    enemigoCorrecto.omega = 0.0;
    enemigoCorrecto.vida = 1;
    enemigoCorrecto.radio = 6;
    enemigoCorrecto.ticksColor = 0;

    insertarFinal(motor.enemigos, enemigoCorrecto);

    Bala balaCorrecta{ 100.0, 0.0, 0.0, 0.0, 0, 0 };
    insertarInicio(motor.balas, balaCorrecta);

    moverBalasYResolverImpactos(motor);

    jugador->cargaPulso = 100;

    AccionMotor usarPulso;
    usarPulso.cambiarAngulo = false;
    usarPulso.anguloObjetivo = motor.nave.anguloObjetivo;
    usarPulso.cambiarColor = false;
    usarPulso.colorNuevo = motor.nave.color;
    usarPulso.solicitarPulso = true;

    aplicarAccion(motor, usarPulso);

    destruirListaEnemigos(motor.enemigos);
    inicializarListaEnemigos(motor.enemigos);

    jugador->vidaNucleo = 5;

    Enemigo enemigoNucleo;
    enemigoNucleo.id = 3;
    enemigoNucleo.tipo = Dron;
    enemigoNucleo.color = 0;
    enemigoNucleo.r = 10.0;
    enemigoNucleo.theta = 0.0;
    enemigoNucleo.omega = 0.0;
    enemigoNucleo.vida = 1;
    enemigoNucleo.radio = 6;
    enemigoNucleo.ticksColor = 0;

    insertarFinal(motor.enemigos, enemigoNucleo);

    resolverChoquesEnemigos(motor);
    verificarFinMotor(motor);

    destruirMotor(motor);
    destruirColaSpawn(cola);

    actualizarMetricasFinalesPartida(partida);

    bool resultado = true;

    if (jugador->metricas.disparos != 1) resultado = false;
    if (jugador->metricas.impactosCorrectos != 1) resultado = false;
    if (jugador->metricas.impactosErroneos != 1) resultado = false;
    if (jugador->metricas.bajasDron != 1) resultado = false;
    if (jugador->metricas.pulsosUtilizados != 1) resultado = false;
    if (jugador->metricas.danoTotal != 5) resultado = false;
    if (jugador->metricas.ticksSobrevividos != 1) resultado = false;
    if (jugador->puntaje != 100) resultado = false;

    if (jugador->metricas.precisionColor != 0.5) {
        resultado = false;
    }

    if (partida.bitacora.tam != 5) {
        resultado = false;
    }

    HistorialPartidas historial;
    inicializarHistorial(historial);

    agregarResultadosPartidaHistorial(partida, 77, "lotes", historial);

    if (contarRegistros(historial) != 1) {
        resultado = false;
    }

    destruirPartida(partida);

    if (historial.cabeza == nullptr || historial.cabeza->dato.jugador != "ANA" || historial.cabeza->dato.puntaje != 100) {
        resultado = false;
    }

    destruirHistorial(historial);

    return resultado;
}