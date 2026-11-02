#include "ReportePartida.h"
#include <fstream>

//escribe los datos de un jugador con los nombres de campo y el orden establecidos para el reporte
//la posicion de registro identifica su bloque y es distinta del puesto final de la partida
static void imprimirJugadorReporte(const Jugador& jugador, int posicionRegistro, ostream& salida) {
    const MetricasJugador& metricas = jugador.metricas; //consulta las metricas guardadas sin copiarlas ni recalcularlas

    salida << "[Jugador " << posicionRegistro << "]" << '\n'; //abre el bloque del jugador segun su orden de registro

    salida << "alias=" << jugador.alias << '\n'; //identifica al participante
    salida << "esBot=" << (jugador.esBot ? 1 : 0) << '\n'; //escribe uno para un bot y cero para un jugador humano

    salida << "posicion=" << metricas.posicionFinal << '\n'; //muestra el puesto final que ya tiene asignado
    salida << "puntaje=" << jugador.puntaje << '\n'; //guarda los puntos acumulados
    salida << "oleadaAlcanzada=" << jugador.oleadaAlcanzada << '\n'; //indica la ultima oleada que llego a iniciar
    salida << "vidaFinal=" << jugador.vidaNucleo << '\n'; //muestra la vida restante del nucleo

    salida << "disparos=" << metricas.disparos << '\n'; //registra cuantas balas disparo
    salida << "correctos=" << metricas.impactosCorrectos << '\n'; //cuenta los impactos que acertaron el color del enemigo
    salida << "erroneos=" << metricas.impactosErroneos << '\n'; //cuenta los impactos de un color distinto al enemigo

    salida << "comboMaximo=" << metricas.comboMaximo << '\n'; //guarda el mayor combo alcanzado
    salida << "pulsos=" << metricas.pulsosUtilizados << '\n'; //indica cuantos pulsos activo
    salida << "danoTotal=" << metricas.danoTotal << '\n'; //registra la vida perdida por el nucleo
    salida << "ticks=" << metricas.ticksSobrevividos << '\n'; //guarda los ticks jugados entre todos sus turnos

    salida << "bajasDron=" << metricas.bajasDron << '\n'; //muestra cuantos drones elimino
    salida << "bajasGirador=" << metricas.bajasGirador << '\n'; //muestra cuantos giradores elimino
    salida << "bajasDivisor=" << metricas.bajasDivisor << '\n'; //muestra cuantos divisores elimino
    salida << "bajasMini=" << metricas.bajasMini << '\n'; //muestra cuantos minis elimino
    salida << "bajasTanque=" << metricas.bajasTanque << '\n'; //muestra cuantos tanques elimino
    salida << "bajasCamaleon=" << metricas.bajasCamaleon << '\n'; //muestra cuantos camaleones elimino
}

//escribe los datos generales y un bloque por participante conservando el formato del reporte
void imprimirReportePartida(const Partida& partida, ostream& salida) {
    salida << "FASE_ORBITAL_REPORTE_V1" << '\n'; //identifica la version del formato usado

    salida << "semilla=" << partida.semillaPartida << '\n'; //guarda la semilla con la que se genero la partida
    salida << "numeroRonda=" << partida.numeroRonda << '\n'; //indica la ronda que tiene registrada la partida
    salida << "limiteOleadas=" << partida.limiteOleadas << '\n'; //guarda el maximo de oleadas configurado
    salida << "cantidadJugadores=" << partida.cantidadJugadores << '\n'; //cuenta todos los participantes incluidos los eliminados
    salida << "finalizada=" << (partida.finalizada ? 1 : 0) << '\n'; //escribe uno si la partida termino y cero si sigue activa

    for (int i = 0; i < partida.cantidadJugadores; i++) { //recorre los jugadores en el orden en que se registraron
        salida << '\n'; //separa cada bloque de jugador del contenido anterior
        imprimirJugadorReporte(partida.jugadores[i], i + 1, salida); //escribe el bloque numerandolo desde uno
    }
}

//guarda el reporte sin convertir los saltos de linea y comprueba el resultado despues de cerrar el archivo
bool guardarReportePartida(const Partida& partida, const string& ruta) {
    ofstream archivo(ruta, ios::binary); //crea o reemplaza el archivo conservando los bytes de los saltos de linea

    if (!archivo.is_open()) { //comprueba si pudo abrir la ruta para escribir
        return false; //avisa que no pudo iniciar el guardado
    }

    imprimirReportePartida(partida, archivo); //escribe el estado actual de la partida con el formato establecido
    archivo.close(); //envia los datos pendientes y cierra el archivo antes de comprobar el resultado

    return archivo.good(); //confirma el guardado solo si no fallo la escritura ni el cierre
}

//compara el contenido y la longitud de dos archivos sin convertir caracteres ni saltos de linea
//solo los considera iguales si ambos llegan al final sin errores de lectura
bool compararArchivosExactos(const string& rutaA, const string& rutaB) {
    ifstream archivoA(rutaA, ios::binary); //abre el primer archivo para leer sus bytes originales
    ifstream archivoB(rutaB, ios::binary); //abre el segundo archivo en el mismo modo

    if (!archivoA.is_open() || !archivoB.is_open()) { //comprueba que ambos archivos esten disponibles
        return false; //no puede confirmar igualdad si falta alguno de los archivos
    }

    char caracterA; //recibira el siguiente byte del primer archivo
    char caracterB; //recibira el siguiente byte del segundo archivo

    while (true) { //continua hasta encontrar una diferencia un error o el final de ambos archivos
        bool leyoA = static_cast<bool>(archivoA.get(caracterA)); //intenta leer un byte del primer archivo
        bool leyoB = static_cast<bool>(archivoB.get(caracterB)); //intenta leer un byte del segundo archivo

        if (leyoA != leyoB) { //detecta que solo uno de los archivos pudo entregar otro byte
            return false; //descarta la igualdad por una longitud distinta o un fallo de lectura
        }

        if (!leyoA) { //atiende el caso en que ninguna de las dos lecturas obtuvo un byte
            return archivoA.eof() && archivoB.eof() && !archivoA.bad() && !archivoB.bad(); //acepta solo el final normal de ambos archivos
        }

        if (caracterA != caracterB) { //compara los bytes leidos en la misma posicion
            return false; //termina al encontrar la primera diferencia
        }
    }
}
