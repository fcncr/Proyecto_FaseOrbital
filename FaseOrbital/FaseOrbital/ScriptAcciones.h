#ifndef SCRIPT_ACCIONES_H
#define SCRIPT_ACCIONES_H

#include <string>
#include "ModelosCombate.h"

using namespace std; //permite usar string sin escribir el prefijo

struct AccionScript { //relaciona una accion con el jugador y el momento en que debe aplicarse
    string jugador; //alias del participante al que pertenece la accion
    int oleada; //oleada en la que se debe ejecutar
    int tick; //tick del turno en que corresponde aplicar la accion
    AccionMotor accion; //cambios de angulo fase o solicitud de pulso que recibira el motor
};

struct NodoAccionScript { //guarda una accion y el enlace a la siguiente
    AccionScript dato; //datos de una linea valida del archivo
    NodoAccionScript* sig; //siguiente nodo o nullptr si es el ultimo
};

struct ScriptAcciones { //conserva las acciones en el orden en que se leyeron
    NodoAccionScript* cabeza; //primer nodo de la lista
    NodoAccionScript* cola; //ultimo nodo para agregar acciones sin recorrer la lista
    int tam; //cantidad de acciones guardadas
};

//prepara el script para su primer uso sin liberar nodos de una lista anterior
void inicializarScriptAcciones(ScriptAcciones& script);

//libera todas las acciones y deja el script vacio para poder reutilizarlo
void destruirScriptAcciones(ScriptAcciones& script);

//carga las lineas validas en un script inicializado y conserva el contenido anterior si no puede abrir el archivo
bool cargarScriptAcciones(ScriptAcciones& script, const string& ruta);

//combina las acciones del jugador en la oleada y tick indicados usando el ultimo angulo y color que encuentre
AccionMotor obtenerAccionScript(const ScriptAcciones& script, const string& jugador, int oleada, int tick);

#endif