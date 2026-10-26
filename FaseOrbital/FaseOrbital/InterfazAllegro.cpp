#include "InterfazAllegro.h"

#include <allegro5/allegro_primitives.h>

#include "Estadisticas.h"
#include "Ranking.h"

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

static void dibujarHistogramaEstadisticas(const VistaAllegro& vista, const HistogramaPuntajes& histograma) {
    float inicioX = 500.0f;
    float inicioY = 190.0f;
    float anchoMaximo = 150.0f;
    float altoBarra = 16.0f;
    float separacion = 27.0f;

    al_draw_text(vista.fuente, al_map_rgb(255, 210, 30), 560.0f, 150.0f, ALLEGRO_ALIGN_CENTER, "HISTOGRAMA DE PUNTAJES");

    if (histograma.total <= 0) {
        al_draw_text(vista.fuente, al_map_rgb(180, 190, 210), 560.0f, 220.0f, ALLEGRO_ALIGN_CENTER, "Sin puntajes registrados");
        return;
    }

    if (histograma.minimo == histograma.maximo) {
        int frecuencia = histograma.frecuencias[0];

        al_draw_textf(vista.fuente, al_map_rgb(220, 220, 220), inicioX - 15.0f, 220.0f, ALLEGRO_ALIGN_RIGHT, "%.0f pts", histograma.minimo);

        al_draw_rectangle(inicioX, 220.0f, inicioX + anchoMaximo, 220.0f + altoBarra, al_map_rgb(80, 100, 140), 1.0f);

        if (frecuencia > 0) {
            al_draw_filled_rectangle(inicioX, 220.0f, inicioX + anchoMaximo, 220.0f + altoBarra, al_map_rgb(0, 190, 230));
        }

        al_draw_textf(vista.fuente, al_map_rgb(255, 255, 255), inicioX + anchoMaximo + 10.0f, 220.0f, 0, "%d", frecuencia);

        return;
    }

    int frecuenciaMaxima = 0;

    for (int i = 0; i < 10; i++) {
        if (histograma.frecuencias[i] > frecuenciaMaxima) {
            frecuenciaMaxima = histograma.frecuencias[i];
        }
    }

    for (int i = 0; i < 10; i++) {
        float y = inicioY + separacion * static_cast<float>(i);

        double inferior = histograma.minimo + histograma.anchoIntervalo * static_cast<double>(i);
        double superior = inferior + histograma.anchoIntervalo;

        if (i == 9) {
            superior = histograma.maximo;
        }

        float ancho = 0.0f;

        if (frecuenciaMaxima > 0) {
            ancho = anchoMaximo * static_cast<float>(histograma.frecuencias[i]) / static_cast<float>(frecuenciaMaxima);
        }

        al_draw_textf(vista.fuente, al_map_rgb(180, 190, 210), inicioX - 15.0f, y, ALLEGRO_ALIGN_RIGHT, "%.0f - %.0f", inferior, superior);

        al_draw_rectangle(inicioX, y, inicioX + anchoMaximo, y + altoBarra, al_map_rgb(80, 100, 140), 1.0f);

        if (ancho > 0.0f) {
            al_draw_filled_rectangle(inicioX, y, inicioX + ancho, y + altoBarra, al_map_rgb(0, 190, 230));
        }

        al_draw_textf(vista.fuente, al_map_rgb(255, 255, 255), inicioX + anchoMaximo + 10.0f, y, 0, "%d", histograma.frecuencias[i]);
    }
}

static void dibujarRankingGrafico(const VistaAllegro& vista, const Ranking& ranking) {
    dibujarFondoInterfaz(vista);
    dibujarTitulo(vista, "RANKING TOP 10");

    al_draw_text(vista.fuente, al_map_rgb(150, 160, 180), 130.0f, 155.0f, 0, "POS");
    al_draw_text(vista.fuente, al_map_rgb(150, 160, 180), 190.0f, 155.0f, 0, "JUGADOR");
    al_draw_text(vista.fuente, al_map_rgb(150, 160, 180), 430.0f, 155.0f, 0, "PUNTAJE");
    al_draw_text(vista.fuente, al_map_rgb(150, 160, 180), 570.0f, 155.0f, 0, "PRECISION");

    const NodoRanking* actual = ranking.cabeza;
    int posicion = 1;
    float y = 195.0f;

    while (actual != nullptr && posicion <= 10) {
        ALLEGRO_COLOR color = al_map_rgb(220, 220, 220);

        if (posicion == 1) {
            color = al_map_rgb(255, 210, 30);
        }

        al_draw_textf(vista.fuente, color, 140.0f, y, ALLEGRO_ALIGN_CENTER, "%d", posicion);
        al_draw_text(vista.fuente, color, 190.0f, y, 0, actual->dato.alias.c_str());
        al_draw_textf(vista.fuente, color, 430.0f, y, 0, "%d", actual->dato.mejorPuntaje);
        al_draw_textf(vista.fuente, color, 570.0f, y, 0, "%.2f%%", actual->dato.precisionColor * 100.0);

        actual = actual->sig;
        posicion++;
        y += 32.0f;
    }

    al_draw_text(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 510.0f, ALLEGRO_ALIGN_CENTER, "ESC o ENTER: volver al menu");

    al_flip_display();
}

static void dibujarRankingVacio(const VistaAllegro& vista) {
    dibujarFondoInterfaz(vista);
    dibujarTitulo(vista, "RANKING TOP 10");

    al_draw_text(vista.fuente, al_map_rgb(220, 220, 220), static_cast<float>(vista.ancho) / 2.0f, 270.0f, ALLEGRO_ALIGN_CENTER, "No hay partidas registradas.");

    al_draw_text(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 330.0f, ALLEGRO_ALIGN_CENTER, "ENTER o ESC para volver");

    al_flip_display();
}

static void dibujarEstadisticasJugadorGrafico(const VistaAllegro& vista, const EstadisticasJugador& estadisticas, int posicion, int totalJugadores) {
    dibujarFondoInterfaz(vista);
    dibujarTitulo(vista, "ESTADISTICAS HISTORICAS");

    ALLEGRO_COLOR texto = al_map_rgb(220, 220, 220);
    ALLEGRO_COLOR destacado = al_map_rgb(255, 210, 30);

    al_draw_textf(vista.fuente, destacado, 130.0f, 145.0f, 0, "Jugador: %s", estadisticas.alias.c_str());

    al_draw_textf(vista.fuente, texto, 130.0f, 180.0f, 0, "Partidas: %d", estadisticas.partidasJugadas);
    al_draw_textf(vista.fuente, texto, 130.0f, 210.0f, 0, "Victorias: %d", estadisticas.victorias);
    al_draw_textf(vista.fuente, texto, 130.0f, 240.0f, 0, "Mejor puntaje: %d", estadisticas.mejorPuntaje);
    al_draw_textf(vista.fuente, texto, 130.0f, 270.0f, 0, "Peor puntaje: %d", estadisticas.peorPuntaje);
    al_draw_textf(vista.fuente, texto, 130.0f, 300.0f, 0, "Media: %.2f", estadisticas.mediaPuntaje);
    al_draw_textf(vista.fuente, texto, 130.0f, 330.0f, 0, "Mediana: %.2f", estadisticas.medianaPuntaje);
    al_draw_textf(vista.fuente, texto, 130.0f, 360.0f, 0, "Desviacion: %.2f", estadisticas.desviacionEstandar);
    al_draw_textf(vista.fuente, texto, 130.0f, 390.0f, 0, "Precision: %.2f%%", estadisticas.precisionColorPromedio * 100.0);

    if (estadisticas.mejorColorDisponible) {
        al_draw_textf(
            vista.fuente,
            texto,
            130.0f,
            420.0f,
            0,
            "Mejor color: %s",
            nombreColorEstadistica(estadisticas.mejorColor)
        );
    }
    else {
        al_draw_text(
            vista.fuente,
            al_map_rgb(150, 160, 180),
            130.0f,
            420.0f,
            0,
            "Mejor color: no disponible"
        );
    }

    if (estadisticas.tipoMasDanoDisponible) {
        al_draw_textf(
            vista.fuente,
            texto,
            130.0f,
            450.0f,
            0,
            "Tipo mas dano: %s",
            nombreTipoEnemigoEstadistica(estadisticas.tipoMasDano)
        );
    }
    else {
        al_draw_text(
            vista.fuente,
            al_map_rgb(150, 160, 180),
            130.0f,
            450.0f,
            0,
            "Tipo mas dano: no disponible"
        );
    }

    dibujarHistogramaEstadisticas(vista, estadisticas.histograma);

    al_draw_textf(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 515.0f, ALLEGRO_ALIGN_CENTER, "Jugador %d de %d", posicion, totalJugadores);

    al_draw_text(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 545.0f, ALLEGRO_ALIGN_CENTER, "IZQUIERDA / DERECHA: cambiar jugador | ESC o ENTER: volver");

    al_flip_display();
}

static void dibujarEstadisticasVacias(const VistaAllegro& vista) {
    dibujarFondoInterfaz(vista);
    dibujarTitulo(vista, "ESTADISTICAS HISTORICAS");

    al_draw_text(vista.fuente, al_map_rgb(220, 220, 220), static_cast<float>(vista.ancho) / 2.0f, 270.0f, ALLEGRO_ALIGN_CENTER, "No hay partidas registradas.");

    al_draw_text(vista.fuente, al_map_rgb(160, 170, 190), static_cast<float>(vista.ancho) / 2.0f, 330.0f, ALLEGRO_ALIGN_CENTER, "ENTER o ESC para volver");

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

void mostrarEstadisticasHistoricas(VistaAllegro& vista, EstadoInterfaz& estado, const HistorialPartidas& historial, bool& cerrarAplicacion) {
    estado.pantallaActual = PANTALLA_ESTADISTICAS;

    Ranking ranking;
    inicializarRanking(ranking);
    construirRankingDesdeHistorial(historial, ranking);

    al_flush_event_queue(vista.colaEventos);

    if (ranking.tam == 0) {
        dibujarEstadisticasVacias(vista);

        while (!cerrarAplicacion) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(vista.colaEventos, &evento);

            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                cerrarAplicacion = true;
                break;
            }

            if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER || evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    break;
                }
            }
        }

        destruirRanking(ranking);
        return;
    }

    int posicion = 1;

    while (!cerrarAplicacion) {
        const RegistroRanking* registro = obtenerPosicionRanking(ranking, posicion);

        if (registro == nullptr) {
            break;
        }

        EstadisticasJugador estadisticas;

        if (!calcularEstadisticasJugador(historial, registro->alias, estadisticas)) {
            break;
        }

        dibujarEstadisticasJugadorGrafico(vista, estadisticas, posicion, ranking.tam);

        ALLEGRO_EVENT evento;
        al_wait_for_event(vista.colaEventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            cerrarAplicacion = true;
            break;
        }

        if (evento.type != ALLEGRO_EVENT_KEY_DOWN) {
            continue;
        }

        if (evento.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
            posicion++;

            if (posicion > ranking.tam) {
                posicion = 1;
            }
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_LEFT) {
            posicion--;

            if (posicion < 1) {
                posicion = ranking.tam;
            }
        }
        else if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER || evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            break;
        }
    }

    destruirRanking(ranking);
}

void mostrarRankingGrafico(VistaAllegro& vista, EstadoInterfaz& estado, const HistorialPartidas& historial, bool& cerrarAplicacion) {
    estado.pantallaActual = PANTALLA_RANKING;

    Ranking ranking;
    inicializarRanking(ranking);
    construirRankingDesdeHistorial(historial, ranking);

    al_flush_event_queue(vista.colaEventos);

    if (ranking.tam == 0) {
        dibujarRankingVacio(vista);
    }
    else {
        dibujarRankingGrafico(vista, ranking);
    }

    while (!cerrarAplicacion) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(vista.colaEventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            cerrarAplicacion = true;
            break;
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE || evento.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                break;
            }
        }
    }

    destruirRanking(ranking);
}