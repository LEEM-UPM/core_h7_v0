/* En este archivo se iran añadiendo las funciones de alto nivel
encargadas de la gestion del resto de placas, toma de decisiones y un largo
etcetera */

#ifndef SYS_UTILITIES_H
#define SYS_UTILITIES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool power;
  bool avionics;
  bool rf;
} sys_state;

void sys_init(sys_state *state);

#endif /* SYS_UTILITIES_H */

