/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "mmu.h"
#include "tss.h"
#include "screen.h"

#include <stdarg.h>


#define POS_INIT_A_X                      1
#define POS_INIT_A_Y                      1
#define POS_INIT_B_X         MAPA_ANCHO - 2
#define POS_INIT_B_Y          MAPA_ALTO - 2

#define CANT_POSICIONES_VISTAS            9
#define MAX_SIN_CAMBIOS                 999

#define BOTINES_CANTIDAD 8

#define MAX_TIEMPO 10000000000000000000000000000000

uint botines[BOTINES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, MONEDAS)
                                        {30,  3, 50}, {31, 38, 50}, {15, 21, 100}, {45, 21, 100} ,
                                        {49,  3, 50}, {48, 38, 50}, {64, 21, 100}, {34, 21, 100}
                                    };

jugador_t jugadorA;
jugador_t jugadorB;

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
void game_calcular_posiciones_vistas(int *vistas_x, int *vistas_y, int x, int y)
{
	int next = 0;
	int i, j;
	for (i = -1; i <= 1; i++)
	{
		for (j = -1; j <= 1; j++)
		{
			vistas_x[next] = x + j;
			vistas_y[next] = y + i;
			next++;
		}
	}
}


void game_inicializar()
{
  // for (int i = 0; i < 16; i++) {
  //   reloj_pirata[i] = 0;
  // }
}

void game_jugador_inicializar_mapa(jugador_t *jug)
{

}

void game_jugador_inicializar(jugador_t *j)
{
  static int indexJug = 0;
  static int idPir = 0;

	j->index = indexJug++;
  j->puntaje = 0;
  j->piratasRestantes = 0;
  for (int k = 0; k < 44; k++) {
    j->posicionesYVistas[k] = 0;
  }
  for (int p = 0; p < 80; p++) {
    j->posicionesXVistas[p] = 0;
  }
  j->piratasRestantes = 8;
  if (j->index == 0) {
    j->puertoX = 1;
    j->puertoY = 1;
    j->colorJug = C_BG_GREEN;
  } else {
    j->puertoY = 43;
    j->puertoX = 78;
    j->colorJug = C_BG_BLUE;
  }
  pirata_t* pirata = NULL;
  for (int i = 0; i < 8; i++) {
    game_pirata_inicializar(pirata, j, i, idPir++);
    j->piratas[i] = *pirata;
  }
}

void game_pirata_inicializar(pirata_t *pirata, jugador_t *j, uint index, uint id)
{
  pirata->index = index;
  pirata->id = id;
  pirata->tipo = explorador;
  if (j->index == JUGADOR_A) {
    pirata->posicionX = 1;
    pirata->posicionY = 1;
  } else {
    pirata->posicionX = 78;
    pirata->posicionY = 43;
  }
  pirata->jugador = j;
  if (j->index == JUGADOR_B) {
    pirata->reloj = reloj_pirata[8 + index];
  } else {
    pirata->reloj = reloj_pirata[index];
  }
  pirata->vivoMuerto = 0;
}

void game_tick(uint id_pirata)
{
  screen_actualizar_reloj_global();
  if (id_pirata2pirata(id_pirata)->vivoMuerto) {
    reloj_pirata[id_pirata2pirata(id_pirata)->id]++;
  }
}


void game_pirata_relanzar(pirata_t *pirata, jugador_t *j, uint tipo)
{
}

pirata_t* game_jugador_erigir_pirata(jugador_t *j, uint tipo)
{
    // ~ completar ~

	return NULL;
}


void game_jugador_lanzar_pirata(jugador_t *j, uint tipo, int x, int y)
{
<<<<<<< HEAD

=======
  j->piratasRestantes--;
  if (j->index == 0) {
    //turno proximo de jugador A. Tengo que explorar las paginas de alrededor
    if (tipo != minero) {
        game_explorar_posicion(j, 1, 2);
    }
    pirata_t tareaPirata = jugadorA.piratas[proximaTareaA];
    tareaPirata.tipo = tipo;
    if (tipo == minero) {
      tareaPirata.posicionXObjetivo = x;
      tareaPirata.posicionYObjetivo = y;
    }
    tss_agregar_piratas_a_gdt(j);
    screen_pintar_pirata(j, &tareaPirata);
  } else {
    //turno proximo de jugador B. Tengo que explorar las paginas de alrededor
    if (tipo != minero) {
        game_explorar_posicion(j, 78, 43);
    }
    pirata_t tareaPirata = jugadorB.piratas[proximaTareaB];
    tareaPirata.tipo = tipo;
    if (tipo == minero) {
      tareaPirata.posicionXObjetivo = x;
      tareaPirata.posicionYObjetivo = y;
    }
    tss_agregar_piratas_a_gdt(j);
    screen_pintar_pirata(j, &tareaPirata);
  }
>>>>>>> bb19678... compila y corre, falta testear
}

void game_pirata_habilitar_posicion(jugador_t *j, pirata_t *pirata, int x, int y)
{

}


void game_explorar_posicion(jugador_t *jugador, int c, int f)
{
}


uint game_syscall_pirata_mover(uint id, direccion dir)
{
  // pirata_t* tareaPirata = id_pirata2pirata(id);
  // if (tareaPirata->tipoPirata == minero) {
  //   //tengo que checkear que la posicion ya está mapeada
  //
  // } else {
  //   //si no es minero tengo que checkear que sea valida
  //   int x, y;
  //   game_dir2xy(dir, x, y)
  //   if (game_posicion_valida(tareaPirata->posicionX + x, tareaPirata->posicionY + y)) {
  //     //va a pasar a una posicion valida en el juego
  //
  //   } else {
  //     //si no va a ir a una posicion valida lo mato
  //
  //   }
  // }
  return 0;
}

uint game_syscall_cavar(uint id)
{
    // ~ completar ~

	return 0;
}

uint game_syscall_pirata_posicion(uint id, int idx)
{
    // ~ completar ~
    return 0;
}

uint game_syscall_manejar(uint syscall, uint param1)
{
    // ~ completar ~
    return 0;
}

void game_pirata_exploto(uint id)
{
}

pirata_t* game_pirata_en_posicion(uint x, uint y)
{
	for (int i = 0; i < 8; i++) {
    if (jugadorA.piratas[i].posicionX == x && jugadorA.piratas[i].posicionY == y) {
      return &(jugadorA.piratas[i]);
    } else if (jugadorB.piratas[i].posicionX == x && jugadorB.piratas[i].posicionY == y) {
      return &(jugadorB.piratas[i]);
    }
  }

  return NULL;
}


void game_jugador_anotar_punto(jugador_t *j)
{
  j->puntaje = j->puntaje + 1;
  // contador_de_tiempo = 0;
}



void game_terminar_si_es_hora()
{
  // if (contador_de_tiempo == MAX_TIEMPO) {
  //   screen_stop_game_show_winner();
  // }
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
      break;
    case KB_shiftR:
      print("shiftR", 65, 0, 0xF);
      break;
  }
}
