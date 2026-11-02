#include "ListaEnemigos.h"

void inicializarListaEnemigos(ListaEnemigos& lista) { //Prepara la lista para su primer uso sin liberar nodos previos
    lista.cabeza = nullptr; //Establece que la lista no tiene un primer nodo
    lista.cola = nullptr; //Establece que la lista no tiene un ultimo nodo
    lista.tam = 0; //Inicia el contador de enemigos en cero
}

NodoEnemigo* insertarFinal(ListaEnemigos& lista, const Enemigo& enemigo) { //Agrega un enemigo despues del ultimo nodo actual
    NodoEnemigo* nuevo = new NodoEnemigo{ enemigo, lista.cola, nullptr }; //Copia el enemigo y enlaza el nuevo nodo con la cola anterior

    if (lista.cola != nullptr) { //Comprueba si ya hay nodos en la lista
        lista.cola->sig = nuevo; //Enlaza la cola anterior con el nuevo nodo
    }
    else { //Atiende el caso de una lista vacia
        lista.cabeza = nuevo; //Coloca el nuevo nodo tambien como cabeza
    }

    lista.cola = nuevo; //Actualiza el ultimo nodo de la lista
    lista.tam++; //Cuenta el enemigo que se acaba de agregar

    return nuevo; //Devuelve el nodo creado para que pueda utilizarse despues
}

NodoEnemigo* primeroEnemigo(const ListaEnemigos& lista) { //Consulta el inicio de la lista sin recorrerla
    return lista.cabeza; //Devuelve el primer nodo o nullptr si la lista esta vacia
}

NodoEnemigo* ultimoEnemigo(const ListaEnemigos& lista) { //Consulta el final de la lista sin recorrerla
    return lista.cola; //Devuelve el ultimo nodo o nullptr si la lista esta vacia
}

int tamanoListaEnemigos(const ListaEnemigos& lista) { //Consulta la cantidad de enemigos sin recorrer los nodos
    return lista.tam; //Devuelve el contador almacenado en la lista
}

bool listaEnemigosVacia(const ListaEnemigos& lista) { //Comprueba si la lista no contiene enemigos
    return lista.tam == 0; //Devuelve verdadero cuando el contador esta en cero
}

void destruirListaEnemigos(ListaEnemigos& lista) { //Libera manualmente todos los nodos de la lista
    NodoEnemigo* actual = lista.cabeza; //Comienza el recorrido desde el primer nodo

    while (actual != nullptr) { //Repite la liberacion mientras queden nodos
        NodoEnemigo* siguiente = actual->sig; //Guarda el siguiente nodo antes de borrar el actual
        delete actual; //Libera la memoria del nodo actual
        actual = siguiente; //Continua con el nodo que se habia guardado
    }

    lista.cabeza = nullptr; //Deja la lista sin un primer nodo
    lista.cola = nullptr; //Deja la lista sin un ultimo nodo
    lista.tam = 0; //Restablece el contador despues de liberar todos los enemigos
}

NodoEnemigo* insertarDespues(ListaEnemigos& lista, NodoEnemigo* nodo, const Enemigo& enemigo) { //Inserta un enemigo inmediatamente despues del nodo recibido
    if (nodo == nullptr) { //Comprueba si falta el nodo que indica la posicion de insercion
        return nullptr; //Termina sin insertar porque no existe un nodo de referencia
    }

    NodoEnemigo* nuevo = new NodoEnemigo{ enemigo, nodo, nodo->sig }; //Copia el enemigo y prepara los enlaces con sus dos vecinos

    if (nodo->sig != nullptr) { //Comprueba si habia otro nodo despues del recibido
        nodo->sig->ant = nuevo; //Hace que ese nodo apunte hacia atras al nuevo nodo
    }
    else { //Atiende la insercion despues del ultimo nodo
        lista.cola = nuevo; //Actualiza la cola porque el nuevo nodo queda al final
    }

    nodo->sig = nuevo; //Enlaza el nodo recibido con el nodo recien insertado
    lista.tam++; //Cuenta el enemigo agregado a la lista

    return nuevo; //Devuelve el nodo creado para permitir nuevas inserciones despues de el
}

NodoEnemigo* eliminarEnemigo(ListaEnemigos& lista, NodoEnemigo* nodo) { //Retira un nodo y conserva conectados los enemigos restantes
    if (nodo == nullptr) { //Comprueba si se recibio un puntero nulo
        return nullptr; //Termina sin modificar la lista cuando no hay nodo que eliminar
    }

    NodoEnemigo* siguiente = nodo->sig; //Guarda el nodo que permitira continuar el recorrido despues de eliminar

    if (nodo->ant != nullptr) { //Comprueba si existe un nodo anterior al que se va a retirar
        nodo->ant->sig = nodo->sig; //Conecta el anterior con el siguiente saltando el nodo retirado
    }
    else { //Atiende la eliminacion del primer nodo
        lista.cabeza = nodo->sig; //Mueve la cabeza al siguiente nodo
    }

    if (nodo->sig != nullptr) { //Comprueba si existe un nodo despues del que se va a retirar
        nodo->sig->ant = nodo->ant; //Conecta el siguiente con el anterior para conservar el enlace inverso
    }
    else { //Atiende la eliminacion del ultimo nodo
        lista.cola = nodo->ant; //Mueve la cola al nodo anterior
    }

    delete nodo; //Libera la memoria del nodo retirado
    lista.tam--; //Descuenta el enemigo eliminado del total de la lista

    return siguiente; //Devuelve el siguiente nodo para continuar sin usar el nodo eliminado
}