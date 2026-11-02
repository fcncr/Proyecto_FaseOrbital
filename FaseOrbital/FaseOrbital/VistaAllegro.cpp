#include "VistaAllegro.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_acodec.h>
#include <cmath>
#include <iostream>

using namespace std; //permite usar las funciones matematicas y cerr sin escribir el prefijo

static const double PI_VISTA = 3.14159265358979323846; //permite convertir angulos y calcular los arcos del dibujo
static const double ESCALA_MUNDO = 0.80; //reduce las distancias del motor para mostrarlas en la ventana
static const int CANTIDAD_DESTELLOS = 48; //limita los efectos guardados a los espacios del arreglo de la vista
static const int DURACION_DESTELLO = 6; //muestra cada explosion durante una fraccion de segundo

//devuelve el color que corresponde a cada fase del juego
static ALLEGRO_COLOR colorJuego(int color) {
    if (color == 0) return al_map_rgb(0, 220, 255); //usa cian para la fase cero
    if (color == 1) return al_map_rgb(255, 60, 190); //usa magenta para la fase uno
    if (color == 2) return al_map_rgb(255, 210, 30); //usa ambar para la fase dos
    return al_map_rgb(200, 200, 200); //usa gris cuando el color no es valido
}

//convierte las coordenadas polares del motor a la pantalla
static void polarAPantalla(const VistaAllegro& vista, double radio, double angulo, float& x, float& y) {
    double radianes = angulo * PI_VISTA / 180.0; //convierte el angulo a radianes
    double centroX = static_cast<double>(vista.ancho) / 2.0; //calcula el centro horizontal
    double centroY = static_cast<double>(vista.alto) / 2.0; //calcula el centro vertical
    x = static_cast<float>(centroX + radio * ESCALA_MUNDO * cos(radianes)); //obtiene la coordenada horizontal
    y = static_cast<float>(centroY - radio * ESCALA_MUNDO * sin(radianes)); //invierte el eje vertical para representar angulos antihorarios
}

//dibuja la orbita el nucleo y un arco de vida sin cambiar los valores del jugador
static void dibujarNucleoYOrbita(const VistaAllegro& vista, const Motor& motor) {
    float centroX = static_cast<float>(vista.ancho) / 2.0f; //obtiene el centro horizontal
    float centroY = static_cast<float>(vista.alto) / 2.0f; //obtiene el centro vertical
    float radioNucleo = static_cast<float>(20.0 * ESCALA_MUNDO); //convierte el radio del nucleo
    float radioOrbita = static_cast<float>(100.0 * ESCALA_MUNDO); //convierte el radio de la orbita
    float radioVida = radioNucleo + 10.0f; //deja un espacio para el indicador de vida

    al_draw_circle(centroX, centroY, radioOrbita, al_map_rgb(90, 100, 120), 1.5f); //dibuja el anillo de la nave
    al_draw_filled_circle(centroX, centroY, radioNucleo, al_map_rgb(70, 90, 130)); //dibuja el nucleo
    al_draw_circle(centroX, centroY, radioNucleo, al_map_rgb(180, 210, 255), 2.0f); //resalta el borde
    al_draw_circle(centroX, centroY, radioVida, al_map_rgb(60, 65, 80), 5.0f); //marca el espacio total del arco

    if (motor.jugador == nullptr) return; //evita consultar un jugador inexistente

    int vida = motor.jugador->vidaNucleo; //copia la vida para limitar solo el arco que se dibuja
    if (vida < 0) vida = 0; //no permite dibujar vida negativa
    if (vida > 100) vida = 100; //respeta el maximo de vida

    ALLEGRO_COLOR colorVida = al_map_rgb(70, 235, 120); //usa verde cuando queda suficiente vida
    if (vida <= 60) colorVida = al_map_rgb(255, 200, 60); //cambia a amarillo si baja la vida
    if (vida <= 25) colorVida = al_map_rgb(255, 80, 80); //usa rojo cuando la vida es baja

    float inicio = static_cast<float>(-PI_VISTA / 2.0); //comienza el arco en la parte superior
    float recorrido = static_cast<float>((2.0 * PI_VISTA) * vida / 100.0); //calcula cuanto circulo corresponde a la vida
    if (vida > 0) al_draw_arc(centroX, centroY, radioVida, inicio, recorrido, colorVida, 5.0f); //muestra la vida disponible
}

//ubica la nave en su orbita y dibuja un triangulo apuntando hacia fuera del nucleo
static void dibujarNave(const VistaAllegro& vista, const Motor& motor) {
    float x;
    float y;
    polarAPantalla(vista, 100.0, motor.nave.anguloActual, x, y); //ubica la nave sobre el anillo

    double radianes = motor.nave.anguloActual * PI_VISTA / 180.0;
    float direccionX = static_cast<float>(cos(radianes));
    float direccionY = static_cast<float>(-sin(radianes)); //orienta la nave con el mismo eje vertical del dibujo
    float perpendicularX = -direccionY;
    float perpendicularY = direccionX;

    float puntaX = x + direccionX * 13.0f;
    float puntaY = y + direccionY * 13.0f;
    float baseX = x - direccionX * 8.0f;
    float baseY = y - direccionY * 8.0f;

    float izquierdaX = baseX + perpendicularX * 7.0f;
    float izquierdaY = baseY + perpendicularY * 7.0f;
    float derechaX = baseX - perpendicularX * 7.0f;
    float derechaY = baseY - perpendicularY * 7.0f;

    al_draw_filled_triangle(puntaX, puntaY, izquierdaX, izquierdaY, derechaX, derechaY, colorJuego(motor.nave.color)); //rellena la nave con su fase
    al_draw_triangle(puntaX, puntaY, izquierdaX, izquierdaY, derechaX, derechaY, al_map_rgb(255, 255, 255), 1.0f); //dibuja el contorno
}

//dibuja las balas usando la misma orientacion que la nave y los enemigos
static void dibujarBalas(const VistaAllegro& vista, const Motor& motor) {
    const NodoBala* actual = motor.balas.cabeza;
    float centroX = static_cast<float>(vista.ancho) / 2.0f;
    float centroY = static_cast<float>(vista.alto) / 2.0f;

    while (actual != nullptr) {
        float x = centroX + static_cast<float>(actual->dato.x * ESCALA_MUNDO);
        float y = centroY - static_cast<float>(actual->dato.y * ESCALA_MUNDO); //proyecta la bala con el mismo eje vertical que la nave
        al_draw_filled_circle(x, y, 3.0f, colorJuego(actual->dato.color));
        actual = actual->sig;
    }
}

//dibuja una figura segun el tipo de enemigo con su fase actual y muestra la vida de los tanques
static void dibujarEnemigo(const VistaAllegro& vista, const Enemigo& enemigo) {
    float x; //recibe la posicion horizontal
    float y; //recibe la posicion vertical
    polarAPantalla(vista, enemigo.r, enemigo.theta, x, y); //convierte la posicion polar

    float radio = static_cast<float>(enemigo.radio * ESCALA_MUNDO); //convierte el tamano del enemigo
    if (radio < 5.0f) radio = 5.0f; //amplia solo el dibujo de los enemigos pequenos sin cambiar sus colisiones

    ALLEGRO_COLOR color = colorJuego(enemigo.color); //obtiene el color de fase
    ALLEGRO_COLOR borde = al_map_rgb(255, 255, 255); //usa el mismo color para los bordes

    switch (enemigo.tipo) { //dibuja la figura que corresponde al tipo
    case Dron: //representa al dron con un circulo
        al_draw_filled_circle(x, y, radio, color); //dibuja un circulo relleno
        al_draw_circle(x, y, radio, borde, 1.0f); //marca el borde del dron
        break; //termina el dibujo de este tipo

    case Girador: //representa al girador con un triangulo
        al_draw_filled_triangle(x, y - radio - 2.0f, x - radio, y + radio, x + radio, y + radio, color); //dibuja un triangulo
        al_draw_triangle(x, y - radio - 2.0f, x - radio, y + radio, x + radio, y + radio, borde, 1.0f); //marca el borde
        break; //termina el dibujo de este tipo

    case Divisor: //representa al divisor con un cuadrado dividido
        al_draw_filled_rectangle(x - radio, y - radio, x + radio, y + radio, color); //dibuja un cuadrado
        al_draw_line(x - radio, y, x + radio, y, borde, 1.0f); //marca la mitad horizontal
        al_draw_line(x, y - radio, x, y + radio, borde, 1.0f); //marca la mitad vertical
        break; //termina el dibujo de este tipo

    case Mini: //distingue al mini con un circulo y un aro exterior
        al_draw_filled_circle(x, y, radio, color); //dibuja un circulo pequeno
        al_draw_circle(x, y, radio + 2.0f, borde, 1.0f); //marca un aro adicional
        break; //termina el dibujo de este tipo

    case Tanque: //representa al tanque con un cuadrado de borde grueso
        al_draw_filled_rectangle(x - radio - 2.0f, y - radio - 2.0f, x + radio + 2.0f, y + radio + 2.0f, color); //dibuja un cuadrado grande
        al_draw_rectangle(x - radio - 2.0f, y - radio - 2.0f, x + radio + 2.0f, y + radio + 2.0f, borde, 2.0f); //marca un borde grueso
        break; //deja pendiente solo el texto de vida que se dibuja despues del switch

    case Camaleon: //forma un rombo para distinguir al camaleon
        al_draw_filled_triangle(x, y - radio - 2.0f, x - radio - 2.0f, y, x, y + radio + 2.0f, color); //dibuja la mitad izquierda
        al_draw_filled_triangle(x, y - radio - 2.0f, x + radio + 2.0f, y, x, y + radio + 2.0f, color); //dibuja la mitad derecha
        al_draw_line(x, y - radio - 2.0f, x + radio + 2.0f, y, borde, 1.0f); //marca el borde superior derecho
        al_draw_line(x + radio + 2.0f, y, x, y + radio + 2.0f, borde, 1.0f); //marca el borde inferior derecho
        al_draw_line(x, y + radio + 2.0f, x - radio - 2.0f, y, borde, 1.0f); //marca el borde inferior izquierdo
        al_draw_line(x - radio - 2.0f, y, x, y - radio - 2.0f, borde, 1.0f); //marca el borde superior izquierdo
        break; //termina el dibujo de este tipo
    }

    if (enemigo.tipo == Tanque) { //comprueba si debe mostrar la vida del tanque
        al_draw_textf(vista.fuente, borde, x, y - radio - 15.0f, ALLEGRO_ALIGN_CENTER, "%d", enemigo.vida); //muestra los puntos de vida restantes
    }
}

//dibuja todos los enemigos sin modificar sus nodos
static void dibujarEnemigos(const VistaAllegro& vista, const Motor& motor) {
    const NodoEnemigo* actual = motor.enemigos.cabeza; //obtiene el primer enemigo

    while (actual != nullptr) { //recorre la lista de enemigos
        dibujarEnemigo(vista, actual->dato); //dibuja el enemigo actual
        actual = actual->sig; //avanza al siguiente nodo
    }
}

//muestra la onda del pulso mientras sigue activa
static void dibujarPulso(const VistaAllegro& vista, const Motor& motor) {
    if (!motor.pulso.activo) return; //no dibuja una onda que termino

    float centroX = static_cast<float>(vista.ancho) / 2.0f; //calcula el centro horizontal
    float centroY = static_cast<float>(vista.alto) / 2.0f; //calcula el centro vertical
    float radio = static_cast<float>(motor.pulso.radioActual * ESCALA_MUNDO); //convierte el radio de la onda
    al_draw_circle(centroX, centroY, radio, al_map_rgb(255, 255, 255), 3.0f); //dibuja el anillo del pulso
}

//desactiva los efectos guardados y vuelve al primer espacio del arreglo para registrar nuevas bajas
static void reiniciarDestellos(VistaAllegro& vista) {
    vista.siguienteDestello = 0; //reinicia la posicion del siguiente efecto

    for (int i = 0; i < CANTIDAD_DESTELLOS; i++) { //recorre el arreglo de destellos
        vista.destellos[i].activo = false; //desactiva el destello anterior
        vista.destellos[i].tickInicio = 0; //reinicia su tick de inicio
        vista.destellos[i].color = 0; //reinicia su color
        vista.destellos[i].r = 0.0; //borra la distancia guardada para el efecto anterior
        vista.destellos[i].theta = 0.0; //borra el angulo guardado para el efecto anterior
    }
}

//guarda el color la posicion y el tick de una baja en el siguiente espacio reutilizable del arreglo
static void agregarDestello(VistaAllegro& vista, const Evento& evento, int tickActual) {
    if (evento.r < 0.0) return; //ignora eventos antiguos o de prueba que no tengan una posicion valida

    int posicion = vista.siguienteDestello; //selecciona un espacio del arreglo
    vista.destellos[posicion].activo = true; //activa el efecto
    vista.destellos[posicion].tickInicio = tickActual; //inicia la duracion visual desde el tick en que se procesa la baja
    vista.destellos[posicion].color = evento.color; //guarda el color de la baja
    vista.destellos[posicion].r = evento.r; //conserva la distancia donde murio el enemigo
    vista.destellos[posicion].theta = evento.theta; //conserva el angulo donde murio el enemigo
    vista.siguienteDestello = (posicion + 1) % CANTIDAD_DESTELLOS; //vuelve al inicio al llenar el arreglo
}

//solicita una reproduccion del sonido al volumen indicado sin esperar a que termine
static void reproducirSonido(ALLEGRO_SAMPLE* sonido, float volumen) {
    if (sonido == nullptr) return; //evita reproducir un recurso inexistente
    al_play_sample(sonido, volumen, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, nullptr); //inicia el sonido sin detener la simulacion
}

//revisa los eventos aun no procesados y aplica los efectos del jugador y la oleada actuales
//al detectar un reinicio descarta los eventos ya presentes y en cada grupo reproduce como maximo un sonido de cada tipo
static void actualizarEfectos(VistaAllegro& vista, const Motor& motor) {
    if (motor.jugador == nullptr || motor.bitacora == nullptr) return; //necesita un jugador y una bitacora

    bool nuevoTurno = motor.tickActual == 0 || vista.bitacoraObservada != motor.bitacora || vista.jugadorObservado != motor.jugador || vista.oleadaObservada != motor.numeroOleada || motor.tickActual < vista.tickObservado || vista.eventosProcesados > motor.bitacora->tam; //detecta cambios de turno o retrocesos del tick o de la bitacora

    if (nuevoTurno) { //reinicia efectos cuando empieza otro turno
        reiniciarDestellos(vista); //quita los destellos antiguos
        vista.bitacoraObservada = motor.bitacora; //guarda la bitacora actual
        vista.jugadorObservado = motor.jugador; //guarda el jugador actual
        vista.oleadaObservada = motor.numeroOleada; //guarda la oleada actual
        vista.tickObservado = motor.tickActual; //guarda el tick inicial
        vista.eventosProcesados = motor.bitacora->tam; //deja como revisados todos los eventos que ya estaban presentes
        return; //espera eventos nuevos antes de reproducir efectos de este turno
    }

    if (vista.eventosProcesados == motor.bitacora->tam) { //comprueba si no hay eventos nuevos
        vista.tickObservado = motor.tickActual; //actualiza el tick observado
        return; //evita reproducir sonidos otra vez
    }

    bool huboBaja = false; //reune las bajas nuevas para reproducir un solo sonido en este grupo
    bool huboError = false; //recuerda si el grupo contiene algun impacto incorrecto
    bool huboDano = false; //recuerda si el grupo contiene algun choque que causo dano
    bool huboPulso = false; //recuerda si el grupo contiene alguna activacion de pulso
    const NodoEvento* actual = motor.bitacora->cabeza; //comienza en el primer evento de la lista

    for (int i = 0; i < vista.eventosProcesados && actual != nullptr; i++) { //salta los eventos que ya fueron procesados
        actual = actual->sig; //avanza sin reproducir los eventos anteriores
    }

    while (actual != nullptr) { //recorre solamente los eventos nuevos
        const Evento& evento = actual->dato; //consulta los datos del evento

        if (evento.oleada == motor.numeroOleada && evento.jugador == motor.jugador->alias) { //selecciona eventos del jugador que esta jugando
            if (evento.tipoEvento == BAJA) { //comprueba si un enemigo fue eliminado
                agregarDestello(vista, evento, motor.tickActual); //crea un efecto visual para la baja
                huboBaja = true; //marca que debe sonar una baja
            }
            else if (evento.tipoEvento == IMPACTO_ERRONEO) { //comprueba si se fallo el color
                huboError = true; //marca que debe sonar un error
            }
            else if (evento.tipoEvento == DANO_NUCLEO || evento.tipoEvento == CHOQUE_NAVE) { //comprueba si el jugador recibio dano
                huboDano = true; //marca que debe sonar el dano
            }
            else if (evento.tipoEvento == PULSO) { //comprueba si se activo el pulso
                huboPulso = true; //marca que debe sonar el pulso
            }
        }

        vista.eventosProcesados++; //cuenta tambien los eventos descartados por ser de otro jugador u oleada
        actual = actual->sig; //avanza al siguiente nodo
    }

    if (huboBaja) reproducirSonido(vista.sonidoBaja, 0.45f); //reproduce una baja para este grupo de eventos
    if (huboError) reproducirSonido(vista.sonidoError, 0.45f); //reproduce el impacto de color incorrecto
    if (huboDano) reproducirSonido(vista.sonidoDano, 0.55f); //reproduce el dano recibido
    if (huboPulso) reproducirSonido(vista.sonidoPulso, 0.60f); //reproduce la activacion del pulso
    vista.tickObservado = motor.tickActual; //recuerda el ultimo tick revisado
}

//dibuja una pequena explosion por cada baja reciente usando un centro un anillo y ocho rayos
//no cambia los espacios vencidos porque se sobrescriben al registrar nuevas bajas o se limpian al reiniciar
static void dibujarDestellos(const VistaAllegro& vista, const Motor& motor) {
    for (int i = 0; i < CANTIDAD_DESTELLOS; i++) { //recorre los efectos almacenados
        const DestelloBajaVista& destello = vista.destellos[i]; //consulta el efecto actual
        if (!destello.activo) continue; //ignora las posiciones sin efecto

        int edad = motor.tickActual - destello.tickInicio; //calcula los ticks desde la baja
        if (edad < 0 || edad >= DURACION_DESTELLO) continue; //omite los efectos que aun no empiezan o que ya cumplieron su duracion

        float x; //recibe la posicion horizontal de la baja en pantalla
        float y; //recibe la posicion vertical de la baja en pantalla
        polarAPantalla(vista, destello.r, destello.theta, x, y); //ubica la explosion donde murio el enemigo

        float radio = 2.5f + static_cast<float>(edad) * 0.8f; //expande un anillo pequeno durante la animacion
        float inicioRayo = radio * 0.65f; //separa los rayos cortos del centro de la explosion
        float finalRayo = radio + 2.5f; //extiende los rayos apenas por fuera del anillo
        float inicioDiagonal = inicioRayo * 0.70f; //ajusta el inicio de los rayos diagonales
        float finalDiagonal = finalRayo * 0.70f; //ajusta el final de los rayos diagonales
        ALLEGRO_COLOR color = colorJuego(destello.color); //utiliza el color de la baja

        al_draw_filled_circle(x, y, 1.5f, color); //marca discretamente el centro de la explosion
        al_draw_circle(x, y, radio, color, 1.0f); //dibuja la pequena onda que se expande
        al_draw_line(x - finalRayo, y, x - inicioRayo, y, color, 1.0f); //dibuja el rayo izquierdo
        al_draw_line(x + inicioRayo, y, x + finalRayo, y, color, 1.0f); //dibuja el rayo derecho
        al_draw_line(x, y - finalRayo, x, y - inicioRayo, color, 1.0f); //dibuja el rayo superior
        al_draw_line(x, y + inicioRayo, x, y + finalRayo, color, 1.0f); //dibuja el rayo inferior
        al_draw_line(x - finalDiagonal, y - finalDiagonal, x - inicioDiagonal, y - inicioDiagonal, color, 1.0f); //dibuja el rayo superior izquierdo
        al_draw_line(x + inicioDiagonal, y - inicioDiagonal, x + finalDiagonal, y - finalDiagonal, color, 1.0f); //dibuja el rayo superior derecho
        al_draw_line(x - finalDiagonal, y + finalDiagonal, x - inicioDiagonal, y + inicioDiagonal, color, 1.0f); //dibuja el rayo inferior izquierdo
        al_draw_line(x + inicioDiagonal, y + inicioDiagonal, x + finalDiagonal, y + finalDiagonal, color, 1.0f); //dibuja el rayo inferior derecho
    }
}

//muestra los datos actuales del jugador en el marcador sin cambiar las metricas del motor
static void dibujarHud(const VistaAllegro& vista, const Motor& motor) {
    al_draw_filled_rectangle(10.0f, 10.0f, 235.0f, 185.0f, al_map_rgb(25, 30, 45)); //dibuja el fondo del marcador
    al_draw_rectangle(10.0f, 10.0f, 235.0f, 185.0f, al_map_rgb(100, 120, 160), 1.0f); //dibuja el borde del marcador
    if (motor.jugador == nullptr) return; //evita leer los datos de un jugador inexistente

    const Jugador& jugador = *motor.jugador; //consulta el jugador actual sin copiarlo
    int multiplicador = calcularMultiplicador(jugador.combo); //calcula el multiplicador de los puntos
    const char* colorTexto = "C"; //usa la letra de cian como texto predeterminado sin cambiar la fase de la nave
    if (motor.nave.color == 1) colorTexto = "M"; //identifica magenta
    else if (motor.nave.color == 2) colorTexto = "A"; //identifica ambar
    ALLEGRO_COLOR blanco = al_map_rgb(255, 255, 255); //usa un color comun para el texto

    al_draw_textf(vista.fuente, blanco, 20.0f, 20.0f, 0, "Jugador: %s", jugador.alias.c_str()); //muestra el alias
    al_draw_textf(vista.fuente, blanco, 20.0f, 42.0f, 0, "Puntaje: %d", jugador.puntaje); //muestra el puntaje
    al_draw_textf(vista.fuente, blanco, 20.0f, 64.0f, 0, "Combo: %d   x%d", jugador.combo, multiplicador); //muestra combo y multiplicador
    al_draw_textf(vista.fuente, blanco, 20.0f, 86.0f, 0, "Oleada: %d", motor.numeroOleada); //muestra la oleada
    al_draw_textf(vista.fuente, blanco, 20.0f, 108.0f, 0, "Vida nucleo: %d", jugador.vidaNucleo); //muestra la vida guardada sin aplicar el limite visual usado por el arco
    al_draw_textf(vista.fuente, blanco, 20.0f, 130.0f, 0, "Carga pulso: %d/100", jugador.cargaPulso); //muestra la carga disponible
    al_draw_textf(vista.fuente, blanco, 20.0f, 152.0f, 0, "Color: %s   Tick: %d", colorTexto, motor.tickActual); //muestra la fase y el tick
}

//muestra los controles que se pueden utilizar durante el turno
static void dibujarAyuda(const VistaAllegro& vista) {
    float centro = static_cast<float>(vista.ancho) / 2.0f; //centra los controles para que no se corten en el borde
    al_draw_text(vista.fuente, al_map_rgb(170, 180, 200), centro, static_cast<float>(vista.alto) - 49.0f, ALLEGRO_ALIGN_CENTER, "FLECHAS O MOUSE: GIRAR | 1/2/3: COLOR"); //muestra los controles de movimiento y color
    al_draw_text(vista.fuente, al_map_rgb(170, 180, 200), centro, static_cast<float>(vista.alto) - 25.0f, ALLEGRO_ALIGN_CENTER, "ESPACIO: PULSO | P/ESC: PAUSA | M EN PAUSA: MENU"); //muestra los controles de pulso y pausa
}

//prepara una vista sin recursos activos y carga la ventana los controles la fuente y los cuatro sonidos
//con addresssanitizer omite el audio para evitar el conflicto entre directsound y el sanitizador
//el mouse es opcional y los fallos de recursos obligatorios liberan lo creado durante este intento
bool inicializarVistaAllegro(VistaAllegro& vista, int ancho, int alto) {
    vista.display = nullptr; //inicializa el puntero de la ventana
    vista.colaEventos = nullptr; //inicializa el puntero de la cola de eventos
    vista.timer = nullptr; //inicializa el puntero del temporizador
    vista.fuente = nullptr; //inicializa el puntero de la fuente
    vista.sonidoBaja = nullptr; //inicializa el sonido de baja
    vista.sonidoError = nullptr; //inicializa el sonido de impacto incorrecto
    vista.sonidoDano = nullptr; //inicializa el sonido de dano
    vista.sonidoPulso = nullptr; //inicializa el sonido del pulso
    vista.ancho = ancho; //guarda el ancho de la ventana
    vista.alto = alto; //guarda el alto de la ventana
    vista.tecladoInstalado = false; //registra que aun no se instalo el teclado
    vista.mouseInstalado = false; //registra que aun no se instalo el mouse
    vista.primitivasIniciadas = false; //registra que aun no se inicializaron las primitivas
    vista.fuentesIniciadas = false; //registra que aun no se inicializaron las fuentes
    vista.ttfIniciado = false; //registra que aun no se inicializo ttf
    vista.audioInstalado = false; //registra que aun no se instalo el audio
    vista.bitacoraObservada = nullptr; //no hay una bitacora observada
    vista.jugadorObservado = nullptr; //no hay un jugador observado
    vista.oleadaObservada = 0; //reinicia la oleada observada
    vista.tickObservado = 0; //reinicia el tick observado
    vista.eventosProcesados = 0; //reinicia la cantidad de eventos atendidos
    reiniciarDestellos(vista); //desactiva los efectos visuales anteriores

    if (!al_init()) { //comprueba si allegro se puede iniciar
        cerr << "No se pudo inicializar Allegro." << '\n'; //informa el fallo de inicio
        return false; //no intenta crear recursos sin allegro
    }

    if (!al_install_keyboard()) { //comprueba si el teclado esta disponible
        cerr << "No se pudo inicializar el teclado." << '\n'; //informa que fallo el teclado
        destruirVistaAllegro(vista); //limpia el estado preparado para este intento de inicializacion
        return false; //detiene la inicializacion
    }
    vista.tecladoInstalado = true; //recuerda que luego debe liberar el teclado

    vista.mouseInstalado = al_install_mouse(); //habilita el mouse para apuntar durante el juego
    if (!vista.mouseInstalado) cerr << "Mouse no disponible. Puede utilizar las flechas." << '\n'; //mantiene el teclado como alternativa

    if (!al_init_primitives_addon()) { //inicia las funciones de dibujo
        cerr << "No se pudo inicializar primitives." << '\n'; //informa que no se pueden dibujar figuras
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }
    vista.primitivasIniciadas = true; //recuerda que el addon quedo iniciado

    if (!al_init_font_addon()) { //inicia las funciones de texto
        cerr << "No se pudo inicializar font." << '\n'; //informa el error de fuentes
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }
    vista.fuentesIniciadas = true; //recuerda que el addon quedo iniciado

    if (!al_init_ttf_addon()) { //inicia la carga de archivos ttf
        cerr << "No se pudo inicializar ttf." << '\n'; //informa el error de ttf
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }
    vista.ttfIniciado = true; //recuerda que el addon quedo iniciado

#ifndef __SANITIZE_ADDRESS__
    if (!al_install_audio()) { //inicia el sistema de sonido
        cerr << "No se pudo inicializar audio." << '\n'; //informa que no hay audio disponible
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }
    vista.audioInstalado = true; //recuerda que se debe liberar el audio al salir

    if (!al_init_acodec_addon()) { //habilita la lectura de archivos wav
        cerr << "No se pudo inicializar acodec." << '\n'; //informa el error de codecs
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }
    if (!al_reserve_samples(16)) { //prepara varios canales para sonidos simultaneos
        cerr << "No se pudieron reservar los canales de audio." << '\n'; //informa el error de reproduccion
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }
#else
    cerr << "Audio desactivado durante la prueba con AddressSanitizer." << '\n'; //explica por que la prueba se ejecuta sin sonido
#endif

    vista.display = al_create_display(ancho, alto); //crea la ventana del juego
    if (vista.display == nullptr) { //comprueba si la ventana se creo correctamente
        cerr << "No se pudo crear la ventana." << '\n'; //informa el fallo de ventana
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }

    vista.colaEventos = al_create_event_queue(); //crea la cola de eventos
    if (vista.colaEventos == nullptr) { //comprueba si la cola se creo
        cerr << "No se pudo crear la cola de eventos." << '\n'; //informa el error
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }

    vista.timer = al_create_timer(1.0 / 30.0); //crea el temporizador de treinta eventos por segundo que la aplicacion iniciara al jugar
    if (vista.timer == nullptr) { //comprueba que el temporizador exista
        cerr << "No se pudo crear el temporizador." << '\n'; //informa el error
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }

    vista.fuente = al_load_ttf_font("recursos/fuente.ttf", 16, 0); //carga una fuente externa para todas las pantallas
    if (vista.fuente == nullptr) { //comprueba si la fuente pudo cargarse
        cerr << "No se pudo cargar: recursos/fuente.ttf" << '\n'; //indica exactamente que recurso falta
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }

#ifndef __SANITIZE_ADDRESS__
    vista.sonidoBaja = al_load_sample("recursos/sonido_baja.wav"); //carga el sonido de eliminacion
    if (vista.sonidoBaja == nullptr) { //comprueba si se encontro el archivo de baja
        cerr << "No se pudo cargar: recursos/sonido_baja.wav" << '\n'; //identifica el recurso faltante
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }

    vista.sonidoError = al_load_sample("recursos/sonido_error.wav"); //carga el sonido de impacto incorrecto
    if (vista.sonidoError == nullptr) { //comprueba si se encontro el archivo de error
        cerr << "No se pudo cargar: recursos/sonido_error.wav" << '\n'; //identifica el recurso faltante
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }

    vista.sonidoDano = al_load_sample("recursos/sonido_dano.wav"); //carga el sonido de dano al jugador
    if (vista.sonidoDano == nullptr) { //comprueba si se encontro el archivo de dano
        cerr << "No se pudo cargar: recursos/sonido_dano.wav" << '\n'; //identifica el recurso faltante
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }

    vista.sonidoPulso = al_load_sample("recursos/sonido_pulso.wav"); //carga el sonido de activacion del pulso
    if (vista.sonidoPulso == nullptr) { //comprueba si se encontro el archivo del pulso
        cerr << "No se pudo cargar: recursos/sonido_pulso.wav" << '\n'; //identifica el recurso faltante
        destruirVistaAllegro(vista); //libera lo creado durante este intento de inicializacion
        return false; //detiene la inicializacion
    }
#endif

    al_register_event_source(vista.colaEventos, al_get_display_event_source(vista.display)); //registra los eventos de ventana
    al_register_event_source(vista.colaEventos, al_get_timer_event_source(vista.timer)); //registra los eventos del temporizador
    al_register_event_source(vista.colaEventos, al_get_keyboard_event_source()); //registra los eventos del teclado
    if (vista.mouseInstalado) al_register_event_source(vista.colaEventos, al_get_mouse_event_source()); //registra los eventos del mouse si esta disponible

    return true; //confirma que allegro y sus recursos estan listos
}

//detiene los sonidos y libera los recursos que llegaron a crearse durante la inicializacion
//deja sus punteros en nulo y borra el seguimiento del turno sin liberar el jugador ni la bitacora prestados
void destruirVistaAllegro(VistaAllegro& vista) {
    if (vista.audioInstalado) al_stop_samples(); //detiene sonidos que aun se esten reproduciendo

    if (vista.sonidoBaja != nullptr) { //comprueba si se cargo el sonido de baja
        al_destroy_sample(vista.sonidoBaja); //libera la muestra de audio
        vista.sonidoBaja = nullptr; //evita conservar un puntero liberado
    }
    if (vista.sonidoError != nullptr) { //comprueba si se cargo el sonido de error
        al_destroy_sample(vista.sonidoError); //libera la muestra de audio
        vista.sonidoError = nullptr; //evita conservar un puntero liberado
    }
    if (vista.sonidoDano != nullptr) { //comprueba si se cargo el sonido de dano
        al_destroy_sample(vista.sonidoDano); //libera la muestra de audio
        vista.sonidoDano = nullptr; //evita conservar un puntero liberado
    }
    if (vista.sonidoPulso != nullptr) { //comprueba si se cargo el sonido del pulso
        al_destroy_sample(vista.sonidoPulso); //libera la muestra de audio
        vista.sonidoPulso = nullptr; //evita conservar un puntero liberado
    }

    if (vista.fuente != nullptr) { //comprueba si se cargo la fuente ttf
        al_destroy_font(vista.fuente); //libera la fuente
        vista.fuente = nullptr; //evita conservar un puntero liberado
    }
    if (vista.timer != nullptr) { //comprueba si existe el temporizador
        al_destroy_timer(vista.timer); //libera el temporizador
        vista.timer = nullptr; //evita conservar un puntero liberado
    }
    if (vista.colaEventos != nullptr) { //comprueba si existe la cola de eventos
        al_destroy_event_queue(vista.colaEventos); //libera la cola
        vista.colaEventos = nullptr; //evita conservar un puntero liberado
    }
    if (vista.display != nullptr) { //comprueba si existe la ventana
        al_destroy_display(vista.display); //libera la ventana
        vista.display = nullptr; //evita conservar un puntero liberado
    }

    if (vista.audioInstalado) { //comprueba si el audio quedo instalado
        al_uninstall_audio(); //libera el sistema de audio
        vista.audioInstalado = false; //registra que ya fue liberado
    }
    if (vista.ttfIniciado) { //comprueba si el addon ttf fue iniciado
        al_shutdown_ttf_addon(); //libera el addon de fuentes ttf
        vista.ttfIniciado = false; //registra que ya fue liberado
    }
    if (vista.fuentesIniciadas) { //comprueba si el addon font fue iniciado
        al_shutdown_font_addon(); //libera el addon de fuentes
        vista.fuentesIniciadas = false; //registra que ya fue liberado
    }
    if (vista.primitivasIniciadas) { //comprueba si el addon de dibujo fue iniciado
        al_shutdown_primitives_addon(); //libera el addon de primitivas
        vista.primitivasIniciadas = false; //registra que ya fue liberado
    }
    if (vista.mouseInstalado) { //comprueba si se instalo el mouse
        al_uninstall_mouse(); //libera el controlador del mouse
        vista.mouseInstalado = false; //evita volver a liberar el mismo controlador
    }
    if (vista.tecladoInstalado) { //comprueba si se instalo el teclado
        al_uninstall_keyboard(); //libera los eventos del teclado
        vista.tecladoInstalado = false; //registra que ya fue liberado
    }

    vista.ancho = 0; //reinicia el ancho de la ventana
    vista.alto = 0; //reinicia el alto de la ventana
    vista.bitacoraObservada = nullptr; //deja de observar la bitacora anterior
    vista.jugadorObservado = nullptr; //deja de observar el jugador anterior
    vista.oleadaObservada = 0; //borra la oleada anterior
    vista.tickObservado = 0; //borra el tick anterior
    vista.eventosProcesados = 0; //borra la cantidad de eventos procesados
    reiniciarDestellos(vista); //desactiva cualquier destello restante
}

//actualiza los efectos y dibuja el turno en el orden previsto sin modificar el motor ni avanzar sus ticks
//el parametro de pausa solo controla el aviso visual y la aplicacion decide si la simulacion avanza
void dibujarMotor(VistaAllegro& vista, const Motor& motor, bool pausado) {
    actualizarEfectos(vista, motor); //prepara los destellos y sonidos de eventos nuevos
    al_clear_to_color(al_map_rgb(10, 12, 22)); //limpia el fotograma anterior

    dibujarNucleoYOrbita(vista, motor); //dibuja el anillo y la vida del nucleo
    dibujarPulso(vista, motor); //dibuja el pulso activo
    dibujarBalas(vista, motor); //dibuja las balas activas
    dibujarEnemigos(vista, motor); //dibuja los enemigos activos
    dibujarDestellos(vista, motor); //dibuja las bajas recientes
    dibujarNave(vista, motor); //dibuja la nave en su angulo actual
    dibujarHud(vista, motor); //muestra las metricas del turno
    dibujarAyuda(vista); //muestra los controles disponibles

    if (pausado) { //comprueba si el juego se encuentra en pausa
        float centroX = static_cast<float>(vista.ancho) / 2.0f; //obtiene el centro horizontal
        float centroY = static_cast<float>(vista.alto) / 2.0f; //obtiene el centro vertical
        al_draw_filled_rectangle(centroX - 80.0f, centroY - 25.0f, centroX + 80.0f, centroY + 25.0f, al_map_rgb(25, 30, 45)); //dibuja el fondo de pausa
        al_draw_text(vista.fuente, al_map_rgb(255, 255, 255), centroX, centroY - 4.0f, ALLEGRO_ALIGN_CENTER, "PAUSA"); //muestra el estado de pausa
    }

    al_flip_display(); //presenta el fotograma en la ventana
}
