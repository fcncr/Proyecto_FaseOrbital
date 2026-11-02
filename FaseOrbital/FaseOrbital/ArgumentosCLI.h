#ifndef ARGUMENTOS_CLI_H
#define ARGUMENTOS_CLI_H

#include <string>
#include <ostream>

using namespace std; //permite usar string y ostream sin escribir el prefijo

enum ModoPrograma { //indica que tarea debe ejecutar la aplicacion
    MODO_GRAFICO, //abre la interfaz cuando no se reciben opciones
    MODO_LOTES, //simula una partida con los datos de la consola
    MODO_ESTADISTICAS, //consulta las estadisticas de un historial
    MODO_RANKING, //consulta la clasificacion de un historial
    MODO_PRUEBAS, //ejecuta las pruebas del proyecto
    MODO_INVALIDO //indica que todavia no se aceptaron los argumentos
};

struct ConfiguracionPrograma { //reune las opciones que usara la aplicacion despues de validar los argumentos
    ModoPrograma modo; //selecciona la forma de ejecutar el programa

    string jugadores[6]; //guarda hasta seis alias en el orden recibido
    int cantidadJugadores; //indica cuantas posiciones del arreglo contienen participantes

    unsigned int semilla; //valor usado para reproducir la generacion de oleadas
    int oleadas; //limite de oleadas para la partida por lotes

    string rutaScript; //archivo de acciones indicado para el modo por lotes
    bool tieneScript; //indica si se recibio la opcion de script

    string rutaHistorial; //archivo que se consulta o se usa para guardar resultados
};

//restablece la configuracion sin jugadores ni script con diez oleadas y la ruta de historial predeterminada
void inicializarConfiguracionPrograma(ConfiguracionPrograma& config);

//reinicia la configuracion y lee los argumentos con el nombre del programa en la primera posicion
//devuelve falso si no son validos y en ese caso la configuracion no debe usarse para ejecutar la aplicacion
bool parsearArgumentos(int argc, char* argv[], ConfiguracionPrograma& config);

//escribe los comandos disponibles y las opciones obligatorias y opcionales del modo por lotes
void imprimirUso(ostream& salida);

#endif
