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

unsigned int pos2mapFis(unsigned int x, unsigned int y) {
	return (0x500000 + ((x + y*MAPA_ANCHO))*PAGE_SIZE);
}

unsigned int pos2mapVir(unsigned int x, unsigned int y) {
	return (0x400000 + ((x + y*MAPA_ANCHO))*PAGE_SIZE);
}

// unsigned int mapear_dir_pirata(pirata_t tarea) {
//
// }

unsigned int mmu_inicializar_dir_pirata(unsigned char jugador, unsigned char tarea) {
  //aca ya tengo la page directory y table de la tarea.

  page_directory_entry* pd = (page_directory_entry*) mmu_proxima_pagina_fisica_libre();
  page_table_entry* pt = (page_table_entry*) mmu_proxima_pagina_fisica_libre();

  pd[0].present = 1;
  pd[0].read_write = 1;
  pd[0].user_supervisor = 0;
  pd[0].write_through = 0;
  pd[0].cache_disable = 0;
  pd[0].accessed = 0;
  pd[0].ignored = 0;
  pd[0].page_size = 0;
  pd[0].global = 0;
  unsigned int page_table_address = (unsigned int)pt;
  pd[0].base_address = page_table_address >> 12;

  for (int i = 0; i < 1024; i++) {
    pt[i].present = 1;
    pt[i].read_write = 1;
    pt[i].user_supervisor = 0;
    pt[i].write_through = 0;
    pt[i].cache_disable = 0;
    pt[i].accessed = 0;
    pt[i].dirty = 0;
    pt[i].attribute_index = 0;
    pt[i].global = 0;
    pt[i].base_address = i;
  }

  unsigned int page_directory_address = (unsigned int)pd;
  if (jugador == JUGADOR_A) {
    //empiezo en la primera posicion
    //primero copio la tarea
    if (tarea == 0) {
      //explorador
      for (int i = 0; i < 1024; i++) {
        ((unsigned int*)(pos2mapFis(1,1)))[i] = ((unsigned int*)((unsigned int)0x10000))[i];
      }
    } else {
      //minero
      for (int i = 0; i < 1024; i++) {
        ((unsigned int*)(pos2mapFis(1,1)))[i] = ((unsigned int*)((unsigned int)0x11000))[i];
      }
    }
    //mapeo donde estamos parados
    mmu_mapear_pagina(pos2mapVir(1,1), (page_directory_address), pos2mapFis(1,1), 1, 1);
    //se mapean las de alrededor para jugador 1
    mmu_mapear_pagina(pos2mapVir(2,1), (page_directory_address), pos2mapFis(2,1), 0, 1);//derecha
    mmu_mapear_pagina(pos2mapVir(2,2), (page_directory_address), pos2mapFis(2,2), 0, 1);//abajo-derecha
    mmu_mapear_pagina(pos2mapVir(1,2), (page_directory_address), pos2mapFis(1,2), 0, 1);//abajo
    mmu_mapear_pagina(pos2mapVir(0,1), (page_directory_address), pos2mapFis(0,1), 0, 1);//izquierda
    mmu_mapear_pagina(pos2mapVir(0,2), (page_directory_address), pos2mapFis(0,2), 0, 1);//abajo-izquierda
    mmu_mapear_pagina(pos2mapVir(1,0), (page_directory_address), pos2mapFis(1,0), 0, 1);//arriba
    mmu_mapear_pagina(pos2mapVir(0,0), (page_directory_address), pos2mapFis(0,0), 0, 1);//arriba-izquierda
    mmu_mapear_pagina(pos2mapVir(2,0), (page_directory_address), pos2mapFis(2,0), 0, 1);//arriba-derecha
  } else {
    //empiezo en la ultima posicion
    //primero copio la tarea
    if (tarea == 0) {
      //explorador
      for (int i = 0; i < 1024; i++) {
        ((unsigned int*)(pos2mapFis(78,43)))[i] = ((unsigned int*)((unsigned int)0x12000))[i];
      }
    } else {
      //minero
      for (int i = 0; i < 1024; i++) {
        ((unsigned int*)(pos2mapFis(78,43)))[i] = ((unsigned int*)((unsigned int)0x13000))[i];
      }
    }
    //mapeo donde estamos parados
    mmu_mapear_pagina(pos2mapVir(78,43), (page_directory_address), pos2mapFis(78,43), 1, 1);
    //al principio solo se mapean las paginas de la izquierda, arriba, arriba-izquierda para jug 2
    mmu_mapear_pagina(pos2mapVir(77,43), (page_directory_address), pos2mapFis(77,43), 0, 1);//izquierda
    mmu_mapear_pagina(pos2mapVir(77,42), (page_directory_address), pos2mapFis(77,42), 0, 1);//arriba-izquierda
    mmu_mapear_pagina(pos2mapVir(78,42), (page_directory_address), pos2mapFis(78,42), 0, 1);//arriba
    mmu_mapear_pagina(pos2mapVir(79,43), (page_directory_address), pos2mapFis(79,43), 0, 1);//derecha
    mmu_mapear_pagina(pos2mapVir(79,42), (page_directory_address), pos2mapFis(79,42), 0, 1);//arriba-derecha
    mmu_mapear_pagina(pos2mapVir(78,44), (page_directory_address), pos2mapFis(78,44), 0, 1);//abajo
    mmu_mapear_pagina(pos2mapVir(79,44), (page_directory_address), pos2mapFis(79,44), 0, 1);//abajo-derecha
    mmu_mapear_pagina(pos2mapVir(77,44), (page_directory_address), pos2mapFis(77,44), 0, 1);//abajo-izquierda
  }

  return page_directory_address;
}

void mmu_mapear_pagina(unsigned int virtual, unsigned int cr3, unsigned int fisica, unsigned char read_write, unsigned char user_supervisor) {
  page_directory_entry* pd = (page_directory_entry*)(cr3);
  unsigned int indiceDirectory = virtual >> 22;
  unsigned int indiceTable = (virtual >> 12) << 10;
  page_directory_entry pde = pd[indiceDirectory];
  if (pde.present == 1) {
    page_table_entry* pt = (page_table_entry*)((pde.base_address << 12) >> 12);
    page_table_entry pte = pt[indiceTable];
    if (pte.present == 1) {
      pte.base_address = (fisica >> 12);
      pte.user_supervisor = user_supervisor;
      pte.read_write = read_write;
    } else {
      pte.present = 1;
      pte.user_supervisor = user_supervisor;
      pte.read_write = read_write;
      pte.base_address = (fisica >> 12);
    }
  } else {
    unsigned int proxima_pag = mmu_proxima_pagina_fisica_libre();
    pde.present = 1;
    pde.read_write = read_write;
    pde.user_supervisor = user_supervisor;
    pde.base_address = proxima_pag >> 12;
    page_table_entry* pt = (page_table_entry*)(proxima_pag << 12);
    pt[indiceTable].present = 1;
    pt[indiceTable].user_supervisor = user_supervisor;
    pt[indiceTable].read_write = read_write;
    pt[indiceTable].base_address = (fisica >> 12);
  }

  tlbflush();
}

void mmu_unmapear_pagina(unsigned int virtual, unsigned int cr3) {
  page_directory_entry* pd = (page_directory_entry*)(cr3);
  unsigned int indiceDirectory = virtual >> 22;
  unsigned int indiceTable = (virtual >> 12) << 10;
  page_directory_entry pde = pd[indiceDirectory];
  page_table_entry* pt = (page_table_entry*)(pde.base_address << 12);
  pt[indiceTable].present = 0;
  int i = 0;
  int estanTodasEnNotPresent = 1;
  while (i < 1024 && estanTodasEnNotPresent == 1) {
    page_table_entry* pt = (page_table_entry*)(pde.base_address << 12);
    page_table_entry pte = pt[i];
    if (pte.present == 1) {
      estanTodasEnNotPresent = 0;
    }
    i++;
  }
  if (estanTodasEnNotPresent == 1) {
    pde.present = 0;
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
