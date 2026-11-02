#include "Ranking.h"
#include "Estadisticas.h"

//decide si el primer registro debe ir antes por puntaje precision o alias en ese orden
static bool registroVaAntes(const RegistroRanking& a, const RegistroRanking& b) {
    if (a.mejorPuntaje != b.mejorPuntaje) { //resuelve la comparacion cuando los puntajes son distintos
        return a.mejorPuntaje > b.mejorPuntaje; //coloca primero al jugador con mas puntos
    }

    if (a.precisionColor != b.precisionColor) { //usa la precision cuando los puntajes empatan
        return a.precisionColor > b.precisionColor; //coloca primero al jugador con mayor precision promedio
    }

    return a.alias < b.alias; //desempata por alias ascendente y devuelve falso si tambien son iguales
}

//quita el registro de un alias y une los nodos restantes antes de liberar su memoria
static void eliminarAliasRanking(Ranking& ranking, const string& alias) {
    NodoRanking* actual = ranking.cabeza; //empieza a buscar desde el primer puesto
    NodoRanking* anterior = nullptr; //permite recuperar el enlace que apunta al nodo por quitar

    while (actual != nullptr) { //recorre la lista hasta encontrar el alias o llegar al final
        if (actual->dato.alias == alias) { //identifica el registro que se va a reemplazar
            if (anterior == nullptr) { //comprueba si el registro esta al inicio de la lista
                ranking.cabeza = actual->sig; //deja al siguiente jugador como primera entrada
            }
            else { //atiende un registro que tiene otro nodo antes
                anterior->sig = actual->sig; //conecta el nodo anterior con el siguiente sin pasar por el que se elimina
            }

            delete actual; //libera el nodo y los datos que tenia guardados
            ranking.tam--; //descuenta el registro que se acaba de quitar
            return; //termina porque el ranking mantiene un solo registro por alias
        }

        anterior = actual; //recuerda el nodo previo para poder cambiar su enlace
        actual = actual->sig; //continua buscando en el siguiente puesto
    }
}

//establece los valores iniciales de un ranking que todavia no tiene nodos a su cargo
void inicializarRanking(Ranking& ranking) {
    ranking.cabeza = nullptr; //indica que no hay un primer jugador
    ranking.tam = 0; //deja el conteo de jugadores en cero
}

//guarda una copia de un registro independiente y la ubica segun puntaje precision y alias
//si el alias ya existe elimina su entrada anterior aunque el nuevo puntaje sea menor
void insertarOrdenadoRanking(Ranking& ranking, const RegistroRanking& registro) {
    eliminarAliasRanking(ranking, registro.alias); //evita que el mismo jugador quede repetido en la lista

    NodoRanking* nuevo = new NodoRanking{ registro, nullptr }; //crea el nodo con los datos recibidos y sin un siguiente

    if (ranking.cabeza == nullptr || registroVaAntes(registro, ranking.cabeza->dato)) { //atiende una lista vacia o un nuevo primer puesto
        nuevo->sig = ranking.cabeza; //conecta el nuevo nodo con el inicio anterior si habia uno
        ranking.cabeza = nuevo; //deja el registro recibido al inicio de la lista
        ranking.tam++; //cuenta la nueva entrada despues de cualquier reemplazo
        return; //termina porque el nodo ya quedo ubicado
    }

    NodoRanking* actual = ranking.cabeza; //empieza a buscar el nodo que debe quedar antes del nuevo

    while (actual->sig != nullptr && !registroVaAntes(registro, actual->sig->dato)) { //avanza mientras el siguiente jugador deba seguir antes del nuevo
        actual = actual->sig; //acerca el recorrido al punto de insercion
    }

    nuevo->sig = actual->sig; //conserva el resto de la lista antes de cambiar el enlace anterior
    actual->sig = nuevo; //inserta el nuevo registro entre los nodos encontrados o al final
    ranking.tam++; //cuenta el jugador que se acaba de insertar
}

//busca un alias y devuelve un puntero al registro guardado sin crear una copia ni transferir su propiedad
//el puntero deja de ser valido si se elimina esa entrada o se destruye o reconstruye el ranking
const RegistroRanking* buscarJugadorRanking(const Ranking& ranking, const string& alias) {
    const NodoRanking* actual = ranking.cabeza; //comienza la busqueda desde el primer puesto

    while (actual != nullptr) { //revisa los jugadores hasta encontrar el alias o terminar la lista
        if (actual->dato.alias == alias) { //comprueba si este nodo pertenece al jugador solicitado
            return &actual->dato; //permite consultar su registro sin modificarlo por este puntero
        }

        actual = actual->sig; //pasa al siguiente jugador
    }

    return nullptr; //indica que el alias no aparece en el ranking
}

//consulta un puesto contando desde uno y devuelve el registro de ese nodo sin copiarlo
//devuelve nullptr si el puesto no existe y el registro sigue siendo propiedad del ranking
const RegistroRanking* obtenerPosicionRanking(const Ranking& ranking, int posicion) {
    if (posicion <= 0) { //descarta numeros anteriores al primer puesto
        return nullptr; //indica que la posicion solicitada no es valida
    }

    const NodoRanking* actual = ranking.cabeza; //empieza desde el jugador mejor ubicado
    int posicionActual = 1; //asigna el primer puesto al inicio de la lista

    while (actual != nullptr) { //avanza hasta encontrar el puesto o agotar los jugadores
        if (posicionActual == posicion) { //comprueba si llego al puesto solicitado
            return &actual->dato; //devuelve acceso de lectura al registro de ese puesto
        }

        actual = actual->sig; //pasa al siguiente jugador de la lista
        posicionActual++; //actualiza el numero de puesto que se esta revisando
    }

    return nullptr; //indica que la lista tiene menos jugadores que la posicion solicitada
}

//visita hasta los primeros k jugadores y entrega cada registro y su puesto a la funcion recibida
void recorrerTopRanking(const Ranking& ranking, int k, void (*procesar)(const RegistroRanking&, int posicion)) {
    if (k <= 0 || procesar == nullptr) { //comprueba que haya puestos solicitados y una funcion para atenderlos
        return; //evita recorrer la lista sin trabajo que realizar
    }

    const NodoRanking* actual = ranking.cabeza; //comienza por el primer jugador del ranking
    int posicion = 1; //numera los puestos desde uno

    while (actual != nullptr && posicion <= k) { //se detiene al llegar al limite solicitado o al final de la lista
        procesar(actual->dato, posicion); //entrega el registro actual y su puesto a la funcion indicada
        actual = actual->sig; //continua con el siguiente jugador
        posicion++; //prepara el numero de puesto de la siguiente visita
    }
}

//reconstruye un ranking ya inicializado con una entrada por alias usando su mejor puntaje y precision promedio
void construirRankingDesdeHistorial(const HistorialPartidas& historial, Ranking& ranking) {
    destruirRanking(ranking); //libera el ranking anterior antes de cargar los resultados del historial

    const NodoRegistroPartida* actual = historial.cabeza; //empieza desde la primera partida guardada

    while (actual != nullptr) { //recorre las partidas para encontrar todos los alias
        if (buscarJugadorRanking(ranking, actual->dato.jugador) == nullptr) { //calcula los datos de cada jugador una sola vez
            EstadisticasJugador estadisticas; //recibira el resumen de todas las partidas de ese jugador

            if (calcularEstadisticasJugador(historial, actual->dato.jugador, estadisticas)) { //obtiene sus resultados historicos antes de incluirlo
                RegistroRanking registro{ actual->dato.jugador, estadisticas.mejorPuntaje, estadisticas.precisionColorPromedio }; //reune los tres datos que determinan su puesto

                insertarOrdenadoRanking(ranking, registro); //coloca al jugador segun los criterios de orden
            }
        }

        actual = actual->sig; //continua con la siguiente partida del historial
    }
}

//libera los nodos del ranking y restablece su inicio y cantidad de jugadores
void destruirRanking(Ranking& ranking) {
    NodoRanking* actual = ranking.cabeza; //empieza a liberar desde el primer jugador

    while (actual != nullptr) { //continua mientras quede algun nodo por borrar
        NodoRanking* siguiente = actual->sig; //guarda donde continuar antes de perder el nodo actual
        delete actual; //libera el nodo junto con el registro que contiene
        actual = siguiente; //retoma el recorrido en el nodo que habia guardado
    }

    ranking.cabeza = nullptr; //deja la lista sin un primer jugador
    ranking.tam = 0; //indica que ya no quedan registros
}
