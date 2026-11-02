#ifndef PRUEBAS_MOTOR_H
#define PRUEBAS_MOTOR_H

//revisa las operaciones del motor el orden del tick los scripts y el modo lotes hasta el primer fallo
bool pruebaMotorBase();

//comprueba que un impacto de otro color empuje al enemigo sin matarlo y rompa el combo sin dar puntos
bool pruebaP09();

//verifica la creacion de dos minis al abatir un divisor junto con sus enlaces y las recompensas
bool pruebaP10();

//prueba el pulso con carga insuficiente y completa y revisa sus bajas y puntos sin aumentar el combo
bool pruebaP11();

#endif
