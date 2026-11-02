#include "Oleada.h"
#include "Generador.h"

//reemplaza una cola ya inicializada con las apariciones obtenidas del numero y la semilla de la oleada
//conserva el orden de los sorteos para repetir exactamente la misma cola con los mismos datos
void generarOleada(const Oleada& oleada, ColaSpawn& cola) {
    vaciarColaSpawn(cola); //borra las apariciones anteriores antes de guardar las nuevas

    uint32_t semillaOleada = oleada.semillaPartida * 1000u + static_cast<uint32_t>(oleada.numero); //combina la semilla de la partida con el numero de oleada
    int cantidadApariciones = 6 + 3 * oleada.numero; //calcula cuantos enemigos se programan para esta oleada
    int intervaloBase = 35 - 2 * oleada.numero; //reduce el tiempo base entre apariciones conforme avanzan las oleadas

    if (intervaloBase < 9) { //revisa si el intervalo base quedo por debajo del minimo
        intervaloBase = 9; //limita el intervalo base a nueve ticks
    }

    TipoEnemigo tiposDisponibles[7] = { Dron, Dron, Girador, Divisor, Tanque, Camaleon, Girador }; //repite dron y girador para darles mas posiciones en la seleccion
    int cantidadTipos = 2; //empieza usando las primeras dos posiciones que contienen drones

    if (oleada.numero == 2) { //habilita el primer girador a partir de la segunda oleada
        cantidadTipos = 3; //permite elegir entre las primeras tres posiciones
    }
    else if (oleada.numero == 3) { //incorpora el divisor en la tercera oleada
        cantidadTipos = 4; //permite elegir entre las primeras cuatro posiciones
    }
    else if (oleada.numero == 4) { //incorpora el tanque en la cuarta oleada
        cantidadTipos = 5; //permite elegir entre las primeras cinco posiciones
    }
    else if (oleada.numero >= 5) { //habilita el camaleon y la segunda posicion del girador
        cantidadTipos = 7; //permite usar todas las posiciones desde la quinta oleada
    }

    Generador generador; //guarda el estado de la secuencia usada en esta oleada
    inicializarGenerador(generador, semillaOleada); //inicia la secuencia con la semilla calculada

    int tickActual = 60; //programa la primera aparicion para el tick sesenta

    for (int i = 0; i < cantidadApariciones; i++) { //crea todas las apariciones de la oleada en orden
        Spawn spawn; //reune los datos del enemigo que se va a programar
        spawn.tick = tickActual; //guarda el momento en que debe aparecer

        uint32_t x = siguienteNumero(generador); //obtiene un numero para elegir el tipo de enemigo
        spawn.tipo = tiposDisponibles[x % cantidadTipos]; //elige una de las posiciones habilitadas del arreglo

        x = siguienteNumero(generador); //obtiene otro numero para elegir la fase del enemigo
        spawn.color = static_cast<int>(x % 3u); //asigna cero para cian uno para magenta o dos para ambar

        x = siguienteNumero(generador); //obtiene otro numero para elegir el angulo de aparicion
        spawn.angulo = static_cast<int>(x % 360u); //deja el angulo entre cero y trescientos cincuenta y nueve grados

        if (spawn.tipo == Girador) { //solo el girador necesita elegir un sentido de giro
            x = siguienteNumero(generador); //consume un numero adicional para decidir el sentido
            spawn.sentido = static_cast<int>(x % 2u); //elige uno de los dos sentidos representados por cero y uno
        }
        else { //los otros tipos no necesitan sortear el sentido
            spawn.sentido = 0; //deja este dato en cero sin consumir otro numero
        }

        encolarSpawn(cola, spawn); //agrega la aparicion al final de la cola

        if (i < cantidadApariciones - 1) { //calcula el siguiente momento solo si faltan apariciones
            x = siguienteNumero(generador); //obtiene el numero usado para el intervalo y la posible aparicion simultanea
            int intervaloReal = intervaloBase * (60 + static_cast<int>(x % 81u)) / 100; //aplica entre sesenta y ciento cuarenta por ciento del intervalo base con division entera

            if (!(oleada.numero >= 7 && x % 4u == 0u)) { //avanza el tiempo salvo que sea la oleada siete o mayor y el numero sea multiplo de cuatro
                tickActual += intervaloReal; //en los demas casos retrasa la siguiente aparicion segun el intervalo
            }
        }
    }
}
