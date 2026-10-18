#include "ScriptAcciones.h"

#include <fstream>
#include <sstream>

static AccionMotor crearAccionVacia() {
    return AccionMotor{ false, 0.0, false, 0, false };
}

static int colorDesdeTexto(const std::string& texto) {
    if (texto == "C") return 0;
    if (texto == "M") return 1;
    if (texto == "A") return 2;

    return -1;
}

static void agregarAccionScript(ScriptAcciones& script, const AccionScript& accion) {
    NodoAccionScript* nuevo = new NodoAccionScript{ accion, nullptr };

    if (script.cola != nullptr) {
        script.cola->sig = nuevo;
    }
    else {
        script.cabeza = nuevo;
    }

    script.cola = nuevo;
    script.tam++;
}

void inicializarScriptAcciones(ScriptAcciones& script) {
    script.cabeza = nullptr;
    script.cola = nullptr;
    script.tam = 0;
}

void destruirScriptAcciones(ScriptAcciones& script) {
    NodoAccionScript* actual = script.cabeza;

    while (actual != nullptr) {
        NodoAccionScript* siguiente = actual->sig;
        delete actual;
        actual = siguiente;
    }

    script.cabeza = nullptr;
    script.cola = nullptr;
    script.tam = 0;
}

bool cargarScriptAcciones(ScriptAcciones& script, const std::string& ruta) {
    std::ifstream archivo(ruta);

    if (!archivo.is_open()) return false;

    destruirScriptAcciones(script);

    std::string linea;

    while (std::getline(archivo, linea)) {
        size_t posicionComentario = linea.find('#');

        if (posicionComentario != std::string::npos) {
            linea = linea.substr(0, posicionComentario);
        }

        std::istringstream entrada(linea);

        std::string jugador;
        int oleada;
        int tick;
        std::string tipoAccion;

        if (!(entrada >> jugador >> oleada >> tick >> tipoAccion)) {
            continue;
        }

        if (oleada <= 0 || tick < 0) {
            continue;
        }

        AccionMotor accion = crearAccionVacia();
        bool valida = true;

        if (tipoAccion == "COLOR") {
            std::string parametro;

            if (!(entrada >> parametro)) {
                valida = false;
            }
            else {
                int color = colorDesdeTexto(parametro);

                if (color == -1) {
                    valida = false;
                }
                else {
                    accion.cambiarColor = true;
                    accion.colorNuevo = color;
                }
            }
        }
        else if (tipoAccion == "GIRAR_A") {
            double angulo;

            if (!(entrada >> angulo)) {
                valida = false;
            }
            else {
                accion.cambiarAngulo = true;
                accion.anguloObjetivo = angulo;
            }
        }
        else if (tipoAccion == "PULSO") {
            accion.solicitarPulso = true;
        }
        else {
            valida = false;
        }

        if (valida) {
            AccionScript registro{ jugador, oleada, tick, accion };
            agregarAccionScript(script, registro);
        }
    }

    archivo.close();

    return true;
}

AccionMotor obtenerAccionScript(const ScriptAcciones& script, const std::string& jugador, int oleada, int tick) {
    AccionMotor resultado = crearAccionVacia();

    NodoAccionScript* actual = script.cabeza;

    while (actual != nullptr) {
        if (actual->dato.jugador == jugador && actual->dato.oleada == oleada && actual->dato.tick == tick) {
            const AccionMotor& accion = actual->dato.accion;

            if (accion.cambiarAngulo) {
                resultado.cambiarAngulo = true;
                resultado.anguloObjetivo = accion.anguloObjetivo;
            }

            if (accion.cambiarColor) {
                resultado.cambiarColor = true;
                resultado.colorNuevo = accion.colorNuevo;
            }

            if (accion.solicitarPulso) {
                resultado.solicitarPulso = true;
            }
        }

        actual = actual->sig;
    }

    return resultado;
}