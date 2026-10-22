#include "InterfazAllegro.h"

#include <allegro5/allegro_primitives.h>

static bool aliasEmpiezaConBOT(const std::string& alias) {
    return alias.size() >= 3 &&
        alias[0] == 'B' &&
        alias[1] == 'O' &&
        alias[2] == 'T';
}

static bool aliasValido(const std::string& alias) {
    return !alias.empty();
}

static void dibujarFondoInterfaz(
    const VistaAllegro& vista
) {
    al_clear_to_color(
        al_map_rgb(10, 12, 22)
    );

    al_draw_rectangle(
        80.0f,
        60.0f,
        static_cast<float>(vista.ancho) - 80.0f,
        static_cast<float>(vista.alto) - 60.0f,
        al_map_rgb(80, 100, 150),
        2.0f
    );
}

static void dibujarTitulo(
    const VistaAllegro& vista,
    const char* titulo
) {
    al_draw_text(
        vista.fuente,
        al_map_rgb(255, 255, 255),
        static_cast<float>(vista.ancho) / 2.0f,
        100.0f,
        ALLEGRO_ALIGN_CENTER,
        titulo
    );
}

static void dibujarMenuPrincipal(
    const VistaAllegro& vista,
    const EstadoInterfaz& estado
) {
    const char* opciones[4] = {
        "JUGAR",
        "ESTADISTICAS",
        "RANKING",
        "SALIR"
    };

    dibujarFondoInterfaz(vista);
    dibujarTitulo(vista, "FASE ORBITAL");

    for (int i = 0; i < 4; i++) {
        ALLEGRO_COLOR color;

        if (i == estado.opcionSeleccionada) {
            color = al_map_rgb(255, 210, 30);
        }
        else {
            color = al_map_rgb(220, 220, 220);
        }

        al_draw_text(
            vista.fuente,
            color,
            static_cast<float>(vista.ancho) / 2.0f,
            220.0f + static_cast<float>(i * 45),
            ALLEGRO_ALIGN_CENTER,
            opciones[i]
        );
    }

    al_draw_text(
        vista.fuente,
        al_map_rgb(150, 160, 180),
        static_cast<float>(vista.ancho) / 2.0f,
        470.0f,
        ALLEGRO_ALIGN_CENTER,
        "Flechas: seleccionar | ENTER: aceptar | ESC: salir"
    );

    al_flip_display();
}

static void dibujarRegistroCantidad(
    const VistaAllegro& vista,
    int cantidad
) {
    dibujarFondoInterfaz(vista);
    dibujarTitulo(vista, "REGISTRO DE JUGADORES");

    al_draw_textf(
        vista.fuente,
        al_map_rgb(255, 255, 255),
        static_cast<float>(vista.ancho) / 2.0f,
        240.0f,
        ALLEGRO_ALIGN_CENTER,
        "Cantidad de jugadores: %d",
        cantidad
    );

    al_draw_text(
        vista.fuente,
        al_map_rgb(180, 190, 210),
        static_cast<float>(vista.ancho) / 2.0f,
        300.0f,
        ALLEGRO_ALIGN_CENTER,
        "Izquierda / Derecha para cambiar"
    );

    al_draw_text(
        vista.fuente,
        al_map_rgb(180, 190, 210),
        static_cast<float>(vista.ancho) / 2.0f,
        325.0f,
        ALLEGRO_ALIGN_CENTER,
        "ENTER para continuar | ESC para volver"
    );

    al_flip_display();
}

static void dibujarRegistroAlias(
    const VistaAllegro& vista,
    const EstadoInterfaz& estado,
    const RegistroGrafico& registro
) {
    int indice = estado.jugadorRegistroActual;

    const std::string& alias =
        registro.aliasJugadores[indice];

    const char* tipo =
        aliasEmpiezaConBOT(alias)
        ? "BOT"
        : "HUMANO";

    dibujarFondoInterfaz(vista);
    dibujarTitulo(vista, "REGISTRO DE JUGADORES");

    al_draw_textf(
        vista.fuente,
        al_map_rgb(255, 255, 255),
        static_cast<float>(vista.ancho) / 2.0f,
        200.0f,
        ALLEGRO_ALIGN_CENTER,
        "Jugador %d de %d",
        indice + 1,
        registro.cantidadJugadores
    );

    al_draw_textf(
        vista.fuente,
        al_map_rgb(255, 210, 30),
        static_cast<float>(vista.ancho) / 2.0f,
        250.0f,
        ALLEGRO_ALIGN_CENTER,
        "Alias: %s_",
        alias.c_str()
    );

    al_draw_textf(
        vista.fuente,
        al_map_rgb(220, 220, 220),
        static_cast<float>(vista.ancho) / 2.0f,
        300.0f,
        ALLEGRO_ALIGN_CENTER,
        "Tipo: %s",
        tipo
    );

    al_draw_text(
        vista.fuente,
        al_map_rgb(160, 170, 190),
        static_cast<float>(vista.ancho) / 2.0f,
        350.0f,
        ALLEGRO_ALIGN_CENTER,
        "Los alias que comienzan con BOT son jugadores automaticos"
    );

    al_draw_text(
        vista.fuente,
        al_map_rgb(160, 170, 190),
        static_cast<float>(vista.ancho) / 2.0f,
        375.0f,
        ALLEGRO_ALIGN_CENTER,
        "ENTER: confirmar | BACKSPACE: borrar | ESC: volver al menu"
    );

    if (!estado.mensaje.empty()) {
        al_draw_text(
            vista.fuente,
            al_map_rgb(255, 100, 100),
            static_cast<float>(vista.ancho) / 2.0f,
            420.0f,
            ALLEGRO_ALIGN_CENTER,
            estado.mensaje.c_str()
        );
    }

    al_flip_display();
}

static void dibujarAnuncioTurno(
    const VistaAllegro& vista,
    const Jugador& jugador,
    int numeroOleada
) {
    dibujarFondoInterfaz(vista);

    al_draw_text(
        vista.fuente,
        al_map_rgb(255, 210, 30),
        static_cast<float>(vista.ancho) / 2.0f,
        160.0f,
        ALLEGRO_ALIGN_CENTER,
        "TURNO DE"
    );

    al_draw_text(
        vista.fuente,
        al_map_rgb(255, 255, 255),
        static_cast<float>(vista.ancho) / 2.0f,
        210.0f,
        ALLEGRO_ALIGN_CENTER,
        jugador.alias.c_str()
    );

    al_draw_textf(
        vista.fuente,
        al_map_rgb(200, 210, 230),
        static_cast<float>(vista.ancho) / 2.0f,
        260.0f,
        ALLEGRO_ALIGN_CENTER,
        "Oleada: %d",
        numeroOleada
    );

    al_draw_textf(
        vista.fuente,
        al_map_rgb(200, 210, 230),
        static_cast<float>(vista.ancho) / 2.0f,
        290.0f,
        ALLEGRO_ALIGN_CENTER,
        "Vida: %d",
        jugador.vidaNucleo
    );

    al_draw_textf(
        vista.fuente,
        al_map_rgb(200, 210, 230),
        static_cast<float>(vista.ancho) / 2.0f,
        320.0f,
        ALLEGRO_ALIGN_CENTER,
        "Puntaje: %d",
        jugador.puntaje
    );

    if (jugador.esBot) {
        al_draw_text(
            vista.fuente,
            al_map_rgb(255, 100, 190),
            static_cast<float>(vista.ancho) / 2.0f,
            360.0f,
            ALLEGRO_ALIGN_CENTER,
            "BOT"
        );
    }
    else {
        al_draw_text(
            vista.fuente,
            al_map_rgb(0, 220, 255),
            static_cast<float>(vista.ancho) / 2.0f,
            360.0f,
            ALLEGRO_ALIGN_CENTER,
            "HUMANO"
        );
    }

    al_draw_text(
        vista.fuente,
        al_map_rgb(170, 180, 200),
        static_cast<float>(vista.ancho) / 2.0f,
        420.0f,
        ALLEGRO_ALIGN_CENTER,
        "Presione ENTER para comenzar"
    );

    al_flip_display();
}

void inicializarEstadoInterfaz(
    EstadoInterfaz& estado
) {
    estado.pantallaActual = PANTALLA_MENU;
    estado.opcionSeleccionada = 0;
    estado.jugadorRegistroActual = 0;
    estado.mensaje.clear();
}

OpcionMenuPrincipal ejecutarMenuPrincipal(
    VistaAllegro& vista,
    EstadoInterfaz& estado,
    bool& cerrarAplicacion
) {
    estado.pantallaActual =
        PANTALLA_MENU;

    estado.opcionSeleccionada = 0;

    al_flush_event_queue(
        vista.colaEventos
    );

    dibujarMenuPrincipal(
        vista,
        estado
    );

    while (!cerrarAplicacion) {
        ALLEGRO_EVENT evento;

        al_wait_for_event(
            vista.colaEventos,
            &evento
        );

        if (evento.type ==
            ALLEGRO_EVENT_DISPLAY_CLOSE) {

            cerrarAplicacion = true;
            return MENU_SALIR;
        }

        if (evento.type ==
            ALLEGRO_EVENT_KEY_DOWN) {

            if (evento.keyboard.keycode ==
                ALLEGRO_KEY_UP) {

                estado.opcionSeleccionada--;

                if (estado.opcionSeleccionada < 0) {
                    estado.opcionSeleccionada = 3;
                }
            }
            else if (evento.keyboard.keycode ==
                ALLEGRO_KEY_DOWN) {

                estado.opcionSeleccionada++;

                if (estado.opcionSeleccionada > 3) {
                    estado.opcionSeleccionada = 0;
                }
            }
            else if (evento.keyboard.keycode ==
                ALLEGRO_KEY_ENTER) {

                return static_cast<OpcionMenuPrincipal>(
                    estado.opcionSeleccionada
                    );
            }
            else if (evento.keyboard.keycode ==
                ALLEGRO_KEY_ESCAPE) {

                return MENU_SALIR;
            }

            dibujarMenuPrincipal(
                vista,
                estado
            );
        }
    }

    return MENU_SALIR;
}

bool ejecutarRegistroJugadores(
    VistaAllegro& vista,
    EstadoInterfaz& estado,
    RegistroGrafico& registro,
    bool& cerrarAplicacion
) {
    estado.pantallaActual =
        PANTALLA_REGISTRO;

    estado.jugadorRegistroActual = 0;
    estado.mensaje.clear();

    registro.cantidadJugadores = 1;

    for (int i = 0; i < 6; i++) {
        registro.aliasJugadores[i].clear();
    }

    bool seleccionandoCantidad = true;

    al_flush_event_queue(
        vista.colaEventos
    );

    dibujarRegistroCantidad(
        vista,
        registro.cantidadJugadores
    );

    while (!cerrarAplicacion) {
        ALLEGRO_EVENT evento;

        al_wait_for_event(
            vista.colaEventos,
            &evento
        );

        if (evento.type ==
            ALLEGRO_EVENT_DISPLAY_CLOSE) {

            cerrarAplicacion = true;
            return false;
        }

        if (seleccionandoCantidad) {
            if (evento.type ==
                ALLEGRO_EVENT_KEY_DOWN) {

                if (evento.keyboard.keycode ==
                    ALLEGRO_KEY_LEFT) {

                    if (registro.cantidadJugadores > 1) {
                        registro.cantidadJugadores--;
                    }
                }
                else if (evento.keyboard.keycode ==
                    ALLEGRO_KEY_RIGHT) {

                    if (registro.cantidadJugadores < 6) {
                        registro.cantidadJugadores++;
                    }
                }
                else if (evento.keyboard.keycode ==
                    ALLEGRO_KEY_ENTER) {

                    seleccionandoCantidad = false;
                    estado.jugadorRegistroActual = 0;
                    estado.mensaje.clear();

                    dibujarRegistroAlias(
                        vista,
                        estado,
                        registro
                    );

                    continue;
                }
                else if (evento.keyboard.keycode ==
                    ALLEGRO_KEY_ESCAPE) {

                    return false;
                }

                dibujarRegistroCantidad(
                    vista,
                    registro.cantidadJugadores
                );
            }

            continue;
        }

        if (evento.type ==
            ALLEGRO_EVENT_KEY_CHAR) {

            int indice =
                estado.jugadorRegistroActual;

            std::string& alias =
                registro.aliasJugadores[indice];

            if (evento.keyboard.keycode ==
                ALLEGRO_KEY_BACKSPACE) {

                if (!alias.empty()) {
                    alias.pop_back();
                }

                estado.mensaje.clear();
            }
            else if (evento.keyboard.keycode ==
                ALLEGRO_KEY_ENTER) {

                if (!aliasValido(alias)) {
                    estado.mensaje =
                        "El alias no puede estar vacio.";
                }
                else {
                    estado.mensaje.clear();

                    if (indice + 1 >=
                        registro.cantidadJugadores) {

                        return true;
                    }

                    estado.jugadorRegistroActual++;
                }
            }
            else {
                int caracter =
                    evento.keyboard.unichar;

                bool permitido =
                    (caracter >= 'A' &&
                        caracter <= 'Z') ||
                    (caracter >= 'a' &&
                        caracter <= 'z') ||
                    (caracter >= '0' &&
                        caracter <= '9') ||
                    caracter == '_';

                if (permitido &&
                    alias.size() < 15) {

                    alias +=
                        static_cast<char>(
                            caracter
                            );

                    estado.mensaje.clear();
                }
            }

            dibujarRegistroAlias(
                vista,
                estado,
                registro
            );
        }
        else if (evento.type ==
            ALLEGRO_EVENT_KEY_DOWN &&
            evento.keyboard.keycode ==
            ALLEGRO_KEY_ESCAPE) {

            return false;
        }
    }

    return false;
}

void mostrarAnuncioTurno(
    VistaAllegro& vista,
    EstadoInterfaz& estado,
    const Jugador& jugador,
    int numeroOleada,
    bool& cerrarAplicacion
) {
    estado.pantallaActual =
        PANTALLA_ANUNCIO_TURNO;

    al_flush_event_queue(
        vista.colaEventos
    );

    dibujarAnuncioTurno(
        vista,
        jugador,
        numeroOleada
    );

    while (!cerrarAplicacion) {
        ALLEGRO_EVENT evento;

        al_wait_for_event(
            vista.colaEventos,
            &evento
        );

        if (evento.type ==
            ALLEGRO_EVENT_DISPLAY_CLOSE) {

            cerrarAplicacion = true;
            return;
        }

        if (evento.type ==
            ALLEGRO_EVENT_KEY_DOWN &&
            evento.keyboard.keycode ==
            ALLEGRO_KEY_ENTER) {

            return;
        }
    }
}

void mostrarPantallaInformativa(
    VistaAllegro& vista,
    EstadoInterfaz& estado,
    PantallaInterfaz pantalla,
    const char* titulo,
    const char* mensaje,
    bool& cerrarAplicacion
) {
    estado.pantallaActual =
        pantalla;

    al_flush_event_queue(
        vista.colaEventos
    );

    dibujarFondoInterfaz(vista);
    dibujarTitulo(vista, titulo);

    al_draw_text(
        vista.fuente,
        al_map_rgb(220, 220, 220),
        static_cast<float>(vista.ancho) / 2.0f,
        260.0f,
        ALLEGRO_ALIGN_CENTER,
        mensaje
    );

    al_draw_text(
        vista.fuente,
        al_map_rgb(160, 170, 190),
        static_cast<float>(vista.ancho) / 2.0f,
        380.0f,
        ALLEGRO_ALIGN_CENTER,
        "ENTER o ESC para volver"
    );

    al_flip_display();

    while (!cerrarAplicacion) {
        ALLEGRO_EVENT evento;

        al_wait_for_event(
            vista.colaEventos,
            &evento
        );

        if (evento.type ==
            ALLEGRO_EVENT_DISPLAY_CLOSE) {

            cerrarAplicacion = true;
            return;
        }

        if (evento.type ==
            ALLEGRO_EVENT_KEY_DOWN) {

            if (evento.keyboard.keycode ==
                ALLEGRO_KEY_ENTER ||
                evento.keyboard.keycode ==
                ALLEGRO_KEY_ESCAPE) {

                return;
            }
        }
    }
}

void mostrarResumenOleada(VistaAllegro& vista,EstadoInterfaz& estado,const Partida& partida,bool& cerrarAplicacion
) {
    estado.pantallaActual =
        PANTALLA_RESUMEN;

    al_flush_event_queue(
        vista.colaEventos
    );

    dibujarFondoInterfaz(vista);

    al_draw_textf(
        vista.fuente,
        al_map_rgb(255, 210, 30),
        static_cast<float>(vista.ancho) / 2.0f,
        80.0f,
        ALLEGRO_ALIGN_CENTER,
        "OLEADA %d COMPLETADA",
        partida.numeroRonda
    );

    float y = 150.0f;

    for (int i = 0;
        i < partida.cantidadJugadores;
        i++) {

        const Jugador& jugador = partida.jugadores[i];

        ALLEGRO_COLOR color =jugador.eliminado? al_map_rgb(170, 90, 90): al_map_rgb(220, 220, 220);

        al_draw_textf(
            vista.fuente,
            color,
            150.0f,
            y,
            0,
            "%s",
            jugador.alias.c_str()
        );

        al_draw_textf(
            vista.fuente,
            color,
            330.0f,
            y,
            0,
            "Puntos: %d",
            jugador.puntaje
        );

        al_draw_textf(
            vista.fuente,
            color,
            500.0f,
            y,
            0,
            "Vida: %d",
            jugador.vidaNucleo
        );

        if (jugador.eliminado) {
            al_draw_text(
                vista.fuente,
                al_map_rgb(255, 80, 80),
                650.0f,
                y,
                0,
                "ELIMINADO"
            );
        }

        y += 45.0f;
    }

    al_draw_text(
        vista.fuente,
        al_map_rgb(160, 170, 190),
        static_cast<float>(vista.ancho) / 2.0f,
        520.0f,
        ALLEGRO_ALIGN_CENTER,
        "ENTER para continuar"
    );

    al_flip_display();

    while (!cerrarAplicacion) {
        ALLEGRO_EVENT evento;

        al_wait_for_event(
            vista.colaEventos,
            &evento
        );

        if (evento.type ==
            ALLEGRO_EVENT_DISPLAY_CLOSE) {

            cerrarAplicacion = true;
            return;
        }

        if (evento.type ==
            ALLEGRO_EVENT_KEY_DOWN &&
            evento.keyboard.keycode ==
            ALLEGRO_KEY_ENTER) {

            return;
        }
    }
}

void mostrarFinPartida(VistaAllegro& vista, EstadoInterfaz& estado, const Partida& partida, bool& cerrarAplicacion
) {
    estado.pantallaActual =
        PANTALLA_GAME_OVER;

    al_flush_event_queue(
        vista.colaEventos
    );

    dibujarFondoInterfaz(vista);

    al_draw_text(
        vista.fuente,
        al_map_rgb(255, 210, 30),
        static_cast<float>(vista.ancho) / 2.0f,
        70.0f,
        ALLEGRO_ALIGN_CENTER,
        "FIN DE PARTIDA"
    );

    float y = 150.0f;

    for (int posicion = 1;
        posicion <= partida.cantidadJugadores;
        posicion++) {

        for (int i = 0;
            i < partida.cantidadJugadores;
            i++) {

            const Jugador& jugador =
                partida.jugadores[i];

            if (jugador.metricas.posicionFinal ==
                posicion) {

                al_draw_textf(
                    vista.fuente,
                    al_map_rgb(230, 230, 230),
                    static_cast<float>(vista.ancho) / 2.0f,
                    y,
                    ALLEGRO_ALIGN_CENTER,
                    "%d. %s   %d pts",
                    posicion,
                    jugador.alias.c_str(),
                    jugador.puntaje
                );

                y += 40.0f;
            }
        }
    }

    double precisiones[6];

    for (int i = 0;
        i < partida.cantidadJugadores;
        i++) {

        precisiones[i] =
            partida.jugadores[i]
            .metricas.precisionColor;
    }

    const Jugador* ganador =
        determinarGanadorPartida(
            partida,
            precisiones
        );

    if (ganador != nullptr) {
        al_draw_textf(
            vista.fuente,
            al_map_rgb(0, 220, 255),
            static_cast<float>(vista.ancho) / 2.0f,
            430.0f,
            ALLEGRO_ALIGN_CENTER,
            "Ganador: %s",
            ganador->alias.c_str()
        );
    }
    else {
        al_draw_text(
            vista.fuente,
            al_map_rgb(0, 220, 255),
            static_cast<float>(vista.ancho) / 2.0f,
            430.0f,
            ALLEGRO_ALIGN_CENTER,
            "Ganador: empate"
        );
    }

    al_draw_text(
        vista.fuente,
        al_map_rgb(160, 170, 190),
        static_cast<float>(vista.ancho) / 2.0f,
        500.0f,
        ALLEGRO_ALIGN_CENTER,
        "ENTER para volver al menu"
    );

    al_flip_display();

    while (!cerrarAplicacion) {
        ALLEGRO_EVENT evento;

        al_wait_for_event(
            vista.colaEventos,
            &evento
        );

        if (evento.type ==
            ALLEGRO_EVENT_DISPLAY_CLOSE) {

            cerrarAplicacion = true;
            return;
        }

        if (evento.type ==
            ALLEGRO_EVENT_KEY_DOWN &&
            evento.keyboard.keycode ==
            ALLEGRO_KEY_ENTER) {

            return;
        }
    }
}