/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
/* Atributos paginas */
/* -------------------------------------------------------------------------- */
unsigned int proxima_pagina_libre;

page_directory_entry* inicioDirectory = (page_directory_entry*)0x27000;

page_table_entry* inicioTable = (page_table_entry*)0x28000;

void mmu_inicializar_dir_kernel() {
  page_directory_entry pde;

  pde.present = 0;
  pde.read_write = 1;
  pde.user_supervisor = 0;
  pde.write_through = 0;
  pde.cache_disable = 0;
  pde.accessed = 0;
  pde.ignored = 0;
  pde.page_size = 0;
  pde.global = 0;
  pde.base_address = 0x00000;

  for (int j = 0; j < 1024; j++) {
    inicioDirectory[j] = pde;
  }

  inicioDirectory[0].present = 1;
  inicioDirectory[0].base_address = 0x28000 >> 12;

  page_table_entry pte;

  pte.present = 1;
  pte.read_write = 1;
  pte.user_supervisor = 0;
  pte.write_through = 0;
  pte.cache_disable = 0;
  pte.accessed = 0;
  pte.dirty = 0;
  pte.attribute_index = 0;
  pte.global = 0;
  pte.base_address = 0x00000;

  for (int i = 0; i < 1024; i++) {
    pte.base_address = i;
    inicioTable[i] = pte;
  }

  tlbflush();
}

void mmu_inicializar() {
  proxima_pagina_libre = 0x100000;
}

unsigned int mmu_proxima_pagina_fisica_libre() {
  unsigned int pagina_libre = proxima_pagina_libre;
  proxima_pagina_libre += PAGE_SIZE;
  return pagina_libre;
}

void mmu_inicializar_dir_pirata(unsigned int virtual, unsigned int cr3, unsigned char jugador, unsigned int fisica) {
  unsigned int pagina_libre = mmu_proxima_pagina_fisica_libre();
  
  mmu_mapear_pagina(virtual, cr3, fisica);
}

void mmu_mapear_pagina(unsigned int virtual, unsigned int cr3, unsigned int fisica) {
  cr3 = (cr3 >> 12) << 12;
  unsigned int indiceDirectory = virtual >> 22;
  unsigned int indiceTable = (virtual >> 12) << 10;
  page_directory_entry pde = *(cr3 + indiceDirectory);
  if (pde.present == 1) {
    page_table_entry pte = *(pde.base_address + indiceTable);
    if (pte.present == 1) {
      pte.base_address = (fisica >> 12);
    } else {
      pte.present = 1;
      pte.base_address = (fisica >> 12);
    }
  } else {
    pde.present = 1;
    unsigned int proxima_pag = proxima_pagina_libre();
    pde.base_address = proxima_pag << 12;
  }

  tlbflush();
}

void mmu_unmapear_pagina(unsigned int virtual, unsigned int cr3) {
  cr3 = (cr3 >> 12) << 12;
  unsigned int indiceDirectory = virtual >> 22;
  unsigned int indiceTable = (virtual >> 12) << 10;
  page_directory_entry pde = *(cr3 + indiceDirectory);
  page_table_entry pte = *(pde.base_address + indiceTable);
  pte.present = 0;
  int i = 0;
  int estanTodasEnNotPresent = 1;
  while (i < 1024 && estanTodasEnNotPresent == 1) {
    page_table_entry pte = *(pde.base_address + i);
    if (pte.present = 1) {
      estanTodasEnNotPresent = 0;
    }
    i++;
  }
  if (estanTodasEnNotPresent == 1) {
    pte.present = 0;
  }

  tlbflush();
}

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
