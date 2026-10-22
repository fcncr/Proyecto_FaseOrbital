#ifndef INTERFAZ_ALLEGRO_H
#define INTERFAZ_ALLEGRO_H

#include <string>

#include "VistaAllegro.h"
#include "Jugador.h"
#include "Partida.h"

enum PantallaInterfaz {
    PANTALLA_MENU,
    PANTALLA_REGISTRO,
    PANTALLA_ANUNCIO_TURNO,
    PANTALLA_JUEGO,
    PANTALLA_PAUSA,
    PANTALLA_RESUMEN,
    PANTALLA_GAME_OVER,
    PANTALLA_ESTADISTICAS,
    PANTALLA_RANKING
};

enum OpcionMenuPrincipal {
    MENU_JUGAR,
    MENU_ESTADISTICAS,
    MENU_RANKING,
    MENU_SALIR
};

struct EstadoInterfaz {
    PantallaInterfaz pantallaActual;
    int opcionSeleccionada;
    int jugadorRegistroActual;
    std::string mensaje;
};

struct RegistroGrafico {
    std::string aliasJugadores[6];
    int cantidadJugadores;
};

void inicializarEstadoInterfaz(EstadoInterfaz& estado);

OpcionMenuPrincipal ejecutarMenuPrincipal(
    VistaAllegro& vista,
    EstadoInterfaz& estado,
    bool& cerrarAplicacion
);

bool ejecutarRegistroJugadores(
    VistaAllegro& vista,
    EstadoInterfaz& estado,
    RegistroGrafico& registro,
    bool& cerrarAplicacion
);

void mostrarAnuncioTurno(
    VistaAllegro& vista,
    EstadoInterfaz& estado,
    const Jugador& jugador,
    int numeroOleada,
    bool& cerrarAplicacion
);

void mostrarResumenOleada(
    VistaAllegro& vista,
    EstadoInterfaz& estado,
    const Partida& partida,
    bool& cerrarAplicacion
);

void mostrarFinPartida(
    VistaAllegro& vista,
    EstadoInterfaz& estado,
    const Partida& partida,
    bool& cerrarAplicacion
);

void mostrarPantallaInformativa(
    VistaAllegro& vista,
    EstadoInterfaz& estado,
    PantallaInterfaz pantalla,
    const char* titulo,
    const char* mensaje,
    bool& cerrarAplicacion
);

#endif