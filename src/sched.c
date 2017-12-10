/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "sched.h"
#include "i386.h"
#include "screen.h"
#include "game.h"

void sched_inicializar() {
    turnoPirata = 0;
    proximaTareaA = 0;
    estaEnIdle = 1;
    proximaTareaB = 0;
    modoDebugActivado = 1;
    tareaActualA = 0;
    tareaActualB = 0;
    proxTareaAMuerta = 0;
    proxTareaBMuerta = 0;
    modoDebugPantalla = 0;
}

unsigned int sched_tick() {
  if (estaEnIdle == 1) {
    estaEnIdle = 0;
    if (turnoPirata == 0) {
      //turno jug A
      uint proxTarea = EMPIEZAN_TSS + proximaTareaA;
      game_tick(proxTarea);
      turnoPirata = 1;
      uchar noEncontreNinguna = 1;
      uchar todosMuertos = 0;
      int i = proximaTareaA + 1;
      while (noEncontreNinguna == 1 && todosMuertos == 0) {
        if (i == 8) {
          i = 0;
        }
        if (jugadorA.piratas[i].vivoMuerto == 1) {
          //si esta vivo la pongo como la proxima tarea de A
          tareaActualA = proximaTareaA;
          proximaTareaA = i;
          noEncontreNinguna = 0;
        } else {
          if (jugadorA.minerosPendientes->primero != jugadorA.minerosPendientes->ultimo) {
            tTuple elem = pop(jugadorA.minerosPendientes);
            game_jugador_relanzar_pirata(&jugadorA, minero, elem.posicionX, elem.posicionY);
          }
        }

        if (i == proximaTareaA && jugadorA.piratas[i].vivoMuerto == 0) {
          todosMuertos = 1;
        }

        i++;
      }
      if (todosMuertos) {
        //salto a la idle
        return (13) << 3;
      }

      return (proxTarea << 3);
    } else {
      //turno jug B
      uint proxTarea = EMPIEZAN_TSS + 8 + proximaTareaB;
      game_tick(proxTarea);
      turnoPirata = 0;
      uchar noEncontreNinguna = 1;
      uchar todosMuertos = 0;
      int i = proximaTareaB + 1;
      while (noEncontreNinguna == 1 && todosMuertos == 0) {
        if (i == 8) {
          i = 0;
        }
        if (jugadorB.piratas[i].vivoMuerto == 1) {
          //si esta vivo la pongo como la proxima tarea de B
          tareaActualB = proximaTareaB;
          proximaTareaB = i;
          noEncontreNinguna = 0;
        } else {
          if (jugadorB.minerosPendientes->primero != jugadorB.minerosPendientes->ultimo) {
            tTuple elem = pop(jugadorB.minerosPendientes);
            game_jugador_relanzar_pirata(&jugadorB, minero, elem.posicionX, elem.posicionY);
          }
        }

        if (i == proximaTareaB && jugadorB.piratas[i].vivoMuerto == 0) {
          todosMuertos = 1;
        }

        i++;

      }

      if (todosMuertos) {
        //salto a la idle
        return (13) << 3;
      }

      return (proxTarea << 3);
    }
  } else {
    if (turnoPirata == 0) {
      //turno proximo es A
      uint proxTarea = EMPIEZAN_TSS + proximaTareaA;
      game_tick(proxTarea);
      turnoPirata = 1;
      uchar noEncontreNinguna = 1;
      uchar todosMuertos = 0;
      int i = proximaTareaA + 1;
      while (noEncontreNinguna == 1 && todosMuertos == 0) {
        if (i == 8) {
          i = 0;
        }
        if (jugadorA.piratas[i].vivoMuerto == 1) {
          //si esta vivo la pongo como la proxima tarea de A
          tareaActualA = proximaTareaA;
          proximaTareaA = i;
          noEncontreNinguna = 0;
        } else {
          // breakpoint();
          if (jugadorA.minerosPendientes->primero != jugadorA.minerosPendientes->ultimo) {
            tTuple elem = pop(jugadorA.minerosPendientes);
            game_jugador_relanzar_pirata(&jugadorA, minero, elem.posicionX, elem.posicionY);
          }
        }

        if (i == proximaTareaA && jugadorA.piratas[i].vivoMuerto == 0) {
          todosMuertos = 1;
        }

        i++;

      }

      if (todosMuertos) {
        //salto a la idle
        return (13) << 3;
      }

      return (proxTarea << 3);
    } else {
      //turno proximo es B
      uint proxTarea = EMPIEZAN_TSS + 8 + proximaTareaB;
      game_tick(proxTarea);
      turnoPirata = 0;
      uchar noEncontreNinguna = 1;
      uchar todosMuertos = 0;
      int i = proximaTareaB + 1;
      while (noEncontreNinguna == 1 && todosMuertos == 0) {
        if (i == 8) {
          i = 0;
        }
        if (jugadorB.piratas[i].vivoMuerto == 1) {
          //si esta vivo la pongo como la proxima tarea de B
          tareaActualB = proximaTareaB;
          proximaTareaB = i;
          noEncontreNinguna = 0;
        } else {
          if (jugadorB.minerosPendientes->primero != jugadorB.minerosPendientes->ultimo) {
            tTuple elem = pop(jugadorB.minerosPendientes);
            game_jugador_relanzar_pirata(&jugadorB, minero, elem.posicionX, elem.posicionY);
          }
        }

        if (i == proximaTareaB && jugadorB.piratas[i].vivoMuerto == 0) {
          todosMuertos = 1;
        }

        i++;

      }
      if (todosMuertos) {
        //salto a la idle
        return (13) << 3;
      }

      return (proxTarea) << 3;
    }
  }
}

unsigned int sched_proxima_a_ejecutar() {
  return sched_tick();
}

void sched_intercambiar_por_idle() {
  estaEnIdle = 1;
}

void sched_nointercambiar_por_idle() {
  estaEnIdle = 0;
}

void sched_toggle_debug() {
  if (modoDebugPantalla) {
    game_modoDebug_close();
    modoDebugPantalla = 0;
  } else {
    if (modoDebugActivado) {
      modoDebugActivado = 0;
    } else {
      modoDebugActivado = 1;
    }
  }
}

void sched_agregar(jugador_t* jugador) {
  if (jugador->index == 0) {
    //es un pirata de A
    int i = tareaActualA + 1;
    uchar estanTodosVivos = 0;
    while (i < 8 && jugador->piratas[i].vivoMuerto == 1 && estanTodosVivos == 0) {
      if (i == 8) {
        i = 0;
      }

      if (i == tareaActualA && jugador->piratas[i].vivoMuerto == 1) {
        estanTodosVivos = 1;
      }

      i++;

    }

    if (estanTodosVivos) {
      proxTareaAMuerta = -1;
    } else {
      proxTareaAMuerta = i;
    }
  } else {
    //es un pirata de B
    int i = tareaActualB + 1;
    uchar estanTodosVivos = 0;
    while (i < 8 && jugador->piratas[i].vivoMuerto == 1 && estanTodosVivos == 0) {
      if (i == 8) {
        i = 0;
      }

      if (i == tareaActualB && jugador->piratas[i].vivoMuerto == 1) {
        estanTodosVivos = 1;
      }

      i++;

    }
    if (estanTodosVivos) {
      proxTareaBMuerta = -1;
    } else {
      proxTareaBMuerta = i;
    }
  }
}

void sched_sacar(jugador_t* jug, uint idx) {
  if (jug->index == 0) {
    //es jugadorA
    if (proxTareaAMuerta == -1) {
      // breakpoint();
      proxTareaAMuerta = idx;
    } else {
      if (proxTareaAMuerta > idx) {
        proxTareaAMuerta = idx;
      }
    }
  } else {
    //es jugadorB
    if (proxTareaBMuerta == -1) {
      proxTareaBMuerta = idx;
    } else {
      if (proxTareaBMuerta > idx) {
        proxTareaBMuerta = idx;
      }
    }
  }
}
