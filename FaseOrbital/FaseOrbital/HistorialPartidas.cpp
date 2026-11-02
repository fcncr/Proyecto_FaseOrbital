#include "HistorialPartidas.h"

void inicializarHistorial(HistorialPartidas& historial) { //establece los valores iniciales sin borrar nodos que ya existieran
    historial.cabeza = nullptr; //indica que todavia no hay un primer registro
    historial.cola = nullptr; //indica que tampoco hay un ultimo registro
    historial.tam = 0; //empieza el contador de registros en cero
}

void agregarRegistroFinal(HistorialPartidas& historial, const RegistroPartida& registro) { //guarda un nuevo registro al final del historial
    NodoRegistroPartida* nuevo = new NodoRegistroPartida; //reserva espacio para el registro y su enlace al siguiente
    nuevo->dato = registro; //copia los datos del registro recibido
    nuevo->sig = nullptr; //deja el nuevo nodo sin siguiente porque ira al final

    if (historial.cola == nullptr) { //revisa si el historial esta vacio
        historial.cabeza = nuevo; //coloca el nuevo registro como el primero de la lista
    }
    else { //entra aqui cuando ya hay registros guardados
        historial.cola->sig = nuevo; //conecta el ultimo nodo anterior con el nuevo
    }

    historial.cola = nuevo; //deja el nuevo nodo al final en cualquiera de los dos casos
    historial.tam++; //suma el registro que se acaba de agregar
}

void recorrerHistorial(const HistorialPartidas& historial, void (*procesar)(const RegistroPartida&)) { //pasa los registros a la funcion recibida uno por uno
    if (procesar == nullptr) { //revisa si falta la funcion que debe usar los registros
        return; //sale porque no hay una funcion que pueda llamar
    }

    const NodoRegistroPartida* actual = historial.cabeza; //empieza a revisar desde el primer registro

    while (actual != nullptr) { //sigue mientras queden registros por revisar
        procesar(actual->dato); //entrega el registro actual a la funcion indicada
        actual = actual->sig; //pasa al siguiente registro de la lista
    }
}

int contarRegistros(const HistorialPartidas& historial) { //consulta la cantidad de registros sin recorrer la lista
    return historial.tam; //devuelve el contador que se mantiene al agregar y borrar registros
}

void buscarRegistrosJugador(const HistorialPartidas& origen, const std::string& jugador, HistorialPartidas& destino) { //busca los registros del jugador indicado y los deja en destino
    HistorialPartidas temporal; //guarda primero las coincidencias para no borrar datos que aun se necesitan
    inicializarHistorial(temporal); //prepara la lista donde se guardaran los registros encontrados

    const NodoRegistroPartida* actual = origen.cabeza; //empieza por el primer registro del historial original

    while (actual != nullptr) { //revisa todos los registros sin cambiar el orden en que aparecen
        if (actual->dato.jugador == jugador) { //comprueba si el registro pertenece al jugador buscado
            agregarRegistroFinal(temporal, actual->dato); //guarda una copia del registro que coincide
        }

        actual = actual->sig; //continua buscando en el siguiente registro
    }

    destruirHistorial(destino); //borra los nodos anteriores del destino cuando ya termino la busqueda

    destino.cabeza = temporal.cabeza; //hace que el destino empiece en el primer registro encontrado
    destino.cola = temporal.cola; //guarda donde termina la lista de resultados
    destino.tam = temporal.tam; //deja en destino la cantidad de registros encontrados

    temporal.cabeza = nullptr; //desconecta la lista temporal porque sus nodos ahora pertenecen al destino
    temporal.cola = nullptr; //quita tambien su referencia al ultimo nodo sin borrarlo
    temporal.tam = 0; //deja la lista temporal sin registros a su cargo
}

void destruirHistorial(HistorialPartidas& historial) { //borra todos los nodos que guardan los resultados
    NodoRegistroPartida* actual = historial.cabeza; //empieza por el primer nodo del historial

    while (actual != nullptr) { //continua mientras quede algun nodo por borrar
        NodoRegistroPartida* siguiente = actual->sig; //guarda donde seguir antes de borrar el nodo actual
        delete actual; //libera el nodo junto con los datos del registro
        actual = siguiente; //continua con el nodo que se habia guardado
    }

    historial.cabeza = nullptr; //deja el historial sin un primer registro
    historial.cola = nullptr; //deja el historial sin un ultimo registro
    historial.tam = 0; //pone el contador en cero porque ya no quedan registros
}