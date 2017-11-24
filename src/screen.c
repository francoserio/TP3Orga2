/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "screen.h"
#include "game.h"


extern jugador_t jugadorA, jugadorB;


static ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;

const char reloj[] = "|/-\\";
#define reloj_size 4


void screen_actualizar_reloj_global()
{
    static uint reloj_global = 0;

    reloj_global = (reloj_global + 1) % reloj_size;

    screen_pintar(reloj[reloj_global], C_BW, 49, 79);

    for (int i = 0; i < 8; i++) {
      screen_actualizar_reloj_pirata(&jugadorA, &(jugadorA.piratas[i]));

      screen_actualizar_reloj_pirata(&jugadorB, &(jugadorB.piratas[i]));
    }

    screen_pintar_reloj_piratas(&jugadorA);

    screen_pintar_reloj_piratas(&jugadorB);

    contador_de_tiempo++;
}

void screen_actualizar_reloj_pirata(jugador_t *j, pirata_t *pirata) {
  if (pirata->vivoMuerto == 0) {
    //si se murio
    pirata->reloj = 0;
    reloj_pirata[pirata->id] = 0;
  } else {
    pirata->reloj = (pirata->reloj + 1) % reloj_size;
    reloj_pirata[pirata->id] = pirata->reloj;
  }
}

void screen_pintar(uchar c, uchar color, uint fila, uint columna)
{
    p[fila][columna].c = c;
    p[fila][columna].a = color;
}

uchar screen_valor_actual(uint fila, uint columna)
{
    return p[fila][columna].c;
}

void print(const char * text, uint x, uint y, unsigned short attr) {
    int i;
    for (i = 0; text[i] != 0; i++)
     {
        screen_pintar(text[i], attr, y, x);

        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}

void print_hex(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char hexa[8];
    char letras[16] = "0123456789ABCDEF";
    hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
    hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
    hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
    hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
    hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
    hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
    hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
    hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
    for(i = 0; i < size; i++) {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;
    }
}

void print_dec(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char letras[16] = "0123456789";

    for(i = 0; i < size; i++) {
        int resto  = numero % 10;
        numero = numero / 10;
        p[y][x + size - i - 1].c = letras[resto];
        p[y][x + size - i - 1].a = attr;
    }
}

void screen_pintar_nombre() {
  print("ENIFRA", 74, 0, 0xF);
}

void screen_pintar_puntajes() {
  //puntaje
  int i = 47;
  int j = 35;
  print("000", j, i, 0xF);
  j = j + 7;
  print("000", j, i, 0xF);
}

void screen_pintar_reloj_pirata(jugador_t* j, pirata_t* pirata) {
  if (pirata->vivoMuerto == 0) {
    //si es 0 esta muerto
    if (j->index == 0) {
      //jugadorA
      screen_pintar('x', C_FG_RED, 48, pirata->index*2 + 4);
    } else {
      //jugadorB
      screen_pintar('x', C_FG_BLUE, 48, pirata->index*2 + 60);
    }
  } else {
    //esta vivo
    if (j->index == 0) {
      //jugadorA
      screen_pintar(reloj[pirata->reloj], C_BW, 48, pirata->index*2 + 4);
    } else {
      //jugadorB
      screen_pintar(reloj[pirata->reloj], C_BW, 48, pirata->index*2 + 60);
    }
  }
}

void screen_pintar_reloj_piratas(jugador_t* j) {
  for (int i = 0; i < 8; i++) {
    screen_pintar_reloj_pirata(j, &(j->piratas[i]));
  }
}

void screen_inicializar() {
  int i = 0;
  int j;
  while (i < 50) {
    j = 0;
     while (j < 80) {
       screen_pintar(0x00, 0x07 << 4, i, j);
       j++;
     }
     i++;
  }

  i = 45;
  while (i < 50) {
    j = 0;
    while (j < 80) {
      screen_pintar(0x00, 0x00 << 4, i, j);
      j++;
    }
    i++;
  }

  i = 45;
  while (i < 50) {
    j = 33;
    while (j < 40) {
      screen_pintar(0x00, 0x04 << 4, i, j);
      j++;
    }
    while (j < 47) {
      screen_pintar(0x00, 0x01 << 4, i, j);
      j++;
    }
    i++;
  }

  i = 0;
  j = 0;
  while (j < 80) {
    screen_pintar(0x00, 0x00 << 4, i, j);
    j++;
  }

  i = 46;
  j = 4;
  print("1 2 3 4 5 6 7 8", j, i, 0xF);
  j = j + 56;
  print("1 2 3 4 5 6 7 8", j, i, 0xF);
  screen_pintar_reloj_piratas(&jugadorA);
  screen_pintar_reloj_piratas(&jugadorB);
}

void screen_pintar_piratas() {
  for (int i = 0; i < 8; i++) {
    screen_pintar_pirata(&jugadorA, &(jugadorA.piratas[i]));
    screen_pintar_pirata(&jugadorB, &(jugadorB.piratas[i]));
  }
}

void screen_pintar_pirata(jugador_t *j, pirata_t *pirata) {
  for (int i = 0; i < 45; i++) {
    for (int p = 0; p < 80; p++) {
      if (j->posicionesXVistas[p] == 1 && j->posicionesYVistas[i] == 1 && game_pirata_en_posicion(p, i) == NULL) {
        screen_pintar(' ', j->colorJug, i, p);
      } else if (game_pirata_en_posicion(p, i) != NULL) {
        if (game_pirata_en_posicion(p, i)->tipo == minero) {
          screen_pintar('M', j->colorJug, pirata->posicionY, pirata->posicionX);
        } else {
          screen_pintar('E', j->colorJug, pirata->posicionY, pirata->posicionX);
        }
      }
    }
  }
  if (pirata->tipo == minero) {
    screen_pintar('M', j->colorJug, pirata->posicionY, pirata->posicionX);
  } else {
    screen_pintar('E', j->colorJug, pirata->posicionY, pirata->posicionX);
  }
}

void screen_borrar_pirata(jugador_t *j, pirata_t *pirata) {
  screen_pintar(' ', j->colorJug, pirata->posicionY, pirata->posicionX);
}

void screen_stop_game_show_winner(jugador_t *j) {
  print("GANADOR:" + j->index, 39, 22, 0xF);
}
