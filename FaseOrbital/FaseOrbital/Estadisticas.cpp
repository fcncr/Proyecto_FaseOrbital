#include "Estadisticas.h"
#include "ListaPuntajes.h"
#include "ModelosCombate.h"
#include <cmath>

//traduce el numero de una fase al nombre que se muestra en las estadisticas
const char* nombreColorEstadistica(int color) {
    switch (color) { //elige el nombre segun el color recibido
    case 0: //identifica la fase cian
        return "Cian"; //devuelve el nombre de la primera fase

    case 1: //identifica la fase magenta
        return "Magenta"; //devuelve el nombre de la segunda fase

    case 2: //identifica la fase ambar
        return "Ambar"; //devuelve el nombre de la tercera fase

    default: //atiende un numero que no pertenece a las fases
        return "Desconocido"; //evita mostrar un nombre de color que no corresponde
    }
}

//traduce el numero de un tipo de enemigo al nombre que se muestra en los reportes
const char* nombreTipoEnemigoEstadistica(int tipo) {
    switch (static_cast<TipoEnemigo>(tipo)) { //permite comparar el numero con los tipos del juego
    case Dron: //reconoce el tipo dron
        return "Dron"; //devuelve su nombre para el reporte

    case Girador: //reconoce el tipo girador
        return "Girador"; //devuelve su nombre para el reporte

    case Divisor: //reconoce el tipo divisor
        return "Divisor"; //devuelve su nombre para el reporte

    case Mini: //reconoce el tipo mini
        return "Mini"; //devuelve su nombre para el reporte

    case Tanque: //reconoce el tipo tanque
        return "Tanque"; //devuelve su nombre para el reporte

    case Camaleon: //reconoce el tipo camaleon
        return "Camaleon"; //devuelve su nombre para el reporte

    default: //atiende un numero que no corresponde a un enemigo
        return "Desconocido"; //indica que no se pudo identificar el tipo
    }
}

//calcula la proporcion de impactos correctos de una partida y devuelve cero si no hubo impactos
static double calcularPrecisionRegistro(const RegistroPartida& registro) {
    int impactosTotales = registro.correctos + registro.erroneos; //cuenta las balas que golpearon un enemigo

    if (impactosTotales <= 0) { //comprueba si faltan impactos para calcular la precision
        return 0.0; //usa cero para esta partida y evita dividir entre cero
    }

    return static_cast<double>(registro.correctos) / static_cast<double>(impactosTotales); //obtiene la fraccion de impactos que acertaron el color
}

//deja el histograma vacio antes de calcular los limites y contar los puntajes
static void inicializarHistograma(HistogramaPuntajes& histograma) {
    histograma.minimo = 0.0; //deja el limite inferior pendiente de los datos
    histograma.maximo = 0.0; //deja el limite superior pendiente de los datos
    histograma.anchoIntervalo = 0.0; //empieza sin una separacion entre intervalos
    histograma.total = 0; //indica que todavia no hay puntajes considerados

    for (int i = 0; i < 10; i++) { //recorre los diez intervalos del histograma
        histograma.frecuencias[i] = 0; //borra el conteo de este intervalo
    }
}

//cuenta un puntaje en su intervalo usando los limites y el total que ya fueron calculados
static void agregarPuntajeHistograma(HistogramaPuntajes& histograma, int puntaje) {
    if (histograma.total <= 0) { //comprueba si el histograma tiene datos para trabajar
        return; //evita clasificar puntajes en un histograma vacio
    }

    if (histograma.minimo == histograma.maximo) { //detecta que todos los puntajes son iguales
        histograma.frecuencias[0]++; //reune todos los puntajes en el primer intervalo
        return; //termina sin dividir entre un ancho de intervalo igual a cero
    }

    int indice; //guardara el intervalo al que pertenece el puntaje

    if (static_cast<double>(puntaje) >= histograma.maximo) { //incluye el puntaje maximo en el ultimo intervalo
        indice = 9; //elige la ultima de las diez posiciones
    }
    else { //ubica los puntajes que quedan por debajo del maximo
        indice = static_cast<int>((static_cast<double>(puntaje) - histograma.minimo) / histograma.anchoIntervalo); //calcula cuantos intervalos hay desde el minimo
    }

    if (indice < 0) { //detecta una posicion por debajo del arreglo
        indice = 0; //la ajusta al primer intervalo
    }

    if (indice > 9) { //detecta una posicion por encima del arreglo
        indice = 9; //la ajusta al ultimo intervalo
    }

    histograma.frecuencias[indice]++; //cuenta el puntaje sin cambiar el total que ya estaba calculado
}

//reune las partidas del alias y calcula sus puntajes precision e histograma sin modificar el historial
//devuelve falso si no encuentra registros y usa el detalle por color y enemigo solamente cuando esta disponible
bool calcularEstadisticasJugador(const HistorialPartidas& historial, const string& alias, EstadisticasJugador& estadisticas) {
    estadisticas.alias = alias; //guarda a quien pertenecen los resultados
    estadisticas.partidasJugadas = 0; //reinicia la cantidad de partidas encontradas
    estadisticas.victorias = 0; //reinicia el conteo de primeros puestos
    estadisticas.mejorPuntaje = 0; //deja un valor inicial mientras encuentra la primera partida
    estadisticas.peorPuntaje = 0; //deja un valor inicial mientras encuentra la primera partida
    estadisticas.mediaPuntaje = 0.0; //descarta el promedio de una consulta anterior
    estadisticas.medianaPuntaje = 0.0; //descarta la mediana de una consulta anterior
    estadisticas.desviacionEstandar = 0.0; //reinicia la medida de dispersion
    estadisticas.precisionColorPromedio = 0.0; //reinicia el promedio de precision

    estadisticas.mejorColorDisponible = false; //todavia no hay impactos por color para comparar
    estadisticas.mejorColor = -1; //indica que aun no se ha elegido una fase

    estadisticas.tipoMasDanoDisponible = false; //todavia no hay dano por tipo para comparar
    estadisticas.tipoMasDano = -1; //indica que aun no se ha elegido un tipo de enemigo

    inicializarHistograma(estadisticas.histograma); //borra los limites y frecuencias de la consulta anterior

    ListaPuntajes puntajes; //guarda una copia de los puntajes para calcular la mediana
    inicializarListaPuntajes(puntajes); //prepara la lista temporal antes de agregar datos

    long long correctosPorColor[3] = { 0, 0, 0 }; //acumula aciertos de cian magenta y ambar
    long long erroneosPorColor[3] = { 0, 0, 0 }; //acumula errores en ese mismo orden de colores

    long long danoPorTipo[6] = { 0, 0, 0, 0, 0, 0 }; //acumula el dano siguiendo el orden de los tipos de enemigo

    bool hayDetalle = false; //recuerda si alguna partida contiene datos por color y enemigo

    const NodoRegistroPartida* actual = historial.cabeza; //empieza a buscar desde el primer registro

    long long sumaPuntajes = 0; //acumula los puntajes para obtener la media
    double sumaPrecisiones = 0.0; //acumula la precision individual de cada partida
    bool primerRegistro = true; //permite tomar los limites del primer puntaje real

    while (actual != nullptr) { //revisa todos los registros del historial
        const RegistroPartida& registro = actual->dato; //consulta los datos del nodo sin copiarlos ni modificarlos

        if (registro.jugador == alias) { //considera solamente las partidas del jugador solicitado
            estadisticas.partidasJugadas++; //cuenta esta partida para los promedios

            if (registro.posicion == 1) { //comprueba si termino en primer lugar
                estadisticas.victorias++; //cuenta la victoria aunque el primer puesto sea compartido
            }

            if (primerRegistro) { //usa el primer puntaje como referencia de ambos extremos
                estadisticas.mejorPuntaje = registro.puntaje; //establece el mayor puntaje conocido hasta ahora
                estadisticas.peorPuntaje = registro.puntaje; //establece el menor puntaje conocido hasta ahora
                primerRegistro = false; //deja las siguientes partidas para las comparaciones
            }
            else { //compara las demas partidas con los extremos encontrados
                if (registro.puntaje > estadisticas.mejorPuntaje) { //busca un nuevo maximo
                    estadisticas.mejorPuntaje = registro.puntaje; //actualiza el mejor resultado
                }

                if (registro.puntaje < estadisticas.peorPuntaje) { //busca un nuevo minimo
                    estadisticas.peorPuntaje = registro.puntaje; //actualiza el peor resultado
                }
            }

            sumaPuntajes += registro.puntaje; //suma los puntos de esta partida
            sumaPrecisiones += calcularPrecisionRegistro(registro); //incluye su precision con el mismo peso que las demas partidas

            agregarPuntajeFinal(puntajes, registro.puntaje); //conserva el puntaje en la lista usada para la mediana

            if (registro.detalleDisponible) { //evita usar campos de detalle que no estaban guardados
                hayDetalle = true; //habilita la comparacion de colores y tipos al terminar el recorrido

                correctosPorColor[0] += registro.correctosC; //suma los aciertos de la fase cian
                erroneosPorColor[0] += registro.erroneosC; //suma los errores de la fase cian

                correctosPorColor[1] += registro.correctosM; //suma los aciertos de la fase magenta
                erroneosPorColor[1] += registro.erroneosM; //suma los errores de la fase magenta

                correctosPorColor[2] += registro.correctosA; //suma los aciertos de la fase ambar
                erroneosPorColor[2] += registro.erroneosA; //suma los errores de la fase ambar

                danoPorTipo[0] += registro.danoDron; //suma el dano causado por drones
                danoPorTipo[1] += registro.danoGirador; //suma el dano causado por giradores
                danoPorTipo[2] += registro.danoDivisor; //suma el dano causado por divisores
                danoPorTipo[3] += registro.danoMini; //suma el dano causado por minis
                danoPorTipo[4] += registro.danoTanque; //suma el dano causado por tanques
                danoPorTipo[5] += registro.danoCamaleon; //suma el dano causado por camaleones
            }
        }

        actual = actual->sig; //continua con el siguiente registro del historial
    }

    if (estadisticas.partidasJugadas == 0) { //comprueba si el alias no aparecio en el historial
        destruirListaPuntajes(puntajes); //deja liberada la lista temporal antes de salir
        return false; //avisa que no hay partidas para calcular estadisticas
    }

    estadisticas.mediaPuntaje = static_cast<double>(sumaPuntajes) / static_cast<double>(estadisticas.partidasJugadas); //divide los puntos entre la cantidad de partidas
    estadisticas.medianaPuntaje = calcularMedianaPuntajes(puntajes); //obtiene el centro de los puntajes ordenados
    estadisticas.precisionColorPromedio = sumaPrecisiones / static_cast<double>(estadisticas.partidasJugadas); //promedia las precisiones incluyendo las partidas sin impactos

    double sumaCuadrados = 0.0; //acumula las diferencias al cuadrado respecto a la media

    actual = historial.cabeza; //vuelve al inicio para medir la dispersion de los puntajes

    while (actual != nullptr) { //recorre de nuevo las partidas guardadas
        const RegistroPartida& registro = actual->dato; //consulta la partida del nodo actual

        if (registro.jugador == alias) { //mantiene el calculo limitado al jugador solicitado
            double diferencia = static_cast<double>(registro.puntaje) - estadisticas.mediaPuntaje; //mide cuanto se aparta este puntaje del promedio
            sumaCuadrados += diferencia * diferencia; //acumula esa distancia al cuadrado
        }

        actual = actual->sig; //pasa a la siguiente partida
    }

    estadisticas.desviacionEstandar = sqrt(sumaCuadrados / static_cast<double>(estadisticas.partidasJugadas)); //calcula la desviacion poblacional dividiendo entre todas las partidas

    estadisticas.histograma.minimo = static_cast<double>(estadisticas.peorPuntaje); //usa el menor puntaje como inicio de los intervalos
    estadisticas.histograma.maximo = static_cast<double>(estadisticas.mejorPuntaje); //usa el mayor puntaje como final de los intervalos
    estadisticas.histograma.total = estadisticas.partidasJugadas; //guarda cuantos puntajes se van a distribuir

    if (estadisticas.mejorPuntaje != estadisticas.peorPuntaje) { //comprueba que exista un rango distinto de cero
        estadisticas.histograma.anchoIntervalo = (estadisticas.histograma.maximo - estadisticas.histograma.minimo) / 10.0; //divide el rango en diez partes iguales
    }

    actual = historial.cabeza; //regresa al inicio para contar los puntajes por intervalo

    while (actual != nullptr) { //visita todos los resultados guardados
        if (actual->dato.jugador == alias) { //selecciona los puntajes del jugador
            agregarPuntajeHistograma(estadisticas.histograma, actual->dato.puntaje); //aumenta la frecuencia del intervalo correspondiente
        }

        actual = actual->sig; //continua con el siguiente resultado
    }

    if (hayDetalle) { //compara colores y enemigos solo si encontro datos detallados
        int mejorColor = -1; //mantiene la fase sin elegir hasta encontrar impactos
        double mejorPrecision = -1.0; //permite que una precision de cero sea la primera candidata

        for (int color = 0; color < 3; color++) { //compara cian magenta y ambar en ese orden
            long long impactos = correctosPorColor[color] + erroneosPorColor[color]; //cuenta los impactos acumulados de esta fase

            if (impactos > 0) { //considera solo colores que tienen impactos registrados
                double precision = static_cast<double>(correctosPorColor[color]) / static_cast<double>(impactos); //calcula la proporcion de aciertos de este color

                if (mejorColor == -1 || precision > mejorPrecision) { //elige una mejora y conserva el primer color si hay empate
                    mejorColor = color; //guarda la fase con mayor precision hasta ahora
                    mejorPrecision = precision; //guarda su precision para las siguientes comparaciones
                }
            }
        }

        if (mejorColor != -1) { //comprueba si encontro al menos un color con impactos
            estadisticas.mejorColorDisponible = true; //indica que el resultado se puede mostrar
            estadisticas.mejorColor = mejorColor; //guarda la fase elegida en las estadisticas
        }

        int tipoMasDano = -1; //mantiene el tipo sin elegir hasta encontrar dano positivo
        long long mayorDano = 0; //usa cero como minimo para considerar un enemigo

        for (int tipo = 0; tipo < 6; tipo++) { //revisa los seis tipos en el orden definido por el juego
            if (danoPorTipo[tipo] > mayorDano) { //elige un dano mayor y conserva el primer tipo si hay empate
                mayorDano = danoPorTipo[tipo]; //actualiza el mayor dano acumulado
                tipoMasDano = tipo; //recuerda que enemigo causo ese dano
            }
        }

        if (tipoMasDano != -1) { //comprueba si algun tipo causo dano positivo
            estadisticas.tipoMasDanoDisponible = true; //habilita este dato para los reportes
            estadisticas.tipoMasDano = tipoMasDano; //guarda el enemigo que mas dano causo
        }
    }

    destruirListaPuntajes(puntajes); //libera los nodos temporales usados para calcular la mediana

    return true; //confirma que encontro partidas y completo las estadisticas
}

//imprime diez barras con sus intervalos y deja cerrado el limite superior del ultimo intervalo
//si todos los puntajes son iguales conserva diez lineas con el mismo rango
void imprimirHistogramaASCII(const HistogramaPuntajes& histograma, ostream& salida) {
    if (histograma.total == 0) { //detecta que no hay puntajes para representar
        salida << "Sin puntajes." << '\n'; //escribe el aviso de histograma vacio
        return; //termina sin imprimir intervalos
    }

    if (histograma.minimo == histograma.maximo) { //atiende el caso en que todos los puntajes coinciden
        for (int i = 0; i < 10; i++) { //conserva las diez filas del histograma
            salida << "[" << histograma.minimo << ", " << histograma.maximo << "] "; //muestra el unico puntaje como ambos limites

            for (int j = 0; j < histograma.frecuencias[i]; j++) { //recorre los puntajes contados en esta fila
                salida << '*'; //representa cada puntaje con un asterisco
            }

            salida << '\n'; //separa esta barra de la siguiente
        }

        return; //termina sin calcular intervalos de ancho cero
    }

    for (int i = 0; i < 10; i++) { //imprime una fila por cada intervalo
        double inferior = histograma.minimo + histograma.anchoIntervalo * i; //obtiene el inicio de este intervalo
        double superior = inferior + histograma.anchoIntervalo; //calcula donde termina este intervalo
        char cierre = ')'; //excluye el limite superior para no repetir puntajes entre intervalos

        if (i == 9) { //ajusta el ultimo intervalo para incluir el puntaje maximo
            superior = histograma.maximo; //usa el maximo exacto en lugar de acumular redondeos
            cierre = ']'; //indica que este limite superior tambien pertenece al intervalo
        }

        salida << "[" << inferior << ", " << superior << cierre << ' '; //escribe los limites con el cierre que corresponde

        for (int j = 0; j < histograma.frecuencias[i]; j++) { //recorre los puntajes que cayeron en este intervalo
            salida << '*'; //agrega un asterisco por cada puntaje
        }

        salida << '\n'; //deja la siguiente barra en otra linea
    }
}

//resume los eventos del jugador en una oleada y acumula los puntos de las bajas hasta esa oleada
//devuelve verdadero si encontro algun evento en la oleada solicitada aunque no haya bajas
bool calcularResumenOleadaJugador(const BitacoraEventos& bitacora, const string& jugador, int oleada, ResumenOleadaJugador& resumen) {
    resumen.oleada = oleada; //guarda la oleada que se quiere consultar
    resumen.bajas = 0; //reinicia el conteo de enemigos eliminados
    resumen.danoRecibido = 0; //reinicia el dano al nucleo de esa oleada
    resumen.puntajeOleada = 0; //reinicia los puntos obtenidos en esa oleada
    resumen.puntajeAcumulado = 0; //reinicia los puntos obtenidos hasta esa oleada

    if (oleada <= 0) { //descarta numeros anteriores a la primera oleada
        return false; //deja los conteos en cero y avisa que la oleada no es valida
    }

    bool encontroEvento = false; //recuerda si encontro actividad en la oleada consultada

    const NodoEvento* actual = bitacora.cabeza; //empieza desde el primer evento guardado

    while (actual != nullptr) { //revisa toda la bitacora sin depender del orden de las oleadas
        const Evento& evento = actual->dato; //consulta el evento sin copiarlo

        if (evento.jugador == jugador && evento.oleada <= oleada) { //considera al jugador hasta la oleada solicitada inclusive
            if (evento.tipoEvento == BAJA) { //identifica los eventos que aportan puntos
                resumen.puntajeAcumulado += evento.valor; //suma puntos de la oleada actual y de las anteriores

                if (evento.oleada == oleada) { //separa los resultados propios de la oleada consultada
                    resumen.bajas++; //cuenta el enemigo eliminado en esa oleada
                    resumen.puntajeOleada += evento.valor; //acumula solamente sus puntos
                }
            }

            if (evento.oleada == oleada) { //revisa la actividad especifica de la oleada consultada
                encontroEvento = true; //marca que existe al menos un evento aunque no sea una baja

                if (evento.tipoEvento == DANO_NUCLEO || evento.tipoEvento == CHOQUE_NAVE) { //incluye choques con el nucleo y con la nave
                    resumen.danoRecibido += evento.valor; //suma la vida perdida por esos choques
                }
            }
        }

        actual = actual->sig; //continua con el siguiente evento de la bitacora
    }

    return encontroEvento; //indica si hubo actividad registrada en esa oleada
}

//muestra por oleada las bajas el dano y los puntos acumulados e incluye las oleadas sin eventos
void imprimirLineaTiempoJugador(const BitacoraEventos& bitacora, const string& jugador, int ultimaOleada, ostream& salida) {
    if (ultimaOleada <= 0) { //comprueba si no hay oleadas que mostrar
        return; //termina sin escribir una linea de tiempo
    }

    salida << "Jugador: " << jugador << '\n'; //identifica a quien pertenece la linea de tiempo

    for (int oleada = 1; oleada <= ultimaOleada; oleada++) { //muestra las oleadas en orden desde la primera
        ResumenOleadaJugador resumen; //recibira los resultados de la oleada actual

        calcularResumenOleadaJugador(bitacora, jugador, oleada, resumen); //llena el resumen incluso cuando no hubo eventos en esa oleada

        salida << "Oleada " << resumen.oleada << '\n'; //muestra el numero de oleada
        salida << "Bajas: " << resumen.bajas << '\n'; //muestra cuantos enemigos elimino en ella
        salida << "Dano recibido: " << resumen.danoRecibido << '\n'; //muestra la vida que perdio el nucleo en ella
        salida << "Puntaje acumulado: " << resumen.puntajeAcumulado << '\n'; //muestra los puntos reunidos hasta ese momento
    }
}
