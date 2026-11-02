#ifndef BOT_H
#define BOT_H

#include "Motor.h"

//elige un objetivo fuera de la orbita y decide los cambios de angulo fase y solicitud de pulso sin modificar el motor
AccionMotor obtenerAccionBot(const Motor& motor);

#endif