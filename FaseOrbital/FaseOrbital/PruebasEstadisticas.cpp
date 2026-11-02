#include "PruebasEstadisticas.h"
#include "ListaPuntajes.h"
#include "Estadisticas.h"
#include "Ranking.h"
#include "Motor.h"
#include "Partida.h"

#include <cmath>
#include <sstream>

//revisa cada par de puntajes vecinos y acepta listas vacias o con un solo nodo
static bool listaOrdenadaAscendente(const ListaPuntajes& lista) {
    const NodoPuntaje* actual = lista.cabeza; //empieza a consultar los puntajes desde la cabeza

    while (actual != nullptr && actual->sig != nullptr) { //avanza mientras exista otro puntaje con el que comparar
        if (actual->puntaje > actual->sig->puntaje) { //detecta un par de vecinos que rompe el orden ascendente
            return false; //informa que los puntajes no estan ordenados
        }

        actual = actual->sig; //continua con el siguiente puntaje
    }

    return true; //confirma que no encontro ningun par fuera de orden
}

//cuenta cuantas veces aparece un puntaje para comprobar que el ordenamiento conserve los repetidos
static int contarValorPuntaje(const ListaPuntajes& lista, int valor) {
    int cantidad = 0; //inicia el conteo de coincidencias
    const NodoPuntaje* actual = lista.cabeza; //empieza a consultar los puntajes desde la cabeza

    while (actual != nullptr) { //revisa cada nodo de la lista
        if (actual->puntaje == valor) { //selecciona los nodos con el puntaje buscado
            cantidad++; //cuenta una aparicion mas del mismo puntaje
        }

        actual = actual->sig; //continua con el siguiente puntaje
    }

    return cantidad; //entrega el total de apariciones encontradas
}

//comprueba el ordenamiento de listas vacias cortas ordenadas inversas y con repetidos
//revisa tambien la mediana con cero uno y cantidades pares e impares de puntajes
bool pruebaP12() {
    bool resultado = true; //conserva cualquier fallo sin detener las demas comprobaciones

    ListaPuntajes vacia; //prepara el caso de una lista sin puntajes
    inicializarListaPuntajes(vacia); //deja la lista vacia para el caso sin puntajes
    ordenarListaPuntajesMergeSort(vacia); //ordena los nodos de este caso mediante mezcla

    if (vacia.cabeza != nullptr || vacia.cola != nullptr || vacia.tam != 0) { //comprueba que ordenar una lista vacia conserve sus extremos y su tamano
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (calcularMedianaPuntajes(vacia) != 0.0) { //exige una mediana de cero cuando no hay puntajes
        resultado = false; //marca el fallo de esta comprobacion
    }

    destruirListaPuntajes(vacia); //libera los nodos usados en el caso sin puntajes

    ListaPuntajes unNodo; //guarda el caso con un unico puntaje
    inicializarListaPuntajes(unNodo); //deja la lista vacia para el caso con un solo puntaje
    agregarPuntajeFinal(unNodo, 500); //agrega el puntaje 500 al final de la lista del caso
    ordenarListaPuntajesMergeSort(unNodo); //ordena los nodos de este caso mediante mezcla

    if (unNodo.cabeza == nullptr || unNodo.cabeza != unNodo.cola || unNodo.cabeza->puntaje != 500 || unNodo.tam != 1) { //revisa que el unico nodo conserve su puntaje y ocupe ambos extremos
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (calcularMedianaPuntajes(unNodo) != 500.0) { //comprueba que la mediana sea el unico puntaje disponible
        resultado = false; //marca el fallo de esta comprobacion
    }

    destruirListaPuntajes(unNodo); //libera los nodos usados en el caso con un solo puntaje

    ListaPuntajes dosNodos; //guarda dos puntajes que se insertaran fuera de orden
    inicializarListaPuntajes(dosNodos); //deja la lista vacia para el caso con dos puntajes
    agregarPuntajeFinal(dosNodos, 900); //agrega el puntaje 900 al final de la lista del caso
    agregarPuntajeFinal(dosNodos, 300); //agrega el puntaje 300 al final de la lista del caso
    ordenarListaPuntajesMergeSort(dosNodos); //ordena los nodos de este caso mediante mezcla

    if (!listaOrdenadaAscendente(dosNodos) || dosNodos.cabeza == nullptr || dosNodos.cola == nullptr) { //revisa que ambos extremos existan y los dos puntajes queden ordenados
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (dosNodos.cabeza != nullptr && dosNodos.cabeza->puntaje != 300) { //comprueba que el menor puntaje quede en la cabeza
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (dosNodos.cola != nullptr && dosNodos.cola->puntaje != 900) { //comprueba que el mayor puntaje quede en la cola
        resultado = false; //marca el fallo de esta comprobacion
    }

    destruirListaPuntajes(dosNodos); //libera los nodos usados en el caso con dos puntajes

    ListaPuntajes ordenada; //prepara una lista que ya estara ordenada
    inicializarListaPuntajes(ordenada); //deja la lista vacia para el caso que ya tiene orden ascendente
    agregarPuntajeFinal(ordenada, 100); //agrega el puntaje 100 al final de la lista del caso
    agregarPuntajeFinal(ordenada, 200); //agrega el puntaje 200 al final de la lista del caso
    agregarPuntajeFinal(ordenada, 300); //agrega el puntaje 300 al final de la lista del caso
    agregarPuntajeFinal(ordenada, 400); //agrega el puntaje 400 al final de la lista del caso
    ordenarListaPuntajesMergeSort(ordenada); //ordena los nodos de este caso mediante mezcla

    if (!listaOrdenadaAscendente(ordenada) || ordenada.tam != 4) { //verifica que la lista ya ordenada conserve el orden y sus cuatro nodos
        resultado = false; //marca el fallo de esta comprobacion
    }

    destruirListaPuntajes(ordenada); //libera los nodos usados en el caso que ya tiene orden ascendente

    ListaPuntajes inversa; //prepara puntajes en orden descendente
    inicializarListaPuntajes(inversa); //deja la lista vacia para el caso que comienza en orden descendente
    agregarPuntajeFinal(inversa, 500); //agrega el puntaje 500 al final de la lista del caso
    agregarPuntajeFinal(inversa, 400); //agrega el puntaje 400 al final de la lista del caso
    agregarPuntajeFinal(inversa, 300); //agrega el puntaje 300 al final de la lista del caso
    agregarPuntajeFinal(inversa, 200); //agrega el puntaje 200 al final de la lista del caso
    agregarPuntajeFinal(inversa, 100); //agrega el puntaje 100 al final de la lista del caso
    ordenarListaPuntajesMergeSort(inversa); //ordena los nodos de este caso mediante mezcla

    if (!listaOrdenadaAscendente(inversa) || inversa.cabeza == nullptr || inversa.cola == nullptr) { //comprueba que los puntajes inversos queden en orden ascendente con ambos extremos
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (inversa.cabeza != nullptr && inversa.cabeza->puntaje != 100) { //revisa que el menor puntaje pase al inicio
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (inversa.cola != nullptr && inversa.cola->puntaje != 500) { //revisa que el mayor puntaje pase al final
        resultado = false; //marca el fallo de esta comprobacion
    }

    destruirListaPuntajes(inversa); //libera los nodos usados en el caso que comienza en orden descendente

    ListaPuntajes repetidos; //guarda puntajes repetidos para comprobar que no se pierdan
    inicializarListaPuntajes(repetidos); //deja la lista vacia para el caso con puntajes repetidos
    agregarPuntajeFinal(repetidos, 500); //agrega el puntaje 500 al final de la lista del caso
    agregarPuntajeFinal(repetidos, 200); //agrega el puntaje 200 al final de la lista del caso
    agregarPuntajeFinal(repetidos, 500); //agrega el puntaje 500 al final de la lista del caso
    agregarPuntajeFinal(repetidos, 100); //agrega el puntaje 100 al final de la lista del caso
    agregarPuntajeFinal(repetidos, 500); //agrega el puntaje 500 al final de la lista del caso
    ordenarListaPuntajesMergeSort(repetidos); //ordena los nodos de este caso mediante mezcla

    if (!listaOrdenadaAscendente(repetidos) || repetidos.tam != 5) { //comprueba que ordenar los repetidos conserve cinco nodos en orden
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (contarValorPuntaje(repetidos, 500) != 3) { //exige conservar las tres apariciones del puntaje quinientos
        resultado = false; //marca el fallo de esta comprobacion
    }

    destruirListaPuntajes(repetidos); //libera los nodos usados en el caso con puntajes repetidos

    ListaPuntajes impar; //prepara cinco puntajes para revisar la mediana impar
    inicializarListaPuntajes(impar); //deja la lista vacia para el caso con cinco puntajes
    agregarPuntajeFinal(impar, 9); //agrega el puntaje 9 al final de la lista del caso
    agregarPuntajeFinal(impar, 1); //agrega el puntaje 1 al final de la lista del caso
    agregarPuntajeFinal(impar, 5); //agrega el puntaje 5 al final de la lista del caso
    agregarPuntajeFinal(impar, 3); //agrega el puntaje 3 al final de la lista del caso
    agregarPuntajeFinal(impar, 7); //agrega el puntaje 7 al final de la lista del caso

    if (calcularMedianaPuntajes(impar) != 5.0) { //revisa el puntaje central de la lista con cinco elementos
        resultado = false; //marca el fallo de esta comprobacion
    }

    destruirListaPuntajes(impar); //libera los nodos usados en el caso con cinco puntajes

    ListaPuntajes par; //prepara cuatro puntajes para revisar la mediana par
    inicializarListaPuntajes(par); //deja la lista vacia para el caso con cuatro puntajes
    agregarPuntajeFinal(par, 7); //agrega el puntaje 7 al final de la lista del caso
    agregarPuntajeFinal(par, 1); //agrega el puntaje 1 al final de la lista del caso
    agregarPuntajeFinal(par, 5); //agrega el puntaje 5 al final de la lista del caso
    agregarPuntajeFinal(par, 3); //agrega el puntaje 3 al final de la lista del caso

    if (calcularMedianaPuntajes(par) != 4.0) { //comprueba el promedio de los dos puntajes centrales
        resultado = false; //marca el fallo de esta comprobacion
    }

    destruirListaPuntajes(par); //libera los nodos usados en el caso con cuatro puntajes

    return resultado; //devuelve si todas las comprobaciones del caso fueron correctas
}

//prepara un resultado de prueba con los datos recibidos y deja en cero las metricas que no se usan
static RegistroPartida crearRegistroEstadisticas(int idPartida, const string& jugador, int posicion, int puntaje, int correctos, int erroneos) {
    RegistroPartida registro{}; //inicia los numeros en cero las banderas en falso y los textos vacios

    registro.idPartida = idPartida; //identifica la partida del resultado
    registro.semilla = 42u; //usa la misma semilla en los registros de prueba
    registro.modo = "lotes"; //identifica el modo utilizado por estos datos de prueba
    registro.jugador = jugador; //copia el alias del participante
    registro.posicion = posicion; //guarda el puesto final indicado
    registro.puntaje = puntaje; //asigna los puntos que se usaran en los calculos
    registro.oleada = 3; //fija la oleada alcanzada en los registros de prueba
    registro.disparos = correctos + erroneos; //hace coincidir los disparos con la suma de impactos del escenario
    registro.correctos = correctos; //guarda la cantidad de impactos del color correcto
    registro.erroneos = erroneos; //guarda la cantidad de impactos del color incorrecto
    registro.ticks = 100; //fija el tiempo jugado para completar el registro

    return registro; //entrega los datos preparados para agregarlos al historial
}

//considera iguales dos resultados cuando su diferencia absoluta es menor que una millonesima
static bool casiIguales(double a, double b) {
    return fabs(a - b) < 0.000001; //compara la diferencia sin exigir igualdad exacta de decimales
}

//comprueba las estadisticas por jugador el histograma y los resultados con y sin detalle
//incluye un alias inexistente y puntajes iguales para revisar los casos sin datos o sin dispersion
bool pruebaEstadisticasHistoricas() {
    HistorialPartidas historial; //guarda las partidas del jugador consultado y de otro alias
    inicializarHistorial(historial); //prepara el historial sin registros

    agregarRegistroFinal(historial, crearRegistroEstadisticas(1, "ANA", 1, 100, 1, 1)); //agrega una victoria con la mitad de los impactos correctos
    agregarRegistroFinal(historial, crearRegistroEstadisticas(2, "ANA", 2, 200, 2, 0)); //agrega un segundo puesto con todos los impactos correctos
    agregarRegistroFinal(historial, crearRegistroEstadisticas(3, "ANA", 1, 300, 3, 3)); //agrega otra victoria con la mitad de los impactos correctos
    agregarRegistroFinal(historial, crearRegistroEstadisticas(4, "ANA", 3, 400, 0, 0)); //incluye una partida sin impactos que debe aportar precision cero
    agregarRegistroFinal(historial, crearRegistroEstadisticas(5, "OTRO", 1, 999, 10, 0)); //agrega datos ajenos que no deben entrar en el resumen del jugador consultado

    bool resultado = true; //conserva cualquier fallo sin detener las demas comprobaciones

    EstadisticasJugador estadisticas; //recibe los resultados calculados a partir del historial

    if (!calcularEstadisticasJugador(historial, "ANA", estadisticas)) { //comprueba que se encuentren las partidas del alias solicitado
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (estadisticas.alias != "ANA") { //revisa que el resultado corresponda al jugador consultado
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (estadisticas.partidasJugadas != 4) { //exige contar solamente las cuatro partidas del mismo alias
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (estadisticas.victorias != 2) { //comprueba que los dos primeros puestos cuenten como victorias
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (estadisticas.mejorPuntaje != 400) { //revisa el mayor puntaje del jugador
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (estadisticas.peorPuntaje != 100) { //revisa el menor puntaje del jugador
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (!casiIguales(estadisticas.mediaPuntaje, 250.0)) { //comprueba el promedio de sus cuatro puntajes
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (!casiIguales(estadisticas.medianaPuntaje, 250.0)) { //revisa el promedio de los dos valores centrales del historial ordenado
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (!casiIguales(estadisticas.desviacionEstandar, 111.80339887498948)) { //comprueba la desviacion poblacional esperada para los cuatro puntajes
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (!casiIguales(estadisticas.precisionColorPromedio, 0.5)) { //revisa el promedio de las precisiones por partida incluida la partida sin impactos
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (estadisticas.mejorColorDisponible || estadisticas.mejorColor != -1) { //exige que el mejor color quede sin definir cuando no hay detalle
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (estadisticas.tipoMasDanoDisponible || estadisticas.tipoMasDano != -1) { //comprueba que no se atribuya el dano a un tipo sin datos detallados
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (!casiIguales(estadisticas.histograma.minimo, 100.0)) { //revisa el limite inferior del histograma
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (!casiIguales(estadisticas.histograma.maximo, 400.0)) { //revisa el limite superior del histograma
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (!casiIguales(estadisticas.histograma.anchoIntervalo, 30.0)) { //comprueba que los diez intervalos repartan el rango de puntajes
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (estadisticas.histograma.frecuencias[0] != 1) { //ubica el puntaje minimo en el primer intervalo
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (estadisticas.histograma.frecuencias[3] != 1) { //comprueba el intervalo donde cae el puntaje doscientos
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (estadisticas.histograma.frecuencias[6] != 1) { //comprueba el intervalo donde cae el puntaje trescientos
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (estadisticas.histograma.frecuencias[9] != 1) { //exige incluir el puntaje maximo en el ultimo intervalo
        resultado = false; //marca el fallo de esta comprobacion
    }

    EstadisticasJugador inexistente; //recibe el resultado de consultar un alias ausente

    if (calcularEstadisticasJugador(historial, "NO_EXISTE", inexistente)) { //detecta si se acepta calcular estadisticas para un jugador sin registros
        resultado = false; //marca el fallo de esta comprobacion
    }

    HistorialPartidas historialDetalle; //guarda las partidas con impactos por color y dano por enemigo
    inicializarHistorial(historialDetalle); //prepara el historial de los casos con detalle

    RegistroPartida detalle1 = crearRegistroEstadisticas(20, "DETALLE", 1, 700, 22, 8); //crea el primer resultado al que se agregara el desglose por color y enemigo

    detalle1.detalleDisponible = true; //habilita el uso del detalle de la primera partida

    detalle1.correctosC = 8; //registra ocho impactos correctos de cian
    detalle1.erroneosC = 2; //agrega dos impactos erroneos de cian

    detalle1.correctosM = 5; //registra cinco impactos correctos de magenta
    detalle1.erroneosM = 5; //deja la precision magenta en la mitad de sus impactos

    detalle1.correctosA = 9; //registra nueve impactos correctos de ambar
    detalle1.erroneosA = 1; //agrega un impacto erroneo de ambar

    detalle1.danoDron = 10; //asigna al dron diez puntos de dano en la primera partida
    detalle1.danoTanque = 22; //asigna al tanque el mayor dano de la primera partida

    RegistroPartida detalle2 = crearRegistroEstadisticas(21, "DETALLE", 2, 500, 9, 3); //crea otra partida del mismo alias para comprobar la acumulacion del detalle

    detalle2.detalleDisponible = true; //habilita el detalle de la segunda partida

    detalle2.correctosC = 2; //agrega dos aciertos cian al historial detallado
    detalle2.erroneosC = 0; //deja esta partida sin errores de cian

    detalle2.correctosM = 1; //agrega un acierto magenta
    detalle2.erroneosM = 1; //agrega un error magenta para mantener su precision en la mitad

    detalle2.correctosA = 5; //agrega cinco aciertos ambar
    detalle2.erroneosA = 0; //deja esta partida sin errores de ambar

    detalle2.danoDron = 10; //suma otros diez puntos de dano del dron
    detalle2.danoTanque = 12; //agrega doce puntos mas al dano del tanque

    agregarRegistroFinal(historialDetalle, detalle1); //incorpora la primera partida con detalle
    agregarRegistroFinal(historialDetalle, detalle2); //incorpora la segunda partida del mismo jugador

    EstadisticasJugador estadisticasDetalle; //recibe los calculos que incluyen los desgloses

    if (!calcularEstadisticasJugador(historialDetalle, "DETALLE", estadisticasDetalle)) { //comprueba que se puedan calcular las estadisticas del historial detallado
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (!estadisticasDetalle.mejorColorDisponible) { //exige que los impactos por color permitan elegir una fase
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (estadisticasDetalle.mejorColor != 2) { //comprueba que ambar tenga la mayor precision acumulada
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (!estadisticasDetalle.tipoMasDanoDisponible) { //revisa que el dano positivo permita identificar un tipo de enemigo
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (estadisticasDetalle.tipoMasDano != static_cast<int>(Tanque)) { //comprueba que el tanque acumule el mayor dano entre ambas partidas
        resultado = false; //marca el fallo de esta comprobacion
    }

    destruirHistorial(historialDetalle); //libera los registros con detalle despues de consultarlos

    HistorialPartidas repetidos; //guarda tres partidas con el mismo puntaje
    inicializarHistorial(repetidos); //prepara el historial del caso sin dispersion

    agregarRegistroFinal(repetidos, crearRegistroEstadisticas(10, "IGUAL", 1, 500, 0, 0)); //agrega el primer resultado de quinientos puntos sin impactos
    agregarRegistroFinal(repetidos, crearRegistroEstadisticas(11, "IGUAL", 2, 500, 0, 0)); //repite el puntaje con un puesto final distinto
    agregarRegistroFinal(repetidos, crearRegistroEstadisticas(12, "IGUAL", 3, 500, 0, 0)); //completa los tres resultados iguales

    EstadisticasJugador iguales; //recibe las estadisticas del historial sin variacion de puntajes

    if (!calcularEstadisticasJugador(repetidos, "IGUAL", iguales)) { //comprueba que se puedan resumir los puntajes repetidos
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (!casiIguales(iguales.mediaPuntaje, 500.0) || !casiIguales(iguales.medianaPuntaje, 500.0) || !casiIguales(iguales.desviacionEstandar, 0.0)) { //exige media y mediana de quinientos y desviacion cero
        resultado = false; //marca el fallo de esta comprobacion
    }

    if (iguales.histograma.frecuencias[0] != 3) { //comprueba que los tres puntajes iguales queden en el primer intervalo
        resultado = false; //marca el fallo de esta comprobacion
    }

    for (int i = 1; i < 10; i++) { //revisa los otros nueve intervalos del histograma
        if (iguales.histograma.frecuencias[i] != 0) { //detecta si se asignaron puntajes a un intervalo que deberia estar vacio
            resultado = false; //marca el fallo de esta comprobacion
        }
    }

    ostringstream salida; //recoge el histograma impreso para comprobar que haya salida
    imprimirHistogramaASCII(estadisticas.histograma, salida); //escribe en memoria el histograma del primer escenario

    if (salida.str().empty()) { //detecta si el reporte no produjo ningun texto
        resultado = false; //marca el fallo de esta comprobacion
    }

    destruirHistorial(repetidos); //libera los registros del caso de puntajes iguales
    destruirHistorial(historial); //libera las partidas del primer escenario

    return resultado; //devuelve si todas las comprobaciones del caso fueron correctas
}

//comprueba el orden por puntaje precision y alias el reemplazo de jugadores y la construccion desde el historial
bool pruebaRanking() {
    Ranking ranking; //guarda la clasificacion que se usara en la prueba
    inicializarRanking(ranking); //prepara una lista sin jugadores

    bool resultado = true; //conserva cualquier fallo sin detener las comprobaciones siguientes

    if (ranking.cabeza != nullptr || ranking.tam != 0) { //comprueba que la inicializacion deje el ranking vacio
        resultado = false; //marca el estado inicial incorrecto
    }

    RegistroRanking luis{ "LUIS", 500, 0.60 }; //prepara un jugador con puntaje intermedio y menor precision que sus rivales empatados
    RegistroRanking ana{ "ANA", 700, 0.50 }; //prepara el mayor puntaje de la primera clasificacion
    RegistroRanking mario{ "MARIO", 300, 0.90 }; //prepara el menor puntaje aunque su precision sea mayor
    RegistroRanking bea{ "BEA", 500, 0.80 }; //iguala el puntaje intermedio con una precision superior
    RegistroRanking alan{ "ALAN", 500, 0.80 }; //iguala puntaje y precision para probar el desempate por alias

    insertarOrdenadoRanking(ranking, luis); //inserta el primer jugador en el ranking vacio

    if (ranking.cabeza == nullptr || ranking.cabeza->dato.alias != "LUIS" || ranking.tam != 1) { //revisa el jugador inicial y la cantidad de entradas
        resultado = false; //marca un fallo en la primera insercion
    }

    insertarOrdenadoRanking(ranking, ana); //agrega un puntaje que debe quedar antes del primero
    insertarOrdenadoRanking(ranking, mario); //agrega el puntaje que debe quedar al final
    insertarOrdenadoRanking(ranking, bea); //inserta una jugadora que debe superar a su rival por precision
    insertarOrdenadoRanking(ranking, alan); //inserta el alias que debe ganar el empate completo

    const RegistroRanking* posicion1 = obtenerPosicionRanking(ranking, 1); //consulta el primer puesto sin copiar el registro
    const RegistroRanking* posicion2 = obtenerPosicionRanking(ranking, 2); //consulta el segundo puesto
    const RegistroRanking* posicion3 = obtenerPosicionRanking(ranking, 3); //consulta el tercer puesto
    const RegistroRanking* posicion4 = obtenerPosicionRanking(ranking, 4); //consulta el cuarto puesto
    const RegistroRanking* posicion5 = obtenerPosicionRanking(ranking, 5); //consulta el ultimo puesto de la lista

    if (posicion1 == nullptr || posicion1->alias != "ANA") { //comprueba que el mayor puntaje ocupe el primer lugar
        resultado = false; //marca un orden incorrecto por puntaje
    }

    if (posicion2 == nullptr || posicion2->alias != "ALAN") { //revisa que el orden del alias resuelva el empate de puntaje y precision
        resultado = false; //marca un desempate incorrecto por alias
    }

    if (posicion3 == nullptr || posicion3->alias != "BEA") { //comprueba el puesto de la otra jugadora con igual puntaje y precision
        resultado = false; //marca un tercer puesto incorrecto
    }

    if (posicion4 == nullptr || posicion4->alias != "LUIS") { //revisa que la menor precision lo deje detras de sus rivales con el mismo puntaje
        resultado = false; //marca un orden incorrecto por precision
    }

    if (posicion5 == nullptr || posicion5->alias != "MARIO") { //comprueba que la precision no compense un puntaje menor
        resultado = false; //marca una prioridad incorrecta entre puntaje y precision
    }

    RegistroRanking marioActualizado{ "MARIO", 800, 0.40 }; //prepara una mejora que debe mover al ultimo jugador al primer puesto
    insertarOrdenadoRanking(ranking, marioActualizado); //reemplaza la entrada del mismo alias y vuelve a ubicarla

    if (ranking.tam != 5) { //detecta si actualizar un alias creo una entrada adicional
        resultado = false; //marca una cantidad incorrecta tras el reemplazo
    }

    posicion1 = obtenerPosicionRanking(ranking, 1); //vuelve a consultar el primer puesto despues del cambio

    if (posicion1 == nullptr || posicion1->alias != "MARIO" || posicion1->mejorPuntaje != 800) { //comprueba que el registro actualizado encabece la lista con su nuevo puntaje
        resultado = false; //marca un reemplazo o una ubicacion incorrectos
    }

    const RegistroRanking* marioBuscado = buscarJugadorRanking(ranking, "MARIO"); //busca por alias la entrada que se acaba de reemplazar

    if (marioBuscado == nullptr || marioBuscado->mejorPuntaje != 800) { //revisa que la busqueda entregue el puntaje actualizado
        resultado = false; //marca un resultado de busqueda incorrecto
    }

    if (buscarJugadorRanking(ranking, "NO_EXISTE") != nullptr) { //comprueba que un alias ausente no devuelva un registro
        resultado = false; //marca una coincidencia que no existe
    }

    if (obtenerPosicionRanking(ranking, 0) != nullptr) { //revisa que los puestos comiencen en uno
        resultado = false; //marca la aceptacion de una posicion invalida
    }

    if (obtenerPosicionRanking(ranking, 100) != nullptr) { //consulta una posicion que supera la cantidad de jugadores
        resultado = false; //marca una posicion inexistente como fallo
    }

    destruirRanking(ranking); //libera las entradas de la primera clasificacion

    if (ranking.cabeza != nullptr || ranking.tam != 0) { //comprueba que la destruccion restablezca el estado vacio
        resultado = false; //marca un vaciado incompleto
    }

    HistorialPartidas historial; //guarda las partidas usadas para construir otro ranking
    inicializarHistorial(historial); //prepara la lista de resultados historicos

    agregarRegistroFinal(historial, crearRegistroEstadisticas(1, "ANA", 1, 100, 1, 0)); //agrega una partida de precision completa para el primer alias
    agregarRegistroFinal(historial, crearRegistroEstadisticas(2, "ANA", 2, 900, 1, 1)); //agrega su mejor puntaje con precision menor
    agregarRegistroFinal(historial, crearRegistroEstadisticas(3, "BOB", 1, 900, 4, 1)); //iguala el mejor puntaje anterior con mayor precision promedio
    agregarRegistroFinal(historial, crearRegistroEstadisticas(4, "CARLOS", 2, 700, 3, 1)); //agrega un tercer alias con menor puntaje

    inicializarRanking(ranking); //prepara el ranking vaciado para la construccion desde el historial
    construirRankingDesdeHistorial(historial, ranking); //reune el mejor puntaje y la precision promedio de cada alias

    if (ranking.tam != 3) { //comprueba que las dos partidas del mismo alias generen una sola entrada
        resultado = false; //marca una cantidad incorrecta de jugadores distintos
    }

    posicion1 = obtenerPosicionRanking(ranking, 1); //consulta al mejor jugador del historial
    posicion2 = obtenerPosicionRanking(ranking, 2); //consulta el segundo puesto reconstruido
    posicion3 = obtenerPosicionRanking(ranking, 3); //consulta el ultimo de los tres alias

    if (posicion1 == nullptr || posicion1->alias != "BOB") { //revisa que la precision promedio resuelva el empate de mejor puntaje
        resultado = false; //marca un ganador historico incorrecto
    }

    if (posicion2 == nullptr || posicion2->alias != "ANA") { //comprueba el puesto del alias que acumula dos partidas
        resultado = false; //marca una agregacion o un orden incorrectos
    }

    if (posicion3 == nullptr || posicion3->alias != "CARLOS") { //revisa que el menor puntaje quede en tercer lugar
        resultado = false; //marca una tercera posicion incorrecta
    }

    destruirRanking(ranking); //libera las entradas construidas desde el historial
    destruirHistorial(historial); //libera los registros usados como entrada

    return resultado; //informa si se cumplieron todas las comprobaciones del ranking
}

//comprueba las bajas el dano y los puntos por oleada separando jugadores y acumulando las oleadas anteriores
//revisa tambien que la salida de texto incluya ambas oleadas y el puntaje acumulado esperado
bool pruebaLineaTiempo() {
    BitacoraEventos bitacora; //guarda eventos de dos jugadores y dos oleadas
    inicializarBitacora(bitacora); //prepara la lista vacia antes de agregar los eventos

    Evento evento1{ 10, 1, "ANA", BAJA, 0, 0, 100 }; //prepara la primera baja de ana con cien puntos
    Evento evento2{ 20, 1, "ANA", BAJA, 1, 1, 150 }; //prepara otra baja de ana en la misma oleada
    Evento evento3{ 30, 1, "ANA", DANO_NUCLEO, 0, 0, 10 }; //agrega dano al nucleo que no debe aumentar los puntos
    Evento evento4{ 40, 1, "BOB", BAJA, 0, 0, 500 }; //prepara una baja ajena que no debe sumarse al resumen de ana
    Evento evento5{ 50, 2, "ANA", BAJA, 4, 2, 400 }; //prepara la baja de ana en la segunda oleada
    Evento evento6{ 60, 2, "ANA", CHOQUE_NAVE, 4, 2, 12 }; //incluye dano por choque con la nave en la segunda oleada
    Evento evento7{ 70, 2, "ANA", FIN_OLEADA, -1, -1, 0 }; //cierra la segunda oleada sin sumar bajas ni puntos

    agregarEventoFinal(bitacora, evento1); //registra la primera baja
    agregarEventoFinal(bitacora, evento2); //registra la segunda baja de la primera oleada
    agregarEventoFinal(bitacora, evento3); //registra el dano al nucleo
    agregarEventoFinal(bitacora, evento4); //intercala el evento del otro jugador
    agregarEventoFinal(bitacora, evento5); //registra la baja de la segunda oleada
    agregarEventoFinal(bitacora, evento6); //registra el choque con la nave
    agregarEventoFinal(bitacora, evento7); //agrega el cierre de oleada al final

    bool resultado = true; //acumula los fallos de los resumenes y de la salida

    ResumenOleadaJugador ronda1; //recibe el resumen de ana para la primera oleada

    if (!calcularResumenOleadaJugador(bitacora, "ANA", 1, ronda1)) { //comprueba que se encuentren eventos de ana en la primera oleada
        resultado = false; //marca la ausencia de actividad esperada
    }

    if (ronda1.oleada != 1) { //revisa que el resumen conserve la oleada consultada
        resultado = false; //marca un numero de oleada incorrecto
    }

    if (ronda1.bajas != 2) { //comprueba que solo se cuenten las dos bajas de ana
        resultado = false; //marca un conteo incorrecto de bajas
    }

    if (ronda1.danoRecibido != 10) { //revisa el dano del evento de contacto con el nucleo
        resultado = false; //marca una suma incorrecta de dano
    }

    if (ronda1.puntajeOleada != 250) { //comprueba la suma de los dos premios de la primera oleada
        resultado = false; //marca un puntaje de oleada incorrecto
    }

    if (ronda1.puntajeAcumulado != 250) { //revisa que el acumulado inicial coincida con los puntos de esa oleada
        resultado = false; //marca un acumulado incorrecto
    }

    ResumenOleadaJugador ronda2; //recibe el resumen de ana para la segunda oleada

    if (!calcularResumenOleadaJugador(bitacora, "ANA", 2, ronda2)) { //comprueba que se encuentren los eventos de la segunda oleada
        resultado = false; //marca la ausencia del segundo resumen
    }

    if (ronda2.bajas != 1) { //revisa que el resumen cuente solamente la baja de esa oleada
        resultado = false; //marca una cantidad de bajas incorrecta
    }

    if (ronda2.danoRecibido != 12) { //comprueba que el choque con la nave se incluya como dano recibido
        resultado = false; //marca el dano del choque como incorrecto
    }

    if (ronda2.puntajeOleada != 400) { //revisa el premio exclusivo de la segunda oleada
        resultado = false; //marca un puntaje de oleada incorrecto
    }

    if (ronda2.puntajeAcumulado != 650) { //comprueba que se sumen los puntos de ambas oleadas de ana
        resultado = false; //marca un acumulado incorrecto al cerrar la segunda oleada
    }

    ResumenOleadaJugador bob; //recibe los datos del otro jugador para comprobar el filtro por alias

    if (!calcularResumenOleadaJugador(bitacora, "BOB", 1, bob)) { //consulta por separado los eventos de bob
        resultado = false; //marca la ausencia del resumen del segundo jugador
    }

    if (bob.bajas != 1 || bob.puntajeAcumulado != 500) { //comprueba que bob conserve solo su baja y sus puntos
        resultado = false; //marca una mezcla de datos entre jugadores
    }

    ostringstream salida; //recoge el reporte en memoria para revisar su texto
    imprimirLineaTiempoJugador(bitacora, "ANA", 2, salida); //escribe la linea de tiempo de ana hasta la segunda oleada

    string texto = salida.str(); //obtiene el contenido generado por el reporte

    if (texto.find("Oleada 1") == string::npos) { //busca el encabezado de la primera oleada
        resultado = false; //marca la falta de la primera oleada en el texto
    }

    if (texto.find("Oleada 2") == string::npos) { //busca el encabezado de la segunda oleada
        resultado = false; //marca la falta de la segunda oleada en el texto
    }

    if (texto.find("Puntaje acumulado: 650") == string::npos) { //revisa que el reporte incluya el acumulado final esperado
        resultado = false; //marca un acumulado ausente o distinto en la salida
    }

    destruirBitacora(bitacora); //libera los siete eventos de la prueba

    return resultado; //devuelve si los resumenes y los textos comprobados fueron correctos
}

//provoca un disparo un fallo un acierto un pulso y la perdida del nucleo para revisar las metricas y los eventos
//copia los resultados al historial y comprueba que conserven su detalle despues de destruir la partida
bool pruebaIntegracionEstadisticasMotor() {
    Partida partida; //guarda al jugador y la bitacora que compartira el motor
    inicializarPartida(partida, 42u, 1); //prepara la partida con una oleada y semilla fija

    if (!agregarJugadorPartida(partida, "ANA", false)) { //comprueba que se pueda registrar al participante de la prueba
        destruirPartida(partida); //libera cualquier dato creado si falla el registro
        return false; //termina porque no hay jugador con el que preparar el escenario
    }

    Jugador* jugador = verJugadorPartida(partida, 0); //consulta el participante que conserva la partida

    if (jugador == nullptr) { //evita preparar el escenario si no se puede recuperar al jugador
        destruirPartida(partida); //libera la partida incompleta
        return false; //informa que no se pudo iniciar la comprobacion
    }

    jugador->oleadaAlcanzada = 1; //situa al jugador en la primera oleada
    jugador->metricas.oleadaAlcanzada = 1; //deja la misma oleada en sus metricas

    ColaSpawn cola; //guarda una aparicion que mantendra abierto el turno
    inicializarColaSpawn(cola); //prepara la cola original del escenario

    Spawn pendiente; //define la aparicion que no debe ocurrir durante estas comprobaciones
    pendiente.tick = 1000; //reserva al enemigo para un tick lejano
    pendiente.tipo = Dron; //usa un dron como aparicion pendiente
    pendiente.color = 0; //asigna la fase cian al enemigo futuro
    pendiente.angulo = 0; //define su angulo de entrada
    pendiente.sentido = 0; //deja sin giro el dato de sentido de esta aparicion

    encolarSpawn(cola, pendiente); //agrega la aparicion que evitara el fin anticipado de la oleada

    Motor motor; //guarda la simulacion usada para generar las metricas
    inicializarMotor(motor, jugador, 1, cola, &partida.bitacora); //copia las apariciones y vincula el jugador y la bitacora externos

    AccionMotor sinAccion; //prepara un tick sin solicitudes de los controles
    sinAccion.cambiarAngulo = false; //mantiene el destino de la nave
    sinAccion.anguloObjetivo = 270.0; //conserva el angulo inicial como dato de la accion
    sinAccion.cambiarColor = false; //mantiene la fase actual
    sinAccion.colorNuevo = 0; //deja cian como dato de color sin solicitar el cambio
    sinAccion.solicitarPulso = false; //evita activar el pulso en este primer paso

    procesarTick(motor, sinAccion); //genera el disparo automatico inicial y cuenta un tick sobrevivido

    Enemigo enemigoIncorrecto; //prepara el objetivo del impacto de color incorrecto
    enemigoIncorrecto.id = 1; //identifica al primer enemigo del escenario
    enemigoIncorrecto.tipo = Dron; //usa un dron con un punto de vida
    enemigoIncorrecto.color = 1; //asigna magenta para que no coincida con la bala cian
    enemigoIncorrecto.r = 100.0; //coloca al enemigo a cien unidades del centro
    enemigoIncorrecto.theta = 0.0; //lo situa sobre el eje horizontal positivo
    enemigoIncorrecto.omega = 0.0; //deja al enemigo sin giro angular
    enemigoIncorrecto.vida = 1; //establece la vida inicial del dron
    enemigoIncorrecto.radio = 6; //define el alcance de su colision con la bala
    enemigoIncorrecto.ticksColor = 0; //reinicia el contador de cambio de fase

    insertarFinal(motor.enemigos, enemigoIncorrecto); //agrega el objetivo que debe sobrevivir al impacto incorrecto

    Bala balaIncorrecta{ 100.0, 0.0, 0.0, 0.0, 0, 0 }; //coloca una bala cian inmovil en la posicion del enemigo magenta

    insertarInicio(motor.balas, balaIncorrecta); //agrega la bala del primer impacto sin contarla como un disparo automatico

    moverBalasYResolverImpactos(motor); //resuelve el impacto incorrecto y actualiza sus metricas y evento

    destruirListaEnemigos(motor.enemigos); //retira al enemigo que sobrevivio para preparar el siguiente caso
    inicializarListaEnemigos(motor.enemigos); //deja la lista lista para el objetivo del acierto

    Enemigo enemigoCorrecto; //prepara el objetivo que debe caer por una bala de su misma fase
    enemigoCorrecto.id = 2; //asigna otro identificador al segundo enemigo
    enemigoCorrecto.tipo = Dron; //usa un dron para que un acierto sea suficiente
    enemigoCorrecto.color = 0; //iguala su fase con la bala cian
    enemigoCorrecto.r = 100.0; //coloca al objetivo a cien unidades del centro
    enemigoCorrecto.theta = 0.0; //lo ubica en el mismo eje que la bala
    enemigoCorrecto.omega = 0.0; //mantiene al enemigo sin giro angular
    enemigoCorrecto.vida = 1; //deja un solo punto de vida para comprobar la baja
    enemigoCorrecto.radio = 6; //fija el radio usado al resolver el impacto
    enemigoCorrecto.ticksColor = 0; //reinicia el contador de color del objetivo

    insertarFinal(motor.enemigos, enemigoCorrecto); //agrega el enemigo del impacto correcto

    Bala balaCorrecta{ 100.0, 0.0, 0.0, 0.0, 0, 0 }; //prepara una bala inmovil de la misma fase que el objetivo

    insertarInicio(motor.balas, balaCorrecta); //agrega el disparo preparado para comprobar el acierto

    moverBalasYResolverImpactos(motor); //elimina al dron y registra el acierto la baja y sus puntos

    jugador->cargaPulso = 100; //completa la carga para probar la activacion del pulso

    AccionMotor usarPulso; //prepara una accion que solo solicita el pulso
    usarPulso.cambiarAngulo = false; //mantiene el destino de la nave
    usarPulso.anguloObjetivo = motor.nave.anguloObjetivo; //copia el destino actual como dato de la accion
    usarPulso.cambiarColor = false; //conserva la fase de la nave
    usarPulso.colorNuevo = motor.nave.color; //copia la fase actual sin pedir que cambie
    usarPulso.solicitarPulso = true; //solicita activar el pulso con la carga completa

    aplicarAccion(motor, usarPulso); //activa el pulso y cuenta su uso sin avanzar otro tick

    destruirListaEnemigos(motor.enemigos); //retira cualquier enemigo que haya quedado antes del choque final
    inicializarListaEnemigos(motor.enemigos); //prepara la lista para el enemigo que alcanzara el nucleo

    jugador->vidaNucleo = 5; //limita la vida disponible para comprobar que el dano registrado no la exceda

    Enemigo enemigoNucleo; //prepara el enemigo que provocara la eliminacion del jugador
    enemigoNucleo.id = 3; //asigna el identificador del ultimo enemigo de la prueba
    enemigoNucleo.tipo = Dron; //usa un tipo cuyo dano supera la vida restante
    enemigoNucleo.color = 0; //asigna la fase del dron del choque
    enemigoNucleo.r = 10.0; //coloca al enemigo dentro del alcance de colision del nucleo
    enemigoNucleo.theta = 0.0; //situa al enemigo sobre el eje horizontal positivo
    enemigoNucleo.omega = 0.0; //deja su giro angular en cero
    enemigoNucleo.vida = 1; //asigna la vida habitual del dron
    enemigoNucleo.radio = 6; //establece su radio de colision
    enemigoNucleo.ticksColor = 0; //reinicia el contador de cambio de fase

    insertarFinal(motor.enemigos, enemigoNucleo); //agrega el enemigo que debe consumir la vida restante

    resolverChoquesEnemigos(motor); //aplica el choque y registra solo los cinco puntos de vida perdidos
    verificarFinMotor(motor); //cierra el turno por falta de vida y registra la eliminacion

    destruirMotor(motor); //libera sus estructuras conservando el jugador y la bitacora de la partida
    destruirColaSpawn(cola); //libera la aparicion original pendiente

    actualizarMetricasFinalesPartida(partida); //calcula las proporciones y la posicion con los resultados obtenidos

    bool resultado = true; //acumula las diferencias entre las metricas reales y las esperadas

    if (jugador->metricas.disparos != 1) { //comprueba que solo el disparo automatico se haya contado como disparo realizado
        resultado = false; //marca un conteo de disparos incorrecto
    }

    if (jugador->metricas.impactosCorrectos != 1) { //revisa el unico impacto de color correcto preparado
        resultado = false; //marca un conteo de aciertos incorrecto
    }

    if (jugador->metricas.impactosErroneos != 1) { //revisa el unico impacto de color incorrecto preparado
        resultado = false; //marca un conteo de errores incorrecto
    }

    if (jugador->metricas.bajasDron != 1) { //comprueba que se cuente solo el dron abatido por la bala
        resultado = false; //marca un conteo de bajas incorrecto
    }

    if (jugador->metricas.pulsosUtilizados != 1) { //revisa que la activacion se cuente una sola vez
        resultado = false; //marca un conteo de pulsos incorrecto
    }

    if (jugador->metricas.danoTotal != 5) { //comprueba que el dano total coincida con la vida que realmente se perdio
        resultado = false; //marca un dano acumulado incorrecto
    }

    if (jugador->metricas.ticksSobrevividos != 1) { //revisa que las operaciones directas posteriores no cuenten como ticks adicionales
        resultado = false; //marca un tiempo de simulacion incorrecto
    }

    if (jugador->puntaje != 100) { //comprueba el premio de la unica baja del escenario
        resultado = false; //marca un puntaje incorrecto
    }

    if (jugador->metricas.precisionColor != 0.5) { //revisa la precision de un acierto entre dos impactos
        resultado = false; //marca una precision calculada incorrecta
    }

    if (partida.bitacora.tam != 5) { //comprueba el total de eventos de error baja pulso dano y eliminacion
        resultado = false; //marca una cantidad de eventos incorrecta
    }

    HistorialPartidas historial; //recibira una copia de los resultados del participante
    inicializarHistorial(historial); //prepara un historial vacio para el registro final

    agregarResultadosPartidaHistorial(partida, 77, "lotes", historial); //copia las metricas de la partida al historial con el identificador de prueba

    if (contarRegistros(historial) != 1) { //revisa que se guarde un solo registro para el unico participante
        resultado = false; //marca una cantidad de registros incorrecta
    }

    destruirPartida(partida); //libera la partida para comprobar que el registro historico sea independiente

    if (historial.cabeza == nullptr || historial.cabeza->dato.jugador != "ANA" || historial.cabeza->dato.puntaje != 100) { //revisa el alias y los puntos despues de destruir el estado original
        resultado = false; //marca un resultado historico ausente o incorrecto
    }

    if (historial.cabeza != nullptr) { //consulta el detalle solo si se creo el registro esperado
        const RegistroPartida& registro = historial.cabeza->dato; //accede a la copia que ahora pertenece al historial

        if (!registro.detalleDisponible) { //comprueba que el registro conserve las metricas de detalle
            resultado = false; //marca el detalle como ausente
        }

        if (registro.correctosC != 1) { //revisa el acierto realizado por la bala cian
            resultado = false; //marca un conteo por color incorrecto
        }

        if (registro.erroneosC != 1) { //revisa el fallo realizado tambien por una bala cian
            resultado = false; //marca un conteo de errores por color incorrecto
        }

        if (registro.danoDron != 5) { //comprueba que el dano real quede atribuido al dron del choque
            resultado = false; //marca un desglose de dano incorrecto
        }
    }

    destruirHistorial(historial); //libera la copia de los resultados al terminar las comprobaciones

    return resultado; //informa si las metricas los eventos y el registro historico coincidieron con lo esperado
}
