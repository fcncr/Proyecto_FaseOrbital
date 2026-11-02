#include "ListaBalas.h" //Incluye las estructuras y funciones de la lista de balas

void inicializarListaBalas(ListaBalas& lista) { //Prepara la lista para su primer uso sin liberar nodos previos
    lista.cabeza = nullptr; //Establece que la lista no tiene un primer nodo
    lista.tam = 0; //Inicia el contador de balas en cero
}

void insertarInicio(ListaBalas& lista, const Bala& bala) { //Agrega una bala antes del primer nodo actual
    NodoBala* nuevo = new NodoBala{ bala, lista.cabeza }; //Copia la bala y enlaza el nuevo nodo con la cabeza anterior
    lista.cabeza = nuevo; //Coloca el nuevo nodo al inicio de la lista
    lista.tam++; //Cuenta la bala que se acaba de agregar
}

int eliminarBalasSi(ListaBalas& lista, bool (*predicado)(const Bala&)) { //Recorre la lista y elimina las balas que cumplen la condicion
    if (predicado == nullptr) { //Comprueba si falta la funcion que evalua las balas
        return 0; //Termina sin eliminar nodos cuando no se recibe una condicion
    }

    int eliminadas = 0; //Cuenta las balas eliminadas durante este recorrido
    NodoBala** enlace = &lista.cabeza; //Apunta al enlace que permite cambiar directamente la cabeza

    while (*enlace != nullptr) { //Continua mientras el enlace actual apunte a un nodo
        NodoBala* actual = *enlace; //Obtiene el nodo que se va a evaluar

        if (predicado(actual->dato)) { //Consulta si la bala debe eliminarse
            *enlace = actual->sig; //Conecta la cabeza o el nodo anterior con el siguiente nodo
            delete actual; //Libera la memoria del nodo retirado
            lista.tam--; //Descuenta la bala eliminada del total de la lista
            eliminadas++; //Actualiza la cantidad de balas eliminadas en esta llamada
        }
        else { //Avanza el enlace solamente cuando la bala se conserva
            enlace = &actual->sig; //Permite revisar y modificar el enlace al siguiente nodo
        }
    }

    return eliminadas; //Devuelve la cantidad de balas que fueron eliminadas
}

int tamanoListaBalas(const ListaBalas& lista) { //Consulta la cantidad de balas sin recorrer los nodos
    return lista.tam; //Devuelve el contador almacenado en la lista
}

bool listaBalasVacia(const ListaBalas& lista) { //Comprueba si la lista no contiene balas
    return lista.tam == 0; //Devuelve verdadero cuando el contador esta en cero
}

void destruirListaBalas(ListaBalas& lista) { //Libera manualmente todos los nodos de la lista
    NodoBala* actual = lista.cabeza; //Comienza el recorrido desde el primer nodo

    while (actual != nullptr) { //Repite la liberacion mientras queden nodos
        NodoBala* siguiente = actual->sig; //Guarda el siguiente nodo antes de borrar el actual
        delete actual; //Libera la memoria del nodo actual
        actual = siguiente; //Continua con el nodo que se habia guardado
    }

    lista.cabeza = nullptr; //Deja la lista sin un primer nodo
    lista.tam = 0; //Restablece el contador despues de liberar todas las balas
}