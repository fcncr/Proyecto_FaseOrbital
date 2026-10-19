#include "Motor.h"
#include <cmath>

static double gradosARadianes(double grados) {
    const double PI = 3.14159265358979323846;
    return grados * PI / 180.0;
}

static void polarACartesiano(double radio, double angulo, double& x, double& y) {
    double radianes = gradosARadianes(angulo);

    x = radio * cos(radianes);
    y = radio * sin(radianes);
}

static void registrarEventoMotor(Motor& motor, TipoEvento tipoEvento, int tipoEnemigo, int color, int valor) {
    if (motor.bitacora == nullptr || motor.jugador == nullptr) {
        return;
    }

    Evento evento;
    evento.tick = motor.tickActual;
    evento.oleada = motor.numeroOleada;
    evento.jugador = motor.jugador->alias;
    evento.tipoEvento = tipoEvento;
    evento.tipoEnemigo = tipoEnemigo;
    evento.color = color;
    evento.valor = valor;

    agregarEventoFinal(*motor.bitacora, evento);
}

static void registrarImpactoMetricas(Motor& motor, int color, bool correcto) {
    if (motor.jugador == nullptr) {
        return;
    }

    MetricasJugador& metricas = motor.jugador->metricas;

    if (correcto) {
        metricas.impactosCorrectos++;
    }
    else {
        metricas.impactosErroneos++;
    }

    if (color == 0) {
        if (correcto) metricas.impactosCorrectosC++;
        else metricas.impactosErroneosC++;
    }
    else if (color == 1) {
        if (correcto) metricas.impactosCorrectosM++;
        else metricas.impactosErroneosM++;
    }
    else if (color == 2) {
        if (correcto) metricas.impactosCorrectosA++;
        else metricas.impactosErroneosA++;
    }
}

static void registrarBajaMetricas(Motor& motor, const Enemigo& enemigo) {
    if (motor.jugador == nullptr) {
        return;
    }

    MetricasJugador& metricas = motor.jugador->metricas;

    switch (enemigo.tipo) {
    case Dron:
        metricas.bajasDron++;
        break;
    case Girador:
        metricas.bajasGirador++;
        break;
    case Divisor:
        metricas.bajasDivisor++;
        break;
    case Mini:
        metricas.bajasMini++;
        break;
    case Tanque:
        metricas.bajasTanque++;
        break;
    case Camaleon:
        metricas.bajasCamaleon++;
        break;
    }

    if (enemigo.color == 0) metricas.bajasColor0++;
    else if (enemigo.color == 1) metricas.bajasColor1++;
    else if (enemigo.color == 2) metricas.bajasColor2++;

    if (motor.jugador->combo > metricas.comboMaximo) {
        metricas.comboMaximo = motor.jugador->combo;
    }
}

static void registrarDanoMetricas(Motor& motor, TipoEnemigo tipo, int dano) {
    if (motor.jugador == nullptr) {
        return;
    }

    MetricasJugador& metricas = motor.jugador->metricas;
    metricas.danoTotal += dano;

    switch (tipo) {
    case Dron:
        metricas.danoDron += dano;
        break;
    case Girador:
        metricas.danoGirador += dano;
        break;
    case Divisor:
        metricas.danoDivisor += dano;
        break;
    case Mini:
        metricas.danoMini += dano;
        break;
    case Tanque:
        metricas.danoTanque += dano;
        break;
    case Camaleon:
        metricas.danoCamaleon += dano;
        break;
    }
}

static void actualizarMetricasPasoTick(Motor& motor) {
    if (motor.jugador == nullptr) {
        return;
    }

    MetricasJugador& metricas = motor.jugador->metricas;

    metricas.ticksSobrevividos++;
    metricas.puntaje = motor.jugador->puntaje;
    metricas.oleadaAlcanzada = motor.jugador->oleadaAlcanzada;

    actualizarMetricasCalculadas(metricas);
}

static int aplicarDanioNucleo(Motor& motor, int danio) {
    if (motor.jugador == nullptr) {
        return 0;
    }

    int vidaAntes = motor.jugador->vidaNucleo;

    motor.jugador->vidaNucleo -= danio;

    if (motor.jugador->vidaNucleo < 0) {
        motor.jugador->vidaNucleo = 0;
    }

    motor.jugador->combo = 0;

    return vidaAntes - motor.jugador->vidaNucleo;
}

static bool hayColisionBalaEnemigo(const Bala& bala, const Enemigo& enemigo) {
    double enemigoX;
    double enemigoY;

    polarACartesiano(enemigo.r, enemigo.theta, enemigoX, enemigoY);

    double diferenciaX = bala.x - enemigoX;
    double diferenciaY = bala.y - enemigoY;

    double distancia = sqrt(diferenciaX * diferenciaX + diferenciaY * diferenciaY);

    return distancia < enemigo.radio;
}

static int premiarBajaPorPulso(Motor& motor, TipoEnemigo tipo) {
    if (motor.jugador == nullptr) {
        return 0;
    }

    int multiplicador = calcularMultiplicador(motor.jugador->combo);
    int puntos = puntosBaseEnemigo(tipo) * multiplicador;

    motor.jugador->puntaje += puntos;

    return puntos;
}

void inicializarMotor(Motor& motor, Jugador* jugador, int numeroOleada, const ColaSpawn& spawns, BitacoraEventos* bitacora) {
    motor.tickActual = 0;
    motor.numeroOleada = numeroOleada;

    motor.nave.anguloActual = 270.0;
    motor.nave.anguloObjetivo = 270.0;
    motor.nave.color = 0;

    motor.pulso.activo = false;
    motor.pulso.radioActual = 0.0;

    motor.jugador = jugador;
    motor.bitacora = bitacora;

    inicializarListaEnemigos(motor.enemigos);
    inicializarListaBalas(motor.balas);
    inicializarColaSpawn(motor.spawns);
    copiarColaSpawn(spawns, motor.spawns);

    motor.siguienteIdEnemigo = 1;
    motor.oleadaTerminada = false;
    motor.turnoTerminado = false;

    if (motor.jugador != nullptr) {
        motor.jugador->combo = 0;
    }
}

void destruirMotor(Motor& motor) {
    destruirListaEnemigos(motor.enemigos);
    destruirListaBalas(motor.balas);
    destruirColaSpawn(motor.spawns);

    motor.jugador = nullptr;
    motor.bitacora = nullptr;
}

double normalizarAngulo(double angulo) {
    while (angulo >= 360.0) angulo -= 360.0;
    while (angulo < 0.0) angulo += 360.0;

    return angulo;
}

void aplicarAccion(Motor& motor, const AccionMotor& accion) {
    if (accion.cambiarAngulo) motor.nave.anguloObjetivo = normalizarAngulo(accion.anguloObjetivo);

    if (accion.cambiarColor && accion.colorNuevo >= 0 && accion.colorNuevo <= 2) {
        motor.nave.color = accion.colorNuevo;
    }

    if (accion.solicitarPulso && motor.jugador != nullptr && motor.jugador->cargaPulso == 100 && !motor.pulso.activo) {
        motor.pulso.activo = true;
        motor.pulso.radioActual = 20.0;
        motor.jugador->cargaPulso = 0;

        motor.jugador->metricas.pulsosUtilizados++;
        registrarEventoMotor(motor, PULSO, -1, -1, 0);
    }
}

void actualizarNave(Motor& motor) {
    motor.nave.anguloActual = normalizarAngulo(motor.nave.anguloActual);
    motor.nave.anguloObjetivo = normalizarAngulo(motor.nave.anguloObjetivo);

    double diferencia = motor.nave.anguloObjetivo - motor.nave.anguloActual;

    if (diferencia > 180.0) diferencia -= 360.0;
    if (diferencia < -180.0) diferencia += 360.0;

    if (diferencia > 6.0) motor.nave.anguloActual += 6.0;
    else if (diferencia < -6.0) motor.nave.anguloActual -= 6.0;
    else motor.nave.anguloActual = motor.nave.anguloObjetivo;

    motor.nave.anguloActual = normalizarAngulo(motor.nave.anguloActual);
}

Enemigo crearEnemigoDesdeSpawn(Motor& motor, const Spawn& spawn) {
    Enemigo enemigo;

    enemigo.id = motor.siguienteIdEnemigo;
    motor.siguienteIdEnemigo++;

    enemigo.tipo = spawn.tipo;
    enemigo.color = spawn.color;
    enemigo.r = 300.0;
    enemigo.theta = normalizarAngulo(spawn.angulo);
    enemigo.omega = 0.0;
    enemigo.vida = 1;
    enemigo.radio = 6;
    enemigo.ticksColor = 0;

    double velocidadAngularGirador = 0.9 + 0.05 * motor.numeroOleada;

    switch (spawn.tipo) {
    case Dron:
        enemigo.vida = 1;
        enemigo.radio = 6;
        break;

    case Girador:
        enemigo.vida = 1;
        enemigo.radio = 6;

        if (spawn.sentido == 0) enemigo.omega = -velocidadAngularGirador;
        else enemigo.omega = velocidadAngularGirador;

        break;

    case Divisor:
        enemigo.vida = 1;
        enemigo.radio = 8;
        break;

    case Mini:
        enemigo.vida = 1;
        enemigo.radio = 4;
        break;

    case Tanque:
        enemigo.vida = 3;
        enemigo.radio = 10;
        break;

    case Camaleon:
        enemigo.vida = 1;
        enemigo.radio = 7;
        break;
    }

    return enemigo;
}

void procesarSpawns(Motor& motor) {
    const Spawn* frente = verFrenteSpawn(motor.spawns);

    while (frente != nullptr && frente->tick <= motor.tickActual) {
        Spawn spawn;
        desencolarSpawn(motor.spawns, spawn);

        Enemigo enemigo = crearEnemigoDesdeSpawn(motor, spawn);
        insertarFinal(motor.enemigos, enemigo);

        frente = verFrenteSpawn(motor.spawns);
    }
}

void moverEnemigos(Motor& motor) {
    double velocidadBase = 0.8 + 0.08 * motor.numeroOleada;

    NodoEnemigo* actual = motor.enemigos.cabeza;

    while (actual != nullptr) {
        Enemigo& enemigo = actual->dato;

        switch (enemigo.tipo) {
        case Dron:
            enemigo.r -= 1.00 * velocidadBase;
            break;

        case Girador:
            enemigo.r -= 0.85 * velocidadBase;
            enemigo.theta = normalizarAngulo(enemigo.theta + enemigo.omega);
            break;

        case Divisor:
            enemigo.r -= 0.80 * velocidadBase;
            break;

        case Mini:
            enemigo.r -= 1.25 * velocidadBase;
            enemigo.theta = normalizarAngulo(enemigo.theta + enemigo.omega);
            break;

        case Tanque:
            enemigo.r -= 0.50 * velocidadBase;
            break;

        case Camaleon:
            enemigo.r -= 0.85 * velocidadBase;
            enemigo.ticksColor++;

            if (enemigo.ticksColor >= 42) {
                enemigo.color = (enemigo.color + 1) % 3;
                enemigo.ticksColor = 0;
            }

            break;
        }

        actual = actual->sig;
    }
}

void dispararAutomaticamente(Motor& motor) {
    if (motor.tickActual % 4 != 0) return;

    double x;
    double y;

    polarACartesiano(100.0, motor.nave.anguloActual, x, y);

    double radianes = gradosARadianes(motor.nave.anguloActual);

    Bala bala{
        x,
        y,
        9.0 * cos(radianes),
        9.0 * sin(radianes),
        motor.nave.color,
        0
    };

    insertarInicio(motor.balas, bala);
    if (motor.jugador != nullptr) { //los disparos no generan Evento
        motor.jugador->metricas.disparos++;
    }
}

void resolverChoquesEnemigos(Motor& motor) {
    if (motor.jugador == nullptr) return;

    double naveX;
    double naveY;

    polarACartesiano(100.0, motor.nave.anguloActual, naveX, naveY);

    NodoEnemigo* actual = motor.enemigos.cabeza;

    while (actual != nullptr) {
        double enemigoX;
        double enemigoY;

        polarACartesiano(actual->dato.r, actual->dato.theta, enemigoX, enemigoY);

        double diferenciaX = enemigoX - naveX;
        double diferenciaY = enemigoY - naveY;
        double distanciaNave = sqrt(diferenciaX * diferenciaX + diferenciaY * diferenciaY);

        bool choqueNave = distanciaNave < actual->dato.radio + 5.0;
        bool choqueNucleo = actual->dato.r <= 20.0 + 0.4 * actual->dato.radio;

        if (choqueNave) {
            Enemigo enemigo = actual->dato;

            int danoAplicado = aplicarDanioNucleo(motor, 12);

            registrarDanoMetricas(motor, enemigo.tipo, danoAplicado);
            registrarEventoMotor(motor, CHOQUE_NAVE, static_cast<int>(enemigo.tipo), enemigo.color, danoAplicado);

            actual = eliminarEnemigo(motor.enemigos, actual);
        }

        else if (choqueNucleo) {
            Enemigo enemigo = actual->dato;
            int dano = enemigo.tipo == Tanque ? 22 : 10;

            int danoAplicado = aplicarDanioNucleo(motor, dano);

            registrarDanoMetricas(motor, enemigo.tipo, danoAplicado);
            registrarEventoMotor(motor, DANO_NUCLEO, static_cast<int>(enemigo.tipo), enemigo.color, danoAplicado);

            actual = eliminarEnemigo(motor.enemigos, actual);
        }

        else {
            actual = actual->sig;
        }
    }
}

int calcularMultiplicador(int combo) {
    int multiplicador = 1 + combo / 6;

    if (multiplicador > 8) multiplicador = 8;
    if (multiplicador < 1) multiplicador = 1;

    return multiplicador;
}

int puntosBaseEnemigo(TipoEnemigo tipo) {
    switch (tipo) {
    case Dron:
        return 100;

    case Girador:
        return 150;

    case Divisor:
        return 200;

    case Mini:
        return 75;

    case Tanque:
        return 400;

    case Camaleon:
        return 300;
    }

    return 0;
}

static int premiarBajaPorBala(Motor& motor, TipoEnemigo tipo) {
    if (motor.jugador == nullptr) {
        return 0;
    }

    motor.jugador->combo++;

    int multiplicador = calcularMultiplicador(motor.jugador->combo);
    int puntos = puntosBaseEnemigo(tipo) * multiplicador;

    motor.jugador->puntaje += puntos;

    if (tipo == Tanque) motor.jugador->cargaPulso += 14;
    else motor.jugador->cargaPulso += 5;

    if (motor.jugador->cargaPulso > 100) {
        motor.jugador->cargaPulso = 100;
    }

    if (motor.jugador->combo > motor.jugador->metricas.comboMaximo) {
        motor.jugador->metricas.comboMaximo = motor.jugador->combo;
    }

    return puntos;
}

static void insertarMinisDeDivisor(Motor& motor, NodoEnemigo* divisor) {
    Enemigo padre = divisor->dato;

    Enemigo mini1{
        motor.siguienteIdEnemigo++,
        Mini,
        (padre.color + 1) % 3,
        padre.r,
        normalizarAngulo(padre.theta - 4.0),
        -1.7,
        1,
        4,
        0
    };

    Enemigo mini2{
        motor.siguienteIdEnemigo++,
        Mini,
        (padre.color + 2) % 3,
        padre.r,
        normalizarAngulo(padre.theta + 4.0),
        1.7,
        1,
        4,
        0
    };

    NodoEnemigo* nodoMini1 = insertarDespues(motor.enemigos, divisor, mini1);
    insertarDespues(motor.enemigos, nodoMini1, mini2);
}

void moverBalasYResolverImpactos(Motor& motor) {
    NodoBala** enlace = &motor.balas.cabeza;

    while (*enlace != nullptr) {
        NodoBala* balaActual = *enlace;

        balaActual->dato.x += balaActual->dato.vx;
        balaActual->dato.y += balaActual->dato.vy;

        double radioBala = sqrt(
            balaActual->dato.x * balaActual->dato.x +
            balaActual->dato.y * balaActual->dato.y
        );

        bool eliminarBala = radioBala > 320.0;

        if (!eliminarBala) {
            NodoEnemigo* enemigoActual = motor.enemigos.cabeza;

            while (enemigoActual != nullptr) {
                if (hayColisionBalaEnemigo(balaActual->dato, enemigoActual->dato)) {
                    eliminarBala = true;

                    if (balaActual->dato.color == enemigoActual->dato.color) {
                        registrarImpactoMetricas(motor, balaActual->dato.color, true);

                        enemigoActual->dato.vida--;

                        if (enemigoActual->dato.vida <= 0) {
                            Enemigo enemigoMuerto = enemigoActual->dato;
                            int puntos = premiarBajaPorBala(motor, enemigoMuerto.tipo);

                            registrarBajaMetricas(motor, enemigoMuerto);
                            registrarEventoMotor(motor, BAJA, static_cast<int>(enemigoMuerto.tipo), enemigoMuerto.color, puntos);

                            if (enemigoMuerto.tipo == Divisor) {
                                insertarMinisDeDivisor(motor, enemigoActual);
                            }

                            eliminarEnemigo(motor.enemigos, enemigoActual);
                        }
                    }
                    else {
                        registrarImpactoMetricas(motor, balaActual->dato.color, false);
                        registrarEventoMotor(motor, IMPACTO_ERRONEO, static_cast<int>(enemigoActual->dato.tipo), balaActual->dato.color, 0);

                        enemigoActual->dato.r += 9.0;

                        if (motor.jugador != nullptr) {
                            motor.jugador->combo = 0;
                        }
                    }

                    break;
                }
                enemigoActual = enemigoActual->sig;
            }
        }
        if (eliminarBala) {
            *enlace = balaActual->sig;
            delete balaActual;
            motor.balas.tam--;
        }
        else {
            enlace = &balaActual->sig;
        }
    }
}

void actualizarPulso(Motor& motor) {
    if (!motor.pulso.activo) return;

    motor.pulso.radioActual += 12.0;

    if (motor.pulso.radioActual > 320.0) {
        motor.pulso.activo = false;
        motor.pulso.radioActual = 0.0;
        return;
    }

    NodoEnemigo* actual = motor.enemigos.cabeza;

    while (actual != nullptr) {
        double diferencia = fabs(actual->dato.r - motor.pulso.radioActual);

        if (diferencia < actual->dato.radio + 3.0) {
            Enemigo enemigoMuerto = actual->dato;
            int puntos = premiarBajaPorPulso(motor, enemigoMuerto.tipo);

            registrarBajaMetricas(motor, enemigoMuerto);
            registrarEventoMotor(motor, BAJA, static_cast<int>(enemigoMuerto.tipo), enemigoMuerto.color, puntos);

            if (enemigoMuerto.tipo == Divisor) {
                insertarMinisDeDivisor(motor, actual);
            }

            actual = eliminarEnemigo(motor.enemigos, actual);
        }

        else {
            actual = actual->sig;
        }
    }
}

void verificarFinMotor(Motor& motor) {
    if (motor.jugador == nullptr) {
        motor.turnoTerminado = true;
        return;
    }

    if (motor.jugador->vidaNucleo <= 0) {
        motor.jugador->vidaNucleo = 0;

        registrarEventoMotor(motor, ELIMINACION, -1, -1, 0);

        motor.turnoTerminado = true;
        return;
    }

    if (!motor.oleadaTerminada && colaSpawnVacia(motor.spawns) && listaEnemigosVacia(motor.enemigos)) {
        motor.oleadaTerminada = true;
        motor.turnoTerminado = true;

        registrarEventoMotor(motor, FIN_OLEADA, -1, -1, 0);

        motor.jugador->vidaNucleo += 8;

        if (motor.jugador->vidaNucleo > 100) {
            motor.jugador->vidaNucleo = 100;
        }
    }
}

void procesarTick(Motor& motor, const AccionMotor& accion) {
    if (motor.turnoTerminado) return;

    aplicarAccion(motor, accion);

    actualizarNave(motor);

    procesarSpawns(motor);

    dispararAutomaticamente(motor);

    moverEnemigos(motor);

    resolverChoquesEnemigos(motor);

    moverBalasYResolverImpactos(motor);

    actualizarPulso(motor);

    actualizarMetricasPasoTick(motor);

    verificarFinMotor(motor);

    motor.tickActual++;
}