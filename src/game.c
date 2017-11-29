/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "mmu.h"
#include "tss.h"
#include "screen.h"
#include "sched.h"


#define POS_INIT_A_X                      1
#define POS_INIT_A_Y                      1
#define POS_INIT_B_X         (MAPA_ANCHO - 2)
#define POS_INIT_B_Y          (MAPA_ALTO - 2)

#define CANT_POSICIONES_VISTAS            9
#define MAX_SIN_CAMBIOS                 999


#define BOTINES_CANTIDAD 8

uint botines[BOTINES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, MONEDAS)
                                      {30,  3, 50}, {31, 38, 50}, {15, 21, 100}, {45, 21, 100},
                                      {49,  3, 50}, {48, 38, 50}, {64, 21, 100}, {34, 21, 100}
                                    };

jugador_t jugadorA;
jugador_t jugadorB;

unsigned short segmentos_debug[6];
unsigned int registros_control_debug[4];
unsigned int registros_debug[9];
unsigned int* stack_debug;
unsigned int eflags_debug;

unsigned int mapa_backup[80*25] = {};

char* segmentos[6] = {
  "ss",
  "gs",
  "fs",
  "es",
  "ds",
	"cs"
};

char* registros[9] = {
  "edi",
  "esi",
  "ebp",
  "esp",
  "ebx",
  "edx",
  "ecx",
	"eax",
  "eip"
};

char* registros_control[4] = {
  "cr4",
  "cr3",
  "cr2",
  "cr0"
};

void* error()
{
	__asm__ ("int3");
	return 0;
}

uint game_xy2lineal (uint x, uint y) {
	return (y * MAPA_ANCHO + x);
}

uint game_posicion_valida(int x, int y) {
	return (x >= 0 && y >= 0 && x < MAPA_ANCHO && y < MAPA_ALTO);
}

pirata_t* id_pirata2pirata(uint id_pirata)
{
  for (int i = 0; i < 8; i++) {
    if (id_pirata == jugadorA.piratas[i].id) {
      return &(jugadorA.piratas[i]);
    } else if (id_pirata == jugadorB.piratas[i].id) {
      return &(jugadorB.piratas[i]);
    }
  }
  return NULL;
}

uint game_dir2xy(direccion dir, int *x, int *y)
{
	switch (dir)
	{
		case IZQ: *x = -1; *y =  0; break;
		case DER: *x =  1; *y =  0; break;
		case ABA: *x =  0; *y =  1; break;
		case ARR: *x =  0; *y = -1; break;
    	default: return -1;
	}

	return 0;
}

uint game_valor_tesoro(uint x, uint y)
{
	int i;
	for (i = 0; i < BOTINES_CANTIDAD; i++)
	{
		if (botines[i][0] == x && botines[i][1] == y)
			return botines[i][2];
	}
	return 0;
}

// dada una posicion (x,y) guarda las posiciones de alrededor en dos arreglos, uno para las x y otro para las y
void game_calcular_posiciones_vistas(jugador_t* jugador, int x, int y)
{
  if (y == 0) {
    //si es la primera fila no se agrega la fila 0 porque no existe
    for (int i = -1; i <= 1; i++) {
      for (int j = 0; j <= 1; j++) {
        (jugador->posicionesXYVistas)[x + i][y + j] = 1;
        if (game_valor_tesoro(x + i, y + j) > 0) {
          if (jugador->piratasRestantes == 8) {
            //no puede mandar mas piratas
            (jugador->minerosPendientes)++;
          } else {
            //si puede, lanzar pirata minero
            game_jugador_lanzar_pirata(jugador, minero, x + i, y + j);
          }
        }
      }
    }
  } else if (y == 44) {
    //si es la ultima fila no se agreg la fila 45 porque no existe
    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 0; j++) {
        (jugador->posicionesXYVistas)[x + i][y + j] = 1;
        if (game_valor_tesoro(x + i, y + j) > 0) {
          if (jugador->piratasRestantes == 8) {
            //no puede mandar mas piratas
            (jugador->minerosPendientes)++;
          } else {
            //si puede, lanzar pirata minero
            game_jugador_lanzar_pirata(jugador, minero, x + i, y + j);
          }
        }
      }
    }
  } else if (x == 0) {
    //si es la primera columna no se agreg la columna -1 porque no existe
    for (int i = -1; i <= 1; i++) {
      for (int j = 0; j <= 1; j++) {
        (jugador->posicionesXYVistas)[x + j][y + i] = 1;
        if (game_valor_tesoro(x + i, y + j) > 0) {
          if (jugador->piratasRestantes == 8) {
            //no puede mandar mas piratas
            (jugador->minerosPendientes)++;
          } else {
            //si puede, lanzar pirata minero
            game_jugador_lanzar_pirata(jugador, minero, x + i, y + j);
          }
        }
      }
    }
  } else if (x == 79) {
    //si es la ultima columna no se agrega la columna 80 porque no existe
    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 0; j++) {
        (jugador->posicionesXYVistas)[x + j][y + i] = 1;
        if (game_valor_tesoro(x + i, y + j) > 0) {
          if (jugador->piratasRestantes == 8) {
            //no puede mandar mas piratas
            (jugador->minerosPendientes)++;
          } else {
            //si puede, lanzar pirata minero
            game_jugador_lanzar_pirata(jugador, minero, x + i, y + j);
          }
        }
      }
    }
  } else {
  	int i, j;
    // breakpoint();
    for (i = -1; i <= 1; i++)
    {
      for (j = -1; j <= 1; j++)
      {
        (jugador->posicionesXYVistas)[x + j][y + i] = 1;
        if (game_valor_tesoro(x + i, y + j) > 0) {
          if (jugador->piratasRestantes == 8) {
            //no puede mandar mas piratas
            (jugador->minerosPendientes)++;
          } else {
            if (existeMineroParaEse(x + i, y + j) == 0) {
              //si puede, lanzar pirata minero
              game_jugador_lanzar_pirata(jugador, minero, x + i, y + j);
            }
          }
        }
      }
    }
  }
}

uchar existeMineroParaEse(unsigned int x, unsigned int y) {
  uchar res = 0;
  for (int i = 0; i < 8 && res == 0; i++) {
    if ((jugadorA.piratas[i].posicionXObjetivo == x && jugadorA.piratas[i].posicionYObjetivo == y) ||(jugadorB.piratas[i].posicionXObjetivo == x && jugadorB.piratas[i].posicionYObjetivo == y)) {
      res = 1;
    }
  }
  return res;
}

void game_inicializar()
{
  game_jugador_inicializar(&jugadorA);
  game_jugador_inicializar(&jugadorB);

  for (int i = 0; i < 16; i++) {
    reloj_pirata[i] = 0;
  }

  siempreIgual = 1;
  contador_de_tiempo = 0;
}

void game_modoDebug_open(unsigned int* info) {
  breakpoint();

  for (int i = 0; i < 6; i++) {
    segmentos_debug[i] = info[i];
  }

  for (int i = 0; i < 4; i++) {
    registros_control_debug[i] = info[6 + i];
  }

  int i = 0;
  while (i < 8) {
    registros_debug[i] = info[6 + 4 + i];
    i++;
  }

  // paso uno porque es el error code que no me interesa
  // ahora falta meter el eip

  registros_debug[i] = info[19];

  //ahora cambiamos el cs

  segmentos_debug[5] = info[20];

  //ahora entra el eflags

  eflags_debug = info[21];

  //ahora cambiamos el esp

  registros_debug[3] = info[22];

  stack_debug = (unsigned int*) info[22];

  //por ultimo el ss

  segmentos_debug[0] = info[23];

  // estoy listo para imprimir
  // backapeo la pantalla

  int j;

  unsigned int* video = (unsigned int*) VIDEO;
  for(j = 0; j < 80*25; j++) {
    // print(" ", i + DEBUG_CORNER_X, j + DEBUG_CORNER_Y, C_BG_BLACK);
    mapa_backup[j] = video[j];
  }

  //dibujo bordes del debug
  for(i = 0; i < DEBUG_WIDTH; i++) {
    for(j = 0; j< DEBUG_HEIGHT; j++) {
      print(" ", i + DEBUG_CORNER_X, j + DEBUG_CORNER_Y, C_BG_BLACK);
    }
  }
  breakpoint();

  //pinto interior
  for(i = 1; i < DEBUG_WIDTH - 1; i++) {
    for(j = 2; j< DEBUG_HEIGHT - 1; j++) {
      print(" ", i + DEBUG_CORNER_X , j + DEBUG_CORNER_Y, C_BG_LIGHT_GREY);
    }
  }
  breakpoint();

  // imprimo segmentos
  for(i = 0; i < 6; i++) {
      print(segmentos[i], DEBUG_REGISTROS_X + 20, DEBUG_REGISTROS_Y + i*2, C_FG_BLACK | C_BG_LIGHT_GREY);
      print_hex(segmentos_debug[i], 4, DEBUG_REGISTROS_X + 24, DEBUG_REGISTROS_Y + i*2, C_FG_WHITE | C_BG_LIGHT_GREY);
  }

  breakpoint();

  //imprimo registros comunes
  for(i = 0; i < 9; i++) {
      print(registros[i], DEBUG_REGISTROS_X, DEBUG_REGISTROS_Y + i*2, C_FG_BLACK | C_BG_LIGHT_GREY);
      print_hex(registros_debug[i], 8, DEBUG_REGISTROS_X + 4, DEBUG_REGISTROS_Y + i*2, C_FG_WHITE | C_BG_LIGHT_GREY);
  }

  breakpoint();

  //imprimo registros de control
  for (i = 0; i < 4; i++) {
    print(registros_control[i], DEBUG_REGISTROS_X, DEBUG_REGISTROS_Y + 18 + i*2, C_FG_BLACK | C_BG_LIGHT_GREY);
    print_hex(registros_control_debug[i], 8, DEBUG_REGISTROS_X + 4, DEBUG_REGISTROS_Y + 18 + i*2, C_FG_WHITE | C_BG_LIGHT_GREY);
  }

  breakpoint();

  //imprimo stack
  print("Stack: ", DEBUG_REGISTROS_X + 20, DEBUG_REGISTROS_Y + 12, C_FG_BLACK | C_BG_LIGHT_GREY);
  for(i = 0; i < 4; i++) {
      print("[        ]", DEBUG_REGISTROS_X + 29, i + DEBUG_REGISTROS_Y + 14, C_FG_BLACK | C_BG_LIGHT_GREY);
      print_hex((unsigned int)stack_debug, 8, DEBUG_REGISTROS_X + 20, i + DEBUG_REGISTROS_Y + 14, C_FG_WHITE | C_BG_LIGHT_GREY);
      print_hex(*stack_debug, 8, DEBUG_REGISTROS_X + 30, i + DEBUG_REGISTROS_Y + 14, C_FG_WHITE | C_BG_LIGHT_GREY);
      stack_debug++;
  }

  breakpoint();
  //imprimo eflags
  print("eflags", DEBUG_REGISTROS_X + 20, DEBUG_REGISTROS_Y + 20, C_FG_BLACK | C_BG_LIGHT_GREY);
  print_hex(eflags_debug, 8, DEBUG_REGISTROS_X + 27, DEBUG_REGISTROS_Y +20, C_FG_WHITE | C_BG_LIGHT_GREY);

  breakpoint();

  modoDebugPantalla = 1;

  breakpoint();
}

void game_modoDebug_close() {
	//vuelve el mapa a como estaba antes
  breakpoint();
  unsigned int* video = (unsigned int*) VIDEO;
  breakpoint();
  for(int j = 0; j < 80*25; j++) {
    // print(" ", i + DEBUG_CORNER_X, j + DEBUG_CORNER_Y, C_BG_BLACK);
    video[j] = mapa_backup[j];
  }
}

void game_jugador_inicializar(jugador_t *j)
{
  static int indexJug = 0;
  uint idPir;
  if (indexJug == 0) {
    idPir = 0;
  } else {
    idPir = 8;
  }

	j->index = indexJug++;
  j->puntaje = 0;
  j->piratasRestantes = 0;
  j->minerosPendientes = 0;
  for (int k = 0; k < 80; k++) {
    for (int p = 0; p < 45; p++) {
      j->posicionesXYVistas[k][p] = 0;
    }
  }

  j->piratasRestantes = 8;
  if (j->index == 0) {
    j->puertoX = 1;
    j->puertoY = 2;
    j->colorJug = C_BG_GREEN;
  } else {
    j->puertoY = 43;
    j->puertoX = 78;
    j->colorJug = C_BG_BLUE;
  }

  for (int i = 0; i < 8; i++) {
    // print_dec(idPir, 3, 25, 25, 0x0F);
    if (j->index == 0) {
      game_pirata_inicializar(&(j->piratas[i]), j, i, idPir);
      // j->piratas[i] = piratasA[i];
    } else {
      game_pirata_inicializar(&(j->piratas[i]), j, i, idPir);
      // j->piratas[i] = piratasB[i];
    }

    idPir++;
  }
}

void game_pirata_inicializar(pirata_t *pirata, jugador_t *j, uint index, uint id)
{
  pirata->index = index;
  pirata->id = id;
  pirata->tipo = explorador;
  pirata->jugador = j;
  if (j->index == JUGADOR_B) {
    pirata->reloj = reloj_pirata[8 + index];
  } else {
    pirata->reloj = reloj_pirata[index];
  }
  pirata->vivoMuerto = 0;
  pirata->posicionYObjetivo = -1;
  pirata->posicionXObjetivo = -1;
}

void game_tick(uint id_pirata)
{
  screen_actualizar_reloj_global();
}

void game_jugador_lanzar_pirata(jugador_t *j, uint tipo, uint x, uint y)
{
  // print_hex((unsigned int)(mapa_backup), 11, 15, 15, 0x0F);
  j->piratasRestantes--;
  if (j->index == 0) {
    //turno de jugador A. Tengo que explorar las paginas de alrededor
    if (tipo != minero) {
        game_explorar_posicion(j, 1, 2);
    }
    pirata_t* tareaPirata = &(jugadorA.piratas[proxTareaAMuerta]);
    tareaPirata->tipo = tipo;
    tareaPirata->vivoMuerto = 1;
    tareaPirata->posicionX = 1;
    tareaPirata->posicionY = 2;
    if (tipo == minero) {
      tareaPirata->posicionXObjetivo = x;
      tareaPirata->posicionYObjetivo = y;
    }
    tss_agregar_piratas_a_gdt(j, proxTareaAMuerta);
    // print_hex(proxTareaAMuerta, 20, 13, 13, 0x0F);
    mmu_mapear_pagina(0x00400000, tss_jugadorA[proxTareaAMuerta].cr3, pos2mapFis(1, 2), 1, 1);
    if (tareaPirata->tipo == explorador) {
      //explorador
      memcpy((unsigned int)0x10000, pos2mapFis(1, 2), PAGE_SIZE, 1, 1);
    } else {
      //minero
      memcpy((unsigned int)0x11000, pos2mapFis(1, 2), PAGE_SIZE, 1, 1);
      memcpyPila(8, tss_jugadorA[proxTareaAMuerta].cr3, 1, 1, tareaPirata->posicionYObjetivo);
      memcpyPila(4, tss_jugadorA[proxTareaAMuerta].cr3, 1, 1, tareaPirata->posicionXObjetivo);
    }
    screen_pintar_piratas();
    sched_agregar(&jugadorA);
    // breakpoint();
  } else {
    //turno de jugador B. Tengo que explorar las paginas de alrededor
    if (tipo != minero) {
        game_explorar_posicion(j, 78, 43);
    }
    pirata_t* tareaPirata = &(jugadorB.piratas[proxTareaBMuerta]);
    tareaPirata->tipo = tipo;
    tareaPirata->vivoMuerto = 1;
    tareaPirata->posicionX = 78;
    tareaPirata->posicionY = 43;
    if (tipo == minero) {
      tareaPirata->posicionXObjetivo = x;
      tareaPirata->posicionYObjetivo = y;
    }
    tss_agregar_piratas_a_gdt(j, proxTareaBMuerta);
    mmu_mapear_pagina(0x00400000, tss_jugadorB[proxTareaBMuerta].cr3, pos2mapFis(78,43), 1, 1);
    if (tareaPirata->tipo == explorador) {
      //explorador
      memcpy(0x12000, pos2mapFis(78,43), PAGE_SIZE, 1, 1);
    } else {
      //minero
      memcpy(0x13000, pos2mapFis(78,43), PAGE_SIZE, 1, 1);
      memcpyPila(8, tss_jugadorB[proxTareaBMuerta].cr3, 1, 1, tareaPirata->posicionYObjetivo);
      memcpyPila(4, tss_jugadorB[proxTareaBMuerta].cr3, 1, 1, tareaPirata->posicionXObjetivo);
    }
    screen_pintar_piratas();
    sched_agregar(&jugadorB);
  }
}

void game_explorar_posicion(jugador_t *jugador, int c, int f)
{
  game_calcular_posiciones_vistas(jugador, c, f);
}

uint game_posicion_ya_vista(pirata_t* tareaPir, direccion dir) {
  int x = 0;
  int y = 0;
  game_dir2xy(dir, &x, &y);
  // print_dec(x, 3, 15, 15, 0x0F);
  // print_dec(y, 3, 14, 14, 0x0F);
  if ((tareaPir->jugador)->posicionesXYVistas[tareaPir->posicionX + x][tareaPir->posicionY + y] == 1) {
    return 1;
  } else {
    return 0;
  }
}

uint game_syscall_pirata_mover(uint id, direccion dir)
{
  pirata_t* tareaPirata = id_pirata2pirata(id);
  // print_dec(id, 3, 25, 25, 0x0F);
  // breakpoint();
  // print_dec(id, 3, 25, 25, 0x0F);
  if (tareaPirata->tipo == minero) {
    //MINERO. tengo que checkear que sea valida
    int x = 0;
    int y = 0;
    game_dir2xy(dir, &x, &y);
    if (game_posicion_valida(tareaPirata->posicionX + x, tareaPirata->posicionY + y) == 1) {
      //tengo que checkear que la posicion ya esté mapeada
      // print_dec(tareaPirata->posicionX, 3, 20, 20, 0x0F);
      // print_dec(tareaPirata->posicionY, 3, 21, 21, 0x0F);
      if (game_posicion_ya_vista(tareaPirata, dir) == 0) {
        //si no está mapeada, lo mato
        game_pirata_exploto(id);
        // screen_borrar_pirata(tareaPirata->jugador, tareaPirata);
      } else {
        //puedo seguir
        tareaPirata->posicionX = tareaPirata->posicionX + x;
        tareaPirata->posicionY = tareaPirata->posicionY + y;
        if ((tareaPirata->jugador)->index == 0) {
          mmu_unmapear_pagina(0x00400000, tss_jugadorA[tareaActualA].cr3);
          memmov(pos2mapFis(tareaPirata->posicionX - x, tareaPirata->posicionY - y), tss_jugadorA[tareaActualA].cr3, pos2mapFis(tareaPirata->posicionX, tareaPirata->posicionY), PAGE_SIZE, 1, 1);
          mmu_mapear_pagina(0x00400000, tss_jugadorA[tareaActualA].cr3, pos2mapFis(tareaPirata->posicionX, tareaPirata->posicionY), 1, 1);
          mmu_mapear_pagina(pos2mapVir(tareaPirata->posicionX, tareaPirata->posicionY), tss_jugadorA[tareaActualA].cr3, pos2mapFis(tareaPirata->posicionX, tareaPirata->posicionY), 1, 1);
        } else {
          mmu_unmapear_pagina(0x00400000, tss_jugadorB[tareaActualB].cr3);
          memmov(pos2mapFis(tareaPirata->posicionX - x, tareaPirata->posicionY - y), tss_jugadorB[tareaActualB].cr3, pos2mapFis(tareaPirata->posicionX, tareaPirata->posicionY), PAGE_SIZE, 1, 1);
          mmu_mapear_pagina(0x00400000, tss_jugadorB[tareaActualB].cr3, pos2mapFis(tareaPirata->posicionX, tareaPirata->posicionY), 1, 1);
          mmu_mapear_pagina(pos2mapVir(tareaPirata->posicionX, tareaPirata->posicionY), tss_jugadorB[tareaActualB].cr3, pos2mapFis(tareaPirata->posicionX, tareaPirata->posicionY), 1, 1);
        }
        screen_pintar_piratas();
      }
    }
  } else {
    //EXPLORADOR. tengo que checkear que sea valida
    int x = 0;
    int y = 0;
    game_dir2xy(dir, &x, &y);
    if (game_posicion_valida(tareaPirata->posicionX + x, tareaPirata->posicionY + y)) {
      //va a pasar a una posicion valida en el juego
      tareaPirata->posicionX = tareaPirata->posicionX + x;
      tareaPirata->posicionY = tareaPirata->posicionY + y;
      mmu_inicializar_dir_pirataConocidas(tareaPirata->jugador);
      if ((tareaPirata->jugador)->index == 0) {
        mmu_unmapear_pagina(0x00400000, tss_jugadorA[tareaActualA].cr3);
        memmov(pos2mapFis(tareaPirata->posicionX - x, tareaPirata->posicionY - y), tss_jugadorA[tareaActualA].cr3, pos2mapFis(tareaPirata->posicionX, tareaPirata->posicionY), PAGE_SIZE, 1, 1);
        mmu_mapear_pagina(0x00400000, tss_jugadorA[tareaActualA].cr3, pos2mapFis(tareaPirata->posicionX, tareaPirata->posicionY), 1, 1);
        mmu_mapear_pagina(pos2mapVir(tareaPirata->posicionX, tareaPirata->posicionY), tss_jugadorA[tareaActualA].cr3, pos2mapFis(tareaPirata->posicionX, tareaPirata->posicionY), 1, 1);
      } else {
        mmu_unmapear_pagina(0x00400000, tss_jugadorB[tareaActualB].cr3);
        memmov(pos2mapFis(tareaPirata->posicionX - x, tareaPirata->posicionY - y), tss_jugadorB[tareaActualB].cr3, pos2mapFis(tareaPirata->posicionX, tareaPirata->posicionY), PAGE_SIZE, 1, 1);
        mmu_mapear_pagina(0x00400000, tss_jugadorB[tareaActualB].cr3, pos2mapFis(tareaPirata->posicionX, tareaPirata->posicionY), 1, 1);
        mmu_mapear_pagina(pos2mapVir(tareaPirata->posicionX, tareaPirata->posicionY), tss_jugadorB[tareaActualB].cr3, pos2mapFis(tareaPirata->posicionX, tareaPirata->posicionY), 1, 1);
      }
      game_explorar_posicion(tareaPirata->jugador, tareaPirata->posicionX, tareaPirata->posicionY);
      screen_pintar_piratas();
    } else {
      //si no va a ir a una posicion valida lo mato
      game_pirata_exploto(id);
      // screen_borrar_pirata(tareaPirata->jugador, tareaPirata);
    }
  }
  return 0;
}

uint game_syscall_cavar(uint id)
{
  pirata_t* tareaPirata = id_pirata2pirata(id);
  if (game_valor_tesoro(tareaPirata->posicionX, tareaPirata->posicionY) == 0) {
    //lo mato
    game_pirata_exploto(id);
    screen_borrar_pirata(tareaPirata->jugador, tareaPirata);
  } else {
    for (int i = 0; i < BOTINES_CANTIDAD; i++) {
      if (botines[i][0] == tareaPirata->posicionX && botines[i][1] == tareaPirata->posicionY) {
        botines[i][2] = botines[i][2] - 1;
      }
    }
    game_jugador_anotar_punto(tareaPirata->jugador);
  }

	return 0;
}

uint game_syscall_pirata_posicion(uint id, int idx)
{
  // print_dec(idx, int size, uint x, uint y, unsigned short attr)
  if (idx == -1) {
    //tengo que dar la posicion del propio pirata.
    pirata_t* tareaPirata = id_pirata2pirata(id);
    return (((tareaPirata->posicionY) << 8) | (tareaPirata->posicionX));
  } else {
    //sino es un indice del 0-7 para indicar el indice de pirata del propio jugador.
    if (turnoPirataActual == 0) {
      //turno jugador A
      pirata_t tareaPirata = jugadorA.piratas[idx];
      return ((tareaPirata.posicionY << 8) | (tareaPirata.posicionX));
    } else {
      //turno jugador B
      pirata_t tareaPirata = jugadorB.piratas[idx];
      return ((tareaPirata.posicionY << 8) | (tareaPirata.posicionX));
    }
  }
  return 0;
}

uint game_syscall_manejar(uint syscall, uint param1)
{
  // print_dec(syscall, 5, 35, 20, C_FG_WHITE);
  if (syscall == 0x4) { //debug
    print_dec(param1, 4, 7, 7, 0x0F);
  }
  if (turnoPirataActual == 0) {
    //turno jugador A
    pirata_t pirataA = jugadorA.piratas[tareaActualA];
    if (syscall == 0x1) {
      return game_syscall_pirata_mover(pirataA.id, param1);
    } else if (syscall == 0x2) {
      return game_syscall_cavar(pirataA.id);
    } else if (syscall == 0x3) {
      return game_syscall_pirata_posicion(pirataA.id, param1);
    }
  } else {
    //turno jugador B
    pirata_t pirataB = jugadorB.piratas[tareaActualB];
    if (syscall == 0x1) {
      return game_syscall_pirata_mover(pirataB.id, param1);
    } else if (syscall == 0x2) {
      return game_syscall_cavar(pirataB.id);
    } else if (syscall == 0x3) {
      return game_syscall_pirata_posicion(pirataB.id, param1);
    }
  }

  return 0;
}

void game_pirata_exploto(uint id)
{
  sched_intercambiar_por_idle();
  pirata_t* tareaPirata = id_pirata2pirata(id);
  ((tareaPirata->jugador)->piratasRestantes)++;
  tareaPirata->vivoMuerto = 0;
  tareaPirata->reloj = 0;
  screen_pintar(' ', (tareaPirata->jugador)->colorJug, tareaPirata->posicionY, tareaPirata->posicionX);
  sched_sacar(tareaPirata->jugador, tareaPirata->index);
}

void game_pirata_explotoisr()
{
  sched_intercambiar_por_idle();
  if (turnoPirataActual == 0) {
    //jugador A
    pirata_t* tareaPirata = id_pirata2pirata(tareaActualA);
    ((tareaPirata->jugador)->piratasRestantes)++;
    tareaPirata->vivoMuerto = 0;
    tareaPirata->reloj = 0;
    screen_pintar(' ', (tareaPirata->jugador)->colorJug, tareaPirata->posicionY, tareaPirata->posicionX);
    sched_sacar(tareaPirata->jugador, tareaPirata->index);
  } else {
    //jugador B
    pirata_t* tareaPirata = id_pirata2pirata(8 + tareaActualB);
    ((tareaPirata->jugador)->piratasRestantes)++;
    tareaPirata->vivoMuerto = 0;
    tareaPirata->reloj = 0;
    screen_pintar(' ', (tareaPirata->jugador)->colorJug, tareaPirata->posicionY, tareaPirata->posicionX);
    sched_sacar(tareaPirata->jugador, tareaPirata->index);
  }
}

pirata_t* game_pirata_en_posicion(uint x, uint y)
{
	for (int i = 0; i < 8; i++) {
    if ((jugadorA.piratas[i]).posicionX == x && (jugadorA.piratas[i]).posicionY == y) {
      return &(jugadorA.piratas[i]);
    } else if (jugadorB.piratas[i].posicionX == x && jugadorB.piratas[i].posicionY == y) {
      return &(jugadorB.piratas[i]);
    }
  }

  return NULL;
}

uint game_valores_tesoros() {
  uint sum = 0;
  for (int i = 0; i < BOTINES_CANTIDAD; i++) {
    sum = sum + botines[i][2];
  }
  return sum;
}

void game_jugador_anotar_punto(jugador_t *j)
{
  j->puntaje = j->puntaje + 1;
  if (game_valores_tesoros() == 0) {
    game_terminar_si_es_hora();
  } else if (contador_de_tiempo == MAX_SIN_CAMBIOS && siempreIgual == 1) {
    game_terminar_si_es_hora();
  }
  screen_actualizar_puntajes();

  siempreIgual = 0;
  contador_de_tiempo = 0;
}

void game_terminar_si_es_hora()
{
  // breakpoint();
  if (jugadorA.puntaje > jugadorB.puntaje) {
    screen_stop_game_show_winner(&jugadorA);
  } else {
    screen_stop_game_show_winner(&jugadorB);
  }
  hlt();
}


#define KB_w        0x11 // 0x91
#define KB_s        0x1f // 0x9f
#define KB_a        0x1e // 0x9e
#define KB_d        0x20 // 0xa0
#define KB_e        0x12 // 0x92
#define KB_q        0x10 // 0x90
#define KB_i        0x17 // 0x97
#define KB_k        0x25 // 0xa5
#define KB_j        0x24 // 0xa4
#define KB_l        0x26 // 0xa6
#define KB_shiftL   0x2a // 0xaa
#define KB_shiftR   0x36 // 0xb6
#define KB_y        0x15


void game_atender_teclado(unsigned char tecla)
{
  switch(tecla) {
    case KB_w:
      print("      ", 65, 0, 0x0);
      screen_pintar('W', 0xF, 0, 65);
      break;
    case KB_s:
      print("      ", 65, 0, 0x0);
      screen_pintar('S', 0xF, 0, 65);
      break;
    case KB_a:
      print("      ", 65, 0, 0x0);
      screen_pintar('A', 0xF, 0, 65);
      break;
    case KB_d:
      print("      ", 65, 0, 0x0);
      screen_pintar('D', 0xF, 0, 65);
      break;
    case KB_e:
      print("      ", 65, 0, 0x0);
      screen_pintar('E', 0xF, 0, 65);
      break;
    case KB_q:
      print("      ", 65, 0, 0x0);
      screen_pintar('Q', 0xF, 0, 65);
      break;
    case KB_i:
      print("      ", 65, 0, 0x0);
      screen_pintar('I', 0xF, 0, 65);
      break;
    case KB_k:
      print("      ", 65, 0, 0x0);
      screen_pintar('K', 0xF, 0, 65);
      break;
    case KB_j:
      print("      ", 65, 0, 0x0);
      screen_pintar('J', 0xF, 0, 65);
      break;
    case KB_l:
      print("      ", 65, 0, 0x0);
      screen_pintar('L', 0xF, 0, 65);
      break;
    case KB_shiftL:
      print("shiftL", 65, 0, 0xF);
      game_jugador_lanzar_pirata(&jugadorA, explorador, 1, 2);
      break;
    case KB_shiftR:
      print("shiftR", 65, 0, 0xF);
      game_jugador_lanzar_pirata(&jugadorB, explorador, 78, 43);
      break;
    case KB_y:
      print("      ", 65, 0, 0x0);
      screen_pintar('Y', 0xF, 0, 65);
      sched_toggle_debug();
      break;
  }
}
