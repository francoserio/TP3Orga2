/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
#include "tss.h"
#include "sched.h"
/* Atributos paginas */
/* -------------------------------------------------------------------------- */

void mmu_mapear_dir_kernel(unsigned int page_dir, unsigned int page_table) {
  page_directory_entry* pd = (page_directory_entry*) page_dir;

  pd[0].present = 1;
  pd[0].read_write = 1;
  pd[0].user_supervisor = 0;
  pd[0].write_through = 0;
  pd[0].cache_disable = 0;
  pd[0].accessed = 0;
  pd[0].ignored = 0;
  pd[0].page_size = 0;
  pd[0].global = 0;
  pd[0].base_address = page_table >> 12;

  page_table_entry* pt = (page_table_entry*) page_table;

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

  tlbflush();
}

void mmu_inicializar_dir_kernel() {
  mmu_mapear_dir_kernel(KERNEL_PAGE_DIR, KERNEL_PAGE_TAB);
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
	return (0x0800000 + ((x + y*MAPA_ANCHO))*PAGE_SIZE);
}

void mmu_inicializar_dir_pirataConocidas(jugador_t* jugador) {
  for (int i = 0; i < 8; i++) {
    //cada pirata del jugador.
    for (int j = 0; j < 80; j++) {
      for (int k = 0; k < 45; k++) {
        if (jugador->posicionesXVistas[j] == 1 && jugador->posicionesYVistas[k] == 1) {
          mmu_mapear_pagina(pos2mapVir(j, k), tss_jugadorA[i].cr3, pos2mapFis(j, k), 0, 1);
        }
      }
    }
  }
}

unsigned int mmu_inicializar_dir_pirata(jugador_t* jugador, pirata_t* tarea) {
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
  if (jugador->index == JUGADOR_A) {
    //empiezo en la primera posicion
    //mapeo donde estamos parados
    mmu_mapear_pagina(pos2mapVir(1,2), page_directory_address, pos2mapFis(1,2), 1, 1);
    //se mapean las de alrededor para jugador 1
    mmu_mapear_pagina(pos2mapVir(2,1), page_directory_address, pos2mapFis(2,1), 0, 1);//derecha
    mmu_mapear_pagina(pos2mapVir(2,2), page_directory_address, pos2mapFis(2,2), 0, 1);//abajo-derecha
    mmu_mapear_pagina(pos2mapVir(1,3), page_directory_address, pos2mapFis(1,3), 0, 1);//abajo
    mmu_mapear_pagina(pos2mapVir(0,1), page_directory_address, pos2mapFis(0,1), 0, 1);//izquierda
    mmu_mapear_pagina(pos2mapVir(0,2), page_directory_address, pos2mapFis(0,2), 0, 1);//abajo-izquierda
    mmu_mapear_pagina(pos2mapVir(1,1), page_directory_address, pos2mapFis(1,1), 0, 1);//arriba
    mmu_mapear_pagina(pos2mapVir(0,3), page_directory_address, pos2mapFis(0,3), 0, 1);//arriba-izquierda
    mmu_mapear_pagina(pos2mapVir(2,3), page_directory_address, pos2mapFis(2,3), 0, 1);//arriba-derecha

  } else {
    //empiezo en la ultima posicion
    //mapeo donde estamos parados
    mmu_mapear_pagina(pos2mapVir(78,43), page_directory_address, pos2mapFis(78,43), 1, 1);
    //al principio solo se mapean las paginas de la izquierda, arriba, arriba-izquierda para jug 2
    mmu_mapear_pagina(pos2mapVir(77,43), page_directory_address, pos2mapFis(77,43), 0, 1);//izquierda
    mmu_mapear_pagina(pos2mapVir(77,42), page_directory_address, pos2mapFis(77,42), 0, 1);//arriba-izquierda
    mmu_mapear_pagina(pos2mapVir(78,42), page_directory_address, pos2mapFis(78,42), 0, 1);//arriba
    mmu_mapear_pagina(pos2mapVir(79,43), page_directory_address, pos2mapFis(79,43), 0, 1);//derecha
    mmu_mapear_pagina(pos2mapVir(79,42), page_directory_address, pos2mapFis(79,42), 0, 1);//arriba-derecha
    mmu_mapear_pagina(pos2mapVir(78,44), page_directory_address, pos2mapFis(78,44), 0, 1);//abajo
    mmu_mapear_pagina(pos2mapVir(79,44), page_directory_address, pos2mapFis(79,44), 0, 1);//abajo-derecha
    mmu_mapear_pagina(pos2mapVir(77,44), page_directory_address, pos2mapFis(77,44), 0, 1);//abajo-izquierda
  }

  tlbflush();
  return page_directory_address;
}

void mmu_mapear_pagina(unsigned int virtual, unsigned int cr3, unsigned int fisica, unsigned char read_write, unsigned char user_supervisor) {
  page_directory_entry* pd = (page_directory_entry*)(cr3);
  unsigned int indiceDirectory = virtual >> 22;
  unsigned int indiceTable = (virtual << 10) >> 22;
  page_directory_entry pde = pd[indiceDirectory];
  if (pde.present == 1) {
    page_table_entry* pt = (page_table_entry*)((pde.base_address << 12));
    page_table_entry pte = pt[indiceTable];
    if (pte.present == 1) {
      pte.user_supervisor = user_supervisor;
      pte.read_write = read_write;
      pte.base_address = (fisica >> 12);
    } else {
      pte.present = 1;
      pte.user_supervisor = user_supervisor;
      pte.read_write = read_write;
      pte.base_address = (fisica >> 12);
    }
  } else {
    unsigned int proxima_pag = mmu_proxima_pagina_fisica_libre();
    page_table_entry* pt = (page_table_entry*)(proxima_pag);

    for (int i = 0; i < 1024; i++) {
			pt[i].present = 0;
		}

    pde.present = 1;
    pde.read_write = read_write;
    pde.user_supervisor = user_supervisor;
    pde.write_through = 0;
    pde.cache_disable = 0;
    pde.accessed = 0;
    pde.ignored = 0;
    pde.page_size = 0;
    pde.global = 0;
    pde.disponible = 0;
    pde.base_address = 0;
    pde.base_address = proxima_pag >> 12;

    pt[indiceTable].present = 1;
    pt[indiceTable].user_supervisor = user_supervisor;
    pt[indiceTable].read_write = read_write;
    pt[indiceTable].write_through = 0;
    pt[indiceTable].cache_disable = 0;
    pt[indiceTable].accessed = 0;
    pt[indiceTable].dirty = 0;
    pt[indiceTable].attribute_index = 0;
    pt[indiceTable].global = 0;
    pt[indiceTable].disponible = 0;
    pt[indiceTable].base_address = (fisica >> 12);
  }

  tlbflush();
}

void mmu_unmapear_pagina(unsigned int virtual, unsigned int cr3) {
  page_directory_entry* pd = (page_directory_entry*)(cr3);
  unsigned int indiceDirectory = virtual >> 22;
  unsigned int indiceTable = (virtual << 10) >> 22;
  page_directory_entry pde = pd[indiceDirectory];
  if (pde.present != 1) {
    page_table_entry* pt = (page_table_entry*)(pde.base_address << 12);
    pt[indiceTable].present = 0;
  }

  tlbflush();
}

void memcpy(unsigned int src, unsigned int dest, unsigned int len, unsigned char rd, unsigned char us) {
  unsigned int cr3 = rcr3();
  breakpoint();
	mmu_mapear_pagina(dest, cr3, dest, rd, us);
  breakpoint();
	char* srcp = (char*)src;
	char* destp = (char*)dest;
  breakpoint();
	for (int i = 0; i < len; ++i) {
		destp[i] = srcp[i];
	}
  breakpoint();
	mmu_unmapear_pagina(dest, cr3);
  breakpoint();
  tlbflush();
}

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
