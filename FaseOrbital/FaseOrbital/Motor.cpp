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

static void aplicarDanioNucleo(Motor& motor, int danio) {
    if (motor.jugador == nullptr) return;

    motor.jugador->vidaNucleo -= danio;

    if (motor.jugador->vidaNucleo < 0) {
        motor.jugador->vidaNucleo = 0;
    }

    motor.jugador->combo = 0;
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

static void premiarBajaPorPulso(Motor& motor, TipoEnemigo tipo) {
    if (motor.jugador == nullptr) return;

    int multiplicador = calcularMultiplicador(motor.jugador->combo);
    motor.jugador->puntaje += puntosBaseEnemigo(tipo) * multiplicador;
}

void inicializarMotor(Motor& motor, Jugador* jugador, int numeroOleada, const ColaSpawn& spawns) {
    motor.tickActual = 0;
    motor.numeroOleada = numeroOleada;

    motor.nave.anguloActual = 270.0;
    motor.nave.anguloObjetivo = 270.0;
    motor.nave.color = 0;

    motor.pulso.activo = false;
    motor.pulso.radioActual = 0.0;

    motor.jugador = jugador;

    inicializarListaEnemigos(motor.enemigos);
    inicializarListaBalas(motor.balas);
    inicializarColaSpawn(motor.spawns);
    copiarColaSpawn(spawns, motor.spawns);

    motor.siguienteIdEnemigo = 1;

    motor.oleadaTerminada = false;
    motor.turnoTerminado = false;

    if (motor.jugador != nullptr) motor.jugador->combo = 0;
}

void destruirMotor(Motor& motor) {
    destruirListaEnemigos(motor.enemigos);
    destruirListaBalas(motor.balas);
    destruirColaSpawn(motor.spawns);

    motor.jugador = nullptr;
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
            aplicarDanioNucleo(motor, 12);
            actual = eliminarEnemigo(motor.enemigos, actual);
        }
        else if (choqueNucleo) {
            int danio = actual->dato.tipo == Tanque ? 22 : 10;

            aplicarDanioNucleo(motor, danio);
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

static void premiarBajaPorBala(Motor& motor, TipoEnemigo tipo) {
    if (motor.jugador == nullptr) return;

    motor.jugador->combo++;

    int multiplicador = calcularMultiplicador(motor.jugador->combo);
    motor.jugador->puntaje += puntosBaseEnemigo(tipo) * multiplicador;

    if (tipo == Tanque) motor.jugador->cargaPulso += 14;
    else motor.jugador->cargaPulso += 5;

    if (motor.jugador->cargaPulso > 100) {
        motor.jugador->cargaPulso = 100;
    }
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
                        enemigoActual->dato.vida--;

                        if (enemigoActual->dato.vida <= 0) {
                            TipoEnemigo tipoMuerto = enemigoActual->dato.tipo;

                            premiarBajaPorBala(motor, tipoMuerto);

                            if (tipoMuerto == Divisor) {
                                insertarMinisDeDivisor(motor, enemigoActual);
                            }

                            eliminarEnemigo(motor.enemigos, enemigoActual);
                        }
                    }
                    else {
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
            TipoEnemigo tipoMuerto = actual->dato.tipo;

            premiarBajaPorPulso(motor, tipoMuerto);

            if (tipoMuerto == Divisor) {
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
        motor.turnoTerminado = true;
        return;
    }

    if (!motor.oleadaTerminada && colaSpawnVacia(motor.spawns) && listaEnemigosVacia(motor.enemigos)) {
        motor.oleadaTerminada = true;
        motor.turnoTerminado = true;

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

    // Paso 9 reservado para eventos y estadisticas.

    verificarFinMotor(motor);

    motor.tickActual++;
}