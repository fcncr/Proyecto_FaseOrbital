#ifndef INTERFAZ_ALLEGRO_H
#define INTERFAZ_ALLEGRO_H

#include <string>

#include "VistaAllegro.h"
#include "ModelosJugador.h"
#include "Partida.h"
#include "HistorialPartidas.h"

using namespace std; //permite usar string sin escribir el prefijo

enum PantallaInterfaz { //identifica la pantalla que esta atendiendo la aplicacion
    PANTALLA_MENU, //permite elegir entre jugar consultar resultados o salir
    PANTALLA_REGISTRO, //recibe la cantidad de jugadores y sus alias
    PANTALLA_ANUNCIO_TURNO, //presenta al participante antes de comenzar su turno
    PANTALLA_JUEGO, //indica que la partida esta en marcha
    PANTALLA_PAUSA, //indica que la simulacion esta pausada
    PANTALLA_RESUMEN, //muestra el estado de los jugadores al terminar una oleada
    PANTALLA_GAME_OVER, //presenta los puestos y el ganador al terminar la partida
    PANTALLA_ESTADISTICAS, //permite consultar los resultados historicos por jugador
    PANTALLA_RANKING //muestra los primeros puestos del historial
};

enum OpcionMenuPrincipal { //relaciona cada opcion del menu con la tarea que debe ejecutar la aplicacion
    MENU_JUGAR, //inicia el registro de una partida nueva
    MENU_ESTADISTICAS, //abre la consulta de estadisticas
    MENU_RANKING, //abre la clasificacion historica
    MENU_SALIR //solicita terminar la aplicacion
};

struct EstadoInterfaz { //conserva la pantalla y las selecciones que se muestran al usuario
    PantallaInterfaz pantallaActual; //identifica la pantalla actual
    int opcionSeleccionada; //opcion marcada en el menu contando desde cero
    int jugadorRegistroActual; //posicion del alias que se esta escribiendo contando desde cero
    string mensaje; //aviso que se muestra al validar el registro de jugadores
};

struct RegistroGrafico { //reune los participantes elegidos antes de crear la partida
    string aliasJugadores[6]; //guarda hasta seis alias en el orden de registro
    int cantidadJugadores; //cantidad elegida entre uno y seis al completar el registro
};

//deja la interfaz en el menu con la primera opcion seleccionada y sin mensajes pendientes
void inicializarEstadoInterfaz(EstadoInterfaz& estado);

//atiende el menu con las flechas y devuelve la opcion elegida con enter o salir con escape
//si recibe el cierre de la ventana tambien activa la solicitud de cerrar la aplicacion
OpcionMenuPrincipal ejecutarMenuPrincipal(VistaAllegro& vista, EstadoInterfaz& estado, bool& cerrarAplicacion);

//recibe de uno a seis alias no vacios de hasta quince caracteres entre letras sin acentos digitos y guiones bajos
//devuelve verdadero al completar el registro y falso si se cancela o se cierra la ventana
bool ejecutarRegistroJugadores(VistaAllegro& vista, EstadoInterfaz& estado, RegistroGrafico& registro, bool& cerrarAplicacion);

//presenta al jugador y la oleada y espera enter para comenzar o el cierre de la ventana
void mostrarAnuncioTurno(VistaAllegro& vista, EstadoInterfaz& estado, const Jugador& jugador, int numeroOleada, bool& cerrarAplicacion);

//muestra los puntos y la vida de todos los jugadores y espera enter o el cierre de la ventana
void mostrarResumenOleada(VistaAllegro& vista, EstadoInterfaz& estado, const Partida& partida, bool& cerrarAplicacion);

//muestra los puestos ya calculados y el ganador o empate y espera enter o el cierre de la ventana
void mostrarFinPartida(VistaAllegro& vista, EstadoInterfaz& estado, const Partida& partida, bool& cerrarAplicacion);

//presenta un titulo y mensaje y espera enter escape o el cierre de la ventana
void mostrarPantallaInformativa(VistaAllegro& vista, EstadoInterfaz& estado, PantallaInterfaz pantalla, const char* titulo, const char* mensaje, bool& cerrarAplicacion);

//permite recorrer las estadisticas de los jugadores con las flechas siguiendo el orden del ranking
//termina con enter escape o el cierre de la ventana y libera el ranking temporal
void mostrarEstadisticasHistoricas(VistaAllegro& vista, EstadoInterfaz& estado, const HistorialPartidas& historial, bool& cerrarAplicacion);

//muestra hasta diez puestos del historial y espera enter escape o el cierre de la ventana
void mostrarRankingGrafico(VistaAllegro& vista, EstadoInterfaz& estado, const HistorialPartidas& historial, bool& cerrarAplicacion);

#endif
