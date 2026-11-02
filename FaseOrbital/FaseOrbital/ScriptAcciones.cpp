#include "ScriptAcciones.h"

#include <fstream>
#include <sstream>

//crea una accion que no solicita cambios de destino fase ni activacion de pulso
static AccionMotor crearAccionVacia() {
    return AccionMotor{ false, 0.0, false, 0, false }; //deja las solicitudes apagadas y los valores de referencia en cero
}

//convierte las letras de las fases a sus numeros y devuelve menos uno si la letra no es valida
static int colorDesdeTexto(const string& texto) {
    if (texto == "C") return 0; //reconoce la fase cian
    if (texto == "M") return 1; //reconoce la fase magenta
    if (texto == "A") return 2; //reconoce la fase ambar

    return -1; //indica que el texto no coincide con ninguna fase permitida
}

//agrega una copia de la accion al final para conservar el orden del archivo
static void agregarAccionScript(ScriptAcciones& script, const AccionScript& accion) {
    NodoAccionScript* nuevo = new NodoAccionScript{ accion, nullptr }; //crea el nodo con la accion recibida y sin un siguiente

    if (script.cola != nullptr) { //comprueba si ya habia acciones guardadas
        script.cola->sig = nuevo; //conecta la ultima accion anterior con la nueva
    }
    else { //atiende la primera insercion en una lista vacia
        script.cabeza = nuevo; //marca este nodo como inicio de la lista
    }

    script.cola = nuevo; //deja la nueva accion como ultima en ambos casos
    script.tam++; //cuenta la accion que se acaba de agregar
}

//establece los valores iniciales de una lista que todavia no tiene acciones a su cargo
void inicializarScriptAcciones(ScriptAcciones& script) {
    script.cabeza = nullptr; //indica que no hay una primera accion
    script.cola = nullptr; //indica que tampoco hay una ultima accion
    script.tam = 0; //inicia el contador en cero
}

//borra los nodos del script y restablece sus extremos y contador
void destruirScriptAcciones(ScriptAcciones& script) {
    NodoAccionScript* actual = script.cabeza; //empieza a liberar desde la primera accion

    while (actual != nullptr) { //continua mientras quede un nodo por borrar
        NodoAccionScript* siguiente = actual->sig; //guarda donde continuar antes de borrar el nodo actual
        delete actual; //libera el nodo junto con los datos de su accion
        actual = siguiente; //continua con el nodo que habia guardado
    }

    script.cabeza = nullptr; //deja la lista sin un primer nodo
    script.cola = nullptr; //deja la lista sin un ultimo nodo
    script.tam = 0; //indica que ya no quedan acciones guardadas
}

//abre el archivo y reemplaza las acciones anteriores por las lineas validas que encuentre
//devuelve falso si no logra abrirlo y acepta un archivo sin acciones validas como un script vacio
bool cargarScriptAcciones(ScriptAcciones& script, const string& ruta) {
    ifstream archivo(ruta); //intenta abrir el archivo de acciones para leerlo

    if (!archivo.is_open()) return false; //conserva el script anterior cuando el archivo no se pudo abrir

    destruirScriptAcciones(script); //borra las acciones anteriores despues de confirmar que el archivo abrio

    string linea; //recibira el texto de cada linea del archivo

    while (getline(archivo, linea)) { //lee el archivo una linea a la vez
        size_t posicionComentario = linea.find('#'); //busca donde empieza un comentario dentro de la linea

        if (posicionComentario != string::npos) { //comprueba si encontro el signo de comentario
            linea = linea.substr(0, posicionComentario); //conserva solamente el texto anterior al comentario
        }

        istringstream entrada(linea); //permite extraer los datos separados por espacios

        string jugador; //recibira el alias del participante
        int oleada; //recibira el numero de oleada
        int tick; //recibira el momento del turno en que se aplica la accion
        string tipoAccion; //recibira el nombre de la accion que se quiere realizar

        if (!(entrada >> jugador >> oleada >> tick >> tipoAccion)) { //comprueba que pueda leer los cuatro datos iniciales
            continue; //omite lineas vacias incompletas o con datos que no se pueden leer
        }

        if (oleada <= 0 || tick < 0) { //rechaza oleadas menores que uno y ticks negativos
            continue; //pasa a la siguiente linea sin guardar esta accion
        }

        AccionMotor accion = crearAccionVacia(); //prepara una accion sin solicitudes antes de llenar sus datos

        if (tipoAccion == "COLOR") { //procesa una solicitud de cambio de fase
            string parametro; //recibira la letra que identifica la fase

            if (!(entrada >> parametro)) { //comprueba que venga una letra despues de la accion
                continue; //omite la linea si falta la fase
            }

            int color = colorDesdeTexto(parametro); //convierte la letra de fase al numero que utiliza el motor

            if (color == -1) { //comprueba si la letra no corresponde a una fase permitida
                continue; //descarta la linea sin agregar una accion incompleta
            }

            accion.cambiarColor = true; //activa la solicitud de cambio de fase
            accion.colorNuevo = color; //guarda la fase que debera usar la nave
        }
        else if (tipoAccion == "GIRAR_A") { //procesa un cambio del destino angular de la nave
            double angulo; //recibira el angulo solicitado en grados

            if (!(entrada >> angulo)) { //comprueba que el parametro pueda leerse como un numero
                continue; //omite la linea si falta el angulo o no se puede leer
            }

            accion.cambiarAngulo = true; //activa la solicitud de cambiar el destino
            accion.anguloObjetivo = angulo; //guarda el angulo que el motor normalizara al aplicar la accion
        }
        else if (tipoAccion == "PULSO") { //procesa una solicitud que no necesita parametro
            accion.solicitarPulso = true; //pide activar el pulso cuando el motor lo permita
        }
        else { //descarta nombres de accion que el programa no reconoce
            continue; //continua leyendo sin guardar esta linea
        }

        AccionScript registro{ jugador, oleada, tick, accion }; //relaciona la accion valida con su participante y momento
        agregarAccionScript(script, registro); //guarda el registro al final para mantener el orden de lectura
    }

    archivo.close(); //cierra el archivo despues de terminar la lectura

    return true; //confirma que el archivo se abrio aunque no haya dejado acciones validas
}

//reune las acciones que coinciden con jugador oleada y tick y da prioridad al ultimo giro y color leidos
AccionMotor obtenerAccionScript(const ScriptAcciones& script, const string& jugador, int oleada, int tick) {
    AccionMotor resultado = crearAccionVacia(); //empieza sin solicitudes por si no encuentra acciones para ese momento

    NodoAccionScript* actual = script.cabeza; //empieza a buscar desde la primera accion cargada

    while (actual != nullptr) { //revisa todas las acciones para poder combinar las que coincidan
        if (actual->dato.jugador == jugador && actual->dato.oleada == oleada && actual->dato.tick == tick) { //selecciona solo las del participante y momento solicitados
            const AccionMotor& accion = actual->dato.accion; //consulta los cambios guardados en este registro

            if (accion.cambiarAngulo) { //revisa si este registro pide otro destino para la nave
                resultado.cambiarAngulo = true; //incluye el cambio de destino en la respuesta
                resultado.anguloObjetivo = accion.anguloObjetivo; //conserva el ultimo angulo encontrado para ese tick
            }

            if (accion.cambiarColor) { //revisa si el registro pide cambiar la fase
                resultado.cambiarColor = true; //incluye el cambio de fase en la respuesta
                resultado.colorNuevo = accion.colorNuevo; //conserva el ultimo color encontrado para ese tick
            }

            if (accion.solicitarPulso) { //revisa si alguna accion coincidente solicita un pulso
                resultado.solicitarPulso = true; //mantiene la solicitud aunque despues encuentre otras acciones
            }
        }

        actual = actual->sig; //continua buscando coincidencias en el siguiente registro
    }

    return resultado; //entrega los cambios combinados o una accion sin solicitudes si no hubo coincidencias
}
