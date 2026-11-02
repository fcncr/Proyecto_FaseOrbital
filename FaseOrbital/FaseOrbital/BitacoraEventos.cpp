#include "BitacoraEventos.h"

void inicializarBitacora(BitacoraEventos& bitacora) { //establece los valores iniciales sin liberar nodos que ya existieran
    bitacora.cabeza = nullptr; //indica que todavia no hay un primer evento
    bitacora.cola = nullptr; //indica que tampoco hay un ultimo evento
    bitacora.tam = 0; //empieza el contador de eventos en cero
}

void agregarEventoFinal(BitacoraEventos& bitacora, const Evento& evento) { //guarda un nuevo evento al final de la bitacora
    NodoEvento* nuevo = new NodoEvento; //reserva espacio para guardar el evento y su enlace
    nuevo->dato = evento; //copia los datos del evento recibido
    nuevo->sig = nullptr; //deja este nodo sin siguiente porque ira al final

    if (bitacora.cola == nullptr) { //revisa si la bitacora esta vacia
        bitacora.cabeza = nuevo; //coloca el nuevo evento como el primero de la lista
    }
    else { //entra aqui cuando ya hay eventos guardados
        bitacora.cola->sig = nuevo; //conecta el ultimo nodo anterior con el nuevo
    }

    bitacora.cola = nuevo; //actualiza el ultimo nodo tanto si la lista estaba vacia como si no
    bitacora.tam++; //suma el evento que se acaba de guardar
}

void recorrerBitacora(const BitacoraEventos& bitacora, void (*procesar)(const Evento&)) { //pasa cada evento a la funcion recibida
    if (procesar == nullptr) { //revisa si falta la funcion que debe usar los eventos
        return; //sale porque no hay una funcion que pueda llamar
    }

    const NodoEvento* actual = bitacora.cabeza; //empieza a revisar desde el primer evento

    while (actual != nullptr) { //sigue mientras queden eventos por revisar
        procesar(actual->dato); //entrega el evento actual a la funcion recibida
        actual = actual->sig; //pasa al siguiente evento de la lista
    }
}

void filtrarPorJugador(const BitacoraEventos& origen, const std::string& jugador, BitacoraEventos& destino) { //selecciona los eventos que pertenecen al jugador indicado
    BitacoraEventos temporal; //guarda los resultados aparte para poder usar la misma bitacora como origen y destino
    inicializarBitacora(temporal); //prepara la lista donde se guardaran las coincidencias

    const NodoEvento* actual = origen.cabeza; //empieza a revisar los eventos del origen

    while (actual != nullptr) { //revisa todos los eventos en el orden en que estaban guardados
        if (actual->dato.jugador == jugador) { //comprueba si el evento corresponde al jugador buscado
            agregarEventoFinal(temporal, actual->dato); //guarda una copia del evento que coincide
        }

        actual = actual->sig; //continua con el siguiente evento aunque el anterior no coincidiera
    }

    destruirBitacora(destino); //borra los nodos anteriores del destino cuando ya termino de revisar el origen

    destino.cabeza = temporal.cabeza; //hace que el destino empiece en el primer resultado encontrado
    destino.cola = temporal.cola; //deja el ultimo resultado como final de la lista destino
    destino.tam = temporal.tam; //guarda en destino la cantidad de eventos encontrados

    temporal.cabeza = nullptr; //desconecta la lista temporal porque sus nodos ahora pertenecen al destino
    temporal.cola = nullptr; //quita tambien su referencia al ultimo nodo sin borrarlo
    temporal.tam = 0; //deja la lista temporal sin eventos a su cargo
}

void filtrarPorOleada(const BitacoraEventos& origen, int oleada, BitacoraEventos& destino) { //selecciona los eventos que ocurrieron en la oleada indicada
    BitacoraEventos temporal; //reune los resultados aparte antes de reemplazar el destino
    inicializarBitacora(temporal); //prepara una lista vacia para los eventos que coincidan

    const NodoEvento* actual = origen.cabeza; //empieza por el primer evento de la bitacora original

    while (actual != nullptr) { //recorre los eventos sin cambiar el orden en que aparecen
        if (actual->dato.oleada == oleada) { //revisa si el evento pertenece a la oleada buscada
            agregarEventoFinal(temporal, actual->dato); //agrega una copia del evento a los resultados
        }

        actual = actual->sig; //avanza al siguiente evento para seguir buscando
    }

    destruirBitacora(destino); //libera el contenido anterior del destino despues de terminar la busqueda

    destino.cabeza = temporal.cabeza; //coloca los resultados encontrados al inicio del destino
    destino.cola = temporal.cola; //guarda donde termina la nueva lista de eventos
    destino.tam = temporal.tam; //actualiza la cantidad de eventos que quedaron en destino

    temporal.cabeza = nullptr; //deja de apuntar a los nodos que ya pasaron al destino
    temporal.cola = nullptr; //quita la referencia al ultimo nodo sin liberar los resultados
    temporal.tam = 0; //marca la lista temporal como vacia
}

void destruirBitacora(BitacoraEventos& bitacora) { //borra todos los nodos que guardan los eventos
    NodoEvento* actual = bitacora.cabeza; //empieza por el primer nodo de la lista

    while (actual != nullptr) { //continua mientras quede algun nodo por borrar
        NodoEvento* siguiente = actual->sig; //guarda donde seguir antes de borrar el nodo actual
        delete actual; //libera el nodo y los datos del evento que contenia
        actual = siguiente; //continua con el nodo que se habia guardado
    }

    bitacora.cabeza = nullptr; //deja la lista sin un primer evento
    bitacora.cola = nullptr; //deja la lista sin un ultimo evento
    bitacora.tam = 0; //pone el contador en cero porque ya no quedan eventos
}