/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "sched.h"
#include "i386.h"
#include "screen.h"

void sched_inicializar() {
    turnoPirata = 0;
    proximaTareaA = 0;
    estaEnIdle = 1;
    proximaTareaB = 0;
    modoDebug = 0;
    tareaActualA = 0;
    tareaActualB = 0;
    turnoPirataActual = 0;
    proxTareaAMuerta = 0;
    proxTareaBMuerta = 0;
}

unsigned int sched_tick() {
  turnoPirataActual = turnoPirata;
  if (estaEnIdle == 1) {
    estaEnIdle = 0;
    if (turnoPirata == 0) {
      //turno jug A
      breakpoint();
      uint proxTarea = EMPIEZAN_TSS + proximaTareaA;
      game_tick(proxTarea);
      turnoPirata = 1;
      uchar noEncontreNinguna = 1;
      uchar todosMuertos = 0;
      int i = proximaTareaA + 1;
      breakpoint();
      while (noEncontreNinguna == 1 && todosMuertos == 0) {
        if (jugadorA.piratas[i].vivoMuerto == 1) {
          //si esta vivo la pongo como la proxima tarea de A
          tareaActualA = proximaTareaA;
          proximaTareaA = i;
          noEncontreNinguna = 0;
        }

        if (i == proximaTareaA && jugadorA.piratas[i].vivoMuerto == 0) {
          todosMuertos = 1;
        }

        i++;

        if (i == 8) {
          i = 0;
        }
      }
      breakpoint();
      if (todosMuertos) {
        //salto a la idle
        return (13) << 3;
      }

      print_dec(proxTarea, 10, 35, 20, C_FG_WHITE);
      breakpoint();


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
        if (jugadorB.piratas[i].vivoMuerto == 1) {
          //si esta vivo la pongo como la proxima tarea de B
          tareaActualB = proximaTareaB;
          proximaTareaB = i;
          noEncontreNinguna = 0;
        }

        if (i == proximaTareaB && jugadorB.piratas[i].vivoMuerto == 0) {
          todosMuertos = 1;
        }

        i++;

        if (i == 8) {
          i = 0;
        }
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
        if (jugadorA.piratas[i].vivoMuerto == 1) {
          //si esta vivo la pongo como la proxima tarea de A
          tareaActualA = proximaTareaA;
          proximaTareaA = i;
          noEncontreNinguna = 0;
        }

        if (i == proximaTareaA && jugadorA.piratas[i].vivoMuerto == 0) {
          todosMuertos = 1;
        }

        i++;

        if (i == 8) {
          i = 0;
        }
      }

      if (todosMuertos) {
        //salto a la idle
        return (13) << 3;
      }

      print_dec(proxTarea, 10, 35, 20, C_FG_WHITE);
      breakpoint();

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
        if (jugadorB.piratas[i].vivoMuerto == 1) {
          //si esta vivo la pongo como la proxima tarea de B
          tareaActualB = proximaTareaB;
          proximaTareaB = i;
          noEncontreNinguna = 0;
        }

        if (i == proximaTareaB && jugadorB.piratas[i].vivoMuerto == 0) {
          todosMuertos = 1;
        }

        i++;

        if (i == 8) {
          i = 0;
        }
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
  if (modoDebug) {
    modoDebug = FALSE;
  } else {
    modoDebug = TRUE;
  }
}

void sched_agregar(jugador_t* jugador) {
  if (jugador->index == 0) {
    //es un pirata de A
    int i = tareaActualA;
    while (i < 8 && jugador->piratas[i].vivoMuerto == 1) {

      i++;

      if (i == 8) {
        i = 0;
      }
    }

    proxTareaAMuerta = i;
  } else {
    //es un pirata de B
    int i = tareaActualB;
    while (i < 8 && jugador->piratas[i].vivoMuerto == 1) {

      i++;

      if (i == 8) {
        i = 0;
      }
    }

    proxTareaBMuerta = i;
  }
}

void sched_sacar(unsigned int idx) {

}
