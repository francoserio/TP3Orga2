/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "sched.h"
#include "i386.h"

void sched_inicializar() {
    turnoPirata = 0;
    proximaTareaA = 0;
    estaEnIdle = 1;
    proximaTareaB = 0;
}

unsigned int sched_tick() {
  if (estaEnIdle) {
    game_tick(13);
    return (13 << 3);
  } else {
    if (turnoPirata == 0) {
      //turno proximo es A
      uint proxTarea = EMPIEZAN_TSS + proximaTareaA;
      game_tick(proxTarea);
      return (proxTarea) << 3;
    } else {
      //turno proximo es B
      uint proxTarea = EMPIEZAN_TSS + 8 + proximaTareaB;
      game_tick(proxTarea);
      return (proxTarea) << 3;
    }
  }
}

unsigned int sched_proxima_a_ejecutar() {
  if (turnoPirata == 0) {
    //turno del jugador A
    return proximaTareaA;
  } else {
    //turno del jugador B
    return proximaTareaB;
  }
}

void sched_intercambiar_por_idle() {
  estaEnIdle = 1;
}

void sched_nointercambiar_por_idle() {
  estaEnIdle = 0;
}

void sched_toggle_debug() {
  if (modoDebug) {
    modoDebug = FALSE;
  } else {
    modoDebug = TRUE;
  }
}
