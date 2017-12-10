/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "game.h"
#include "tss.h"

int tareaActualA;  //indice 0-7
int tareaActualB;  //indice 0-7
uint proximaTareaA; //indice 0-7
uint proximaTareaB; //indice 0-7
uchar turnoPirata; //0 A, 1 B
uchar estaEnIdle; // 0 NO, 1 SI
uint modoDebugPantalla; //pantalla debug 0 NO, 1 SI
uint modoDebugActivado; //y esta apretado o no. 0 NO, 1 SI
int proxTareaAMuerta;
int proxTareaBMuerta;

void sched_inicializar();
unsigned int sched_proxima_a_ejecutar();
unsigned int sched_tick();
void sched_toggle_debug();
void sched_nointercambiar_por_idle();
void sched_intercambiar_por_idle();
void sched_agregar(jugador_t* jugador);
void sched_sacar(jugador_t* jug, uint idx);

#endif	/* !__SCHED_H__ */
