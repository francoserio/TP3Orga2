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
    if (jugador->piratas[i].vivoMuerto == 1) {
      for (int j = 0; j < 80; j++) {
        for (int k = 0; k < 45; k++) {
          if (jugador->posicionesXYVistas[j][k] == 1) {
            if (jugador->index == 0) {
              //jugadorA
              if (jugador->piratas[i].posicionX != j && jugador->piratas[i].posicionY != k) {
                mmu_mapear_pagina(pos2mapVir(j, k), tss_jugadorA[i].cr3, pos2mapFis(j, k), 1, 1);
              }
            } else {
              //jugadorB
              if (jugador->piratas[i].posicionX != j && jugador->piratas[i].posicionY != k) {
                mmu_mapear_pagina(pos2mapVir(j, k), tss_jugadorB[i].cr3, pos2mapFis(j, k), 1, 1);
              }
            }
          }
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
  if (jugador->index == 0) {
    //empiezo en la primera posicion
    //mapeo donde estamos parados
    mmu_mapear_pagina(pos2mapVir(1,2), page_directory_address, pos2mapFis(1,2), 1, 1);
    //se mapean las de alrededor para jugador 1
    mmu_mapear_pagina(pos2mapVir(2,1), page_directory_address, pos2mapFis(2,1), 1, 1);//derecha
    mmu_mapear_pagina(pos2mapVir(2,2), page_directory_address, pos2mapFis(2,2), 1, 1);//abajo-derecha
    mmu_mapear_pagina(pos2mapVir(1,3), page_directory_address, pos2mapFis(1,3), 1, 1);//abajo
    mmu_mapear_pagina(pos2mapVir(0,1), page_directory_address, pos2mapFis(0,1), 1, 1);//izquierda
    mmu_mapear_pagina(pos2mapVir(0,2), page_directory_address, pos2mapFis(0,2), 1, 1);//abajo-izquierda
    mmu_mapear_pagina(pos2mapVir(1,1), page_directory_address, pos2mapFis(1,1), 1, 1);//arriba
    mmu_mapear_pagina(pos2mapVir(0,3), page_directory_address, pos2mapFis(0,3), 1, 1);//arriba-izquierda
    mmu_mapear_pagina(pos2mapVir(2,3), page_directory_address, pos2mapFis(2,3), 1, 1);//arriba-derecha
  } else {
    //empiezo en la ultima posicion
    //mapeo donde estamos parados
    // breakpoint();
    mmu_mapear_pagina(pos2mapVir(78,43), page_directory_address, pos2mapFis(78,43), 1, 1);
    // breakpoint();
    //al principio solo se mapean las paginas de la izquierda, arriba, arriba-izquierda para jug 2
    mmu_mapear_pagina(pos2mapVir(77,43), page_directory_address, pos2mapFis(77,43), 1, 1);//izquierda
    mmu_mapear_pagina(pos2mapVir(77,42), page_directory_address, pos2mapFis(77,42), 1, 1);//arriba-izquierda
    mmu_mapear_pagina(pos2mapVir(78,42), page_directory_address, pos2mapFis(78,42), 1, 1);//arriba
    mmu_mapear_pagina(pos2mapVir(79,43), page_directory_address, pos2mapFis(79,43), 1, 1);//derecha
    mmu_mapear_pagina(pos2mapVir(79,42), page_directory_address, pos2mapFis(79,42), 1, 1);//arriba-derecha
    mmu_mapear_pagina(pos2mapVir(78,44), page_directory_address, pos2mapFis(78,44), 1, 1);//abajo
    mmu_mapear_pagina(pos2mapVir(79,44), page_directory_address, pos2mapFis(79,44), 1, 1);//abajo-derecha
    mmu_mapear_pagina(pos2mapVir(77,44), page_directory_address, pos2mapFis(77,44), 1, 1);//abajo-izquierda
    // breakpoint();
  }

  return page_directory_address;
}

void mmu_mapear_pagina(unsigned int virtual, unsigned int cr3, unsigned int fisica, unsigned char read_write, unsigned char user_supervisor) {
  page_directory_entry* pd = (page_directory_entry*)(cr3);
  unsigned int indiceDirectory = virtual >> 22;
  unsigned int indiceTable = (virtual << 10) >> 22;
  // breakpoint();
  if (pd[indiceDirectory].present == 1) {
    page_table_entry* pt = (page_table_entry*)((pd[indiceDirectory].base_address << 12));
    if (pt[indiceTable].present == 1) {
      pt[indiceTable].user_supervisor = user_supervisor;
      pt[indiceTable].read_write = read_write;
      pt[indiceTable].base_address = (fisica >> 12);
    } else {
      pt[indiceTable].present = 1;
      pt[indiceTable].user_supervisor = user_supervisor;
      pt[indiceTable].read_write = read_write;
      pt[indiceTable].base_address = (fisica >> 12);
    }
  } else {
    unsigned int proxima_pag = mmu_proxima_pagina_fisica_libre();
    page_table_entry* pt = (page_table_entry*)(proxima_pag);

    for (int i = 0; i < 1024; i++) {
			pt[i].present = 0;
		}

    pd[indiceDirectory].present = 1;
    pd[indiceDirectory].read_write = read_write;
    pd[indiceDirectory].user_supervisor = user_supervisor;
    pd[indiceDirectory].write_through = 0;
    pd[indiceDirectory].cache_disable = 0;
    pd[indiceDirectory].accessed = 0;
    pd[indiceDirectory].ignored = 0;
    pd[indiceDirectory].page_size = 0;
    pd[indiceDirectory].global = 0;
    pd[indiceDirectory].disponible = 0;
    pd[indiceDirectory].base_address = 0;
    pd[indiceDirectory].base_address = proxima_pag >> 12;

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
  if (pd[indiceDirectory].present == 1) {
    page_table_entry* pt = (page_table_entry*)(pd[indiceDirectory].base_address << 12);
    pt[indiceTable].present = 0;
  }

  tlbflush();
}

void memcpy(unsigned int src, unsigned int dest, unsigned int len, unsigned char rd, unsigned char us) {
  // breakpoint();
  unsigned int cr3 = rcr3();
	mmu_mapear_pagina(dest, cr3, dest, rd, us);
  // breakpoint();
  char* srcp = (char*)src;
	char* destp = (char*)dest;
  // breakpoint();
	for (int i = 0; i < len; ++i) {
		destp[i] = srcp[i];
	}
  // breakpoint();
	mmu_unmapear_pagina(dest, cr3);
}

void memmov(unsigned int src, unsigned int cr3, unsigned int dest, unsigned int len, unsigned char rd, unsigned char us) {
	// lcr3(cr3);
  // breakpoint();
	mmu_mapear_pagina(src, rcr3(), src, rd, us);
	mmu_mapear_pagina(dest, rcr3(), dest, rd, us);
	char* srcp = (char*)src;
	char* destp = (char*)dest;
	unsigned int i;
	for (i = 0; i < len; ++i) {
		destp[i] = srcp[i];
	}
  // breakpoint();
	mmu_unmapear_pagina(src, rcr3());
	mmu_unmapear_pagina(dest, rcr3());
}

void memcpyPila(unsigned int destVir, unsigned int cr3, unsigned char rd, unsigned char us, int value, jugador_t* jug) {
  // breakpoint();
  lcr3(cr3);
  // breakpoint();
  // mmu_mapear_pagina(0x00401000, cr3, 0x00401000, rd, us);
  if (jug->index == 0) {
    *((int*)(tss_jugadorA[proxTareaAMuerta].ebp + destVir)) = (int)value;
  } else {
    *((int*)(tss_jugadorB[proxTareaBMuerta].ebp + destVir)) = (int)value;
  }
  // mmu_unmapear_pagina(0x00401000, cr3);
  // breakpoint();
}

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
