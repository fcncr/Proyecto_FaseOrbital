#include "ListaPuntajes.h"

static void dividirListaPuntajes(NodoPuntaje* cabeza, NodoPuntaje*& izquierda, NodoPuntaje*& derecha) { //separa la lista en dos partes para ordenarlas
    if (cabeza == nullptr || cabeza->sig == nullptr) { //con menos de dos nodos no hace falta dividir
        izquierda = cabeza; //deja el nodo disponible en la primera parte
        derecha = nullptr; //la segunda parte queda vacia
        return; //termina porque no hay mas nodos que repartir
    }

    NodoPuntaje* lento = cabeza; //avanza de uno en uno para encontrar la mitad
    NodoPuntaje* rapido = cabeza->sig; //avanza de dos en dos hasta llegar al final

    while (rapido != nullptr) { //busca donde separar las dos partes
        rapido = rapido->sig; //da el primer paso del recorrido rapido

        if (rapido != nullptr) { //si queda otro nodo se puede completar el avance
            lento = lento->sig; //mueve un lugar el punto de separacion
            rapido = rapido->sig; //da el segundo paso del recorrido rapido
        }
    }

    izquierda = cabeza; //la primera parte empieza donde comenzaba la lista
    derecha = lento->sig; //la segunda parte empieza despues del punto de separacion
    lento->sig = nullptr; //corta el enlace para que las partes queden separadas
}

static NodoPuntaje* fusionarListasPuntajes(NodoPuntaje* izquierda, NodoPuntaje* derecha) { //une dos partes que ya estan ordenadas
    if (izquierda == nullptr) { //si la primera parte se termino solo queda la segunda
        return derecha; //aprovecha los nodos que faltan de la segunda parte
    }

    if (derecha == nullptr) { //si la segunda parte se termino solo queda la primera
        return izquierda; //aprovecha los nodos que faltan de la primera parte
    }

    NodoPuntaje* resultado = nullptr; //guarda el nodo que debe ir primero

    if (izquierda->puntaje <= derecha->puntaje) { //en un empate conserva primero el nodo de la izquierda
        resultado = izquierda; //elige el menor puntaje disponible
        resultado->sig = fusionarListasPuntajes(izquierda->sig, derecha); //continua uniendo los nodos que faltan
    }
    else { //el puntaje de la derecha es menor
        resultado = derecha; //coloca primero el nodo de la derecha
        resultado->sig = fusionarListasPuntajes(izquierda, derecha->sig); //continua sin volver a tomar el nodo elegido
    }

    return resultado; //devuelve el inicio de la parte que quedo ordenada
}

static NodoPuntaje* mergeSortPuntajes(NodoPuntaje* cabeza) { //ordena la lista dividiendola y uniendo sus partes
    if (cabeza == nullptr || cabeza->sig == nullptr) { //una lista vacia o con un nodo ya esta ordenada
        return cabeza; //conserva el inicio que recibio
    }

    NodoPuntaje* izquierda = nullptr; //inicio de la primera parte
    NodoPuntaje* derecha = nullptr; //inicio de la segunda parte

    dividirListaPuntajes(cabeza, izquierda, derecha); //reparte los nodos entre las dos partes

    izquierda = mergeSortPuntajes(izquierda); //ordena la primera parte con el mismo proceso
    derecha = mergeSortPuntajes(derecha); //ordena la segunda parte con el mismo proceso

    return fusionarListasPuntajes(izquierda, derecha); //une las partes ordenadas usando los mismos nodos
}

void inicializarListaPuntajes(ListaPuntajes& lista) { //prepara la lista antes de guardar puntajes
    lista.cabeza = nullptr; //todavia no hay un primer nodo
    lista.cola = nullptr; //todavia no hay un ultimo nodo
    lista.tam = 0; //la lista empieza sin puntajes
}

void agregarPuntajeFinal(ListaPuntajes& lista, int puntaje) { //guarda un nuevo puntaje al final
    NodoPuntaje* nuevo = new NodoPuntaje; //crea el nodo para el puntaje recibido
    nuevo->puntaje = puntaje; //guarda el valor dentro del nodo
    nuevo->sig = nullptr; //el nuevo nodo no tiene otro despues

    if (lista.cola == nullptr) { //si la lista estaba vacia este sera su primer nodo
        lista.cabeza = nuevo; //marca el inicio de la lista
    }
    else { //si ya habia puntajes se agrega despues del ultimo
        lista.cola->sig = nuevo; //enlaza el ultimo nodo con el nuevo
    }

    lista.cola = nuevo; //en ambos casos el nuevo nodo queda al final
    lista.tam++; //cuenta el puntaje que se acaba de agregar
}

void ordenarListaPuntajesMergeSort(ListaPuntajes& lista) { //ordena los puntajes y actualiza los extremos de la lista
    lista.cabeza = mergeSortPuntajes(lista.cabeza); //guarda el primer nodo despues de ordenar

    lista.cola = lista.cabeza; //empieza a buscar el nuevo ultimo nodo desde el inicio

    if (lista.cola != nullptr) { //solo recorre la lista si tiene puntajes
        while (lista.cola->sig != nullptr) { //avanza mientras haya otro nodo despues
            lista.cola = lista.cola->sig; //acerca la cola al final de la lista ordenada
        }
    }
}

double calcularMedianaPuntajes(ListaPuntajes& lista) { //obtiene el valor central despues de ordenar los puntajes
    if (lista.tam == 0) { //una lista vacia no tiene valores centrales
        return 0.0; //usa cero cuando no hay puntajes
    }

    ordenarListaPuntajesMergeSort(lista); //deja la lista recibida ordenada de menor a mayor

    int indiceIzquierdo = (lista.tam - 1) / 2; //posicion del primer valor central contando desde cero
    int indiceDerecho = lista.tam / 2; //coincide con la anterior si la cantidad es impar

    const NodoPuntaje* actual = lista.cabeza; //empieza a leer los puntajes desde el primer nodo
    int indice = 0; //lleva la posicion del nodo que se esta leyendo
    int valorIzquierdo = 0; //guardara el primer valor central
    int valorDerecho = 0; //guardara el segundo valor central

    while (actual != nullptr && indice <= indiceDerecho) { //recorre solo hasta encontrar ambos valores centrales
        if (indice == indiceIzquierdo) { //revisa si llego a la primera posicion central
            valorIzquierdo = actual->puntaje; //guarda el puntaje de esa posicion
        }

        if (indice == indiceDerecho) { //tambien se revisa cuando ambas posiciones coinciden
            valorDerecho = actual->puntaje; //guarda el segundo valor aunque sea el mismo nodo
        }

        actual = actual->sig; //pasa al siguiente puntaje
        indice++; //actualiza la posicion del recorrido
    }

    return (static_cast<double>(valorIzquierdo) + static_cast<double>(valorDerecho)) / 2.0; //convierte antes de sumar para evitar desbordar enteros y conservar decimales
}

void destruirListaPuntajes(ListaPuntajes& lista) { //libera todos los nodos de la lista
    NodoPuntaje* actual = lista.cabeza; //empieza por el primer nodo

    while (actual != nullptr) { //continua mientras queden nodos por borrar
        NodoPuntaje* siguiente = actual->sig; //guarda el siguiente antes de borrar el actual
        delete actual; //libera la memoria del nodo actual
        actual = siguiente; //continua con el nodo que habia guardado
    }

    lista.cabeza = nullptr; //la lista ya no tiene primer nodo
    lista.cola = nullptr; //la lista ya no tiene ultimo nodo
    lista.tam = 0; //no quedan puntajes guardados
}