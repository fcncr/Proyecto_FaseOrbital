#include "ControladorAllegro.h"
#include <cmath>

using namespace std;

static const double PI_CONTROL = 3.14159265358979323846;

//crea una accion sin solicitudes y conserva la direccion y el color actuales
static AccionMotor crearAccionVaciaAllegro(const Motor& motor) {
    AccionMotor accion; //guarda las acciones que recibira el motor
    accion.cambiarAngulo = false; //no cambia la direccion si no hay controles activos
    accion.anguloObjetivo = motor.nave.anguloObjetivo; //conserva el objetivo anterior de la nave
    accion.cambiarColor = false; //no cambia la fase sin una tecla de color
    accion.colorNuevo = motor.nave.color; //conserva el color actual
    accion.solicitarPulso = false; //no activa el pulso sin una solicitud
    return accion; //devuelve la accion preparada
}

//convierte la posicion del cursor en un angulo antihorario respecto al centro
static void actualizarAnguloMouse(const ALLEGRO_EVENT& evento, EstadoControles& controles) {
    if (evento.mouse.display == nullptr) return;

    double centroX = al_get_display_width(evento.mouse.display) / 2.0;
    double centroY = al_get_display_height(evento.mouse.display) / 2.0;
    double diferenciaX = static_cast<double>(evento.mouse.x) - centroX;
    double diferenciaY = centroY - static_cast<double>(evento.mouse.y); //convierte la altura del cursor al eje vertical del motor

    if (diferenciaX == 0.0 && diferenciaY == 0.0) return; //conserva la direccion cuando el cursor esta en el centro

    double angulo = atan2(diferenciaY, diferenciaX) * 180.0 / PI_CONTROL;
    if (angulo < 0.0) angulo += 360.0;

    controles.anguloMouse = angulo;
    controles.usarMouse = true;
}

//limpia las teclas y solicitudes para comenzar un turno nuevo
void inicializarEstadoControles(EstadoControles& controles) {
    controles.izquierda = false; //la flecha izquierda comienza sin presionar
    controles.derecha = false; //la flecha derecha comienza sin presionar
    controles.usarMouse = false; //el mouse no controla la nave hasta que se mueva
    controles.anguloMouse = 270.0; //prepara un angulo inicial valido
    controles.cambiarC = false; //no hay cambio pendiente a cian
    controles.cambiarM = false; //no hay cambio pendiente a magenta
    controles.cambiarA = false; //no hay cambio pendiente a ambar
    controles.solicitarPulso = false; //no hay pulso pendiente
    controles.alternarPausa = false; //no hay solicitud de pausa
    controles.volverMenu = false; //no hay solicitud de regresar al menu
}

//guarda los eventos de teclado y mouse para convertirlos en acciones del motor
void procesarEventoControlador(const ALLEGRO_EVENT& evento, EstadoControles& controles) {
    if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) { //revisa los movimientos del cursor
        if (evento.mouse.dx != 0 || evento.mouse.dy != 0) { //ignora los movimientos de la rueda
            actualizarAnguloMouse(evento, controles); //apunta la nave hacia la posicion del cursor
        }
        return; //el evento del mouse no necesita procesarse como una tecla
    }

    if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) { //permite apuntar tambien al hacer clic
        actualizarAnguloMouse(evento, controles); //usa la posicion donde se presiono el boton
        return; //no modifica los controles del teclado
    }

    if (evento.type == ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY) { //detecta cuando el cursor sale de la ventana
        controles.usarMouse = false; //deja de apuntar con una posicion exterior
        return; //termina de atender el evento de salida
    }

    if (evento.type == ALLEGRO_EVENT_DISPLAY_SWITCH_OUT) { //detecta cuando la ventana pierde el foco
        controles.izquierda = false; //evita dejar una flecha presionada al cambiar de ventana
        controles.derecha = false; //evita dejar una flecha presionada al cambiar de ventana
        controles.usarMouse = false; //desactiva el mouse hasta que vuelva a moverse
        return; //no procesa mas entradas sin foco
    }

    if (evento.type == ALLEGRO_EVENT_KEY_DOWN) { //revisa las teclas que fueron presionadas
        switch (evento.keyboard.keycode) { //identifica el control utilizado
        case ALLEGRO_KEY_LEFT:
            controles.izquierda = true; //mantiene el giro hacia la izquierda mientras este presionada
            controles.usarMouse = false; //da prioridad al teclado sobre el mouse
            break;

        case ALLEGRO_KEY_RIGHT:
            controles.derecha = true; //mantiene el giro hacia la derecha mientras este presionada
            controles.usarMouse = false; //da prioridad al teclado sobre el mouse
            break;

        case ALLEGRO_KEY_1:
            controles.cambiarC = true; //solicita la fase cian
            controles.cambiarM = false; //evita dejar otro color pendiente
            controles.cambiarA = false; //evita dejar otro color pendiente
            break;

        case ALLEGRO_KEY_2:
            controles.cambiarC = false; //evita dejar otro color pendiente
            controles.cambiarM = true; //solicita la fase magenta
            controles.cambiarA = false; //evita dejar otro color pendiente
            break;

        case ALLEGRO_KEY_3:
            controles.cambiarC = false; //evita dejar otro color pendiente
            controles.cambiarM = false; //evita dejar otro color pendiente
            controles.cambiarA = true; //solicita la fase ambar
            break;

        case ALLEGRO_KEY_SPACE:
            controles.solicitarPulso = true; //deja el pulso pendiente hasta el siguiente tick
            break;

        case ALLEGRO_KEY_P:
        case ALLEGRO_KEY_ESCAPE:
            controles.alternarPausa = true; //solicita un cambio del estado de pausa
            break;

        case ALLEGRO_KEY_M:
            controles.volverMenu = true; //solicita regresar al menu si el juego esta pausado
            break;
        }
        return; //termina de procesar la tecla presionada
    }

    if (evento.type == ALLEGRO_EVENT_KEY_UP) { //revisa las teclas que dejaron de estar presionadas
        switch (evento.keyboard.keycode) { //identifica la flecha liberada
        case ALLEGRO_KEY_LEFT:
            controles.izquierda = false; //detiene el giro por la flecha izquierda
            break;

        case ALLEGRO_KEY_RIGHT:
            controles.derecha = false; //detiene el giro por la flecha derecha
            break;
        }
    }
}

//prepara la accion que el motor aplicara durante un solo tick
AccionMotor obtenerAccionAllegro(EstadoControles& controles, const Motor& motor) {
    AccionMotor accion = crearAccionVaciaAllegro(motor);

    if (controles.izquierda && !controles.derecha) {
        accion.cambiarAngulo = true;
        accion.anguloObjetivo = motor.nave.anguloActual + 3.0; //gira en sentido antihorario
    }
    else if (controles.derecha && !controles.izquierda) {
        accion.cambiarAngulo = true;
        accion.anguloObjetivo = motor.nave.anguloActual - 3.0; //gira en sentido horario
    }
    else if (controles.usarMouse) {
        accion.cambiarAngulo = true;
        accion.anguloObjetivo = controles.anguloMouse; //apunta hacia la posicion del cursor
    }

    if (controles.cambiarC) {
        accion.cambiarColor = true;
        accion.colorNuevo = 0;
    }
    else if (controles.cambiarM) {
        accion.cambiarColor = true;
        accion.colorNuevo = 1;
    }
    else if (controles.cambiarA) {
        accion.cambiarColor = true;
        accion.colorNuevo = 2;
    }

    if (controles.solicitarPulso) {
        accion.solicitarPulso = true;
    }

    controles.cambiarC = false; //consume las solicitudes de color de este tick
    controles.cambiarM = false;
    controles.cambiarA = false;
    controles.solicitarPulso = false; //consume la solicitud de pulso

    return accion;
}

//consume una sola solicitud de pausar o reanudar
bool consumirAlternarPausa(EstadoControles& controles) {
    if (!controles.alternarPausa) return false; //no cambia la pausa si no hay una solicitud
    controles.alternarPausa = false; //evita procesar la misma tecla dos veces
    return true; //confirma que debe cambiarse el estado de pausa
}

//consume la solicitud de volver al menu principal
bool consumirVolverMenu(EstadoControles& controles) {
    if (!controles.volverMenu) return false; //no abandona si no se presiono la tecla indicada
    controles.volverMenu = false; //evita procesar la misma solicitud dos veces
    return true; //informa que el usuario solicito volver al menu
}