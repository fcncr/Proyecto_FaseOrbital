#include "Pruebas.h"
#include "../FaseOrbital/ColaSpawn.h"
#include "../FaseOrbital/RondaJugadores.h"
#include "../FaseOrbital/Generador.h"
#include "../FaseOrbital/Partida.h"
#include "ModoLotes.h"
#include "Bot.h"
#include "BitacoraEventos.h"
#include "Estadisticas.h" 

//comprueba el orden de salida de una cola y su estado al retirar todas las apariciones
bool pruebaP05() {
    ColaSpawn cola; //guarda las apariciones de la prueba de orden de salida
    inicializarColaSpawn(cola); //prepara la cola sin nodos

    Spawn spawn1{ 10, Dron, 0, 45, 1 }; //prepara la primera aparicion con tick diez
    Spawn spawn2{ 20, Girador, 1, 90, -1 }; //prepara la segunda aparicion con tick veinte
    Spawn spawn3{ 30, Tanque, 2, 180, 1 }; //prepara la tercera aparicion con tick treinta

    encolarSpawn(cola, spawn1); //coloca la primera aparicion al frente
    encolarSpawn(cola, spawn2); //agrega la segunda despues de la primera
    encolarSpawn(cola, spawn3); //deja la tercera aparicion al final

    bool resultado = true; //acumula los fallos sin detener las comprobaciones restantes

    if (cola.tam != 3) { //detecta si falta alguna de las tres apariciones encoladas
        resultado = false; //marca la prueba como fallida
    }

    Spawn salida; //recibe la aparicion que se retire de la cola

    if (!desencolarSpawn(cola, salida) || salida.tick != 10) { //revisa que salga primero la aparicion del tick diez
        resultado = false; //marca la prueba como fallida
    }

    if (!desencolarSpawn(cola, salida) || salida.tick != 20) { //comprueba que la segunda salida corresponda al tick veinte
        resultado = false; //marca la prueba como fallida
    }

    if (!desencolarSpawn(cola, salida) || salida.tick != 30) { //comprueba que la ultima salida corresponda al tick treinta
        resultado = false; //marca la prueba como fallida
    }

    if (cola.frente != nullptr || cola.final != nullptr || cola.tam != 0) { //revisa que al vaciar la cola no queden extremos ni elementos registrados
        resultado = false; //marca la prueba como fallida
    }

    if (desencolarSpawn(cola, salida)) { //detecta si se acepta retirar de una cola vacia
        resultado = false; //marca la prueba como fallida
    }

    destruirColaSpawn(cola); //libera cualquier nodo que haya quedado en la cola

    return resultado; //devuelve el resultado de todas las comprobaciones realizadas
}

//revisa las copias por constructor y asignacion su independencia y la autoasignacion de una cola
bool pruebaP06() {
    ColaSpawn original; //guarda la cola que se copiara
    inicializarColaSpawn(original); //deja la cola original vacia

    Spawn spawn1{ 10, Dron, 0, 45, 1 }; //prepara la primera aparicion con tick diez
    Spawn spawn2{ 20, Girador, 1, 90, -1 }; //prepara la segunda aparicion con tick veinte
    Spawn spawn3{ 30, Tanque, 2, 180, 1 }; //prepara la tercera aparicion con tick treinta

    encolarSpawn(original, spawn1); //agrega la primera aparicion a la cola original
    encolarSpawn(original, spawn2); //agrega la segunda aparicion a la cola original
    encolarSpawn(original, spawn3); //completa las tres apariciones que se copiaran

    bool resultado = true; //acumula los fallos sin detener las comprobaciones restantes

    ColaSpawn copiaConstructor(original); //crea una copia mediante el constructor de copia

    ColaSpawn copiaAsignacion; //guarda el destino de la asignacion
    inicializarColaSpawn(copiaAsignacion); //prepara la cola que recibira la asignacion

    Spawn viejo{ 999, Camaleon, 0, 270, -1 }; //crea un dato previo que la asignacion debera reemplazar
    encolarSpawn(copiaAsignacion, viejo); //deja un nodo anterior en el destino para probar su reemplazo

    copiaAsignacion = original; //copia la cola original sobre un destino que ya tiene datos

    if (original.tam != 3) { //comprueba que la cola original conserve sus tres apariciones
        resultado = false; //marca la prueba como fallida
    }

    if (copiaConstructor.tam != 3) { //revisa que la copia construida tenga las tres apariciones
        resultado = false; //marca la prueba como fallida
    }

    if (copiaAsignacion.tam != 3) { //revisa que el destino asignado contenga tres apariciones
        resultado = false; //marca la prueba como fallida
    }

    if (original.frente == copiaConstructor.frente) { //detecta si el constructor dejo compartido el primer nodo
        resultado = false; //marca la prueba como fallida
    }

    if (original.frente == copiaAsignacion.frente) { //detecta si la asignacion reutilizo el primer nodo del original
        resultado = false; //marca la prueba como fallida
    }

    if (copiaConstructor.frente == copiaAsignacion.frente) { //comprueba que las dos copias tampoco compartan su primer nodo
        resultado = false; //marca la prueba como fallida
    }

    if (original.final == copiaConstructor.final) { //detecta si la copia construida comparte el ultimo nodo con el original
        resultado = false; //marca la prueba como fallida
    }

    if (original.final == copiaAsignacion.final) { //detecta si la copia asignada comparte el ultimo nodo con el original
        resultado = false; //marca la prueba como fallida
    }

    Spawn salida; //recibe la aparicion que se retire de la cola

    if (!desencolarSpawn(copiaConstructor, salida) || salida.tick != 10) { //revisa el primer dato de la copia al retirarlo
        resultado = false; //marca la prueba como fallida
    }

    if (!desencolarSpawn(copiaConstructor, salida) || salida.tick != 20) { //revisa el segundo dato de la copia al retirarlo
        resultado = false; //marca la prueba como fallida
    }

    if (!desencolarSpawn(copiaConstructor, salida) || salida.tick != 30) { //revisa el ultimo dato de la copia al retirarlo
        resultado = false; //marca la prueba como fallida
    }

    if (!colaSpawnVacia(copiaConstructor)) { //comprueba que la copia construida haya quedado vacia
        resultado = false; //marca la prueba como fallida
    }

    if (original.tam != 3) { //comprueba que la cola original conserve sus tres apariciones
        resultado = false; //marca la prueba como fallida
    }

    if (copiaAsignacion.tam != 3) { //revisa que el destino asignado contenga tres apariciones
        resultado = false; //marca la prueba como fallida
    }

    if (original.frente == nullptr || original.frente->dato.tick != 10) { //revisa que el primer dato original no cambie al vaciar la copia
        resultado = false; //marca la prueba como fallida
    }

    if (original.final == nullptr || original.final->dato.tick != 30) { //revisa que el ultimo dato original siga disponible
        resultado = false; //marca la prueba como fallida
    }

    if (copiaAsignacion.frente == nullptr || copiaAsignacion.frente->dato.tick != 10) { //comprueba que la otra copia conserve su primer dato
        resultado = false; //marca la prueba como fallida
    }

    copiarColaSpawn(original, copiaConstructor); //vuelve a llenar la copia vaciada mediante la funcion de copia

    if (copiaConstructor.tam != 3) { //revisa que la copia construida tenga las tres apariciones
        resultado = false; //marca la prueba como fallida
    }

    if (copiaConstructor.frente == original.frente) { //detecta si la funcion de copia compartio el primer nodo
        resultado = false; //marca la prueba como fallida
    }

    copiaAsignacion = copiaAsignacion; //asigna la cola a si misma para comprobar que conserve sus datos

    if (copiaAsignacion.tam != 3) { //revisa que el destino asignado contenga tres apariciones
        resultado = false; //marca la prueba como fallida
    }

    destruirColaSpawn(copiaConstructor); //libera los nodos de la copia creada por constructor
    destruirColaSpawn(copiaAsignacion); //libera los nodos de la copia creada por asignacion
    destruirColaSpawn(original); //libera los nodos de la cola original

    if (!colaSpawnVacia(original)) { //revisa el estado vacio de la cola original despues de destruirla
        resultado = false; //marca la prueba como fallida
    }

    if (!colaSpawnVacia(copiaConstructor)) { //comprueba que la copia construida haya quedado vacia
        resultado = false; //marca la prueba como fallida
    }

    if (!colaSpawnVacia(copiaAsignacion)) { //revisa el estado vacio de la copia asignada despues de destruirla
        resultado = false; //marca la prueba como fallida
    }

    return resultado; //devuelve el resultado de todas las comprobaciones realizadas
}

static Jugador* jugadoresRecorridos[6]; //guarda las direcciones visitadas sin hacerse cargo de destruir los jugadores
static int cantidadJugadoresRecorridos = 0; //cuenta los jugadores registrados por el recorrido actual

//guarda hasta seis jugadores visitados para comprobar el orden de los recorridos de la ronda
static void registrarJugadorRecorrido(Jugador* jugador) {
    if (cantidadJugadoresRecorridos >= 6) { //evita escribir fuera del arreglo que recoge el recorrido
        return; //descarta una visita que no cabe en el arreglo
    }

    jugadoresRecorridos[cantidadJugadoresRecorridos] = jugador; //guarda el jugador en el orden en que fue visitado
    cantidadJugadoresRecorridos++; //aumenta la cantidad de visitas registradas
}

//comprueba los recorridos y las eliminaciones de una ronda circular en ambos sentidos hasta dejarla vacia
bool pruebaP07() {
    RondaJugadores ronda; //guarda los enlaces circulares que se van a probar
    inicializarRondaJugadores(ronda); //prepara una ronda sin participantes

    Jugador jugadorA{ "A", false, 100, 0, 0, 0, 0, false }; //prepara el primer jugador con el nucleo completo
    Jugador jugadorB{ "B", false, 100, 0, 0, 0, 0, false }; //prepara el segundo jugador con el nucleo completo
    Jugador jugadorC{ "C", false, 100, 0, 0, 0, 0, false }; //prepara el tercer jugador con el nucleo completo

    bool resultado = true; //acumula los fallos sin detener las comprobaciones restantes

    agregarJugador(ronda, &jugadorA); //agrega al primer jugador y lo deja como actual
    agregarJugador(ronda, &jugadorB); //agrega al segundo jugador de la ronda
    agregarJugador(ronda, &jugadorC); //completa los tres participantes de la ronda

    if (tamanoRonda(ronda) != 3) { //comprueba que la ronda contenga a los tres participantes
        resultado = false; //marca la prueba como fallida
    }

    if (verJugadorActual(ronda) != &jugadorA) { //revisa que el turno actual corresponda al primer jugador
        resultado = false; //marca la prueba como fallida
    }

    if (ronda.actual == nullptr) { //detecta la falta del nodo actual cuando deben quedar jugadores
        resultado = false; //marca la prueba como fallida
    }
    else if (ronda.actual->sig->ant != ronda.actual || ronda.actual->ant->sig != ronda.actual) { //revisa que los enlaces vecinos regresen al nodo actual
        resultado = false; //marca la prueba como fallida
    }

    cantidadJugadoresRecorridos = 0; //reinicia el conteo antes de comprobar otro recorrido
    recorrerRondaJugadores(ronda, true, registrarJugadorRecorrido); //registra el recorrido en sentido horario desde el jugador actual

    if (cantidadJugadoresRecorridos != 3) { //comprueba que el recorrido visite exactamente tres jugadores
        resultado = false; //marca la prueba como fallida
    }
    else { //revisa el orden de los jugadores despues de confirmar la cantidad de visitas
        if (jugadoresRecorridos[0] != &jugadorA) { //revisa que el recorrido empiece por el jugador actual
            resultado = false; //marca la prueba como fallida
        }

        if (jugadoresRecorridos[1] != &jugadorB) { //comprueba el segundo lugar del recorrido horario
            resultado = false; //marca la prueba como fallida
        }

        if (jugadoresRecorridos[2] != &jugadorC) { //comprueba el tercer lugar del recorrido horario
            resultado = false; //marca la prueba como fallida
        }
    }

    cantidadJugadoresRecorridos = 0; //reinicia el conteo antes de comprobar otro recorrido
    recorrerRondaJugadores(ronda, false, registrarJugadorRecorrido); //registra el recorrido en sentido antihorario

    if (cantidadJugadoresRecorridos != 3) { //comprueba que el recorrido visite exactamente tres jugadores
        resultado = false; //marca la prueba como fallida
    }
    else { //revisa el orden de los jugadores despues de confirmar la cantidad de visitas
        if (jugadoresRecorridos[0] != &jugadorA) { //revisa que el recorrido empiece por el jugador actual
            resultado = false; //marca la prueba como fallida
        }

        if (jugadoresRecorridos[1] != &jugadorC) { //revisa que el sentido inverso visite al tercer jugador en segundo lugar
            resultado = false; //marca la prueba como fallida
        }

        if (jugadoresRecorridos[2] != &jugadorB) { //revisa que el sentido inverso termine en el segundo jugador
            resultado = false; //marca la prueba como fallida
        }
    }

    avanzarJugador(ronda, true); //avanza un turno en sentido horario

    if (verJugadorActual(ronda) != &jugadorB) { //comprueba que el avance horario llegue al segundo jugador
        resultado = false; //marca la prueba como fallida
    }

    avanzarJugador(ronda, false); //retrocede un turno para volver al primer jugador

    if (verJugadorActual(ronda) != &jugadorA) { //revisa que el turno actual corresponda al primer jugador
        resultado = false; //marca la prueba como fallida
    }

    Jugador* eliminado = eliminarJugadorActual(ronda, true); //retira al primer jugador y avanza en sentido horario

    if (eliminado != &jugadorA) { //revisa que se devuelva el jugador retirado
        resultado = false; //marca la prueba como fallida
    }

    if (tamanoRonda(ronda) != 2 || verJugadorActual(ronda) != &jugadorB) { //comprueba que queden dos jugadores y el segundo tenga el turno
        resultado = false; //marca la prueba como fallida
    }

    if (ronda.actual == nullptr) { //detecta la falta del nodo actual cuando deben quedar jugadores
        resultado = false; //marca la prueba como fallida
    }
    else if (ronda.actual->sig->ant != ronda.actual || ronda.actual->ant->sig != ronda.actual) { //revisa que los enlaces vecinos regresen al nodo actual
        resultado = false; //marca la prueba como fallida
    }

    eliminado = eliminarJugadorActual(ronda, false); //retira al segundo jugador y avanza en sentido antihorario

    if (eliminado != &jugadorB) { //revisa que la segunda eliminacion devuelva al jugador esperado
        resultado = false; //marca la prueba como fallida
    }

    if (tamanoRonda(ronda) != 1 || verJugadorActual(ronda) != &jugadorC) { //comprueba que solo quede el tercer jugador con el turno
        resultado = false; //marca la prueba como fallida
    }

    if (ronda.actual == nullptr) { //detecta la falta del nodo actual cuando deben quedar jugadores
        resultado = false; //marca la prueba como fallida
    }
    else if (ronda.actual->sig != ronda.actual || ronda.actual->ant != ronda.actual) { //revisa que el unico nodo se enlace consigo mismo en ambos sentidos
        resultado = false; //marca la prueba como fallida
    }

    eliminado = eliminarJugadorActual(ronda, true); //retira al ultimo participante de la ronda

    if (eliminado != &jugadorC) { //comprueba que se devuelva al ultimo jugador retirado
        resultado = false; //marca la prueba como fallida
    }

    if (!rondaVacia(ronda) || ronda.actual != nullptr || tamanoRonda(ronda) != 0) { //revisa que la ronda quede vacia y sin nodo actual
        resultado = false; //marca la prueba como fallida
    }

    cantidadJugadoresRecorridos = 0; //reinicia el conteo antes de comprobar otro recorrido
    recorrerRondaJugadores(ronda, true, registrarJugadorRecorrido); //registra el recorrido en sentido horario desde el jugador actual

    if (cantidadJugadoresRecorridos != 0) { //detecta si se visito algun jugador en la ronda vacia
        resultado = false; //marca la prueba como fallida
    }

    if (eliminarJugadorActual(ronda, true) != nullptr) { //comprueba que eliminar de una ronda vacia no devuelva un jugador
        resultado = false; //marca la prueba como fallida
    }

    destruirRondaJugadores(ronda); //libera los nodos restantes sin destruir los jugadores locales

    return resultado; //devuelve el resultado de todas las comprobaciones realizadas
}

//verifica el primer valor conocido de la semilla cuarenta y dos y compara las siguientes cincuenta salidas
bool pruebaGeneradorDeterminista() {
    Generador generadorA; //guarda la primera secuencia de numeros
    Generador generadorB; //guarda la segunda secuencia para compararla

    inicializarGenerador(generadorA, 42); //inicia la primera secuencia con una semilla conocida
    inicializarGenerador(generadorB, 42); //usa la misma semilla para reproducir la secuencia

    uint32_t primeroA = siguienteNumero(generadorA); //obtiene el primer numero del primer generador
    uint32_t primeroB = siguienteNumero(generadorB); //obtiene el primer numero del segundo generador

    if (primeroA != 1250496027u || primeroB != 1250496027u) { //compara ambos primeros numeros con el valor esperado
        return false; //informa que la secuencia no cumple la comprobacion
    }

    for (int i = 0; i < 50; i++) { //compara cincuenta salidas adicionales de ambos generadores
        uint32_t valorA = siguienteNumero(generadorA); //avanza la primera secuencia
        uint32_t valorB = siguienteNumero(generadorB); //avanza la segunda secuencia

        if (valorA != valorB) { //detecta cualquier diferencia entre secuencias con la misma semilla
            return false; //informa que la secuencia no cumple la comprobacion
        }
    }

    return true; //confirma que el valor inicial y las secuencias coinciden
}

//vacia la cola de un turno y revisa que la cola original y la del otro turno se conserven
bool pruebaCopiasIndependientesPartida() {
    Partida partida; //guarda la partida que se usara para las comprobaciones
    inicializarPartida(partida, 42u, 10); //prepara una partida reproducible con limite de diez oleadas

    agregarJugadorPartida(partida, "A", false); //registra al primer participante como humano
    agregarJugadorPartida(partida, "B", false); //registra al segundo participante como humano

    generarColaOriginalRonda(partida); //genera las apariciones comunes a los turnos de la ronda

    ColaSpawn colaA; //guarda las apariciones del primer turno
    ColaSpawn colaB; //guarda las apariciones del segundo turno
    inicializarColaSpawn(colaA); //prepara el destino de la primera copia
    inicializarColaSpawn(colaB); //prepara el destino de la segunda copia

    prepararColaJugador(partida, colaA); //copia la oleada para el primer turno
    prepararColaJugador(partida, colaB); //copia la oleada para el segundo turno

    bool resultado = true; //acumula los fallos sin detener las comprobaciones restantes

    if (partida.colaOriginalRonda.tam <= 0) { //comprueba que se haya generado al menos una aparicion
        resultado = false; //marca la prueba como fallida
    }

    if (colaA.tam != partida.colaOriginalRonda.tam || colaB.tam != partida.colaOriginalRonda.tam) { //revisa que ambos turnos reciban la cantidad original de apariciones
        resultado = false; //marca la prueba como fallida
    }

    if (colaA.frente == partida.colaOriginalRonda.frente || colaB.frente == partida.colaOriginalRonda.frente) { //detecta si alguna copia comparte su primer nodo con la cola original
        resultado = false; //marca la prueba como fallida
    }

    if (colaA.frente == colaB.frente) { //comprueba que las colas de ambos turnos tampoco compartan el primer nodo
        resultado = false; //marca la prueba como fallida
    }

    int tamOriginal = partida.colaOriginalRonda.tam; //conserva la cantidad original para compararla despues
    int tamB = colaB.tam; //conserva la cantidad del segundo turno antes de vaciar el primero

    Spawn salida; //recibe la aparicion que se retire de la cola

    while (desencolarSpawn(colaA, salida)) { //retira todas las apariciones del primer turno hasta dejarlo vacio
    }

    if (!colaSpawnVacia(colaA)) { //revisa que la primera cola se haya vaciado por completo
        resultado = false; //marca la prueba como fallida
    }

    if (partida.colaOriginalRonda.tam != tamOriginal) { //comprueba que vaciar una copia no cambie la cantidad original
        resultado = false; //marca la prueba como fallida
    }

    if (colaB.tam != tamB) { //comprueba que vaciar el primer turno no cambie la cantidad del segundo
        resultado = false; //marca la prueba como fallida
    }

    if (partida.colaOriginalRonda.frente == nullptr || colaB.frente == nullptr) { //revisa que las otras dos colas conserven su primer nodo
        resultado = false; //marca la prueba como fallida
    }

    destruirColaSpawn(colaA); //libera cualquier aparicion pendiente del primer turno
    destruirColaSpawn(colaB); //libera las apariciones del segundo turno
    destruirPartida(partida); //libera los nodos y eventos usados por la partida de prueba

    return resultado; //devuelve el resultado de todas las comprobaciones realizadas
}

//comprueba el orden de los turnos en las dos primeras rondas y el inicio de la tercera
bool pruebaOrdenSerpenteantePartida() {
    Partida partida; //guarda la partida que se usara para las comprobaciones
    inicializarPartida(partida, 42u, 10); //prepara una partida reproducible con limite de diez oleadas

    agregarJugadorPartida(partida, "A", false); //registra al primer participante como humano
    agregarJugadorPartida(partida, "B", false); //registra al segundo participante como humano
    agregarJugadorPartida(partida, "C", false); //registra al tercer participante como humano

    bool resultado = true; //acumula los fallos sin detener las comprobaciones restantes

    if (partida.numeroRonda != 1 || !sentidoHorarioRondaPartida(partida)) { //comprueba que la primera ronda comience en sentido horario
        resultado = false; //marca la prueba como fallida
    }

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "A") { //revisa que el turno corresponda al primer participante
        resultado = false; //marca la prueba como fallida
    }

    avanzarTurnoPartida(partida); //avanza segun el sentido de la ronda actual

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "B") { //comprueba que el turno corresponda al segundo participante
        resultado = false; //marca la prueba como fallida
    }

    avanzarTurnoPartida(partida); //avanza segun el sentido de la ronda actual

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "C") { //comprueba que el turno corresponda al tercer participante
        resultado = false; //marca la prueba como fallida
    }

    finalizarRondaPartida(partida); //cierra la ronda y aplica la regla de avance o fin de partida

    if (partida.numeroRonda != 2 || sentidoHorarioRondaPartida(partida)) { //revisa que la segunda ronda use el sentido antihorario
        resultado = false; //marca la prueba como fallida
    }

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "C") { //comprueba que el turno corresponda al tercer participante
        resultado = false; //marca la prueba como fallida
    }

    avanzarTurnoPartida(partida); //avanza segun el sentido de la ronda actual

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "B") { //comprueba que el turno corresponda al segundo participante
        resultado = false; //marca la prueba como fallida
    }

    avanzarTurnoPartida(partida); //avanza segun el sentido de la ronda actual

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "A") { //revisa que el turno corresponda al primer participante
        resultado = false; //marca la prueba como fallida
    }

    finalizarRondaPartida(partida); //cierra la ronda y aplica la regla de avance o fin de partida

    if (partida.numeroRonda != 3 || !sentidoHorarioRondaPartida(partida)) { //comprueba que la tercera ronda vuelva al sentido horario
        resultado = false; //marca la prueba como fallida
    }

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "A") { //revisa que el turno corresponda al primer participante
        resultado = false; //marca la prueba como fallida
    }

    destruirPartida(partida); //libera los nodos y eventos usados por la partida de prueba

    return resultado; //devuelve el resultado de todas las comprobaciones realizadas
}

//revisa que los eliminados conserven sus resultados y que la partida termine al quedar cero activos o uno al cerrar la ronda
//tambien comprueba que alcanzar el limite de oleadas termine la partida
bool pruebaEliminacionesPartida() {
    Partida partida; //guarda la partida que se usara para las comprobaciones
    inicializarPartida(partida, 42u, 10); //prepara una partida reproducible con limite de diez oleadas

    agregarJugadorPartida(partida, "A", false); //registra al primer participante como humano
    agregarJugadorPartida(partida, "B", false); //registra al segundo participante como humano
    agregarJugadorPartida(partida, "C", false); //registra al tercer participante como humano

    bool resultado = true; //acumula los fallos sin detener las comprobaciones restantes

    avanzarTurnoPartida(partida); //avanza segun el sentido de la ronda actual

    Jugador* jugadorB = verJugadorActualPartida(partida); //obtiene al participante que se eliminara primero

    if (jugadorB == nullptr || jugadorB->alias != "B") { //comprueba que el segundo participante tenga el turno
        resultado = false; //marca la prueba como fallida
    }
    else { //prepara la eliminacion despues de comprobar que se obtuvo al jugador esperado
        jugadorB->vidaNucleo = 0; //deja al segundo jugador sin vida para provocar su eliminacion
        jugadorB->puntaje = 500; //asigna un puntaje que debe conservarse despues de retirarlo
        jugadorB->oleadaAlcanzada = 1; //registra la oleada alcanzada antes de la eliminacion
    }

    if (!retirarJugadorActualSiEliminado(partida)) { //comprueba que se retire al jugador actual cuando ya no tiene vida
        resultado = false; //marca la prueba como fallida
    }

    if (!partida.jugadores[1].eliminado || partida.jugadores[1].puntaje != 500) { //revisa la marca de eliminacion y que se conserve el puntaje
        resultado = false; //marca la prueba como fallida
    }

    if (cantidadJugadoresPartida(partida) != 3 || cantidadJugadoresActivos(partida) != 2) { //distingue los tres jugadores registrados de los dos que siguen activos
        resultado = false; //marca la prueba como fallida
    }

    if (verJugadorActualPartida(partida) == nullptr || verJugadorActualPartida(partida)->alias != "C") { //comprueba que el turno corresponda al tercer participante
        resultado = false; //marca la prueba como fallida
    }

    Jugador* jugadorC = verJugadorActualPartida(partida); //obtiene al siguiente participante para eliminarlo

    if (jugadorC != nullptr) { //comprueba que exista el participante antes de cambiar su vida
        jugadorC->vidaNucleo = 0; //deja sin vida al tercer participante
    }

    if (!retirarJugadorActualSiEliminado(partida)) { //comprueba que se retire al jugador actual cuando ya no tiene vida
        resultado = false; //marca la prueba como fallida
    }

    if (cantidadJugadoresActivos(partida) != 1) { //revisa que quede exactamente un jugador activo
        resultado = false; //marca la prueba como fallida
    }

    if (partida.finalizada) { //detecta si la partida termino antes de cerrar la ronda con un unico activo
        resultado = false; //marca la prueba como fallida
    }

    finalizarRondaPartida(partida); //cierra la ronda y aplica la regla de avance o fin de partida

    if (!partida.finalizada) { //comprueba que cerrar la ronda con un activo termine la partida
        resultado = false; //marca la prueba como fallida
    }

    destruirPartida(partida); //libera los nodos y eventos usados por la partida de prueba

    Partida partidaSinActivos; //guarda el caso en que no queda ningun jugador con vida
    inicializarPartida(partidaSinActivos, 42u, 10); //prepara la partida para eliminar a su unico participante
    agregarJugadorPartida(partidaSinActivos, "SOLO", false); //registra al unico participante del caso

    partidaSinActivos.jugadores[0].vidaNucleo = 0; //deja sin vida al ultimo jugador activo

    if (!retirarJugadorActualSiEliminado(partidaSinActivos)) { //revisa que se retire al unico jugador eliminado
        resultado = false; //marca la prueba como fallida
    }

    if (!partidaSinActivos.finalizada || cantidadJugadoresActivos(partidaSinActivos) != 0) { //comprueba que la partida termine al quedarse sin jugadores activos
        resultado = false; //marca la prueba como fallida
    }

    destruirPartida(partidaSinActivos); //libera la partida usada para el caso sin activos

    Partida partidaLimite; //guarda el caso que termina por limite de oleadas
    inicializarPartida(partidaLimite, 42u, 1); //limita la partida a una sola oleada
    agregarJugadorPartida(partidaLimite, "A", false); //agrega el primer participante del caso limitado
    agregarJugadorPartida(partidaLimite, "B", false); //agrega otro participante para evitar el fin por unico activo

    finalizarRondaPartida(partidaLimite); //cierra la unica ronda permitida

    if (!partidaLimite.finalizada || partidaLimite.numeroRonda != 1) { //comprueba que la partida termine sin avanzar mas alla del limite
        resultado = false; //marca la prueba como fallida
    }

    destruirPartida(partidaLimite); //libera la partida usada para el limite de oleadas

    return resultado; //devuelve el resultado de todas las comprobaciones realizadas
}

//comprueba la eleccion por puntaje los desempates por oleada y precision y el empate sin precision disponible
bool pruebaGanadorPartida() {
    Partida partida; //guarda la partida que se usara para las comprobaciones
    inicializarPartida(partida, 42u, 10); //prepara una partida reproducible con limite de diez oleadas

    agregarJugadorPartida(partida, "A", false); //registra al primer participante como humano
    agregarJugadorPartida(partida, "B", false); //registra al segundo participante como humano
    agregarJugadorPartida(partida, "C", false); //registra al tercer participante como humano

    bool resultado = true; //acumula los fallos sin detener las comprobaciones restantes

    partida.jugadores[0].puntaje = 1000; //asigna al primer jugador un puntaje menor que el del segundo
    partida.jugadores[1].puntaje = 1500; //fija el puntaje del segundo jugador en mil quinientos
    partida.jugadores[2].puntaje = 1200; //deja al tercer jugador con un puntaje intermedio

    const Jugador* ganador = determinarGanadorPartida(partida, nullptr); //busca el ganador sin aportar precisiones para el desempate

    if (ganador == nullptr || ganador->alias != "B") { //comprueba que las reglas elijan al segundo participante
        resultado = false; //marca la prueba como fallida
    }

    partida.jugadores[0].puntaje = 1500; //iguala el puntaje del primer jugador con el del segundo
    partida.jugadores[1].puntaje = 1500; //fija el puntaje del segundo jugador en mil quinientos
    partida.jugadores[0].oleadaAlcanzada = 4; //deja al primero una oleada por debajo para probar el desempate
    partida.jugadores[1].oleadaAlcanzada = 5; //fija en cinco la oleada alcanzada por el segundo jugador

    ganador = determinarGanadorPartida(partida, nullptr); //repite la eleccion con los datos modificados y sin precisiones

    if (ganador == nullptr || ganador->alias != "B") { //comprueba que las reglas elijan al segundo participante
        resultado = false; //marca la prueba como fallida
    }

    partida.jugadores[0].oleadaAlcanzada = 5; //iguala tambien la oleada de los dos jugadores empatados
    partida.jugadores[1].oleadaAlcanzada = 5; //fija en cinco la oleada alcanzada por el segundo jugador

    ganador = determinarGanadorPartida(partida, nullptr); //repite la eleccion con los datos modificados y sin precisiones

    if (ganador != nullptr) { //detecta si se elige un ganador cuando falta el dato para desempatar
        resultado = false; //marca la prueba como fallida
    }

    double precisiones[6] = { 90.0, 95.0, 80.0, 0.0, 0.0, 0.0 }; //asigna la mayor precision al segundo jugador para resolver el empate

    ganador = determinarGanadorPartida(partida, precisiones); //aplica el desempate usando las precisiones proporcionadas

    if (ganador == nullptr || ganador->alias != "B") { //comprueba que las reglas elijan al segundo participante
        resultado = false; //marca la prueba como fallida
    }

    destruirPartida(partida); //libera los nodos y eventos usados por la partida de prueba

    return resultado; //devuelve el resultado de todas las comprobaciones realizadas
}

//compara todos los campos de dos acciones para verificar que el modo por lotes elija la accion esperada
static bool accionesMotorIguales(const AccionMotor& a, const AccionMotor& b) {
    return a.cambiarAngulo == b.cambiarAngulo && a.anguloObjetivo == b.anguloObjetivo && a.cambiarColor == b.cambiarColor && a.colorNuevo == b.colorNuevo && a.solicitarPulso == b.solicitarPulso; //exige coincidencia de las banderas y los valores de angulo color y pulso
}

//compara la accion del modo por lotes con la del script vacio para un humano y con la del bot ante un enemigo
bool pruebaIntegracionModoLotes() {
    ScriptAcciones script; //guarda el script vacio usado para comprobar la seleccion de acciones
    inicializarScriptAcciones(script); //prepara el script sin acciones registradas

    bool resultado = true; //acumula los fallos sin detener las comprobaciones restantes

    Jugador humano{ "HUMANO", false, 100, 0, 0, 0, 0, false }; //crea al participante que debe recibir las acciones del script

    Motor motorHumano{}; //inicia en cero el motor usado solo para consultar la accion humana
    motorHumano.jugador = &humano; //vincula el motor con el participante humano
    motorHumano.numeroOleada = 1; //situa la consulta en la primera oleada
    motorHumano.tickActual = 0; //consulta la accion del primer tick
    motorHumano.nave.anguloActual = 270.0; //coloca la nave en su angulo inicial
    motorHumano.nave.anguloObjetivo = 270.0; //mantiene el destino de la nave en el angulo inicial
    motorHumano.nave.color = 0; //usa la fase cian como color actual

    AccionMotor accionScript = obtenerAccionScript(script, humano.alias, 1, 0); //obtiene directamente la accion esperada del script vacio
    AccionMotor accionHumano = obtenerAccionModoLotes(motorHumano, script); //consulta la accion elegida por el modo por lotes para el humano

    if (!accionesMotorIguales(accionScript, accionHumano)) { //detecta si el modo por lotes altera la accion del script
        resultado = false; //marca la prueba como fallida
    }

    Jugador bot{ "BOT1", true, 100, 0, 0, 0, 0, false }; //crea al participante que debe usar la decision automatica

    Motor motorBot{}; //inicia en cero el motor usado para consultar la accion del bot
    motorBot.jugador = &bot; //vincula el motor con el participante automatico
    motorBot.numeroOleada = 1; //prepara la consulta del bot en la primera oleada
    motorBot.tickActual = 0; //situa al bot en el primer tick de su turno
    motorBot.nave.anguloActual = 270.0; //coloca la nave del bot en su angulo inicial
    motorBot.nave.anguloObjetivo = 270.0; //mantiene el destino inicial hasta consultar la accion
    motorBot.nave.color = 0; //deja la nave del bot en fase cian
    inicializarListaEnemigos(motorBot.enemigos); //prepara la lista que recibira el objetivo del bot

    Enemigo enemigo; //guarda el enemigo que se usara como objetivo
    enemigo.id = 1; //asigna el identificador del objetivo
    enemigo.tipo = Dron; //usa un dron como enemigo de prueba
    enemigo.color = 2; //situa el objetivo en fase ambar
    enemigo.r = 120.0; //coloca el enemigo fuera de la orbita de la nave
    enemigo.theta = 90.0; //pone el objetivo en un angulo distinto al de la nave
    enemigo.omega = 0.0; //deja al objetivo sin giro angular
    enemigo.vida = 1; //asigna un punto de vida al dron
    enemigo.radio = 6; //define el radio de colision del objetivo
    enemigo.ticksColor = 0; //reinicia el contador asociado al cambio de color

    insertarFinal(motorBot.enemigos, enemigo); //agrega el objetivo que observara el bot

    AccionMotor accionBotDirecta = obtenerAccionBot(motorBot); //consulta directamente la decision automatica esperada
    AccionMotor accionBotIntegrada = obtenerAccionModoLotes(motorBot, script); //obtiene la decision del bot a traves del modo por lotes

    if (!accionesMotorIguales(accionBotDirecta, accionBotIntegrada)) { //detecta si la integracion cambia la decision del bot
        resultado = false; //marca la prueba como fallida
    }

    destruirListaEnemigos(motorBot.enemigos); //libera el nodo creado para el objetivo del bot
    destruirScriptAcciones(script); //libera las acciones que pudiera conservar el script

    return resultado; //devuelve el resultado de todas las comprobaciones realizadas
}

//prepara un evento con los datos recibidos para probar la bitacora y sus filtros
static Evento crearEventoPrueba(int tick, int oleada, const string& jugador, TipoEvento tipoEvento, int tipoEnemigo, int color, int valor) {
    Evento evento; //guarda los datos que se agregaran a la bitacora
    evento.tick = tick; //indica el tick en que ocurre el evento
    evento.oleada = oleada; //asocia el evento con la oleada recibida
    evento.jugador = jugador; //guarda el alias del participante relacionado
    evento.tipoEvento = tipoEvento; //identifica la clase de suceso registrado
    evento.tipoEnemigo = tipoEnemigo; //guarda el tipo de enemigo o el valor que indica que no corresponde
    evento.color = color; //guarda la fase relacionada o el valor que indica que no corresponde
    evento.valor = valor; //conserva la cantidad asociada al suceso
    return evento; //entrega el evento preparado sin insertarlo en ninguna lista
}

//comprueba el orden de insercion los filtros por jugador y oleada y el vaciado de la bitacora
bool pruebaBitacoraEventos() {
    BitacoraEventos bitacora; //guarda los eventos originales de la prueba
    inicializarBitacora(bitacora); //prepara la bitacora vacia

    bool resultado = true; //acumula los fallos sin detener las comprobaciones restantes

    if (bitacora.cabeza != nullptr || bitacora.cola != nullptr || bitacora.tam != 0) { //revisa que la bitacora vacia no conserve nodos ni elementos registrados
        resultado = false; //marca la prueba como fallida
    }

    Evento evento1 = crearEventoPrueba(10, 1, "A", BAJA, 0, 1, 100); //prepara una baja del primer jugador en la primera oleada
    Evento evento2 = crearEventoPrueba(20, 1, "B", PULSO, -1, -1, 0); //prepara un pulso del segundo jugador sin enemigo ni color asociado
    Evento evento3 = crearEventoPrueba(30, 2, "A", DANO_NUCLEO, 4, 2, 22); //prepara dano al primer jugador en la segunda oleada
    Evento evento4 = crearEventoPrueba(40, 2, "A", FIN_OLEADA, -1, -1, 0); //prepara el fin de la segunda oleada del primer jugador

    agregarEventoFinal(bitacora, evento1); //inserta el primer evento para revisar el caso de un solo nodo

    if (bitacora.cabeza == nullptr || bitacora.cola == nullptr || bitacora.cabeza != bitacora.cola || bitacora.tam != 1) { //comprueba que un solo evento sea a la vez cabeza y cola
        resultado = false; //marca la prueba como fallida
    }

    agregarEventoFinal(bitacora, evento2); //agrega el segundo evento respetando el orden de llegada
    agregarEventoFinal(bitacora, evento3); //agrega el primer evento de la segunda oleada
    agregarEventoFinal(bitacora, evento4); //deja el fin de oleada como ultimo evento

    if (bitacora.tam != 4) { //comprueba que se hayan registrado los cuatro eventos
        resultado = false; //marca la prueba como fallida
    }

    if (bitacora.cabeza == nullptr || bitacora.cabeza->dato.tick != 10) { //revisa que el evento inicial siga en la cabeza
        resultado = false; //marca la prueba como fallida
    }

    if (bitacora.cabeza != nullptr && bitacora.cabeza->sig != nullptr && bitacora.cabeza->sig->dato.tick != 20) { //comprueba el tick del segundo nodo cuando esta disponible
        resultado = false; //marca la prueba como fallida
    }

    if (bitacora.cola == nullptr || bitacora.cola->dato.tick != 40 || bitacora.cola->sig != nullptr) { //revisa el ultimo evento y que no tenga un nodo siguiente
        resultado = false; //marca la prueba como fallida
    }

    BitacoraEventos porJugador; //guarda el resultado del filtro por participante
    inicializarBitacora(porJugador); //prepara la lista que recibira los eventos del primer jugador
    filtrarPorJugador(bitacora, "A", porJugador); //copia los eventos que corresponden al primer jugador

    if (porJugador.tam != 3) { //revisa que se recuperen sus tres eventos
        resultado = false; //marca la prueba como fallida
    }

    const NodoEvento* actual = porJugador.cabeza; //empieza a revisar los eventos filtrados por jugador

    while (actual != nullptr) { //recorre los eventos del filtro que se esta comprobando
        if (actual->dato.jugador != "A") { //detecta un evento de otro jugador dentro del filtro
            resultado = false; //marca la prueba como fallida
        }

        actual = actual->sig; //avanza al siguiente evento filtrado
    }

    BitacoraEventos porOleada; //guarda el resultado del filtro por numero de oleada
    inicializarBitacora(porOleada); //prepara la lista que recibira los eventos de la segunda oleada
    filtrarPorOleada(bitacora, 2, porOleada); //copia los eventos de la segunda oleada

    if (porOleada.tam != 2) { //comprueba que el filtro recupere los dos eventos esperados
        resultado = false; //marca la prueba como fallida
    }

    actual = porOleada.cabeza; //empieza a recorrer los eventos filtrados por oleada

    while (actual != nullptr) { //recorre los eventos del filtro que se esta comprobando
        if (actual->dato.oleada != 2) { //detecta un evento de otra oleada dentro del filtro
            resultado = false; //marca la prueba como fallida
        }

        actual = actual->sig; //avanza al siguiente evento filtrado
    }

    if (porJugador.cabeza == bitacora.cabeza || porOleada.cabeza == bitacora.cabeza) { //revisa que los filtros no reutilicen la cabeza de la bitacora original
        resultado = false; //marca la prueba como fallida
    }

    destruirBitacora(porJugador); //libera los nodos del filtro por jugador
    destruirBitacora(porOleada); //libera los nodos del filtro por oleada
    destruirBitacora(bitacora); //libera todos los eventos originales de la prueba

    if (bitacora.cabeza != nullptr || bitacora.cola != nullptr || bitacora.tam != 0) { //revisa que la bitacora vacia no conserve nodos ni elementos registrados
        resultado = false; //marca la prueba como fallida
    }

    return resultado; //devuelve el resultado de todas las comprobaciones realizadas
}

//comprueba que los alias repetidos se rechacen sin modificar los jugadores registrados
bool pruebaRegistroAliasUnicos() {
    Partida partida; //guarda la partida utilizada para revisar el registro
    inicializarPartida(partida, 42, 1); //prepara una partida con semilla fija y una oleada
    bool resultado = true; //conserva cualquier fallo encontrado

    if (!agregarJugadorPartida(partida, "Ana", false)) resultado = false; //registra al primer participante
    if (!agregarJugadorPartida(partida, "Luis", true)) resultado = false; //registra al segundo participante

    partida.jugadores[0].puntaje = 350; //coloca un puntaje que debe conservarse
    partida.jugadores[0].vidaNucleo = 72; //coloca una vida que debe conservarse
    partida.jugadores[0].cargaPulso = 40; //coloca una carga que debe conservarse
    partida.jugadores[0].metricas.disparos = 12; //coloca una estadistica que debe conservarse

    NodoJugador* actual = partida.ronda.actual; //guarda el nodo que tiene el turno
    NodoJugador* siguiente = actual != nullptr ? actual->sig : nullptr; //guarda su enlace siguiente
    NodoJugador* anterior = actual != nullptr ? actual->ant : nullptr; //guarda su enlace anterior

    if (agregarJugadorPartida(partida, "Ana", true)) resultado = false; //comprueba que el alias repetido sea rechazado
    if (partida.cantidadJugadores != 2 || partida.ronda.tam != 2 || partida.ronda.actual != actual) resultado = false; //comprueba que no cambien la cantidad ni el turno
    if (actual == nullptr || actual->sig != siguiente || actual->ant != anterior) resultado = false; //comprueba que se conserven los enlaces de la ronda

    if (partida.jugadores[0].alias != "Ana" || partida.jugadores[0].esBot || partida.jugadores[0].puntaje != 350 || partida.jugadores[0].vidaNucleo != 72 || partida.jugadores[0].cargaPulso != 40 || partida.jugadores[0].metricas.disparos != 12) { //revisa que el rechazo no altere los datos existentes
        resultado = false; //registra que algun dato fue modificado
    }

    const string restantes[] = { "Bea", "Carlos", "Diana", "Elena" }; //prepara los nombres que completaran los seis participantes

    for (const string& alias : restantes) { //recorre los cuatro nombres pendientes
        if (!agregarJugadorPartida(partida, alias, false)) resultado = false; //comprueba que se acepten los nombres distintos
    }

    if (agregarJugadorPartida(partida, "Septimo", false)) resultado = false; //comprueba que no se permita un septimo participante
    if (partida.cantidadJugadores != 6 || partida.ronda.tam != 6 || partida.ronda.actual != actual) resultado = false; //revisa que se conserve el limite y el turno inicial

    NodoJugador* nodo = partida.ronda.actual; //comienza el recorrido de la lista circular

    for (int i = 0; i < 6; i++) { //revisa los seis jugadores registrados
        if (nodo == nullptr || nodo->jugador != &partida.jugadores[i] || nodo->sig == nullptr || nodo->ant == nullptr || nodo->sig->ant != nodo || nodo->ant->sig != nodo) { //comprueba el orden y los enlaces de cada nodo
            resultado = false; //registra un fallo en la estructura
            break; //evita continuar por enlaces incorrectos
        }

        nodo = nodo->sig; //avanza al siguiente participante
    }

    if (nodo != partida.ronda.actual) resultado = false; //comprueba que el recorrido termine en el nodo inicial

    Partida otraPartida; //prepara una partida independiente
    inicializarPartida(otraPartida, 7, 1); //inicializa la segunda partida
    if (!agregarJugadorPartida(otraPartida, "Ana", false)) resultado = false; //comprueba que el mismo alias pueda usarse en otra partida

    destruirPartida(otraPartida); //libera los recursos de la segunda partida
    destruirPartida(partida); //libera los recursos de la primera partida

    return resultado; //devuelve el resultado de todas las comprobaciones
}

//comprueba que las estadisticas se separen por jugador y se acumulen entre partidas
bool pruebaEstadisticasAliasUnicos() {
    HistorialPartidas historial; //guarda los resultados de las partidas de prueba
    inicializarHistorial(historial); //prepara el historial vacio

    ScriptAcciones script; //prepara el script utilizado por el modo por lotes
    inicializarScriptAcciones(script); //deja el script vacio porque los participantes son bots

    bool resultado = true; //conserva cualquier fallo encontrado

    for (int idPartida = 1; idPartida <= 2; idPartida++) { //ejecuta dos partidas con los mismos participantes
        Partida partida; //guarda la partida del ciclo actual
        inicializarPartida(partida, 42, 1); //usa la misma semilla y una sola oleada

        if (!agregarJugadorPartida(partida, "BOT1", true)) resultado = false; //registra al primer bot
        if (!agregarJugadorPartida(partida, "BOT2", true)) resultado = false; //registra al segundo bot
        if (agregarJugadorPartida(partida, "BOT1", true)) resultado = false; //comprueba que el primer alias no pueda repetirse
        if (!ejecutarRondaModoLotes(partida, script, 10000) || !partida.finalizada) resultado = false; //ejecuta la partida y comprueba que termine
        if (partida.cantidadJugadores != 2) resultado = false; //comprueba que solamente existan los dos bots originales

        for (int i = 0; i < partida.cantidadJugadores; i++) { //revisa los resultados individuales
            const Jugador& jugador = partida.jugadores[i]; //consulta al jugador actual
            ResumenOleadaJugador resumen; //recibe su resumen calculado desde la bitacora

            if (jugador.puntaje != 900 || !calcularResumenOleadaJugador(partida.bitacora, jugador.alias, 1, resumen) || resumen.puntajeAcumulado != 900 || resumen.bajas != 9) { //comprueba que la linea de tiempo no mezcle los resultados
                resultado = false; //registra una diferencia en los resultados individuales
            }
        }

        agregarResultadosPartidaHistorial(partida, idPartida, "lotes", historial); //agrega los resultados de esta partida al historial

        for (int i = 0; i < partida.cantidadJugadores; i++) { //revisa la continuidad historica de ambos jugadores
            EstadisticasJugador estadisticas; //recibe los resultados historicos del participante

            if (!calcularEstadisticasJugador(historial, partida.jugadores[i].alias, estadisticas) || estadisticas.partidasJugadas != idPartida || estadisticas.mediaPuntaje != 900.0) { //comprueba el numero de partidas y el promedio individual
                resultado = false; //registra un error en la acumulacion historica
            }
        }

        destruirPartida(partida); //libera los recursos antes de iniciar la siguiente partida
    }

    if (historial.tam != 4) resultado = false; //comprueba que existan dos registros por cada una de las dos partidas

    destruirScriptAcciones(script); //libera las acciones del script
    destruirHistorial(historial); //libera los registros del historial

    return resultado; //devuelve el resultado de las comprobaciones
}