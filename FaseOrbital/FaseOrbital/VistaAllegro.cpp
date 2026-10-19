#include "VistaAllegro.h"

#include <allegro5/allegro_primitives.h>
#include <cmath>

static const double PI_VISTA = 3.14159265358979323846;
static const double ESCALA_MUNDO = 0.80;

static ALLEGRO_COLOR colorJuego(int color) {
    if (color == 0) {
        return al_map_rgb(0, 220, 255);
    }

    if (color == 1) {
        return al_map_rgb(255, 60, 190);
    }

    if (color == 2) {
        return al_map_rgb(255, 210, 30);
    }

    return al_map_rgb(200, 200, 200);
}

static void polarAPantalla(
    const VistaAllegro& vista,
    double radio,
    double angulo,
    float& x,
    float& y
) {
    double radianes = angulo * PI_VISTA / 180.0;

    double centroX = static_cast<double>(vista.ancho) / 2.0;
    double centroY = static_cast<double>(vista.alto) / 2.0;

    x = static_cast<float>(
        centroX +
        radio * ESCALA_MUNDO * std::cos(radianes)
        );

    y = static_cast<float>(
        centroY +
        radio * ESCALA_MUNDO * std::sin(radianes)
        );
}

static void dibujarNucleoYOrbita(
    const VistaAllegro& vista
) {
    float centroX =
        static_cast<float>(vista.ancho) / 2.0f;

    float centroY =
        static_cast<float>(vista.alto) / 2.0f;

    float radioNucleo =
        static_cast<float>(20.0 * ESCALA_MUNDO);

    float radioOrbita =
        static_cast<float>(100.0 * ESCALA_MUNDO);

    al_draw_filled_circle(
        centroX,
        centroY,
        radioNucleo,
        al_map_rgb(70, 90, 130)
    );

    al_draw_circle(
        centroX,
        centroY,
        radioNucleo,
        al_map_rgb(180, 210, 255),
        2.0f
    );

    al_draw_circle(
        centroX,
        centroY,
        radioOrbita,
        al_map_rgb(90, 100, 120),
        1.5f
    );
}

static void dibujarNave(
    const VistaAllegro& vista,
    const Motor& motor
) {
    float x;
    float y;

    polarAPantalla(
        vista,
        100.0,
        motor.nave.anguloActual,
        x,
        y
    );

    double radianes =
        motor.nave.anguloActual *
        PI_VISTA /
        180.0;

    float direccionX =
        static_cast<float>(std::cos(radianes));

    float direccionY =
        static_cast<float>(std::sin(radianes));

    float perpendicularX = -direccionY;
    float perpendicularY = direccionX;

    float puntaX = x + direccionX * 13.0f;
    float puntaY = y + direccionY * 13.0f;

    float baseX = x - direccionX * 8.0f;
    float baseY = y - direccionY * 8.0f;

    float izquierdaX =
        baseX + perpendicularX * 7.0f;

    float izquierdaY =
        baseY + perpendicularY * 7.0f;

    float derechaX =
        baseX - perpendicularX * 7.0f;

    float derechaY =
        baseY - perpendicularY * 7.0f;

    ALLEGRO_COLOR color =
        colorJuego(motor.nave.color);

    al_draw_filled_triangle(
        puntaX,
        puntaY,
        izquierdaX,
        izquierdaY,
        derechaX,
        derechaY,
        color
    );

    al_draw_triangle(
        puntaX,
        puntaY,
        izquierdaX,
        izquierdaY,
        derechaX,
        derechaY,
        al_map_rgb(255, 255, 255),
        1.0f
    );
}

static void dibujarBalas(
    const VistaAllegro& vista,
    const Motor& motor
) {
    const NodoBala* actual =
        motor.balas.cabeza;

    float centroX =
        static_cast<float>(vista.ancho) / 2.0f;

    float centroY =
        static_cast<float>(vista.alto) / 2.0f;

    while (actual != nullptr) {
        float x =
            centroX +
            static_cast<float>(
                actual->dato.x * ESCALA_MUNDO
                );

        float y =
            centroY +
            static_cast<float>(
                actual->dato.y * ESCALA_MUNDO
                );

        al_draw_filled_circle(
            x,
            y,
            3.0f,
            colorJuego(actual->dato.color)
        );

        actual = actual->sig;
    }
}

static void dibujarEnemigo(
    const VistaAllegro& vista,
    const Enemigo& enemigo
) {
    float x;
    float y;

    polarAPantalla(
        vista,
        enemigo.r,
        enemigo.theta,
        x,
        y
    );

    float radio =
        static_cast<float>(
            enemigo.radio * ESCALA_MUNDO
            );

    if (radio < 5.0f) {
        radio = 5.0f;
    }

    ALLEGRO_COLOR color =
        colorJuego(enemigo.color);

    ALLEGRO_COLOR borde =
        al_map_rgb(255, 255, 255);

    switch (enemigo.tipo) {
    case Dron:
        al_draw_filled_circle(
            x,
            y,
            radio,
            color
        );

        al_draw_circle(
            x,
            y,
            radio,
            borde,
            1.0f
        );
        break;

    case Girador:
        al_draw_filled_triangle(
            x,
            y - radio - 2.0f,
            x - radio,
            y + radio,
            x + radio,
            y + radio,
            color
        );

        al_draw_triangle(
            x,
            y - radio - 2.0f,
            x - radio,
            y + radio,
            x + radio,
            y + radio,
            borde,
            1.0f
        );
        break;

    case Divisor:
        al_draw_filled_rectangle(
            x - radio,
            y - radio,
            x + radio,
            y + radio,
            color
        );

        al_draw_line(
            x - radio,
            y,
            x + radio,
            y,
            borde,
            1.0f
        );

        al_draw_line(
            x,
            y - radio,
            x,
            y + radio,
            borde,
            1.0f
        );
        break;

    case Mini:
        al_draw_filled_circle(
            x,
            y,
            radio,
            color
        );

        al_draw_circle(
            x,
            y,
            radio + 2.0f,
            borde,
            1.0f
        );
        break;

    case Tanque:
        al_draw_filled_rectangle(
            x - radio - 2.0f,
            y - radio - 2.0f,
            x + radio + 2.0f,
            y + radio + 2.0f,
            color
        );

        al_draw_rectangle(
            x - radio - 2.0f,
            y - radio - 2.0f,
            x + radio + 2.0f,
            y + radio + 2.0f,
            borde,
            2.0f
        );
        break;

    case Camaleon:
        al_draw_filled_triangle(
            x,
            y - radio - 2.0f,
            x - radio - 2.0f,
            y,
            x,
            y + radio + 2.0f,
            color
        );

        al_draw_filled_triangle(
            x,
            y - radio - 2.0f,
            x + radio + 2.0f,
            y,
            x,
            y + radio + 2.0f,
            color
        );

        al_draw_line(
            x,
            y - radio - 2.0f,
            x + radio + 2.0f,
            y,
            borde,
            1.0f
        );

        al_draw_line(
            x + radio + 2.0f,
            y,
            x,
            y + radio + 2.0f,
            borde,
            1.0f
        );

        al_draw_line(
            x,
            y + radio + 2.0f,
            x - radio - 2.0f,
            y,
            borde,
            1.0f
        );

        al_draw_line(
            x - radio - 2.0f,
            y,
            x,
            y - radio - 2.0f,
            borde,
            1.0f
        );
        break;
    }

    if (enemigo.tipo == Tanque) {
        al_draw_textf(
            vista.fuente,
            borde,
            x,
            y - radio - 15.0f,
            ALLEGRO_ALIGN_CENTER,
            "%d",
            enemigo.vida
        );
    }
}

static void dibujarEnemigos(
    const VistaAllegro& vista,
    const Motor& motor
) {
    const NodoEnemigo* actual =
        motor.enemigos.cabeza;

    while (actual != nullptr) {
        dibujarEnemigo(
            vista,
            actual->dato
        );

        actual = actual->sig;
    }
}

static void dibujarPulso(
    const VistaAllegro& vista,
    const Motor& motor
) {
    if (!motor.pulso.activo) {
        return;
    }

    float centroX =
        static_cast<float>(vista.ancho) / 2.0f;

    float centroY =
        static_cast<float>(vista.alto) / 2.0f;

    float radio =
        static_cast<float>(
            motor.pulso.radioActual *
            ESCALA_MUNDO
            );

    al_draw_circle(
        centroX,
        centroY,
        radio,
        al_map_rgb(255, 255, 255),
        3.0f
    );
}

static void dibujarHud(
    const VistaAllegro& vista,
    const Motor& motor
) {
    al_draw_filled_rectangle(
        10.0f,
        10.0f,
        235.0f,
        185.0f,
        al_map_rgb(25, 30, 45)
    );

    al_draw_rectangle(
        10.0f,
        10.0f,
        235.0f,
        185.0f,
        al_map_rgb(100, 120, 160),
        1.0f
    );

    if (motor.jugador == nullptr) {
        return;
    }

    const Jugador& jugador =
        *motor.jugador;

    int multiplicador =
        calcularMultiplicador(
            jugador.combo
        );

    const char* colorTexto = "C";

    if (motor.nave.color == 1) {
        colorTexto = "M";
    }
    else if (motor.nave.color == 2) {
        colorTexto = "A";
    }

    al_draw_textf(
        vista.fuente,
        al_map_rgb(255, 255, 255),
        20.0f,
        20.0f,
        0,
        "Jugador: %s",
        jugador.alias.c_str()
    );

    al_draw_textf(
        vista.fuente,
        al_map_rgb(255, 255, 255),
        20.0f,
        42.0f,
        0,
        "Puntaje: %d",
        jugador.puntaje
    );

    al_draw_textf(
        vista.fuente,
        al_map_rgb(255, 255, 255),
        20.0f,
        64.0f,
        0,
        "Combo: %d   x%d",
        jugador.combo,
        multiplicador
    );

    al_draw_textf(
        vista.fuente,
        al_map_rgb(255, 255, 255),
        20.0f,
        86.0f,
        0,
        "Oleada: %d",
        motor.numeroOleada
    );

    al_draw_textf(
        vista.fuente,
        al_map_rgb(255, 255, 255),
        20.0f,
        108.0f,
        0,
        "Vida nucleo: %d",
        jugador.vidaNucleo
    );

    al_draw_textf(
        vista.fuente,
        al_map_rgb(255, 255, 255),
        20.0f,
        130.0f,
        0,
        "Carga pulso: %d/100",
        jugador.cargaPulso
    );

    al_draw_textf(
        vista.fuente,
        al_map_rgb(255, 255, 255),
        20.0f,
        152.0f,
        0,
        "Color: %s   Tick: %d",
        colorTexto,
        motor.tickActual
    );
}

static void dibujarAyuda(
    const VistaAllegro& vista
) {
    al_draw_text(
        vista.fuente,
        al_map_rgb(170, 180, 200),
        static_cast<float>(vista.ancho) - 15.0f,
        15.0f,
        ALLEGRO_ALIGN_RIGHT,
        "Flechas: girar | 1/2/3: color | ESPACIO: pulso | P/ESC: pausa"
    );
}

bool inicializarVistaAllegro(
    VistaAllegro& vista,
    int ancho,
    int alto
) {
    vista.display = nullptr;
    vista.colaEventos = nullptr;
    vista.timer = nullptr;
    vista.fuente = nullptr;

    vista.ancho = ancho;
    vista.alto = alto;

    if (!al_init()) {
        return false;
    }

    if (!al_install_keyboard()) {
        destruirVistaAllegro(vista);
        return false;
    }

    if (!al_init_primitives_addon()) {
        destruirVistaAllegro(vista);
        return false;
    }

    al_init_font_addon();

    vista.display =
        al_create_display(
            ancho,
            alto
        );

    if (vista.display == nullptr) {
        destruirVistaAllegro(vista);
        return false;
    }

    vista.colaEventos =
        al_create_event_queue();

    if (vista.colaEventos == nullptr) {
        destruirVistaAllegro(vista);
        return false;
    }

    vista.timer =
        al_create_timer(
            1.0 / 30.0
        );

    if (vista.timer == nullptr) {
        destruirVistaAllegro(vista);
        return false;
    }

    vista.fuente =
        al_create_builtin_font();

    if (vista.fuente == nullptr) {
        destruirVistaAllegro(vista);
        return false;
    }

    al_register_event_source(
        vista.colaEventos,
        al_get_display_event_source(
            vista.display
        )
    );

    al_register_event_source(
        vista.colaEventos,
        al_get_timer_event_source(
            vista.timer
        )
    );

    al_register_event_source(
        vista.colaEventos,
        al_get_keyboard_event_source()
    );

    return true;
}

void destruirVistaAllegro(
    VistaAllegro& vista
) {
    if (vista.fuente != nullptr) {
        al_destroy_font(
            vista.fuente
        );

        vista.fuente = nullptr;
    }

    if (vista.timer != nullptr) {
        al_destroy_timer(
            vista.timer
        );

        vista.timer = nullptr;
    }

    if (vista.colaEventos != nullptr) {
        al_destroy_event_queue(
            vista.colaEventos
        );

        vista.colaEventos = nullptr;
    }

    if (vista.display != nullptr) {
        al_destroy_display(
            vista.display
        );

        vista.display = nullptr;
    }

    vista.ancho = 0;
    vista.alto = 0;
}

void dibujarMotor(
    const VistaAllegro& vista,
    const Motor& motor,
    bool pausado
) {
    al_clear_to_color(
        al_map_rgb(10, 12, 22)
    );

    dibujarNucleoYOrbita(
        vista
    );

    dibujarPulso(
        vista,
        motor
    );

    dibujarBalas(
        vista,
        motor
    );

    dibujarEnemigos(
        vista,
        motor
    );

    dibujarNave(
        vista,
        motor
    );

    dibujarHud(
        vista,
        motor
    );

    dibujarAyuda(
        vista
    );

    if (pausado) {
        al_draw_filled_rectangle(
            static_cast<float>(vista.ancho) / 2.0f - 80.0f,
            static_cast<float>(vista.alto) / 2.0f - 25.0f,
            static_cast<float>(vista.ancho) / 2.0f + 80.0f,
            static_cast<float>(vista.alto) / 2.0f + 25.0f,
            al_map_rgb(25, 30, 45)
        );

        al_draw_text(
            vista.fuente,
            al_map_rgb(255, 255, 255),
            static_cast<float>(vista.ancho) / 2.0f,
            static_cast<float>(vista.alto) / 2.0f - 4.0f,
            ALLEGRO_ALIGN_CENTER,
            "PAUSA"
        );
    }

    al_flip_display();
}