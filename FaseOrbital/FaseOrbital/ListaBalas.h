#ifndef LISTA_BALAS_H //Evita declarar estas estructuras y funciones mas de una vez
#define LISTA_BALAS_H //Marca esta cabecera como incluida

#include "ModelosCombate.h" //Incluye la estructura que guarda los datos de cada bala

struct NodoBala { //Agrupa una bala y el enlace al siguiente nodo
    Bala dato; //Guarda la posicion velocidad color y duenio de la bala
    NodoBala* sig; //Apunta al siguiente nodo o a nullptr si es el ultimo
};

struct ListaBalas { //Mantiene el inicio y la cantidad de nodos de la lista
    NodoBala* cabeza; //Apunta al primer nodo o a nullptr si la lista esta vacia
    int tam; //Guarda la cantidad de balas almacenadas
};

void inicializarListaBalas(ListaBalas& lista); //Establece los valores iniciales sin liberar nodos previos
void insertarInicio(ListaBalas& lista, const Bala& bala); //Agrega una copia de la bala al principio de la lista
int eliminarBalasSi(ListaBalas& lista, bool (*predicado)(const Bala&)); //Elimina las balas que cumplen la condicion recibida y devuelve su cantidad
int tamanoListaBalas(const ListaBalas& lista); //Devuelve la cantidad de balas almacenadas
bool listaBalasVacia(const ListaBalas& lista); //Indica si la lista tiene cero balas
void destruirListaBalas(ListaBalas& lista); //Libera todos los nodos y deja la lista vacia

#endif