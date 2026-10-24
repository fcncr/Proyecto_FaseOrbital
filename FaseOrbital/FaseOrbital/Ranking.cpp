#include "Ranking.h"
#include "Estadisticas.h"

static bool registroVaAntes(const RegistroRanking& a, const RegistroRanking& b) {
    if (a.mejorPuntaje != b.mejorPuntaje) {
        return a.mejorPuntaje > b.mejorPuntaje;
    }

    if (a.precisionColor != b.precisionColor) {
        return a.precisionColor > b.precisionColor;
    }

    return a.alias < b.alias;
}

static void eliminarAliasRanking(Ranking& ranking, const std::string& alias) {
    NodoRanking* actual = ranking.cabeza;
    NodoRanking* anterior = nullptr;

    while (actual != nullptr) {
        if (actual->dato.alias == alias) {
            if (anterior == nullptr) {
                ranking.cabeza = actual->sig;
            }
            else {
                anterior->sig = actual->sig;
            }

            delete actual;
            ranking.tam--;
            return;
        }

        anterior = actual;
        actual = actual->sig;
    }
}

void inicializarRanking(Ranking& ranking) {
    ranking.cabeza = nullptr;
    ranking.tam = 0;
}

void insertarOrdenadoRanking(Ranking& ranking, const RegistroRanking& registro) {
    eliminarAliasRanking(ranking, registro.alias);

    NodoRanking* nuevo = new NodoRanking;
    nuevo->dato = registro;
    nuevo->sig = nullptr;

    if (ranking.cabeza == nullptr || registroVaAntes(registro, ranking.cabeza->dato)) {
        nuevo->sig = ranking.cabeza;
        ranking.cabeza = nuevo;
        ranking.tam++;
        return;
    }

    NodoRanking* actual = ranking.cabeza;

    while (actual->sig != nullptr && !registroVaAntes(registro, actual->sig->dato)) {
        actual = actual->sig;
    }

    nuevo->sig = actual->sig;
    actual->sig = nuevo;
    ranking.tam++;
}

const RegistroRanking* buscarJugadorRanking(const Ranking& ranking, const std::string& alias) {
    const NodoRanking* actual = ranking.cabeza;

    while (actual != nullptr) {
        if (actual->dato.alias == alias) {
            return &actual->dato;
        }

        actual = actual->sig;
    }

    return nullptr;
}

const RegistroRanking* obtenerPosicionRanking(const Ranking& ranking, int posicion) {
    if (posicion <= 0) {
        return nullptr;
    }

    const NodoRanking* actual = ranking.cabeza;
    int posicionActual = 1;

    while (actual != nullptr) {
        if (posicionActual == posicion) {
            return &actual->dato;
        }

        actual = actual->sig;
        posicionActual++;
    }

    return nullptr;
}

void recorrerTopRanking(const Ranking& ranking, int k, void (*procesar)(const RegistroRanking&, int posicion)) {
    if (k <= 0 || procesar == nullptr) {
        return;
    }

    const NodoRanking* actual = ranking.cabeza;
    int posicion = 1;

    while (actual != nullptr && posicion <= k) {
        procesar(actual->dato, posicion);
        actual = actual->sig;
        posicion++;
    }
}

void construirRankingDesdeHistorial(const HistorialPartidas& historial, Ranking& ranking) {
    destruirRanking(ranking);

    const NodoRegistroPartida* actual = historial.cabeza;

    while (actual != nullptr) {
        if (buscarJugadorRanking(ranking, actual->dato.jugador) == nullptr) {
            EstadisticasJugador estadisticas;

            if (calcularEstadisticasJugador(historial, actual->dato.jugador, estadisticas)) {
                RegistroRanking registro;
                registro.alias = actual->dato.jugador;
                registro.mejorPuntaje = estadisticas.mejorPuntaje;
                registro.precisionColor = estadisticas.precisionColorPromedio;

                insertarOrdenadoRanking(ranking, registro);
            }
        }
        actual = actual->sig;
    }
}

void destruirRanking(Ranking& ranking) {
    NodoRanking* actual = ranking.cabeza;

    while (actual != nullptr) {
        NodoRanking* siguiente = actual->sig;
        delete actual;
        actual = siguiente;
    }

    ranking.cabeza = nullptr;
    ranking.tam = 0;
}