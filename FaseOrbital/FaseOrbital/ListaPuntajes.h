#ifndef LISTA_PUNTAJES_H
#define LISTA_PUNTAJES_H

struct NodoPuntaje { //guarda un puntaje y el enlace al siguiente nodo
    int puntaje; //valor que se agrega a la lista
    NodoPuntaje* sig; //siguiente nodo o nullptr si es el ultimo
};

struct ListaPuntajes { //reune los puntajes que se usan para calcular la mediana
    NodoPuntaje* cabeza; //primer nodo de la lista
    NodoPuntaje* cola; //ultimo nodo para agregar puntajes al final
    int tam; //cantidad de puntajes guardados
};

void inicializarListaPuntajes(ListaPuntajes& lista); //prepara una lista sin nodos antes de usarla
void agregarPuntajeFinal(ListaPuntajes& lista, int puntaje); //agrega un puntaje al final de la lista
void ordenarListaPuntajesMergeSort(ListaPuntajes& lista); //ordena de menor a mayor cambiando los enlaces entre nodos
double calcularMedianaPuntajes(ListaPuntajes& lista); //ordena la lista y obtiene la mediana o cero si esta vacia
void destruirListaPuntajes(ListaPuntajes& lista); //borra todos los nodos y deja la lista vacia

#endif