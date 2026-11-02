#include "PruebasListaEnemigos.h"
#include "ListaEnemigos.h"

//crea mil enemigos elimina los identificadores pares y revisa los impares en ambos sentidos
//devuelve falso si el tamano el orden o los enlaces externos de la lista no son los esperados
bool pruebaP01() {
    ListaEnemigos lista; //guarda los enemigos que se usaran en la prueba
    inicializarListaEnemigos(lista); //deja ambos extremos vacios y el tamano en cero

    for (int i = 0; i < 1000; i++) { //crea identificadores consecutivos desde cero hasta novecientos noventa y nueve
        Enemigo enemigo{ i, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 }; //prepara un dron con el identificador de esta vuelta
        insertarFinal(lista, enemigo); //mantiene los enemigos en el orden en que se crean
    }

    NodoEnemigo* actual = lista.cabeza; //empieza a revisar desde el primer enemigo

    while (actual != nullptr) { //recorre la lista sin volver a usar los nodos que elimina
        if (actual->dato.id % 2 == 0) actual = eliminarEnemigo(lista, actual); //borra el identificador par y recibe el siguiente nodo valido
        else actual = actual->sig; //conserva el identificador impar y avanza
    }

    bool resultado = true; //conserva cualquier fallo encontrado durante las comprobaciones

    if (lista.tam != 500) resultado = false; //comprueba que permanezca exactamente la mitad de los enemigos

    int esperado = 1; //el primer identificador que debe quedar es uno
    actual = lista.cabeza; //vuelve al inicio para revisar el orden hacia adelante

    while (actual != nullptr) { //visita los enemigos restantes siguiendo el enlace siguiente
        if (actual->dato.id != esperado) resultado = false; //marca un fallo si el identificador rompe la secuencia de impares
        esperado += 2; //prepara el siguiente identificador impar
        actual = actual->sig; //avanza hacia la cola
    }

    esperado = 999; //el ultimo identificador impar debe quedar en la cola
    actual = lista.cola; //prepara la revision en sentido contrario

    while (actual != nullptr) { //visita los mismos enemigos usando el enlace anterior
        if (actual->dato.id != esperado) resultado = false; //detecta un orden incorrecto al retroceder
        esperado -= 2; //prepara el identificador impar anterior
        actual = actual->ant; //retrocede hacia la cabeza
    }

    if (lista.cabeza != nullptr && lista.cabeza->ant != nullptr) resultado = false; //comprueba que no haya un nodo anterior a la cabeza
    if (lista.cola != nullptr && lista.cola->sig != nullptr) resultado = false; //comprueba que no haya un nodo despues de la cola

    destruirListaEnemigos(lista); //libera los enemigos restantes antes de devolver el resultado

    return resultado; //indica si todas las comprobaciones fueron correctas
}

//elimina primero la cabeza luego la cola y por ultimo el unico nodo que queda
//comprueba los extremos y el tamano despues de cada eliminacion
bool pruebaP02() {
    bool resultado = true; //acumula los fallos de las tres eliminaciones

    ListaEnemigos lista; //contiene los tres nodos de la prueba
    inicializarListaEnemigos(lista); //prepara la lista antes de insertar enemigos

    Enemigo enemigo1{ 1, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 }; //prepara el enemigo que ocupara la cabeza
    Enemigo enemigo2{ 2, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 }; //prepara el enemigo que quedara entre los extremos
    Enemigo enemigo3{ 3, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 }; //prepara el enemigo que ocupara la cola

    insertarFinal(lista, enemigo1); //coloca el primer enemigo en la lista vacia
    insertarFinal(lista, enemigo2); //agrega el segundo enemigo detras del primero
    insertarFinal(lista, enemigo3); //deja el tercer enemigo al final

    eliminarEnemigo(lista, lista.cabeza); //retira el primer enemigo y deja dos nodos

    if (lista.cabeza == nullptr || lista.cabeza->dato.id != 2 || lista.cabeza->ant != nullptr || lista.tam != 2) resultado = false; //revisa la nueva cabeza su enlace anterior y el tamano

    eliminarEnemigo(lista, lista.cola); //quita el tercer enemigo para dejar solo el segundo

    if (lista.cola == nullptr || lista.cola->dato.id != 2 || lista.cola->sig != nullptr || lista.tam != 1) resultado = false; //comprueba que el unico enemigo sea la cola y no tenga siguiente

    eliminarEnemigo(lista, lista.cabeza); //elimina el unico nodo que queda

    if (lista.cabeza != nullptr || lista.cola != nullptr || lista.tam != 0) resultado = false; //comprueba que ambos extremos y el tamano indiquen una lista vacia

    destruirListaEnemigos(lista); //libera cualquier nodo que haya quedado si alguna operacion fallo

    return resultado; //devuelve si las tres eliminaciones dejaron el estado esperado
}

//inserta un tercer enemigo despues del ultimo y revisa los enlaces entre ambos y la nueva cola
bool pruebaP03() {
    ListaEnemigos lista; //guarda la lista donde se probara la insercion
    inicializarListaEnemigos(lista); //deja la lista preparada para agregar los nodos

    Enemigo enemigo1{ 1, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 }; //prepara el primer nodo de la lista
    Enemigo enemigo2{ 2, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 }; //prepara el nodo que sera la cola antes de insertar
    Enemigo enemigo3{ 3, Dron, 0, 300.0, 0.0, 0.0, 1, 6, 0 }; //prepara el enemigo que se insertara despues de la cola

    insertarFinal(lista, enemigo1); //crea la cabeza de la lista
    NodoEnemigo* ultimo = insertarFinal(lista, enemigo2); //guarda la cola actual para comprobar sus enlaces despues
    NodoEnemigo* nuevo = insertarDespues(lista, ultimo, enemigo3); //inserta el tercer enemigo y guarda el nodo creado

    bool resultado = true; //mantiene el resultado de las comprobaciones de insercion

    if (lista.cola != nuevo) resultado = false; //comprueba que el nodo nuevo sea la cola
    if (nuevo == nullptr || nuevo->ant != ultimo || nuevo->sig != nullptr) resultado = false; //revisa el enlace hacia la cola anterior y el final de la lista
    if (ultimo->sig != nuevo) resultado = false; //comprueba que la cola anterior apunte al nodo insertado
    if (lista.tam != 3) resultado = false; //verifica que la insercion haya agregado un solo nodo

    destruirListaEnemigos(lista); //libera los tres nodos antes de terminar la prueba

    return resultado; //indica si la insercion dejo los enlaces y el tamano esperados
}
