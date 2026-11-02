#include "Motor.h"
#include <cmath>

using namespace std; //permite usar las funciones matematicas estandar sin el prefijo

//convierte un angulo en grados a radianes para usar las funciones trigonometricas
static double gradosARadianes(double grados) {
    const double PI = 3.14159265358979323846; //guarda el valor de pi usado en la conversion
    return grados * PI / 180.0; //obtiene el mismo angulo expresado en radianes
}

//calcula las coordenadas respecto al centro usando una distancia y un angulo
static void polarACartesiano(double radio, double angulo, double& x, double& y) {
    double radianes = gradosARadianes(angulo); //convierte el angulo antes de calcular sus coordenadas

    x = radio * cos(radianes); //guarda la posicion horizontal en la variable recibida
    y = radio * sin(radianes); //guarda la posicion vertical en la variable recibida
}

//agrega un evento a la bitacora y conserva la posicion cuando corresponde a la baja de un enemigo
static void registrarEventoMotor(Motor& motor, TipoEvento tipoEvento, int tipoEnemigo, int color, int valor, double r = -1.0, double theta = 0.0) {
    if (motor.bitacora == nullptr || motor.jugador == nullptr) { //comprueba que existan la bitacora y el jugador del evento
        return; //sale si falta alguno de los datos necesarios para registrarlo
    }

    Evento evento; //reune la informacion de lo que acaba de ocurrir
    evento.tick = motor.tickActual; //guarda el tick del turno en que ocurrio
    evento.oleada = motor.numeroOleada; //guarda la oleada que se esta jugando
    evento.jugador = motor.jugador->alias; //relaciona el evento con el jugador actual
    evento.tipoEvento = tipoEvento; //guarda que clase de evento ocurrio
    evento.tipoEnemigo = tipoEnemigo; //guarda el tipo de enemigo o menos uno si no corresponde
    evento.color = color; //guarda el color relacionado o menos uno si no corresponde
    evento.valor = valor; //guarda los puntos el dano o el valor recibido para este evento
    evento.r = r; //guarda la distancia de la baja o menos uno para los otros eventos
    evento.theta = theta; //guarda el angulo de la baja cuando se recibio una posicion

    agregarEventoFinal(*motor.bitacora, evento); //agrega el evento despues de los que ya estaban registrados
}

//cuenta un impacto correcto o erroneo y lo clasifica segun el color de la bala
static void registrarImpactoMetricas(Motor& motor, int color, bool correcto) {
    if (motor.jugador == nullptr) { //revisa si existe un jugador al que sumar el impacto
        return; //sale porque no hay estadisticas de jugador disponibles
    }

    MetricasJugador& metricas = motor.jugador->metricas; //trabaja directamente con las estadisticas del jugador

    if (correcto) { //comprueba si la bala coincidio con el color del enemigo
        metricas.impactosCorrectos++; //suma un acierto al total de impactos
    }
    else { //atiende un impacto de una bala de otro color
        metricas.impactosErroneos++; //suma un error al total de impactos
    }

    if (color == 0) { //clasifica los impactos de balas cian
        if (correcto) metricas.impactosCorrectosC++; //suma un acierto con bala cian
        else metricas.impactosErroneosC++; //suma un error con bala cian
    }
    else if (color == 1) { //clasifica los impactos de balas magenta
        if (correcto) metricas.impactosCorrectosM++; //suma un acierto con bala magenta
        else metricas.impactosErroneosM++; //suma un error con bala magenta
    }
    else if (color == 2) { //clasifica los impactos de balas ambar
        if (correcto) metricas.impactosCorrectosA++; //suma un acierto con bala ambar
        else metricas.impactosErroneosA++; //suma un error con bala ambar
    }
}

//cuenta una baja por tipo y color del enemigo y conserva el mayor combo alcanzado
static void registrarBajaMetricas(Motor& motor, const Enemigo& enemigo) {
    if (motor.jugador == nullptr) { //revisa si existe un jugador al que registrar la baja
        return; //sale sin modificar estadisticas cuando falta el jugador
    }

    MetricasJugador& metricas = motor.jugador->metricas; //accede a los conteos que acumula el jugador

    switch (enemigo.tipo) { //elige el contador correspondiente al enemigo eliminado
    case Dron: //selecciona las bajas de drones
        metricas.bajasDron++; //suma un dron eliminado
        break; //termina este caso sin modificar los otros contadores
    case Girador: //selecciona las bajas de giradores
        metricas.bajasGirador++; //suma un girador eliminado
        break; //termina el registro de este tipo
    case Divisor: //selecciona las bajas de divisores
        metricas.bajasDivisor++; //suma un divisor eliminado
        break; //continua despues de la seleccion de tipos
    case Mini: //selecciona las bajas de minis
        metricas.bajasMini++; //suma un mini eliminado
        break; //evita contar esta baja como otro tipo
    case Tanque: //selecciona las bajas de tanques
        metricas.bajasTanque++; //suma un tanque eliminado
        break; //termina este caso
    case Camaleon: //selecciona las bajas de camaleones
        metricas.bajasCamaleon++; //suma un camaleon eliminado
        break; //termina la seleccion del contador de bajas
    }

    if (enemigo.color == 0) metricas.bajasColor0++; //cuenta la baja si el enemigo era cian al morir
    else if (enemigo.color == 1) metricas.bajasColor1++; //cuenta la baja si el enemigo era magenta al morir
    else if (enemigo.color == 2) metricas.bajasColor2++; //cuenta la baja si el enemigo era ambar al morir

    if (motor.jugador->combo > metricas.comboMaximo) { //revisa si el combo actual supera el mayor registrado
        metricas.comboMaximo = motor.jugador->combo; //guarda el nuevo maximo sin aumentar el combo actual
    }
}

//acumula la vida realmente perdida por el nucleo y la clasifica por tipo de enemigo
static void registrarDanoMetricas(Motor& motor, TipoEnemigo tipo, int dano) {
    if (motor.jugador == nullptr) { //revisa si existe el jugador que recibio el dano
        return; //sale cuando no hay estadisticas donde guardarlo
    }

    MetricasJugador& metricas = motor.jugador->metricas; //accede a las estadisticas acumuladas del jugador
    metricas.danoTotal += dano; //suma la vida perdida al dano total del nucleo

    switch (tipo) { //elige a que tipo de enemigo atribuir el dano
    case Dron: //atribuye el dano a un dron
        metricas.danoDron += dano; //suma el dano causado por drones
        break; //termina este caso
    case Girador: //atribuye el dano a un girador
        metricas.danoGirador += dano; //suma el dano causado por giradores
        break; //termina este caso
    case Divisor: //atribuye el dano a un divisor
        metricas.danoDivisor += dano; //suma el dano causado por divisores
        break; //termina este caso
    case Mini: //atribuye el dano a un mini
        metricas.danoMini += dano; //suma el dano causado por minis
        break; //termina este caso
    case Tanque: //atribuye el dano a un tanque
        metricas.danoTanque += dano; //suma el dano causado por tanques
        break; //termina este caso
    case Camaleon: //atribuye el dano a un camaleon
        metricas.danoCamaleon += dano; //suma el dano causado por camaleones
        break; //termina este caso
    }
}

//cuenta el tick procesado y actualiza los resultados y proporciones del jugador
static void actualizarMetricasPasoTick(Motor& motor) {
    if (motor.jugador == nullptr) { //comprueba que haya un jugador con estadisticas
        return; //sale si no puede actualizar sus resultados
    }

    MetricasJugador& metricas = motor.jugador->metricas; //usa una referencia para modificar las estadisticas originales

    metricas.ticksSobrevividos++; //suma este tick a los jugados entre todos sus turnos
    metricas.puntaje = motor.jugador->puntaje; //mantiene el puntaje de las estadisticas igual al del jugador
    metricas.oleadaAlcanzada = motor.jugador->oleadaAlcanzada; //copia la ultima oleada que el jugador llego a iniciar

    actualizarMetricasCalculadas(metricas); //recalcula las precisiones y la eficiencia con los conteos actuales
}

//resta vida al nucleo reinicia el combo y devuelve cuanta vida se perdio realmente
static int aplicarDanioNucleo(Motor& motor, int danio) {
    if (motor.jugador == nullptr) { //comprueba si hay un jugador que pueda recibir el dano
        return 0; //indica que no se perdio vida porque falta el jugador
    }

    int vidaAntes = motor.jugador->vidaNucleo; //guarda la vida previa para calcular la perdida real

    motor.jugador->vidaNucleo -= danio; //descuenta el dano recibido por el nucleo

    if (motor.jugador->vidaNucleo < 0) { //comprueba si el golpe supero la vida disponible
        motor.jugador->vidaNucleo = 0; //evita que el nucleo quede con vida negativa
    }

    motor.jugador->combo = 0; //pierde el combo cuando el nucleo recibe el golpe

    return vidaAntes - motor.jugador->vidaNucleo; //devuelve solo la vida que pudo perder y no el exceso de dano
}

//comprueba si la posicion de la bala queda dentro del radio de colision del enemigo
static bool hayColisionBalaEnemigo(const Bala& bala, const Enemigo& enemigo) {
    double enemigoX; //recibira la posicion horizontal del enemigo
    double enemigoY; //recibira la posicion vertical del enemigo

    polarACartesiano(enemigo.r, enemigo.theta, enemigoX, enemigoY); //convierte la posicion del enemigo al mismo sistema que usa la bala

    double diferenciaX = bala.x - enemigoX; //calcula la separacion horizontal entre ambos
    double diferenciaY = bala.y - enemigoY; //calcula la separacion vertical entre ambos

    double distancia = sqrt(diferenciaX * diferenciaX + diferenciaY * diferenciaY); //obtiene la distancia desde la bala al centro del enemigo

    return distancia < enemigo.radio; //detecta el impacto cuando la bala esta dentro del enemigo
}

//suma los puntos de una baja por pulso usando el combo actual sin aumentarlo ni recargar el pulso
static int premiarBajaPorPulso(Motor& motor, TipoEnemigo tipo) {
    if (motor.jugador == nullptr) { //revisa si hay un jugador al que dar los puntos
        return 0; //no otorga puntos cuando falta el jugador
    }

    int multiplicador = calcularMultiplicador(motor.jugador->combo); //obtiene el multiplicador del combo que ya tenia el jugador
    int puntos = puntosBaseEnemigo(tipo) * multiplicador; //calcula el premio segun el tipo de enemigo eliminado

    motor.jugador->puntaje += puntos; //agrega el premio al puntaje acumulado

    return puntos; //entrega los puntos para poder incluirlos en el evento de baja
}

//prepara un nuevo turno con una copia de las apariciones y reinicia el combo conservando los demas resultados del jugador
void inicializarMotor(Motor& motor, Jugador* jugador, int numeroOleada, const ColaSpawn& spawns, BitacoraEventos* bitacora) {
    motor.tickActual = 0; //empieza a contar el tiempo de este turno desde cero
    motor.numeroOleada = numeroOleada; //guarda la oleada que se va a jugar

    motor.nave.anguloActual = 270.0; //coloca la nave en su angulo inicial
    motor.nave.anguloObjetivo = 270.0; //deja el destino en el mismo lugar hasta recibir una accion
    motor.nave.color = 0; //inicia la nave en fase cian

    motor.pulso.activo = false; //comienza el turno sin un pulso en movimiento
    motor.pulso.radioActual = 0.0; //deja el radio en cero mientras no se active

    motor.jugador = jugador; //guarda el puntero al jugador sin crear una copia
    motor.bitacora = bitacora; //usa la bitacora recibida si se desean registrar eventos

    inicializarListaEnemigos(motor.enemigos); //prepara la lista de enemigos activos
    inicializarListaBalas(motor.balas); //prepara la lista de disparos en movimiento
    inicializarColaSpawn(motor.spawns); //deja vacia la cola que usara este turno
    copiarColaSpawn(spawns, motor.spawns); //copia las apariciones en nodos independientes de la cola recibida

    motor.siguienteIdEnemigo = 1; //asigna el primer identificador disponible para los enemigos del turno
    motor.oleadaTerminada = false; //indica que todavia no se ha completado la oleada
    motor.turnoTerminado = false; //permite comenzar a procesar los ticks del turno

    if (motor.jugador != nullptr) { //revisa si se recibio un jugador
        motor.jugador->combo = 0; //reinicia el combo al comenzar este turno sin borrar las otras estadisticas
    }
}

//libera los enemigos las balas y las apariciones sin destruir el jugador ni la bitacora externa
void destruirMotor(Motor& motor) {
    destruirListaEnemigos(motor.enemigos); //libera los nodos de los enemigos que queden activos
    destruirListaBalas(motor.balas); //libera los nodos de las balas que sigan en movimiento
    destruirColaSpawn(motor.spawns); //libera las apariciones que todavia no se hayan usado

    motor.jugador = nullptr; //desconecta al jugador sin destruirlo ni borrar sus resultados
    motor.bitacora = nullptr; //desconecta la bitacora sin liberar los eventos que guarda
}

//quita o agrega vueltas completas para dejar el angulo entre cero incluido y trescientos sesenta excluido
double normalizarAngulo(double angulo) {
    while (angulo >= 360.0) angulo -= 360.0; //quita vueltas completas si el angulo alcanza o supera una vuelta
    while (angulo < 0.0) angulo += 360.0; //suma vueltas completas hasta que el angulo deje de ser negativo

    return angulo; //devuelve el angulo equivalente entre cero incluido y trescientos sesenta excluido
}

//aplica los cambios de destino y fase y activa el pulso cuando se solicita y tiene carga completa
void aplicarAccion(Motor& motor, const AccionMotor& accion) {
    if (accion.cambiarAngulo) motor.nave.anguloObjetivo = normalizarAngulo(accion.anguloObjetivo); //actualiza el destino de la nave cuando la accion lo solicita

    if (accion.cambiarColor && accion.colorNuevo >= 0 && accion.colorNuevo <= 2) { //acepta el cambio solo si la fase pedida es valida
        motor.nave.color = accion.colorNuevo; //cambia la fase que usaran los siguientes disparos
    }

    if (accion.solicitarPulso && motor.jugador != nullptr && motor.jugador->cargaPulso == 100 && !motor.pulso.activo) { //permite activar el pulso con carga completa y sin otro pulso activo
        motor.pulso.activo = true; //pone en marcha la onda del pulso
        motor.pulso.radioActual = 20.0; //inicia la onda en el radio del nucleo
        motor.jugador->cargaPulso = 0; //consume toda la carga acumulada

        motor.jugador->metricas.pulsosUtilizados++; //cuenta el pulso que se acaba de activar
        registrarEventoMotor(motor, PULSO, -1, -1, 0); //guarda la activacion sin asociarla a un enemigo o color
    }
}

//acerca la nave a su angulo objetivo por el camino corto con un maximo de seis grados por tick
void actualizarNave(Motor& motor) {
    motor.nave.anguloActual = normalizarAngulo(motor.nave.anguloActual); //deja la posicion actual dentro de una vuelta
    motor.nave.anguloObjetivo = normalizarAngulo(motor.nave.anguloObjetivo); //deja el destino dentro de la misma vuelta

    double diferencia = motor.nave.anguloObjetivo - motor.nave.anguloActual; //calcula cuanto falta para llegar al destino

    if (diferencia > 180.0) diferencia -= 360.0; //elige el recorrido mas corto si conviene girar hacia angulos menores
    if (diferencia < -180.0) diferencia += 360.0; //elige el recorrido mas corto si conviene girar hacia angulos mayores

    if (diferencia > 6.0) motor.nave.anguloActual += 6.0; //avanza como maximo seis grados hacia angulos mayores
    else if (diferencia < -6.0) motor.nave.anguloActual -= 6.0; //avanza como maximo seis grados hacia angulos menores
    else motor.nave.anguloActual = motor.nave.anguloObjetivo; //llega al destino cuando la distancia restante cabe en este tick

    motor.nave.anguloActual = normalizarAngulo(motor.nave.anguloActual); //corrige el angulo si el movimiento cruzo el inicio de la vuelta
}

//crea los datos de un enemigo desde una aparicion y reserva su identificador sin insertarlo en la lista
Enemigo crearEnemigoDesdeSpawn(Motor& motor, const Spawn& spawn) {
    Enemigo enemigo; //reune el estado inicial del enemigo que va a entrar al escenario

    enemigo.id = motor.siguienteIdEnemigo; //asigna el identificador que estaba disponible
    motor.siguienteIdEnemigo++; //reserva el siguiente numero para otro enemigo

    enemigo.tipo = spawn.tipo; //copia el tipo indicado en la aparicion
    enemigo.color = spawn.color; //copia la fase con la que debe aparecer
    enemigo.r = 300.0; //lo coloca a trescientas unidades del centro
    enemigo.theta = normalizarAngulo(spawn.angulo); //guarda el angulo de entrada dentro de una vuelta
    enemigo.omega = 0.0; //comienza sin giro hasta aplicar las reglas de su tipo
    enemigo.vida = 1; //asigna la vida inicial que usa la mayoria de los enemigos
    enemigo.radio = 6; //asigna un radio inicial para revisar colisiones
    enemigo.ticksColor = 0; //inicia el contador para posibles cambios de color

    double velocidadAngularGirador = 0.9 + 0.05 * motor.numeroOleada; //aumenta el giro del girador conforme avanzan las oleadas

    switch (spawn.tipo) { //ajusta la vida el tamano y el giro segun el tipo de enemigo
    case Dron: //prepara la vida y el radio de un dron
        enemigo.vida = 1; //un impacto correcto de bala basta para eliminar al dron
        enemigo.radio = 6; //usa el radio de colision del dron
        break; //termina los ajustes de este tipo

    case Girador: //prepara un girador con el sentido recibido
        enemigo.vida = 1; //un impacto correcto de bala basta para eliminar al girador
        enemigo.radio = 6; //usa el radio de colision del girador

        if (spawn.sentido == 0) enemigo.omega = -velocidadAngularGirador; //elige un giro que reduce el angulo
        else enemigo.omega = velocidadAngularGirador; //elige un giro que aumenta el angulo

        break; //termina los ajustes del girador

    case Divisor: //prepara la vida y el radio de un divisor
        enemigo.vida = 1; //un impacto correcto de bala basta para eliminar al divisor
        enemigo.radio = 8; //usa un radio mayor que el del dron
        break; //termina los ajustes del divisor

    case Mini: //prepara un mini sin giro inicial
        enemigo.vida = 1; //un impacto correcto de bala basta para eliminar al mini
        enemigo.radio = 4; //usa el menor radio de colision entre estos tipos
        break; //termina los ajustes del mini

    case Tanque: //prepara un tanque que resiste tres impactos correctos
        enemigo.vida = 3; //asigna al tanque tres puntos de vida frente a los impactos de bala
        enemigo.radio = 10; //usa el mayor radio de colision entre estos tipos
        break; //termina los ajustes del tanque

    case Camaleon: //prepara la vida y el radio de un camaleon
        enemigo.vida = 1; //un impacto correcto de bala basta para eliminar al camaleon
        enemigo.radio = 7; //usa el radio de colision del camaleon
        break; //termina los ajustes del camaleon
    }

    return enemigo; //entrega el enemigo preparado para insertarlo en la lista
}

//retira de la cola todas las apariciones cuyo tick ya llego y agrega sus enemigos al escenario
void procesarSpawns(Motor& motor) {
    const Spawn* frente = verFrenteSpawn(motor.spawns); //consulta la proxima aparicion pendiente

    while (frente != nullptr && frente->tick <= motor.tickActual) { //procesa todas las apariciones cuyo momento ya llego
        Spawn spawn; //recibira los datos que salgan de la cola
        desencolarSpawn(motor.spawns, spawn); //retira la aparicion pendiente y copia sus datos

        Enemigo enemigo = crearEnemigoDesdeSpawn(motor, spawn); //crea el enemigo con las reglas correspondientes a su tipo
        insertarFinal(motor.enemigos, enemigo); //lo agrega al final de los enemigos activos

        frente = verFrenteSpawn(motor.spawns); //consulta la nueva primera aparicion despues de retirar la anterior
    }
}

//mueve cada enemigo segun su tipo y actualiza los cambios de fase del camaleon
void moverEnemigos(Motor& motor) {
    double velocidadBase = 0.8 + 0.08 * motor.numeroOleada; //calcula la velocidad base que aumenta con la oleada

    NodoEnemigo* actual = motor.enemigos.cabeza; //empieza por el primer enemigo activo

    while (actual != nullptr) { //actualiza el movimiento de todos los enemigos de la lista
        Enemigo& enemigo = actual->dato; //modifica directamente el enemigo guardado en este nodo

        switch (enemigo.tipo) { //aplica la velocidad y el comportamiento de cada tipo
        case Dron: //mueve el dron directamente hacia el centro
            enemigo.r -= 1.00 * velocidadBase; //acerca el dron al centro usando toda la velocidad base
            break; //termina el movimiento del dron

        case Girador: //combina el avance del girador con su giro
            enemigo.r -= 0.85 * velocidadBase; //acerca el girador al centro mas despacio que el dron
            enemigo.theta = normalizarAngulo(enemigo.theta + enemigo.omega); //lo hace girar en el sentido guardado
            break; //termina el movimiento del girador

        case Divisor: //mueve el divisor hacia el centro sin hacerlo girar
            enemigo.r -= 0.80 * velocidadBase; //acerca el divisor al centro con su factor de velocidad
            break; //termina el movimiento del divisor

        case Mini: //combina el avance rapido del mini con su giro
            enemigo.r -= 1.25 * velocidadBase; //acerca el mini al centro mas rapido que el dron
            enemigo.theta = normalizarAngulo(enemigo.theta + enemigo.omega); //aplica el giro que tenga asignado el mini
            break; //termina el movimiento del mini

        case Tanque: //mueve el tanque con la mitad de la velocidad base
            enemigo.r -= 0.50 * velocidadBase; //acerca el tanque al centro a la mitad de la velocidad base
            break; //termina el movimiento del tanque

        case Camaleon: //mueve el camaleon y revisa cuando debe cambiar de fase
            enemigo.r -= 0.85 * velocidadBase; //acerca el camaleon al centro con su factor de velocidad
            enemigo.ticksColor++; //cuenta otro tick desde el ultimo cambio de fase

            if (enemigo.ticksColor >= 42) { //revisa si ya llego el momento de cambiar de fase
                enemigo.color = (enemigo.color + 1) % 3; //pasa de cian a magenta luego a ambar y vuelve a cian
                enemigo.ticksColor = 0; //reinicia la espera para el siguiente cambio
            }

            break; //termina el movimiento y la revision de fase del camaleon
        }

        actual = actual->sig; //continua con el siguiente enemigo de la lista
    }
}

//crea una bala hacia afuera cada cuatro ticks usando la posicion y la fase de la nave
void dispararAutomaticamente(Motor& motor) {
    if (motor.tickActual % 4 != 0) return; //solo dispara en los ticks que son multiplos de cuatro

    double x; //recibira la posicion horizontal de la nave respecto al centro
    double y; //recibira la posicion vertical de la nave respecto al centro

    polarACartesiano(100.0, motor.nave.anguloActual, x, y); //ubica la nave en su orbita de radio cien

    double radianes = gradosARadianes(motor.nave.anguloActual); //convierte el angulo para calcular la direccion del disparo

    Bala bala{ x, y, 9.0 * cos(radianes), 9.0 * sin(radianes), motor.nave.color, 0 }; //crea una bala que sale de la nave hacia afuera con su color y velocidad nueve

    insertarInicio(motor.balas, bala); //guarda la nueva bala al principio de la lista
    if (motor.jugador != nullptr) { //comprueba que exista un jugador al que contarle el disparo
        motor.jugador->metricas.disparos++; //cuenta la bala disparada sin agregar un evento a la bitacora
    }
}

//revisa los choques con nave y nucleo descuenta vida del nucleo y retira los enemigos que chocaron
void resolverChoquesEnemigos(Motor& motor) {
    if (motor.jugador == nullptr) return; //sale si no hay un jugador cuyo nucleo pueda recibir dano

    double naveX; //recibira la coordenada horizontal de la nave
    double naveY; //recibira la coordenada vertical de la nave

    polarACartesiano(100.0, motor.nave.anguloActual, naveX, naveY); //calcula donde esta la nave dentro de su orbita

    NodoEnemigo* actual = motor.enemigos.cabeza; //empieza a revisar los enemigos desde el primero

    while (actual != nullptr) { //revisa cada enemigo que sigue en la lista
        double enemigoX; //recibira la coordenada horizontal del enemigo
        double enemigoY; //recibira la coordenada vertical del enemigo

        polarACartesiano(actual->dato.r, actual->dato.theta, enemigoX, enemigoY); //convierte la posicion del enemigo a coordenadas del escenario

        double diferenciaX = enemigoX - naveX; //calcula la separacion horizontal entre el enemigo y la nave
        double diferenciaY = enemigoY - naveY; //calcula la separacion vertical entre el enemigo y la nave
        double distanciaNave = sqrt(diferenciaX * diferenciaX + diferenciaY * diferenciaY); //obtiene la distancia entre sus centros

        bool choqueNave = distanciaNave < actual->dato.radio + 5.0; //comprueba si se superponen el enemigo y la nave de radio cinco
        bool choqueNucleo = actual->dato.r <= 20.0 + 0.4 * actual->dato.radio; //comprueba si el enemigo alcanzo el limite de contacto con el nucleo

        if (choqueNave) { //atiende primero el choque contra la nave
            Enemigo enemigo = actual->dato; //conserva los datos del enemigo antes de retirar su nodo

            int danoAplicado = aplicarDanioNucleo(motor, 12); //intenta quitar doce de vida y obtiene cuanto perdio realmente el nucleo

            registrarDanoMetricas(motor, enemigo.tipo, danoAplicado); //acumula la vida perdida en el total y en el tipo de enemigo
            registrarEventoMotor(motor, CHOQUE_NAVE, static_cast<int>(enemigo.tipo), enemigo.color, danoAplicado); //guarda el choque con su enemigo color y dano real

            actual = eliminarEnemigo(motor.enemigos, actual); //retira al enemigo y continua desde el nodo que quedo despues
        }
        else if (choqueNucleo) { //revisa el choque contra el nucleo cuando no hubo choque con la nave
            Enemigo enemigo = actual->dato; //conserva los datos que se usaran para registrar el dano
            int dano = enemigo.tipo == Tanque ? 22 : 10; //asigna veintidos de dano al tanque y diez a los demas tipos

            int danoAplicado = aplicarDanioNucleo(motor, dano); //descuenta la vida y obtiene el dano que pudo aplicar

            registrarDanoMetricas(motor, enemigo.tipo, danoAplicado); //actualiza las estadisticas con la vida que realmente se perdio
            registrarEventoMotor(motor, DANO_NUCLEO, static_cast<int>(enemigo.tipo), enemigo.color, danoAplicado); //guarda el evento de contacto con el nucleo

            actual = eliminarEnemigo(motor.enemigos, actual); //borra al enemigo que choco y conserva donde seguir revisando
        }
        else { //mantiene al enemigo cuando no alcanzo la nave ni el nucleo
            actual = actual->sig; //continua con el siguiente enemigo
        }
    }
}

//calcula el multiplicador de puntos segun el combo y lo limita entre uno y ocho
int calcularMultiplicador(int combo) {
    int multiplicador = 1 + combo / 6; //aumenta el multiplicador por cada grupo completo de seis bajas del combo

    if (multiplicador > 8) multiplicador = 8; //impide que el multiplicador supere ocho
    if (multiplicador < 1) multiplicador = 1; //mantiene como minimo el valor uno

    return multiplicador; //devuelve el valor que se aplicara a los puntos base
}

//devuelve los puntos base de cada tipo de enemigo antes de aplicar el multiplicador
int puntosBaseEnemigo(TipoEnemigo tipo) {
    switch (tipo) { //elige los puntos segun el tipo de enemigo eliminado
    case Dron: //asigna la recompensa base del dron
        return 100; //entrega cien puntos base por un dron

    case Girador: //asigna la recompensa base del girador
        return 150; //entrega ciento cincuenta puntos base por un girador

    case Divisor: //asigna la recompensa base del divisor
        return 200; //entrega doscientos puntos base por un divisor

    case Mini: //asigna la recompensa base del mini
        return 75; //entrega setenta y cinco puntos base por un mini

    case Tanque: //asigna la recompensa base del tanque
        return 400; //entrega cuatrocientos puntos base por un tanque

    case Camaleon: //asigna la recompensa base del camaleon
        return 300; //entrega trescientos puntos base por un camaleon
    }

    return 0; //no asigna puntos si el tipo recibido no corresponde a los anteriores
}

//premia una baja con bala aumentando el combo antes de calcular los puntos y sumando carga para el pulso
static int premiarBajaPorBala(Motor& motor, TipoEnemigo tipo) {
    if (motor.jugador == nullptr) { //comprueba si falta el jugador que debe recibir la recompensa
        return 0; //no entrega puntos cuando no hay un jugador asociado
    }

    motor.jugador->combo++; //cuenta la baja con bala antes de obtener el multiplicador

    int multiplicador = calcularMultiplicador(motor.jugador->combo); //calcula el bono usando el combo que ya incluye esta baja
    int puntos = puntosBaseEnemigo(tipo) * multiplicador; //multiplica los puntos del enemigo por el bono del combo

    motor.jugador->puntaje += puntos; //agrega la recompensa al puntaje acumulado

    if (tipo == Tanque) motor.jugador->cargaPulso += 14; //suma catorce de carga por eliminar un tanque con bala
    else motor.jugador->cargaPulso += 5; //suma cinco de carga por cualquier otro tipo

    if (motor.jugador->cargaPulso > 100) { //comprueba si la recompensa supero la carga maxima
        motor.jugador->cargaPulso = 100; //deja el pulso cargado hasta su limite
    }

    if (motor.jugador->combo > motor.jugador->metricas.comboMaximo) { //revisa si esta baja establecio un nuevo mejor combo
        motor.jugador->metricas.comboMaximo = motor.jugador->combo; //guarda el mayor combo alcanzado en la partida
    }

    return puntos; //devuelve los puntos ganados para registrarlos en la bitacora
}

//crea dos minis despues del divisor con colores distintos y giros opuestos antes de retirar al padre
static void insertarMinisDeDivisor(Motor& motor, NodoEnemigo* divisor) {
    Enemigo padre = divisor->dato; //copia la posicion y el color del divisor que acaba de morir

    Enemigo mini1{ motor.siguienteIdEnemigo++, Mini, (padre.color + 1) % 3, padre.r, normalizarAngulo(padre.theta - 4.0), -1.7, 1, 4, 0 }; //crea el primer mini con otro color cuatro grados menos y giro negativo

    Enemigo mini2{ motor.siguienteIdEnemigo++, Mini, (padre.color + 2) % 3, padre.r, normalizarAngulo(padre.theta + 4.0), 1.7, 1, 4, 0 }; //crea el segundo mini con el color restante cuatro grados mas y giro positivo

    NodoEnemigo* nodoMini1 = insertarDespues(motor.enemigos, divisor, mini1); //enlaza el primer mini inmediatamente despues del divisor
    insertarDespues(motor.enemigos, nodoMini1, mini2); //coloca el segundo mini despues del primero para conservar ese orden
}

//mueve cada bala resuelve su primer impacto segun el orden de la lista y retira las balas usadas o fuera del escenario
void moverBalasYResolverImpactos(Motor& motor) {
    NodoBala** enlace = &motor.balas.cabeza; //apunta al enlace que conduce a la bala actual para poder cambiarlo al borrar

    while (*enlace != nullptr) { //continua mientras el enlace apunte a una bala pendiente de revisar
        NodoBala* balaActual = *enlace; //obtiene la bala que se va a mover y comprobar

        balaActual->dato.x += balaActual->dato.vx; //avanza la bala en direccion horizontal
        balaActual->dato.y += balaActual->dato.vy; //avanza la bala en direccion vertical

        double radioBala = sqrt(balaActual->dato.x * balaActual->dato.x + balaActual->dato.y * balaActual->dato.y); //calcula la distancia de la bala al centro y no el tamano de la bala

        bool eliminarBala = radioBala > 320.0; //marca para borrar las balas que salieron del limite del escenario

        if (!eliminarBala) { //busca impactos solo mientras la bala permanezca dentro del limite
            NodoEnemigo* enemigoActual = motor.enemigos.cabeza; //empieza la busqueda por el primer enemigo de la lista

            while (enemigoActual != nullptr) { //revisa los enemigos hasta encontrar el primer impacto
                if (hayColisionBalaEnemigo(balaActual->dato, enemigoActual->dato)) { //comprueba si la bala entro en el radio de este enemigo
                    eliminarBala = true; //consume la bala aunque su color no coincida con el enemigo

                    if (balaActual->dato.color == enemigoActual->dato.color) { //comprueba si el impacto corresponde a la fase del enemigo
                        registrarImpactoMetricas(motor, balaActual->dato.color, true); //cuenta un acierto general y del color de la bala

                        enemigoActual->dato.vida--; //quita una unidad de vida por el impacto correcto

                        if (enemigoActual->dato.vida <= 0) { //revisa si el impacto termino con la vida del enemigo
                            Enemigo enemigoMuerto = enemigoActual->dato; //conserva sus datos antes de eliminar el nodo
                            int puntos = premiarBajaPorBala(motor, enemigoMuerto.tipo); //suma combo puntos y carga por la baja con bala

                            registrarBajaMetricas(motor, enemigoMuerto); //cuenta la baja por tipo y por el color que tenia al morir
                            registrarEventoMotor(motor, BAJA, static_cast<int>(enemigoMuerto.tipo), enemigoMuerto.color, puntos, enemigoMuerto.r, enemigoMuerto.theta); //guarda la baja con los puntos y la posicion donde ocurrio

                            if (enemigoMuerto.tipo == Divisor) { //comprueba si este enemigo debe dejar dos minis
                                insertarMinisDeDivisor(motor, enemigoActual); //inserta los minis mientras el nodo del divisor sigue disponible
                            }

                            eliminarEnemigo(motor.enemigos, enemigoActual); //retira al enemigo muerto y conserva los minis que se hayan creado
                        }
                    }
                    else { //aplica la penalizacion por golpear con una bala de otro color
                        registrarImpactoMetricas(motor, balaActual->dato.color, false); //cuenta el error segun el color de la bala
                        registrarEventoMotor(motor, IMPACTO_ERRONEO, static_cast<int>(enemigoActual->dato.tipo), balaActual->dato.color, 0); //guarda el impacto equivocado sin puntos

                        enemigoActual->dato.r += 9.0; //aleja al enemigo nueve unidades del centro sin quitarle vida

                        if (motor.jugador != nullptr) { //comprueba que exista el jugador cuyo combo se debe reiniciar
                            motor.jugador->combo = 0; //pierde el combo por usar un color incorrecto
                        }
                    }

                    break; //deja de buscar porque una bala solo puede golpear a un enemigo
                }
                enemigoActual = enemigoActual->sig; //prueba con el siguiente enemigo cuando no hubo choque
            }
        }
        if (eliminarBala) { //retira la bala si impacto o salio del escenario
            *enlace = balaActual->sig; //hace que el enlace anterior apunte a la siguiente bala
            delete balaActual; //libera el nodo de la bala que ya no se necesita
            motor.balas.tam--; //descuenta la bala sin avanzar el enlace para revisar la que quedo en su lugar
        }
        else { //mantiene la bala cuando todavia sigue en vuelo
            enlace = &balaActual->sig; //pasa al enlace de la siguiente bala despues de conservar la actual
        }
    }
}

//expande la onda activa elimina los enemigos que alcanza y la apaga cuando supera el radio permitido
void actualizarPulso(Motor& motor) {
    if (!motor.pulso.activo) return; //sale cuando no hay una onda de pulso en movimiento

    motor.pulso.radioActual += 12.0; //expande la onda doce unidades en este tick

    if (motor.pulso.radioActual > 320.0) { //comprueba si la onda ya supero el alcance permitido
        motor.pulso.activo = false; //desactiva el pulso al terminar su recorrido
        motor.pulso.radioActual = 0.0; //deja el radio listo para una futura activacion
        return; //termina sin revisar impactos fuera del alcance
    }

    NodoEnemigo* actual = motor.enemigos.cabeza; //empieza a buscar enemigos alcanzados por la onda

    while (actual != nullptr) { //revisa la lista y tambien los minis que se inserten durante este recorrido
        double diferencia = fabs(actual->dato.r - motor.pulso.radioActual); //mide la separacion entre el enemigo y el radio actual de la onda

        if (diferencia < actual->dato.radio + 3.0) { //comprueba si la onda alcanza el enemigo sin importar su color
            Enemigo enemigoMuerto = actual->dato; //conserva los datos del enemigo que el pulso va a eliminar
            int puntos = premiarBajaPorPulso(motor, enemigoMuerto.tipo); //suma puntos con el combo actual sin aumentar combo ni carga

            registrarBajaMetricas(motor, enemigoMuerto); //cuenta la baja por tipo y color del enemigo
            registrarEventoMotor(motor, BAJA, static_cast<int>(enemigoMuerto.tipo), enemigoMuerto.color, puntos, enemigoMuerto.r, enemigoMuerto.theta); //registra el enemigo eliminado con la recompensa y su posicion

            if (enemigoMuerto.tipo == Divisor) { //comprueba si la baja debe producir dos minis
                insertarMinisDeDivisor(motor, actual); //crea los minis antes de borrar el divisor para mantener los enlaces
            }

            actual = eliminarEnemigo(motor.enemigos, actual); //continua por el siguiente nodo que puede ser uno de los minis nuevos
        }
        else { //deja al enemigo en juego cuando la onda no lo alcanzo
            actual = actual->sig; //continua revisando el siguiente enemigo
        }
    }
}

//marca el fin del turno si falta el jugador muere el nucleo o se completa la oleada y cura solo al completarla
void verificarFinMotor(Motor& motor) {
    if (motor.jugador == nullptr) { //comprueba si falta el jugador asociado a este turno
        motor.turnoTerminado = true; //marca el turno como terminado porque no puede seguir sin jugador
        return; //sale sin revisar vida ni enemigos
    }

    if (motor.jugador->vidaNucleo <= 0) { //revisa primero si el nucleo fue destruido
        motor.jugador->vidaNucleo = 0; //evita conservar una cantidad negativa de vida

        registrarEventoMotor(motor, ELIMINACION, -1, -1, 0); //registra que el jugador quedo eliminado sin asociarlo a un enemigo concreto

        motor.turnoTerminado = true; //termina el turno por falta de vida
        return; //evita conceder la recuperacion de vida de una oleada completada
    }

    if (!motor.oleadaTerminada && colaSpawnVacia(motor.spawns) && listaEnemigosVacia(motor.enemigos)) { //comprueba que no queden apariciones ni enemigos y que la oleada no se haya cerrado antes
        motor.oleadaTerminada = true; //marca que el jugador completo la oleada
        motor.turnoTerminado = true; //termina su turno aunque todavia puedan quedar balas en vuelo

        registrarEventoMotor(motor, FIN_OLEADA, -1, -1, 0); //guarda el cierre de la oleada en la bitacora

        motor.jugador->vidaNucleo += 8; //recupera ocho de vida por completar la oleada

        if (motor.jugador->vidaNucleo > 100) { //comprueba si la recuperacion supero la vida maxima
            motor.jugador->vidaNucleo = 100; //limita la vida del nucleo a cien
        }
    }
}

//ejecuta en orden un paso de la simulacion y aumenta el tick salvo que el turno ya hubiera terminado
void procesarTick(Motor& motor, const AccionMotor& accion) {
    if (motor.turnoTerminado) return; //evita seguir simulando cuando el turno ya termino

    aplicarAccion(motor, accion); //recibe los cambios de destino color y solicitud de pulso
    actualizarNave(motor); //acerca la nave al angulo elegido dentro de su limite de movimiento
    procesarSpawns(motor); //crea los enemigos cuya aparicion ya corresponde a este tick
    dispararAutomaticamente(motor); //crea una bala si este tick corresponde a un disparo
    moverEnemigos(motor); //actualiza las posiciones y los cambios de color de los enemigos
    resolverChoquesEnemigos(motor); //aplica los choques con la nave y el nucleo antes de revisar las balas
    moverBalasYResolverImpactos(motor); //mueve las balas y resuelve sus impactos contra los enemigos restantes
    actualizarPulso(motor); //expande la onda y elimina los enemigos que alcanza
    actualizarMetricasPasoTick(motor); //cuenta el tick jugado y actualiza las estadisticas del jugador
    verificarFinMotor(motor); //comprueba si hubo eliminacion o si la oleada quedo completada

    motor.tickActual++; //avanza el contador incluso si este tick acaba de terminar el turno
}
