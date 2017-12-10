/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "defines.h"
#include "game.h"

#define CODIGO_BASE       0x00400000

#define MAPA_BASE_FISICA  0x500000
#define MAPA_BASE_VIRTUAL 0x0800000

unsigned int proxima_pagina_libre;

typedef struct str_page_directory_entry {
  unsigned char present:1; //bit 0
  unsigned char read_write:1; //bit 1
  unsigned char user_supervisor:1; //bit 2
  unsigned char write_through:1; //bit 3
  unsigned char cache_disable:1; //bit 4
  unsigned char accessed:1; //bit 5
  unsigned char ignored:1; //bit 6
  unsigned char page_size:1; //bit 7
  unsigned char global:1; //bit 8
  unsigned char disponible:3; //bits 9-11
  unsigned int base_address:20; //bits 12-31
} __attribute__((__packed__, aligned (4))) page_directory_entry;

typedef struct str_page_table_entry {
  unsigned char present:1; //bit 0
  unsigned char read_write:1; //bit 1
  unsigned char user_supervisor:1; //bit 2
  unsigned char write_through:1; //bit 3
  unsigned char cache_disable:1; //bit 4
  unsigned char accessed:1; //bit 5
  unsigned char dirty:1; //bit 6
  unsigned char attribute_index:1; //bit 7
  unsigned char global:1; //bit 8
  unsigned char disponible:3; //bits 9-11
  unsigned int base_address:20; //bits 12-31
} __attribute__((__packed__, aligned (4))) page_table_entry;

void mmu_inicializar();
void mmu_inicializar_dir_kernel();
void mmu_mapear_dir_kernel(unsigned int page_dir, unsigned int pt);
unsigned int mmu_inicializar_dir_pirata(jugador_t* jugador, pirata_t* tarea);
void mmu_mapear_pagina(unsigned int virtual, unsigned int cr3, unsigned int fisica, unsigned char read_write, unsigned char user_supervisor);
void mmu_unmapear_pagina(unsigned int virtual, unsigned int cr3);
unsigned int mmu_proxima_pagina_fisica_libre();
unsigned int pos2mapFis(unsigned int x, unsigned int y);
unsigned int pos2mapVir(unsigned int x, unsigned int y);
void mmu_inicializar_dir_pirataConocidas(jugador_t* jugador);
void memcpy(unsigned int src, unsigned int dest, unsigned int len, unsigned char rd, unsigned char us);
void memmov(unsigned int src, unsigned int cr3, unsigned int dest, unsigned int len, unsigned char rd, unsigned char us);
void memcpyPila(unsigned int destVir, unsigned int cr3, unsigned char rd, unsigned char us, int value, jugador_t* jug);
#endif	/* !__MMU_H__ */
