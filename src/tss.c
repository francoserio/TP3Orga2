/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "mmu.h"
#include "gdt.h"
#include "sched.h"


void tss_inicializar() {

  //tss_idle

  tss_idle.ptl = 0;
  tss_idle.unused0 = 0;
  tss_idle.esp0 = 0;
  tss_idle.ss0 = 0;
  tss_idle.unused1 = 0;
  tss_idle.esp1 = 0;
  tss_idle.ss1 = 0;
  tss_idle.unused2 = 0;
  tss_idle.esp2 = 0;
  tss_idle.ss2 = 0;
  tss_idle.unused3 = 0;
  tss_idle.cr3 = (unsigned int)0x27000;
  tss_idle.eip = (unsigned int)0x16000;
  tss_idle.eflags = (unsigned int)0x00202;
  tss_idle.eax = 0;
  tss_idle.ecx = 0;
  tss_idle.edx = 0;
  tss_idle.ebx = 0;
  tss_idle.esp = (unsigned int)0x27000;
  tss_idle.ebp = (unsigned int)0x27000;
  tss_idle.esi = 0;
  tss_idle.edi = 0;
  tss_idle.es = (unsigned int)0x48;
  tss_idle.unused4 = 0;
  tss_idle.cs = (unsigned int)0x58;
  tss_idle.unused5 = 0;
  tss_idle.ss = (unsigned int)0x48;
  tss_idle.unused6 = 0;
  tss_idle.ds = (unsigned int)0x48;
  tss_idle.unused7 = 0;
  tss_idle.fs = (unsigned int)0x00060;
  tss_idle.unused8 = 0;
  tss_idle.gs = (unsigned int)0x48;
  tss_idle.unused9 = 0;
  tss_idle.ldt = 0;
  tss_idle.unused10 = 0;
  tss_idle.dtrap = 0;
  tss_idle.iomap = 0;

  //tss_inicial

  tss_inicial.ptl = 0;
  tss_inicial.unused0 = 0;
  tss_inicial.esp0 = 0;
  tss_inicial.ss0 = 0;
  tss_inicial.unused1 = 0;
  tss_inicial.esp1 = 0;
  tss_inicial.ss1 = 0;
  tss_inicial.unused2 = 0;
  tss_inicial.esp2 = 0;
  tss_inicial.ss2 = 0;
  tss_inicial.unused3 = 0;
  tss_inicial.cr3 = 0;
  tss_inicial.eip = 0;
  tss_inicial.eflags = 0;
  tss_inicial.eax = 0;
  tss_inicial.ecx = 0;
  tss_inicial.edx = 0;
  tss_inicial.ebx = 0;
  tss_inicial.esp = 0;
  tss_inicial.ebp = 0;
  tss_inicial.esi = 0;
  tss_inicial.edi = 0;
  tss_inicial.es = 0;
  tss_inicial.unused4 = 0;
  tss_inicial.cs = 0;
  tss_inicial.unused5 = 0;
  tss_inicial.ss = 0;
  tss_inicial.unused6 = 0;
  tss_inicial.ds = 0;
  tss_inicial.unused7 = 0;
  tss_inicial.fs = 0;
  tss_inicial.unused8 = 0;
  tss_inicial.gs = 0;
  tss_inicial.unused9 = 0;
  tss_inicial.ldt = 0;
  tss_inicial.unused10 = 0;
  tss_inicial.dtrap = 0;
  tss_inicial.iomap = 0;
}

void tss_agregar_a_gdt() {
  gdt[GDT_TSS_IDLE] = (gdt_entry) {
      (unsigned short)    0x0067,         /* limit[0:15]  */
      (unsigned short)    (int)(&tss_idle) & 0xFFFF,         /* base[0:15]   */
      (unsigned char)     (int)((int)(&tss_idle) >> 16) & 0x00FF,   /* base[23:16]  */
      (unsigned char)     0x09,           /* type         */
      (unsigned char)     0x00,           /* s            */
      (unsigned char)     0x00,           /* dpl          */
      (unsigned char)     0x01,           /* p            */
      (unsigned char)     0x00,           /* limit[16:19] */
      (unsigned char)     0x00,           /* avl          */
      (unsigned char)     0x00,           /* l            */
      (unsigned char)     0x00,           /* db           */
      (unsigned char)     0x00,           /* g            */
      (unsigned char)     (int)(&tss_idle) >> 24,           /* base[31:24]  */
  };
  gdt[GDT_TSS_INICIAL] = (gdt_entry) {
      (unsigned short)    0x0067,         /* limit[0:15]  */
      (unsigned short)    (int)(&tss_inicial) & 0xFFFF, /* base[0:15]   */
      (unsigned char)     (int)((int)(&tss_inicial) >> 16) & 0x00FF,           /* base[23:16]  */
      (unsigned char)     0x09,           /* type         */
      (unsigned char)     0x00,           /* s            */
      (unsigned char)     0x00,           /* dpl          */
      (unsigned char)     0x01,           /* p            */
      (unsigned char)     0x00,           /* limit[16:19] */
      (unsigned char)     0x00,           /* avl          */
      (unsigned char)     0x00,           /* l            */
      (unsigned char)     0x00,           /* db           */
      (unsigned char)     0x00,           /* g            */
      (unsigned char)     (int)(&tss_inicial) >> 24,           /* base[31:24]  */
  };
}

void completarTssPirata(pirata_t tarea) {
  unsigned int paginaParaPilaCero = mmu_proxima_pagina_fisica_libre() + 0x1000;

  tss* tss_pirata = (*(tarea.jugador)).index == JUGADOR_A ? &tss_jugadorA[tarea.id] : &tss_jugadorB[tarea.id];

  tss_pirata->ptl = 0;
  tss_pirata->unused0 = 0;
  tss_pirata->esp0 = paginaParaPilaCero;
  tss_pirata->ss0 = (unsigned int)0x40;
  tss_pirata->unused1 = 0;
  tss_pirata->esp1 = 0;
  tss_pirata->ss1 = 0;
  tss_pirata->unused2 = 0;
  tss_pirata->esp2 = 0;
  tss_pirata->ss2 = 0;
  tss_pirata->unused3 = 0;
  tss_pirata->cr3 = mmu_inicializar_dir_pirata(tarea.jugador, &tarea);
  tss_pirata->eip = 0x00400000;
  tss_pirata->eflags = (unsigned int)0x00202;
  tss_pirata->eax = 0;
  tss_pirata->ecx = 0;
  tss_pirata->edx = 0;
  tss_pirata->ebx = 0;
  tss_pirata->esp = 0x00401000 - 12;
  tss_pirata->ebp = 0x00401000 - 12;
  tss_pirata->esi = 0;
  tss_pirata->edi = 0;
  tss_pirata->es = (unsigned int)0x40 | 0x3;
  tss_pirata->unused4 = 0;
  tss_pirata->cs = (unsigned int)0x50 | 0x3;
  tss_pirata->unused5 = 0;
  tss_pirata->ss = (unsigned int)0x40 | 0x3;
  tss_pirata->unused6 = 0;
  tss_pirata->ds = (unsigned int)0x40 | 0x3;
  tss_pirata->unused7 = 0;
  tss_pirata->fs = (unsigned int)0x40 | 0x3;
  tss_pirata->unused8 = 0;
  tss_pirata->gs = (unsigned int)0x40 | 0x3;
  tss_pirata->unused9 = 0;
  tss_pirata->ldt = 0;
  tss_pirata->unused10 = 0;
  tss_pirata->dtrap = 0;
  tss_pirata->iomap = 0xFFFF;
}

void tss_agregar_piratas_a_gdt(jugador_t* j) {
  if (j->index == 0)  {
    gdt[EMPIEZAN_TSS + proximaTareaA] = (gdt_entry) {
      (unsigned short)    0x0067,         /* limit[0:15]  */
      (unsigned short)    (int)(&tss_jugadorA[jugadorA.piratas[proximaTareaA].index]) & 0xFFFF, /* base[0:15]   */
      (unsigned char)     (int)((int)(&tss_jugadorA[jugadorA.piratas[proximaTareaA].index]) >> 16) & 0x00FF,           /* base[23:16]  */
      (unsigned char)     0x09,           /* type         */
      (unsigned char)     0x00,           /* s            */
      (unsigned char)     0x03,           /* dpl          */
      (unsigned char)     0x01,           /* p            */
      (unsigned char)     0x00,           /* limit[16:19] */
      (unsigned char)     0x00,           /* avl          */
      (unsigned char)     0x00,           /* l            */
      (unsigned char)     0x00,           /* db           */
      (unsigned char)     0x00,           /* g            */
      (unsigned char)     (int)(&tss_jugadorA[jugadorA.piratas[proximaTareaA].index]) >> 24,           /* base[31:24]  */
    };
    completarTssPirata(jugadorA.piratas[proximaTareaA]);
  } else {
    gdt[EMPIEZAN_TSS + 8 + proximaTareaB] = (gdt_entry) {
      (unsigned short)    0x0067,         /* limit[0:15]  */
      (unsigned short)    (int)(&tss_jugadorB[jugadorB.piratas[proximaTareaB].index]) & 0xFFFF, /* base[0:15]   */
      (unsigned char)     (int)((int)(&tss_jugadorB[jugadorB.piratas[proximaTareaB].index]) >> 16) & 0x00FF,           /* base[23:16]  */
      (unsigned char)     0x09,           /* type         */
      (unsigned char)     0x00,           /* s            */
      (unsigned char)     0x03,           /* dpl          */
      (unsigned char)     0x01,           /* p            */
      (unsigned char)     0x00,           /* limit[16:19] */
      (unsigned char)     0x00,           /* avl          */
      (unsigned char)     0x00,           /* l            */
      (unsigned char)     0x00,           /* db           */
      (unsigned char)     0x00,           /* g            */
      (unsigned char)     (int)(&tss_jugadorB[jugadorB.piratas[proximaTareaB].id]) >> 24,           /* base[31:24]  */
    };
    completarTssPirata(jugadorB.piratas[proximaTareaB]);
  }
}
