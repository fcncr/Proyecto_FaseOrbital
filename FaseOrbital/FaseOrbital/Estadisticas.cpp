#include "Estadisticas.h"
#include "ListaPuntajes.h"
#include "ModelosCombate.h"
#include <cmath>

const char* nombreColorEstadistica(int color) {
    switch (color) {
    case 0:
        return "Cian";

    case 1:
        return "Magenta";

    case 2:
        return "Ambar";

    default:
        return "Desconocido";
    }
}

const char* nombreTipoEnemigoEstadistica(int tipo) {
    switch (static_cast<TipoEnemigo>(tipo)) {
    case Dron:
        return "Dron";

    case Girador:
        return "Girador";

    case Divisor:
        return "Divisor";

    case Mini:
        return "Mini";

    case Tanque:
        return "Tanque";

    case Camaleon:
        return "Camaleon";

    default:
        return "Desconocido";
    }
}

static double calcularPrecisionRegistro(const RegistroPartida& registro) {
    int impactosTotales = registro.correctos + registro.erroneos;

    if (impactosTotales <= 0) {
        return 0.0;
    }

    return static_cast<double>(registro.correctos) / static_cast<double>(impactosTotales);
}

static void inicializarHistograma(HistogramaPuntajes& histograma) {
    histograma.minimo = 0.0;
    histograma.maximo = 0.0;
    histograma.anchoIntervalo = 0.0;
    histograma.total = 0;

    for (int i = 0; i < 10; i++) {
        histograma.frecuencias[i] = 0;
    }
}

static void agregarPuntajeHistograma(HistogramaPuntajes& histograma, int puntaje) {
    if (histograma.total <= 0) {
        return;
    }

    if (histograma.minimo == histograma.maximo) {
        histograma.frecuencias[0]++;
        return;
    }

    int indice;

    if (static_cast<double>(puntaje) >= histograma.maximo) {
        indice = 9;
    }
    else {
        indice = static_cast<int>(
            (static_cast<double>(puntaje) - histograma.minimo) /
            histograma.anchoIntervalo
            );
    }

    if (indice < 0) {
        indice = 0;
    }

    if (indice > 9) {
        indice = 9;
    }

    histograma.frecuencias[indice]++;
}

bool calcularEstadisticasJugador(const HistorialPartidas& historial, const std::string& alias, EstadisticasJugador& estadisticas) {
    estadisticas.alias = alias;
    estadisticas.partidasJugadas = 0;
    estadisticas.victorias = 0;
    estadisticas.mejorPuntaje = 0;
    estadisticas.peorPuntaje = 0;
    estadisticas.mediaPuntaje = 0.0;
    estadisticas.medianaPuntaje = 0.0;
    estadisticas.desviacionEstandar = 0.0;
    estadisticas.precisionColorPromedio = 0.0;

    estadisticas.mejorColorDisponible = false;
    estadisticas.mejorColor = -1;

    estadisticas.tipoMasDanoDisponible = false;
    estadisticas.tipoMasDano = -1;

    inicializarHistograma(estadisticas.histograma);

    ListaPuntajes puntajes;
    inicializarListaPuntajes(puntajes);

    long long correctosPorColor[3] = { 0, 0, 0 };
    long long erroneosPorColor[3] = { 0, 0, 0 };

    long long danoPorTipo[6] = { 0, 0, 0, 0, 0, 0 };

    bool hayDetalle = false;

    const NodoRegistroPartida* actual = historial.cabeza;

    long long sumaPuntajes = 0;
    double sumaPrecisiones = 0.0;
    bool primerRegistro = true;

    while (actual != nullptr) {
        const RegistroPartida& registro = actual->dato;

        if (registro.jugador == alias) {
            estadisticas.partidasJugadas++;

            if (registro.posicion == 1) {
                estadisticas.victorias++;
            }

            if (primerRegistro) {
                estadisticas.mejorPuntaje = registro.puntaje;
                estadisticas.peorPuntaje = registro.puntaje;
                primerRegistro = false;
            }
            else {
                if (registro.puntaje > estadisticas.mejorPuntaje) {
                    estadisticas.mejorPuntaje = registro.puntaje;
                }

                if (registro.puntaje < estadisticas.peorPuntaje) {
                    estadisticas.peorPuntaje = registro.puntaje;
                }
            }

            sumaPuntajes += registro.puntaje;
            sumaPrecisiones += calcularPrecisionRegistro(registro);

            agregarPuntajeFinal(puntajes, registro.puntaje);

            if (registro.detalleDisponible) {
                hayDetalle = true;

                correctosPorColor[0] += registro.correctosC;
                erroneosPorColor[0] += registro.erroneosC;

                correctosPorColor[1] += registro.correctosM;
                erroneosPorColor[1] += registro.erroneosM;

                correctosPorColor[2] += registro.correctosA;
                erroneosPorColor[2] += registro.erroneosA;

                danoPorTipo[0] += registro.danoDron;
                danoPorTipo[1] += registro.danoGirador;
                danoPorTipo[2] += registro.danoDivisor;
                danoPorTipo[3] += registro.danoMini;
                danoPorTipo[4] += registro.danoTanque;
                danoPorTipo[5] += registro.danoCamaleon;
            }
        }

        actual = actual->sig;
    }

    if (estadisticas.partidasJugadas == 0) {
        destruirListaPuntajes(puntajes);
        return false;
    }

    estadisticas.mediaPuntaje =
        static_cast<double>(sumaPuntajes) /
        static_cast<double>(estadisticas.partidasJugadas);

    estadisticas.medianaPuntaje =
        calcularMedianaPuntajes(puntajes);

    estadisticas.precisionColorPromedio =
        sumaPrecisiones /
        static_cast<double>(estadisticas.partidasJugadas);

    double sumaCuadrados = 0.0;

    actual = historial.cabeza;

    while (actual != nullptr) {
        const RegistroPartida& registro = actual->dato;

        if (registro.jugador == alias) {
            double diferencia =
                static_cast<double>(registro.puntaje) -
                estadisticas.mediaPuntaje;

            sumaCuadrados += diferencia * diferencia;
        }

        actual = actual->sig;
    }

    estadisticas.desviacionEstandar =
        std::sqrt(
            sumaCuadrados /
            static_cast<double>(estadisticas.partidasJugadas)
        );

    estadisticas.histograma.minimo =
        static_cast<double>(estadisticas.peorPuntaje);

    estadisticas.histograma.maximo =
        static_cast<double>(estadisticas.mejorPuntaje);

    estadisticas.histograma.total =
        estadisticas.partidasJugadas;

    if (estadisticas.mejorPuntaje != estadisticas.peorPuntaje) {
        estadisticas.histograma.anchoIntervalo =
            (estadisticas.histograma.maximo -
                estadisticas.histograma.minimo) /
            10.0;
    }

    actual = historial.cabeza;

    while (actual != nullptr) {
        if (actual->dato.jugador == alias) {
            agregarPuntajeHistograma(
                estadisticas.histograma,
                actual->dato.puntaje
            );
        }

        actual = actual->sig;
    }

    if (hayDetalle) {
        int mejorColor = -1;
        double mejorPrecision = -1.0;

        for (int color = 0; color < 3; color++) {
            long long impactos =
                correctosPorColor[color] +
                erroneosPorColor[color];

            if (impactos > 0) {
                double precision =
                    static_cast<double>(correctosPorColor[color]) /
                    static_cast<double>(impactos);

                if (mejorColor == -1 || precision > mejorPrecision) {
                    mejorColor = color;
                    mejorPrecision = precision;
                }
            }
        }

        if (mejorColor != -1) {
            estadisticas.mejorColorDisponible = true;
            estadisticas.mejorColor = mejorColor;
        }

        int tipoMasDano = -1;
        long long mayorDano = 0;

        for (int tipo = 0; tipo < 6; tipo++) {
            if (danoPorTipo[tipo] > mayorDano) {
                mayorDano = danoPorTipo[tipo];
                tipoMasDano = tipo;
            }
        }

        if (tipoMasDano != -1) {
            estadisticas.tipoMasDanoDisponible = true;
            estadisticas.tipoMasDano = tipoMasDano;
        }
    }

    destruirListaPuntajes(puntajes);

    return true;
}

void imprimirHistogramaASCII(const HistogramaPuntajes& histograma, std::ostream& salida) {
    if (histograma.total == 0) {
        salida << "Sin puntajes." << '\n';
        return;
    }

    if (histograma.minimo == histograma.maximo) {
        for (int i = 0; i < 10; i++) {
            salida << "[" << histograma.minimo << ", " << histograma.maximo << "] ";

            for (int j = 0; j < histograma.frecuencias[i]; j++) {
                salida << '*';
            }

            salida << '\n';
        }

        return;
    }

    for (int i = 0; i < 10; i++) {
        double inferior =
            histograma.minimo +
            histograma.anchoIntervalo * i;

        double superior =
            inferior +
            histograma.anchoIntervalo;

        if (i == 9) {
            superior = histograma.maximo;

            salida << "["
                << inferior
                << ", "
                << superior
                << "] ";
        }
        else {
            salida << "["
                << inferior
                << ", "
                << superior
                << ") ";
        }

        for (int j = 0; j < histograma.frecuencias[i]; j++) {
            salida << '*';
        }

        salida << '\n';
    }
}

bool calcularResumenOleadaJugador(const BitacoraEventos& bitacora, const std::string& jugador, int oleada, ResumenOleadaJugador& resumen) {
    resumen.oleada = oleada;
    resumen.bajas = 0;
    resumen.danoRecibido = 0;
    resumen.puntajeOleada = 0;
    resumen.puntajeAcumulado = 0;

    if (oleada <= 0) {
        return false;
    }

    bool encontroEvento = false;

    const NodoEvento* actual =
        bitacora.cabeza;

    while (actual != nullptr) {
        const Evento& evento =
            actual->dato;

        if (evento.jugador == jugador &&
            evento.oleada <= oleada) {

            if (evento.tipoEvento == BAJA) {
                resumen.puntajeAcumulado += evento.valor;

                if (evento.oleada == oleada) {
                    resumen.bajas++;
                    resumen.puntajeOleada += evento.valor;
                }
            }

            if (evento.oleada == oleada) {
                encontroEvento = true;

                if (evento.tipoEvento == DANO_NUCLEO ||
                    evento.tipoEvento == CHOQUE_NAVE) {

                    resumen.danoRecibido += evento.valor;
                }
            }
        }

        actual = actual->sig;
    }

    return encontroEvento;
}

void imprimirLineaTiempoJugador(const BitacoraEventos& bitacora, const std::string& jugador, int ultimaOleada, std::ostream& salida) {
    if (ultimaOleada <= 0) {
        return;
    }

    salida << "Jugador: "
        << jugador
        << '\n';

    for (int oleada = 1; oleada <= ultimaOleada; oleada++) {
        ResumenOleadaJugador resumen;

        calcularResumenOleadaJugador(
            bitacora,
            jugador,
            oleada,
            resumen
        );

        salida << "Oleada "
            << resumen.oleada
            << '\n';

        salida << "Bajas: "
            << resumen.bajas
            << '\n';

        salida << "Dano recibido: "
            << resumen.danoRecibido
            << '\n';

        salida << "Puntaje acumulado: "
            << resumen.puntajeAcumulado
            << '\n';
    }
}