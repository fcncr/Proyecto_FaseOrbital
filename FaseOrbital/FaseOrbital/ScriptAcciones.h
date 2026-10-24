#ifndef SCRIPT_ACCIONES_H
#define SCRIPT_ACCIONES_H

#include <string>
#include "ModelosCombate.h" //llama a struct "AccionMotor"

struct AccionScript {
    std::string jugador;
    int oleada;
    int tick;
    AccionMotor accion;
};

struct NodoAccionScript {
    AccionScript dato;
    NodoAccionScript* sig;
};

struct ScriptAcciones {
    NodoAccionScript* cabeza;
    NodoAccionScript* cola;
    int tam;
};

void inicializarScriptAcciones(ScriptAcciones& script);
void destruirScriptAcciones(ScriptAcciones& script);
bool cargarScriptAcciones(ScriptAcciones& script, const std::string& ruta);
AccionMotor obtenerAccionScript(const ScriptAcciones& script, const std::string& jugador, int oleada, int tick);

#endif