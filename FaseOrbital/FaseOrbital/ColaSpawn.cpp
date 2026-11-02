#include "ColaSpawn.h"

ColaSpawn::ColaSpawn() { //crea una cola vacia lista para utilizarse
    frente = nullptr; //establece que no existe una primera aparicion
    final = nullptr; //establece que no existe una ultima aparicion
    tam = 0; //inicia el contador de apariciones en cero
}

ColaSpawn::ColaSpawn(const ColaSpawn& otra) { //construye una cola nueva copiando cada nodo de la original
    frente = nullptr; //prepara el frente de la nueva cola antes de copiar
    final = nullptr; //prepara el final de la nueva cola antes de copiar
    tam = 0; //inicia el contador para registrar los nodos que se copien

    NodoSpawn* actual = otra.frente; //comienza a leer desde el primer nodo de la cola original

    while (actual != nullptr) { //recorre todas las apariciones de la cola original
        encolarSpawn(*this, actual->dato); //crea un nodo propio con los mismos datos y conserva el orden
        actual = actual->sig; //avanza al siguiente nodo de la cola original
    }
}

ColaSpawn& ColaSpawn::operator=(const ColaSpawn& otra) { //reemplaza el contenido actual por una copia de otra cola
    if (this == &otra) { //detecta si se intenta asignar la cola a si misma
        return *this; //conserva los nodos y evita borrar el contenido que se iba a copiar
    }

    vaciarColaSpawn(*this); //libera los nodos anteriores antes de recibir la copia

    NodoSpawn* actual = otra.frente; //ubica la primera aparicion de la cola que se va a copiar

    while (actual != nullptr) { //recorre los nodos de la cola de origen
        encolarSpawn(*this, actual->dato); //agrega una copia de los datos en un nodo independiente
        actual = actual->sig; //continua con la siguiente aparicion del origen
    }

    return *this; //devuelve la cola actual despues de completar la asignacion
}

ColaSpawn::~ColaSpawn() { //libera los nodos al terminar la vida del objeto
    vaciarColaSpawn(*this); //elimina las apariciones que todavia permanezcan en la cola
}

void inicializarColaSpawn(ColaSpawn& cola) { //reinicia una cola ya construida para dejarla vacia
    vaciarColaSpawn(cola); //libera el contenido previo y restablece los valores iniciales
}

void encolarSpawn(ColaSpawn& cola, const Spawn& spawn) { //agrega una aparicion al final sin recorrer la cola
    NodoSpawn* nuevo = new NodoSpawn; //reserva memoria para el nodo de la nueva aparicion
    nuevo->dato = spawn; //copia los datos de la aparicion recibida
    nuevo->sig = nullptr; //marca el nuevo nodo como el ultimo de la cadena

    if (cola.final == nullptr) { //comprueba si la cola esta vacia
        cola.frente = nuevo; //coloca el nuevo nodo tambien como frente
    }
    else { //atiende el caso de una cola que ya contiene apariciones
        cola.final->sig = nuevo; //enlaza el ultimo nodo anterior con el nuevo
    }

    cola.final = nuevo; //actualiza el final de la cola en ambos casos
    cola.tam++; //cuenta la aparicion que se acaba de agregar
}

bool desencolarSpawn(ColaSpawn& cola, Spawn& salida) { //retira la primera aparicion y entrega sus datos
    if (cola.frente == nullptr) { //comprueba si no hay apariciones para retirar
        return false; //indica que la cola estaba vacia y mantiene salida sin cambios
    }

    NodoSpawn* eliminado = cola.frente; //guarda el nodo que se va a retirar
    salida = eliminado->dato; //copia los datos antes de liberar la memoria del nodo
    cola.frente = eliminado->sig; //mueve el frente a la siguiente aparicion

    delete eliminado; //libera la memoria del nodo retirado

    cola.tam--; //descuenta la aparicion que salio de la cola

    if (cola.frente == nullptr) { //detecta si se retiro el unico nodo que quedaba
        cola.final = nullptr; //deja ambos extremos vacios para evitar un puntero a memoria liberada
        cola.tam = 0; //restablece el contador al quedar vacia la cola
    }

    return true; //confirma que se retiro una aparicion y se copiaron sus datos
}

const Spawn* verFrenteSpawn(const ColaSpawn& cola) { //consulta la primera aparicion sin cambiar la cola
    if (cola.frente == nullptr) { //comprueba si la cola esta vacia
        return nullptr; //indica que no existe una aparicion para consultar
    }

    return &cola.frente->dato; //devuelve un puntero de solo lectura al dato almacenado en el primer nodo
}

bool colaSpawnVacia(const ColaSpawn& cola) { //comprueba si quedan apariciones pendientes
    return cola.frente == nullptr; //devuelve verdadero cuando no existe un primer nodo
}

int tamanoColaSpawn(const ColaSpawn& cola) { //consulta la cantidad de apariciones sin recorrer los nodos
    return cola.tam; //devuelve el contador almacenado en la cola
}

void vaciarColaSpawn(ColaSpawn& cola) { //libera todos los nodos y deja la cola disponible para reutilizarse
    NodoSpawn* actual = cola.frente; //comienza el recorrido desde la primera aparicion

    while (actual != nullptr) { //continua mientras queden nodos por liberar
        NodoSpawn* siguiente = actual->sig; //guarda el siguiente nodo antes de borrar el actual
        delete actual; //libera la memoria del nodo actual
        actual = siguiente; //avanza al nodo que se habia guardado
    }

    cola.frente = nullptr; //deja la cola sin una primera aparicion
    cola.final = nullptr; //deja la cola sin una ultima aparicion
    cola.tam = 0; //restablece el contador despues de liberar todos los nodos
}

void destruirColaSpawn(ColaSpawn& cola) { //permite liberar los nodos antes de que termine la vida del objeto
    vaciarColaSpawn(cola); //deja la cola vacia para que una limpieza posterior siga siendo segura
}

void copiarColaSpawn(const ColaSpawn& origen, ColaSpawn& destino) { //copia el contenido sin compartir nodos entre ambas colas
    destino = origen; //utiliza el operador de asignacion que realiza la copia profunda
}