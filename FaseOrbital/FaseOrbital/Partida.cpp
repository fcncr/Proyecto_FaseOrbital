#include "Partida.h"
#include "Oleada.h"

void inicializarPartida(Partida& partida, uint32_t semillaPartida, int limiteOleadas) {
    partida.cantidadJugadores = 0; //empieza sin participantes registrados
    inicializarRondaJugadores(partida.ronda); //prepara la lista que llevara el orden de los turnos
    inicializarColaSpawn(partida.colaOriginalRonda); //deja vacia la cola de apariciones compartida como modelo
    inicializarBitacora(partida.bitacora); //prepara la lista donde se guardaran los eventos
    partida.numeroRonda = 1; //comienza por la primera ronda
    partida.semillaPartida = semillaPartida; //guarda la semilla con la que se generaran las oleadas
    partida.limiteOleadas = limiteOleadas; //guarda hasta que oleada puede llegar la partida
    partida.finalizada = false; //deja la partida disponible para jugar
}

bool agregarJugadorPartida(Partida& partida, const string& alias, bool esBot) {
    if (partida.cantidadJugadores >= 6) { //comprueba si ya se ocuparon los seis espacios disponibles
        return false; //indica que no cabe otro participante
    }

    Jugador& jugador = partida.jugadores[partida.cantidadJugadores]; //trabaja directamente sobre el siguiente espacio libre del arreglo
    jugador.alias = alias; //guarda el nombre con el que se identificara al participante
    jugador.esBot = esBot; //indica si sus acciones las decidira el programa
    jugador.vidaNucleo = 100; //le asigna la vida inicial del nucleo
    jugador.puntaje = 0; //empieza sin puntos acumulados
    jugador.combo = 0; //empieza con el combo en cero
    jugador.cargaPulso = 0; //empieza sin carga para activar el pulso
    jugador.oleadaAlcanzada = 0; //todavia no ha iniciado ninguna oleada
    jugador.eliminado = false; //lo registra como participante activo
    inicializarMetricasJugador(jugador.metricas); //reinicia sus estadisticas una sola vez al registrarlo

    agregarJugador(partida.ronda, &jugador); //agrega a la ronda un nodo que apunta al jugador del arreglo
    partida.cantidadJugadores++; //cuenta al nuevo participante aunque despues pueda ser eliminado

    return true; //confirma que el jugador quedo registrado y enlazado en la ronda
}

Jugador* verJugadorPartida(Partida& partida, int indice) {
    if (indice < 0 || indice >= partida.cantidadJugadores) { //comprueba que la posicion pertenezca a un participante registrado
        return nullptr; //indica que no existe un jugador en esa posicion
    }

    return &partida.jugadores[indice]; //devuelve el jugador original para poder consultar o modificar sus datos
}

const Jugador* verJugadorPartida(const Partida& partida, int indice) {
    if (indice < 0 || indice >= partida.cantidadJugadores) { //comprueba que la posicion este dentro de los participantes registrados
        return nullptr; //indica que la posicion recibida no es valida
    }

    return &partida.jugadores[indice]; //permite consultar al jugador sin modificarlo mediante este puntero
}

Jugador* verJugadorActualPartida(Partida& partida) {
    return verJugadorActual(partida.ronda); //consulta a quien le corresponde jugar o devuelve nullptr si no hay turno
}

int cantidadJugadoresPartida(const Partida& partida) {
    return partida.cantidadJugadores; //cuenta todos los participantes registrados incluidos los eliminados
}

int cantidadJugadoresActivos(const Partida& partida) {
    return tamanoRonda(partida.ronda); //cuenta solamente los jugadores que siguen enlazados en la ronda
}

void destruirPartida(Partida& partida) {
    destruirRondaJugadores(partida.ronda); //borra los nodos de los turnos sin destruir los jugadores del arreglo
    destruirColaSpawn(partida.colaOriginalRonda); //libera las apariciones guardadas para la ronda
    destruirBitacora(partida.bitacora); //libera todos los eventos registrados
    partida.cantidadJugadores = 0; //deja de considerar registrados los jugadores del arreglo
    partida.finalizada = true; //marca que la partida ya no se puede continuar
}

void generarColaOriginalRonda(Partida& partida) {
    Oleada oleada; //reune los datos necesarios para generar los enemigos de esta ronda
    oleada.numero = partida.numeroRonda; //usa el numero de ronda como numero de oleada
    oleada.semillaPartida = partida.semillaPartida; //conserva la semilla comun de la partida
    generarOleada(oleada, partida.colaOriginalRonda); //reemplaza la cola original con las apariciones de esta oleada
}

void prepararColaJugador(const Partida& partida, ColaSpawn& colaJugador) {
    copiarColaSpawn(partida.colaOriginalRonda, colaJugador); //copia las apariciones en nodos propios para este jugador
}

bool sentidoHorarioRondaPartida(const Partida& partida) {
    return partida.numeroRonda % 2 != 0; //usa sentido horario en rondas impares y antihorario en rondas pares
}

void avanzarTurnoPartida(Partida& partida) {
    if (rondaVacia(partida.ronda)) { //revisa si quedan jugadores que puedan recibir el turno
        return; //sale porque una ronda vacia no puede avanzar
    }
    avanzarJugador(partida.ronda, sentidoHorarioRondaPartida(partida)); //mueve el turno al vecino que corresponde segun el sentido de la ronda
}

void finalizarRondaPartida(Partida& partida) {
    if (partida.finalizada) { //revisa si la partida ya habia terminado
        return; //evita volver a cerrar o avanzar una partida finalizada
    }

    if (cantidadJugadoresActivos(partida) <= 1) { //al cerrar la ronda comprueba si queda uno o ningun jugador activo
        partida.finalizada = true; //termina la partida porque ya no hay varios jugadores en competencia
        return; //sale sin comenzar otra ronda
    }

    if (partida.numeroRonda >= partida.limiteOleadas) { //comprueba si ya se jugo la ultima oleada permitida
        partida.finalizada = true; //termina la partida al alcanzar el limite
        return; //sale sin aumentar el numero de ronda
    }

    partida.numeroRonda++; //pasa a la siguiente ronda conservando al jugador actual para invertir el recorrido
}

bool retirarJugadorActualSiEliminado(Partida& partida) {
    Jugador* jugador = verJugadorActualPartida(partida); //busca al jugador que acaba de ocupar el turno

    if (jugador == nullptr) { //revisa si ya no existe un jugador actual
        partida.finalizada = true; //termina la partida porque no hay un turno disponible
        return false; //indica que esta llamada no retiro a ningun jugador
    }

    if (jugador->vidaNucleo > 0) { //comprueba si el nucleo del jugador todavia tiene vida
        return false; //lo mantiene en la ronda porque sigue jugando
    }

    jugador->eliminado = true; //marca su salida conservando los resultados en el arreglo de jugadores
    eliminarJugadorActual(partida.ronda, sentidoHorarioRondaPartida(partida)); //borra su nodo y deja el turno en el vecino que corresponde

    if (rondaVacia(partida.ronda)) { //comprueba si salio el ultimo jugador activo
        partida.finalizada = true; //termina de inmediato cuando no queda nadie en la ronda
    }

    return true; //confirma que se retiro al jugador que estaba en turno
}

const Jugador* determinarGanadorPartida(const Partida& partida, const double* precisiones) {
    if (partida.cantidadJugadores <= 0) { //comprueba si la partida no tiene participantes registrados
        return nullptr; //no puede elegir un ganador sin jugadores
    }

    int mejor = 0; //toma al primer participante como referencia inicial
    bool empateSinResolver = false; //indica si varios jugadores comparten el mejor resultado encontrado

    for (int i = 1; i < partida.cantidadJugadores; i++) { //compara a todos los demas participantes incluidos los eliminados
        const Jugador& candidato = partida.jugadores[i]; //consulta al jugador que se esta evaluando
        const Jugador& actualMejor = partida.jugadores[mejor]; //consulta al jugador que lleva el mejor resultado hasta ahora

        if (candidato.puntaje > actualMejor.puntaje) { //el primer criterio es tener mas puntos
            mejor = i; //deja al candidato como nuevo mejor jugador
            empateSinResolver = false; //el nuevo puntaje supera cualquier empate anterior
            continue; //pasa al siguiente participante sin revisar otros criterios
        }

        if (candidato.puntaje < actualMejor.puntaje) { //descarta al candidato si tiene menos puntos
            continue; //mantiene el mejor resultado y revisa al siguiente jugador
        }

        if (candidato.oleadaAlcanzada > actualMejor.oleadaAlcanzada) { //con puntos iguales gana quien llego a una oleada mayor
            mejor = i; //guarda al candidato que avanzo mas
            empateSinResolver = false; //la diferencia de oleada resuelve el empate anterior
            continue; //no necesita comparar la precision de este candidato
        }

        if (candidato.oleadaAlcanzada < actualMejor.oleadaAlcanzada) { //descarta al candidato si alcanzo una oleada menor
            continue; //conserva al mejor jugador encontrado
        }

        if (precisiones == nullptr) { //comprueba si faltan las precisiones para intentar el ultimo desempate
            empateSinResolver = true; //deja pendiente el empate de puntos y oleada
            continue; //sigue buscando por si otro jugador supera ese resultado
        }

        if (precisiones[i] > precisiones[mejor]) { //con puntos y oleada iguales compara las precisiones recibidas
            mejor = i; //elige al candidato que tiene mayor precision
            empateSinResolver = false; //la precision mayor elimina el empate anterior
            continue; //continua con el siguiente participante
        }

        if (precisiones[i] == precisiones[mejor]) { //revisa si tambien coinciden en la precision
            empateSinResolver = true; //marca que todavia no hay un ganador unico
        }
    }

    if (empateSinResolver) { //comprueba si el mejor resultado quedo compartido
        return nullptr; //indica que los criterios disponibles no dieron un ganador unico
    }

    return &partida.jugadores[mejor]; //devuelve al ganador dentro del arreglo original
}

bool ejecutarTurnoActualModoLotes(Partida& partida, const ScriptAcciones& script, int limiteTicks, ResultadoModoLotes& resultado) {
    if (partida.finalizada || rondaVacia(partida.ronda)) { //comprueba que la partida siga abierta y tenga jugadores activos
        return false; //no ejecuta un turno si falta alguna de esas condiciones
    }

    Jugador* jugador = verJugadorActualPartida(partida); //obtiene al participante que debe jugar este turno

    if (jugador == nullptr) { //comprueba que el turno tenga un jugador asociado
        return false; //sale porque no hay un jugador al que aplicar la simulacion
    }

    if (partida.numeroRonda > jugador->oleadaAlcanzada) { //revisa si este turno inicia una oleada nueva para el jugador
        jugador->oleadaAlcanzada = partida.numeroRonda; //actualiza la mayor oleada que ha comenzado
        jugador->metricas.oleadaAlcanzada = jugador->oleadaAlcanzada; //mantiene la misma oleada en sus estadisticas
    }

    ColaSpawn colaJugador; //guarda una copia independiente de las apariciones del turno
    inicializarColaSpawn(colaJugador); //deja preparada la cola que recibira la copia
    prepararColaJugador(partida, colaJugador); //copia las mismas apariciones que reciben los otros participantes

    bool terminoCorrectamente = ejecutarModoLotes(*jugador, partida.numeroRonda, colaJugador, script, limiteTicks, resultado, &partida.bitacora); //simula el turno y guarda sus resultados y eventos
    destruirColaSpawn(colaJugador); //libera la copia del turno sin modificar la cola original de la ronda

    return terminoCorrectamente; //indica si el turno termino y devuelve falso si agoto los ticks sin terminar
}

bool ejecutarRondaModoLotes(Partida& partida, const ScriptAcciones& script, int limiteTicks) {
    if (partida.finalizada || rondaVacia(partida.ronda)) { //revisa que la partida pueda comenzar otra ronda
        return false; //sale si la partida termino o ya no tiene jugadores activos
    }

    generarColaOriginalRonda(partida); //genera una sola vez las apariciones que compartiran todos los turnos

    int turnosDeLaRonda = cantidadJugadoresActivos(partida); //guarda cuantos jugadores deben participar antes de posibles eliminaciones

    for (int i = 0; i < turnosDeLaRonda; i++) { //da un turno a cada jugador que estaba activo al comenzar
        ResultadoModoLotes resultado; //recibira el resultado del turno que se va a simular

        if (!ejecutarTurnoActualModoLotes(partida, script, limiteTicks, resultado)) { //comprueba si el turno no pudo completarse
            return false; //detiene la ronda conservando los cambios que la simulacion ya haya realizado
        }

        bool eliminado = retirarJugadorActualSiEliminado(partida); //retira al jugador sin vida y en ese caso ya cambia el turno

        if (partida.finalizada) { //revisa si la salida del ultimo jugador termino la partida
            break; //deja de intentar los turnos que faltaban
        }

        if (i < turnosDeLaRonda - 1 && !eliminado) { //avanza solo si faltan turnos y la eliminacion no movio ya al jugador actual
            avanzarTurnoPartida(partida); //entrega el turno al siguiente participante segun el sentido de la ronda
        }
    }

    if (!partida.finalizada) { //revisa si todavia hace falta cerrar la ronda
        finalizarRondaPartida(partida); //decide si termina la partida o prepara el numero de la siguiente ronda
    }

    return true; //confirma que los turnos necesarios se completaron
}

//compara puntos oleada y precision y devuelve uno si el primer jugador gana menos uno si pierde o cero si empatan
static int compararResultadoFinal(const Jugador& a, const Jugador& b) {
    if (a.puntaje > b.puntaje) return 1; //favorece al jugador a si tiene mas puntos
    if (a.puntaje < b.puntaje) return -1; //favorece al jugador b si tiene mas puntos

    if (a.oleadaAlcanzada > b.oleadaAlcanzada) return 1; //con puntos iguales favorece a quien llego mas lejos
    if (a.oleadaAlcanzada < b.oleadaAlcanzada) return -1; //deja atras a quien alcanzo una oleada menor

    if (a.metricas.precisionColor > b.metricas.precisionColor) return 1; //usa la mayor precision como ultimo desempate
    if (a.metricas.precisionColor < b.metricas.precisionColor) return -1; //deja atras a quien tiene menor precision

    return 0; //indica que los tres criterios dieron el mismo resultado
}

bool crearRegistroJugadorPartida(const Partida& partida, int indice, int idPartida, const string& modo, RegistroPartida& registro) {
    if (indice < 0 || indice >= partida.cantidadJugadores) { //comprueba que exista el participante cuyo resultado se quiere guardar
        return false; //sale sin llenar el registro cuando la posicion no es valida
    }

    const Jugador& jugador = partida.jugadores[indice]; //consulta al participante sin copiarlo ni modificarlo
    const MetricasJugador& metricas = jugador.metricas; //permite leer sus estadisticas con un nombre mas corto

    registro.idPartida = idPartida; //identifica a que partida pertenece este resultado
    registro.semilla = partida.semillaPartida; //guarda la semilla con la que se generaron las oleadas
    registro.modo = modo; //guarda el modo de juego recibido
    registro.jugador = jugador.alias; //identifica al jugador por su alias
    registro.esBot = jugador.esBot; //indica si el participante fue controlado por el programa
    registro.posicion = metricas.posicionFinal; //copia el puesto que ya se calculo para este jugador
    registro.puntaje = jugador.puntaje; //guarda los puntos acumulados al terminar
    registro.oleada = jugador.oleadaAlcanzada; //guarda la ultima oleada que llego a iniciar
    registro.disparos = metricas.disparos; //guarda cuantas balas disparo
    registro.correctos = metricas.impactosCorrectos; //guarda los impactos contra enemigos del mismo color que la bala
    registro.erroneos = metricas.impactosErroneos; //guarda los impactos contra enemigos de otro color
    registro.comboMax = metricas.comboMaximo; //guarda el mayor combo alcanzado
    registro.pulsos = metricas.pulsosUtilizados; //guarda cuantos pulsos activo
    registro.danoTotal = metricas.danoTotal; //guarda la vida que perdio su nucleo por los choques
    registro.ticks = metricas.ticksSobrevividos; //guarda los ticks jugados entre todos sus turnos
    registro.bajasDron = metricas.bajasDron; //guarda cuantos drones elimino
    registro.bajasGirador = metricas.bajasGirador; //guarda cuantos giradores elimino
    registro.bajasDivisor = metricas.bajasDivisor; //guarda cuantos divisores elimino
    registro.bajasMini = metricas.bajasMini; //guarda cuantos minis elimino
    registro.bajasTanque = metricas.bajasTanque; //guarda cuantos tanques elimino
    registro.bajasCamaleon = metricas.bajasCamaleon; //guarda cuantos camaleones elimino

    registro.detalleDisponible = true; //indica que el registro incluye impactos por color y dano por tipo de enemigo

    registro.correctosC = metricas.impactosCorrectosC; //guarda los aciertos con balas cian
    registro.erroneosC = metricas.impactosErroneosC; //guarda los errores con balas cian
    registro.correctosM = metricas.impactosCorrectosM; //guarda los aciertos con balas magenta
    registro.erroneosM = metricas.impactosErroneosM; //guarda los errores con balas magenta
    registro.correctosA = metricas.impactosCorrectosA; //guarda los aciertos con balas ambar
    registro.erroneosA = metricas.impactosErroneosA; //guarda los errores con balas ambar

    registro.danoDron = metricas.danoDron; //guarda la vida del nucleo perdida por choques de drones
    registro.danoGirador = metricas.danoGirador; //guarda la vida del nucleo perdida por choques de giradores
    registro.danoDivisor = metricas.danoDivisor; //guarda la vida del nucleo perdida por choques de divisores
    registro.danoMini = metricas.danoMini; //guarda la vida del nucleo perdida por choques de minis
    registro.danoTanque = metricas.danoTanque; //guarda la vida del nucleo perdida por choques de tanques
    registro.danoCamaleon = metricas.danoCamaleon; //guarda la vida del nucleo perdida por choques de camaleones
    return true; //confirma que el registro quedo lleno con los datos del jugador
}

void actualizarMetricasFinalesPartida(Partida& partida) {
    for (int i = 0; i < partida.cantidadJugadores; i++) { //actualiza los resultados de todos los participantes incluidos los eliminados
        Jugador& jugador = partida.jugadores[i]; //trabaja directamente con el jugador que corresponde

        jugador.metricas.puntaje = jugador.puntaje; //copia el puntaje actual en las estadisticas
        jugador.metricas.oleadaAlcanzada = jugador.oleadaAlcanzada; //copia la ultima oleada iniciada en las estadisticas

        actualizarMetricasCalculadas(jugador.metricas); //recalcula las precisiones y la eficiencia con los conteos finales
    }

    for (int i = 0; i < partida.cantidadJugadores; i++) { //calcula el puesto de cada participante sin cambiar el orden del arreglo
        int posicion = 1; //parte del primer lugar y baja un puesto por cada rival con mejor resultado

        for (int j = 0; j < partida.cantidadJugadores; j++) { //compara al participante con todos los demas
            if (i != j && compararResultadoFinal(partida.jugadores[j], partida.jugadores[i]) > 0) { //cuenta solo a los otros jugadores que lo superan
                posicion++; //agrega un puesto por cada rival que queda por delante
            }
        }

        partida.jugadores[i].metricas.posicionFinal = posicion; //guarda el puesto y permite que los empates compartan la misma posicion
    }
}

void agregarResultadosPartidaHistorial(Partida& partida, int idPartida, const string& modo, HistorialPartidas& historial) {
    actualizarMetricasFinalesPartida(partida); //actualiza las proporciones y los puestos antes de guardar los resultados

    for (int i = 0; i < partida.cantidadJugadores; i++) { //prepara un registro por cada participante de la partida
        RegistroPartida registro; //recibira los datos que se agregaran al historial

        if (crearRegistroJugadorPartida(partida, i, idPartida, modo, registro)) { //comprueba que se haya podido completar el registro del jugador
            agregarRegistroFinal(historial, registro); //agrega una copia al final sin borrar los resultados que ya habia
        }
    }
}