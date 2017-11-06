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

  //puntaje
  i = 47;
  j = 36;
  print("0", j, i, 0xF);
  j = j + 7;
  print("0", j, i, 0xF);
}
