#include "RondaJugadores.h"

void inicializarRondaJugadores(RondaJugadores& ronda) { //prepara la ronda para su primer uso sin liberar nodos previos
    ronda.actual = nullptr; //establece que todavia no hay un jugador en turno
    ronda.tam = 0; //inicia el contador de jugadores en cero
}

void agregarJugador(RondaJugadores& ronda, Jugador* jugador) { //agrega un nodo que apunta al jugador recibido
    if (jugador == nullptr) { //comprueba si falta el jugador que se quiere agregar
        return; //termina sin modificar la ronda cuando el puntero es nulo
    }

    NodoJugador* nuevo = new NodoJugador; //reserva memoria para el nodo de la ronda
    nuevo->jugador = jugador; //guarda la direccion del jugador sin crear una copia

    if (ronda.actual == nullptr) { //detecta si este sera el primer jugador de la ronda
        nuevo->sig = nuevo; //cierra el enlace siguiente sobre el mismo nodo
        nuevo->ant = nuevo; //cierra el enlace anterior sobre el mismo nodo
        ronda.actual = nuevo; //asigna el primer turno al jugador agregado
        ronda.tam = 1; //registra que la ronda contiene un solo jugador
        return; //termina porque la ronda de un nodo ya quedo enlazada
    }

    NodoJugador* ultimo = ronda.actual->ant; //obtiene el nodo situado justo antes del actual

    nuevo->sig = ronda.actual; //conecta el nuevo nodo hacia adelante con el actual
    nuevo->ant = ultimo; //conecta el nuevo nodo hacia atras con el anterior al actual

    ultimo->sig = nuevo; //enlaza el nodo anterior con el nuevo nodo
    ronda.actual->ant = nuevo; //completa la insercion antes del actual sin cambiar el turno

    ronda.tam++; //cuenta el jugador que se acaba de agregar
}

void avanzarJugador(RondaJugadores& ronda, bool horario) { //mueve el turno un nodo en el sentido indicado
    if (ronda.actual == nullptr) { //comprueba si la ronda esta vacia
        return; //termina porque no existe un turno que pueda avanzar
    }

    if (horario) { //selecciona el recorrido en sentido horario
        ronda.actual = ronda.actual->sig; //asigna el turno al siguiente nodo
    }
    else { //selecciona el recorrido en sentido antihorario
        ronda.actual = ronda.actual->ant; //asigna el turno al nodo anterior
    }
}

Jugador* eliminarJugadorActual(RondaJugadores& ronda, bool horario) { //retira el nodo del turno actual y conserva el jugador asociado
    if (ronda.actual == nullptr) { //comprueba si no hay jugadores en la ronda
        return nullptr; //indica que no se pudo retirar ningun jugador
    }

    NodoJugador* eliminado = ronda.actual; //guarda el nodo que se va a retirar
    Jugador* jugadorEliminado = eliminado->jugador; //conserva el puntero al jugador para devolverlo despues

    if (ronda.tam == 1) { //atiende el caso en que solo queda un jugador
        delete eliminado; //libera el unico nodo sin destruir el objeto del jugador

        ronda.actual = nullptr; //deja la ronda sin un jugador en turno
        ronda.tam = 0; //registra que la ronda quedo vacia

        return jugadorEliminado; //devuelve el jugador que estaba en el nodo retirado
    }

    NodoJugador* anterior = eliminado->ant; //guarda el vecino anterior antes de modificar los enlaces
    NodoJugador* siguiente = eliminado->sig; //guarda el vecino siguiente antes de modificar los enlaces

    anterior->sig = siguiente; //une el anterior con el siguiente saltando el nodo retirado
    siguiente->ant = anterior; //restablece el enlace inverso entre los nodos que permanecen

    if (horario) { //comprueba si la ronda debe continuar en sentido horario
        ronda.actual = siguiente; //entrega el turno al siguiente nodo
    }
    else { //continua la ronda en sentido antihorario
        ronda.actual = anterior; //entrega el turno al nodo anterior
    }

    delete eliminado; //libera solamente el nodo que salio de la ronda

    ronda.tam--; //descuenta el jugador retirado de la cantidad de participantes

    return jugadorEliminado; //devuelve el jugador retirado para que la partida conserve sus resultados
}

Jugador* verJugadorActual(const RondaJugadores& ronda) { //consulta el jugador del turno actual sin avanzar la ronda
    if (ronda.actual == nullptr) { //comprueba si la ronda esta vacia
        return nullptr; //indica que no hay un jugador en turno
    }

    return ronda.actual->jugador; //devuelve el jugador asociado al nodo actual
}

int tamanoRonda(const RondaJugadores& ronda) { //consulta la cantidad de jugadores sin recorrer los nodos
    return ronda.tam; //devuelve el contador almacenado en la ronda
}

bool rondaVacia(const RondaJugadores& ronda) { //comprueba si la ronda no contiene jugadores
    return ronda.actual == nullptr; //devuelve verdadero cuando no existe un nodo actual
}

void recorrerRondaJugadores(const RondaJugadores& ronda, bool horario, void (*procesar)(Jugador*)) { //recorre una vuelta aplicando la funcion recibida a cada jugador
    if (ronda.actual == nullptr || ronda.tam <= 0 || procesar == nullptr) { //comprueba que existan jugadores y una funcion para procesarlos
        return; //termina cuando no se puede realizar el recorrido
    }

    NodoJugador* nodo = ronda.actual; //usa un puntero local para recorrer sin mover el turno actual

    for (int i = 0; i < ronda.tam; i++) { //limita el recorrido a la cantidad de jugadores de la ronda
        procesar(nodo->jugador); //ejecuta la operacion recibida sobre el jugador visitado

        if (horario) { //selecciona el recorrido en sentido horario
            nodo = nodo->sig; //avanza al siguiente nodo sin modificar el turno
        }
        else { //selecciona el recorrido en sentido antihorario
            nodo = nodo->ant; //avanza al nodo anterior sin modificar el turno
        }
    }
}

void destruirRondaJugadores(RondaJugadores& ronda) { //libera todos los nodos de la ronda sin destruir los jugadores
    while (ronda.actual != nullptr) { //continua mientras quede algun nodo en la ronda
        eliminarJugadorActual(ronda, true); //retira un nodo y mantiene conectados los que faltan por liberar
    }
}