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

// unsigned int proxima_pagina_libre;
//
// void mmu_inicializar(){
// 	proxima_pagina_libre = INICIO_PAGINAS_LIBRES;
// }
//
// unsigned int mmu_proxima_pagina_fisica_libre() {
// 	unsigned int pagina_libre  = proxima_pagina_libre;
// 	proxima_pagina_libre += PAGE_SIZE; // PAGE_SIZE = 1024
// 	return pagina_libre;
// }

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
