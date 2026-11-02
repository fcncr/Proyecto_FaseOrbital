#include "PruebasListaBalas.h"
#include "ListaBalas.h"

//indica a la eliminacion por condicion si una bala tiene el color que se quiere quitar
static bool eliminarColorUno(const Bala& bala) {
    return bala.color == 1; //selecciona solamente las balas de color uno
}

//mezcla cinco balas y elimina las tres de color uno incluidas las que estan juntas y en la cabeza
//comprueba el conteo los colores restantes y el estado vacio despues de destruir la lista
bool pruebaP04() {
    ListaBalas lista; //contiene las balas que se usaran para probar la eliminacion
    inicializarListaBalas(lista); //deja la cabeza vacia y el tamano en cero

    Bala bala1{ 0.0, 0.0, 1.0, 0.0, 0, 1 }; //prepara una bala de color cero que debe conservarse
    Bala bala2{ 0.0, 0.0, 1.0, 0.0, 1, 1 }; //prepara la primera de dos balas consecutivas que deben eliminarse
    Bala bala3{ 0.0, 0.0, 1.0, 0.0, 1, 1 }; //prepara la otra bala de color uno del par consecutivo
    Bala bala4{ 0.0, 0.0, 1.0, 0.0, 0, 1 }; //prepara la segunda bala que debe quedar en la lista
    Bala bala5{ 0.0, 0.0, 1.0, 0.0, 1, 1 }; //prepara una bala de color uno para probar la eliminacion de la cabeza

    insertarInicio(lista, bala1); //inserta la bala que quedara al final de la lista
    insertarInicio(lista, bala2); //coloca una bala eliminable antes de la primera
    insertarInicio(lista, bala3); //deja juntas las dos balas de color uno
    insertarInicio(lista, bala4); //separa ese par de la futura cabeza
    insertarInicio(lista, bala5); //completa la lista con una cabeza que debe eliminarse

    int eliminadas = eliminarBalasSi(lista, eliminarColorUno); //borra las balas seleccionadas y recibe la cantidad retirada
    bool resultado = true; //conserva cualquier fallo que detecten las comprobaciones

    if (eliminadas != 3) resultado = false; //comprueba que se hayan eliminado las tres balas de color uno
    if (lista.tam != 2) resultado = false; //revisa que permanezcan dos balas

    NodoBala* actual = lista.cabeza; //empieza a revisar las balas que quedaron

    while (actual != nullptr) { //recorre los nodos que sobrevivieron al filtro
        if (actual->dato.color == 1) resultado = false; //detecta si quedo una bala que debia eliminarse
        actual = actual->sig; //avanza a la siguiente bala
    }

    destruirListaBalas(lista); //libera las balas restantes y restablece la lista

    if (lista.cabeza != nullptr || lista.tam != 0) resultado = false; //comprueba que la destruccion haya dejado la lista vacia

    return resultado; //devuelve si la eliminacion y el vaciado final fueron correctos
}
