#ifndef ACCION_MOTOR_H
#define ACCION_MOTOR_H

struct AccionMotor {
    bool cambiarAngulo;
    double anguloObjetivo;
    bool cambiarColor;
    int colorNuevo;
    bool solicitarPulso;
};

#endif