#include "PruebasBot.h"
#include "Bot.h"

//prepara un dron con la posicion y el color recibidos para los escenarios de seleccion del bot
static Enemigo crearEnemigoPrueba(int id, int color, double r, double theta) {
    Enemigo enemigo; //guarda los datos del objetivo de prueba
    enemigo.id = id; //identifica al enemigo dentro del escenario
    enemigo.tipo = Dron; //usa el mismo tipo en todas estas pruebas
    enemigo.color = color; //asigna la fase que debe considerar el bot
    enemigo.r = r; //situa al enemigo a la distancia recibida del centro
    enemigo.theta = theta; //ubica al enemigo en el angulo recibido
    enemigo.omega = 0.0; //deja al enemigo sin giro angular
    enemigo.vida = 1; //le asigna un punto de vida
    enemigo.radio = 10; //fija el radio de colision usado en estos escenarios
    enemigo.ticksColor = 0; //inicia el contador de cambio de color
    return enemigo; //entrega los datos sin insertar todavia ningun nodo
}

//comprueba que el bot ignore los enemigos dentro de la orbita y elija el mas cercano por fuera
//revisa que en un empate conserve el primero de la lista y solicite su angulo y su color
static bool pruebaBotSeleccionObjetivo() {
    Jugador jugador{ "BOT1", true, 100, 0, 0, 0, 0, false }; //prepara un bot con el nucleo completo y sin carga de pulso

    Motor motor{}; //deja en cero el estado usado para consultar la decision
    motor.jugador = &jugador; //permite al bot consultar la carga del participante
    motor.nave.anguloActual = 0.0; //situa la nave en el angulo inicial del escenario
    motor.nave.anguloObjetivo = 0.0; //deja el destino inicial junto a la nave
    motor.nave.color = 0; //empieza en una fase distinta a la del objetivo esperado
    inicializarListaEnemigos(motor.enemigos); //prepara la lista de candidatos

    insertarFinal(motor.enemigos, crearEnemigoPrueba(1, 0, 90.0, 20.0)); //agrega un enemigo mas cercano al centro pero dentro de la orbita
    insertarFinal(motor.enemigos, crearEnemigoPrueba(2, 2, 130.0, 45.0)); //agrega un candidato que queda mas lejos que los siguientes
    insertarFinal(motor.enemigos, crearEnemigoPrueba(3, 1, 120.0, 90.0)); //coloca primero al objetivo que debe ganar el empate por distancia
    insertarFinal(motor.enemigos, crearEnemigoPrueba(4, 2, 120.0, 180.0)); //agrega otro candidato a la misma distancia con distinto angulo y fase

    AccionMotor accion = obtenerAccionBot(motor); //consulta la decision sin aplicar sus cambios al motor

    bool resultado = true; //acumula los fallos de seleccion y de fase

    if (!accion.cambiarAngulo || accion.anguloObjetivo != 90.0) { //exige apuntar al primer enemigo situado a ciento veinte unidades
        resultado = false; //marca que se eligio un destino incorrecto
    }

    if (!accion.cambiarColor || accion.colorNuevo != 1) { //comprueba que se solicite la fase del objetivo elegido
        resultado = false; //registra el cambio de color ausente o incorrecto
    }

    if (accion.solicitarPulso) { //rechaza una solicitud de pulso en este escenario sin carga
        resultado = false; //conserva el fallo de la decision de pulso
    }

    destruirListaEnemigos(motor.enemigos); //libera los cuatro candidatos de la prueba

    return resultado; //indica si la decision coincide con el objetivo esperado
}

//comprueba que sin objetivo se cancele el giro pendiente y se conserve la fase durante un tick real
//revisa que el pulso siga disponible con cinco enemigos cercanos aunque ninguno sea un objetivo
static bool pruebaBotSinObjetivo() {
    bool resultado = true; //conserva los fallos de los tres escenarios

    for (int caso = 0; caso < 3; caso++) { //prueba lista vacia enemigos no elegibles y pulso sin objetivo
        ColaSpawn cola; //guarda una aparicion futura para mantener activo el turno
        inicializarColaSpawn(cola); //prepara la cola del escenario
        Spawn futuro{ 60, Dron, 0, 180, 0 }; //evita que la lista vacia cierre la oleada
        encolarSpawn(cola, futuro); //reserva la aparicion sin crear todavia un enemigo

        Jugador jugador{ "BOT1", true, 100, 0, 0, 0, 1, false }; //prepara un bot con vida completa y sin carga

        if (caso == 2) { //habilita el pulso solamente en el ultimo escenario
            jugador.cargaPulso = 100; //completa la carga antes de consultar al bot
        }

        Motor motor; //guarda el estado completo que recibira el tick
        inicializarMotor(motor, &jugador, 1, cola); //inicia las listas y copia la aparicion futura
        motor.tickActual = 1; //elige un tick sin disparo automatico
        motor.nave.anguloActual = 25.0; //fija la orientacion que debe conservarse
        motor.nave.anguloObjetivo = 90.0; //deja un giro pendiente para detectar la regresion
        motor.nave.color = 2; //usa una fase distinta de las de los enemigos

        if (caso >= 1) { //agrega enemigos que el bot debe ignorar como objetivos
            insertarFinal(motor.enemigos, crearEnemigoPrueba(1, 0, 100.0, 180.0)); //incluye el limite exacto de la orbita lejos de la nave
            insertarFinal(motor.enemigos, crearEnemigoPrueba(2, 1, 80.0, 180.0)); //incluye un enemigo dentro de la orbita y lejos del nucleo
        }

        if (caso == 2) { //completa cinco enemigos cercanos sin agregar objetivos elegibles
            insertarFinal(motor.enemigos, crearEnemigoPrueba(3, 0, 90.0, 180.0)); //queda lejos de la nave y de la primera banda del pulso
            insertarFinal(motor.enemigos, crearEnemigoPrueba(4, 1, 85.0, 200.0)); //aumenta el conteo sin provocar choques
            insertarFinal(motor.enemigos, crearEnemigoPrueba(5, 0, 95.0, 220.0)); //alcanza la cantidad necesaria para solicitar el pulso
        }

        AccionMotor accion = obtenerAccionBot(motor); //consulta la decision sin modificar todavia la simulacion

        if (accion.cambiarColor || accion.solicitarPulso != (caso == 2)) { //conserva la fase y pide pulso solamente con carga y cinco enemigos
            resultado = false;
        }

        procesarTick(motor, accion); //ejecuta la accion el movimiento y el pulso con las reglas reales

        if (motor.nave.anguloActual != 25.0 || motor.nave.anguloObjetivo != 25.0 || motor.nave.color != 2) { //exige detener el giro pendiente y conservar la fase
            resultado = false;
        }

        if (motor.tickActual != 2 || jugador.vidaNucleo != 100 || motor.turnoTerminado) { //confirma un tick completo sin choques ni cierre de turno
            resultado = false;
        }

        if (caso == 2) { //revisa el resultado completo de activar el pulso sin objetivo
            if (!motor.pulso.activo || motor.pulso.radioActual != 32.0 || jugador.cargaPulso != 0 || jugador.metricas.pulsosUtilizados != 1 || motor.enemigos.tam != 5) { //la onda avanza doce y consume la carga sin alcanzar todavia los enemigos
                resultado = false;
            }
        }
        else { //sin solicitud no debe aparecer una onda ni contarse una activacion
            if (motor.pulso.activo || jugador.metricas.pulsosUtilizados != 0) {
                resultado = false;
            }
        }

        destruirMotor(motor); //libera las listas y la copia de apariciones del escenario
        destruirColaSpawn(cola); //libera la aparicion original
    }
    return resultado; //indica si los tres escenarios conservaron orientacion y fase con el pulso correcto
}

//comprueba que el bot apunte al enemigo sin pedir otra fase cuando ambos ya tienen el mismo color
static bool pruebaBotMismoColor() {
    Jugador jugador{ "BOT1", true, 100, 0, 0, 0, 0, false }; //prepara al participante del caso de fase compartida

    Motor motor{}; //inicia el estado usado para obtener la accion
    motor.jugador = &jugador; //asocia el bot con el motor de prueba
    motor.nave.anguloActual = 0.0; //deja la nave lejos del angulo del objetivo
    motor.nave.anguloObjetivo = 0.0; //mantiene el destino anterior antes de consultar
    motor.nave.color = 1; //establece la misma fase que tendra el enemigo
    inicializarListaEnemigos(motor.enemigos); //prepara la lista para un unico objetivo

    insertarFinal(motor.enemigos, crearEnemigoPrueba(1, 1, 120.0, 75.0)); //agrega el objetivo de igual color fuera de la orbita

    AccionMotor accion = obtenerAccionBot(motor); //consulta los cambios que el bot considera necesarios

    bool resultado = true; //acumula los resultados del destino y de la fase

    if (!accion.cambiarAngulo || accion.anguloObjetivo != 75.0) { //exige que se solicite el angulo del enemigo
        resultado = false; //registra un destino incorrecto o ausente
    }

    if (accion.cambiarColor) { //detecta un cambio de fase que no hace falta
        resultado = false; //marca la solicitud innecesaria
    }

    destruirListaEnemigos(motor.enemigos); //libera el objetivo de este caso

    return resultado; //indica si el destino y la solicitud de fase fueron correctos
}

//compara la solicitud de pulso con cuatro y cinco enemigos cercanos y con carga completa o incompleta
//revisa tambien que agregar otro enemigo a ciento sesenta unidades mantenga la solicitud ya activa
static bool pruebaBotPulso() {
    Jugador jugador{ "BOT1", true, 100, 0, 0, 100, 0, false }; //prepara al bot con el pulso completamente cargado

    Motor motor{}; //inicia el motor usado para consultar la decision de pulso
    motor.jugador = &jugador; //permite consultar la carga que se cambiara durante la prueba
    motor.nave.anguloActual = 0.0; //fija la posicion inicial de la nave
    motor.nave.anguloObjetivo = 0.0; //deja el destino en la posicion inicial
    motor.nave.color = 0; //usa la misma fase que los enemigos del escenario
    inicializarListaEnemigos(motor.enemigos); //prepara la lista para aumentar la cantidad de enemigos

    insertarFinal(motor.enemigos, crearEnemigoPrueba(1, 0, 120.0, 10.0)); //agrega el primer enemigo cercano
    insertarFinal(motor.enemigos, crearEnemigoPrueba(2, 0, 130.0, 20.0)); //agrega el segundo enemigo cercano
    insertarFinal(motor.enemigos, crearEnemigoPrueba(3, 0, 140.0, 30.0)); //agrega el tercer enemigo cercano
    insertarFinal(motor.enemigos, crearEnemigoPrueba(4, 0, 150.0, 40.0)); //deja cuatro enemigos a menos de ciento sesenta unidades

    AccionMotor accion = obtenerAccionBot(motor); //consulta la decision con cuatro enemigos cercanos

    bool resultado = true; //conserva los fallos de cualquiera de las consultas

    if (accion.solicitarPulso) { //comprueba que cuatro enemigos no basten para pedir el pulso
        resultado = false; //marca una solicitud anticipada
    }

    insertarFinal(motor.enemigos, crearEnemigoPrueba(5, 0, 159.0, 50.0)); //completa los cinco enemigos dentro de la distancia considerada cercana

    accion = obtenerAccionBot(motor); //vuelve a decidir con cinco enemigos y carga completa

    if (!accion.solicitarPulso) { //exige solicitar el pulso al cumplirse ambas condiciones
        resultado = false; //marca la ausencia de la solicitud esperada
    }

    insertarFinal(motor.enemigos, crearEnemigoPrueba(6, 0, 160.0, 60.0)); //agrega un enemigo en el limite sin retirar los cinco que ya estaban cerca

    accion = obtenerAccionBot(motor); //consulta otra vez conservando los cinco enemigos cercanos

    if (!accion.solicitarPulso) { //comprueba que la solicitud siga activa despues de agregar ese enemigo
        resultado = false; //registra que se perdio una solicitud que aun corresponde
    }

    jugador.cargaPulso = 99; //deja la carga un punto por debajo del valor requerido

    accion = obtenerAccionBot(motor); //consulta la decision con los mismos enemigos y carga incompleta

    if (accion.solicitarPulso) { //rechaza una solicitud de pulso con noventa y nueve puntos de carga
        resultado = false; //marca que no se respeto la carga necesaria
    }

    destruirListaEnemigos(motor.enemigos); //libera los seis enemigos del escenario

    return resultado; //devuelve el resultado conjunto de las cuatro consultas
}

//comprueba que consultar la accion conserve las direcciones de dos nodos el tamano el puntaje la carga y los datos de la nave
static bool pruebaBotNoModificaEstado() {
    Jugador jugador{ "BOT1", true, 100, 250, 3, 100, 2, false }; //prepara un jugador con resultados previos y carga completa

    Motor motor{}; //prepara el motor cuyo estado se comparara antes y despues
    motor.jugador = &jugador; //vincula el participante que debe conservar sus datos
    motor.nave.anguloActual = 30.0; //fija la posicion angular que no debe cambiar al consultar
    motor.nave.anguloObjetivo = 40.0; //distingue el destino anterior del angulo de los enemigos
    motor.nave.color = 2; //elige una fase distinta a la del objetivo mas cercano
    inicializarListaEnemigos(motor.enemigos); //prepara la lista que se revisara despues de la consulta

    insertarFinal(motor.enemigos, crearEnemigoPrueba(1, 1, 120.0, 90.0)); //agrega el objetivo que exige cambiar destino y fase en la accion
    insertarFinal(motor.enemigos, crearEnemigoPrueba(2, 2, 130.0, 180.0)); //agrega otro nodo para comprobar que se conserve el enlace

    NodoEnemigo* cabezaAntes = motor.enemigos.cabeza; //recuerda la direccion del primer enemigo
    NodoEnemigo* segundoAntes = cabezaAntes->sig; //recuerda el nodo que sigue a la cabeza
    int tamAntes = motor.enemigos.tam; //guarda la cantidad de enemigos antes de consultar
    int puntajeAntes = jugador.puntaje; //conserva el puntaje que debe permanecer igual
    int cargaAntes = jugador.cargaPulso; //guarda la carga para detectar si se gasta durante la consulta
    double anguloActualAntes = motor.nave.anguloActual; //recuerda la posicion angular de la nave
    double anguloObjetivoAntes = motor.nave.anguloObjetivo; //recuerda el destino anterior de la nave
    int colorAntes = motor.nave.color; //guarda la fase que tiene la nave antes de decidir

    obtenerAccionBot(motor); //consulta la decision y descarta la accion sin aplicarla

    bool resultado = true; //acumula cualquier cambio inesperado en los datos comparados

    if (motor.enemigos.cabeza != cabezaAntes || motor.enemigos.cabeza->sig != segundoAntes || motor.enemigos.tam != tamAntes) { //revisa las direcciones de ambos nodos y el tamano de la lista
        resultado = false; //marca una alteracion de la estructura durante la consulta
    }

    if (jugador.puntaje != puntajeAntes || jugador.cargaPulso != cargaAntes) { //comprueba que decidir no cambie el puntaje ni gaste la carga
        resultado = false; //registra la modificacion de alguno de esos datos del jugador
    }

    if (motor.nave.anguloActual != anguloActualAntes || motor.nave.anguloObjetivo != anguloObjetivoAntes || motor.nave.color != colorAntes) { //compara la posicion el destino y la fase guardados
        resultado = false; //marca un cambio aplicado a la nave antes de procesar la accion
    }

    destruirListaEnemigos(motor.enemigos); //libera los nodos despues de comparar su estado

    return resultado; //informa si todos los datos revisados permanecieron iguales
}

//ejecuta los cinco escenarios del bot en orden y devuelve falso al encontrar el primero que falla
bool pruebaBot() {
    if (!pruebaBotSeleccionObjetivo()) { //revisa la eleccion del objetivo y el desempate por orden de la lista
        return false; //detiene el grupo si el objetivo o la accion son incorrectos
    }

    if (!pruebaBotSinObjetivo()) { //comprueba la decision cuando ningun enemigo queda fuera de la orbita
        return false; //detiene el grupo ante una solicitud inesperada
    }

    if (!pruebaBotMismoColor()) { //revisa que no se solicite una fase que la nave ya tiene
        return false; //termina si el caso de igual color falla
    }

    if (!pruebaBotPulso()) { //comprueba las decisiones de pulso segun la cantidad cercana y la carga
        return false; //detiene las comprobaciones si alguna decision no coincide
    }

    if (!pruebaBotNoModificaEstado()) { //revisa que consultar al bot conserve los datos comparados
        return false; //informa si la consulta modifica alguno de esos datos
    }

    return true; //confirma que los cinco escenarios pasaron
}
