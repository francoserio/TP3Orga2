; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"

desc_0 db   'Divide Error'
desc_len_0 equ     $ - desc_0

desc_1 db   'Reserved'
desc_len_1 equ     $ - desc_1

desc_2 db   'NMI Error'
desc_len_2 equ     $ - desc_2

desc_3 db   'Breakpoint Error'
desc_len_3 equ     $ - desc_3

desc_4 db   'Overflow Error'
desc_len_4 equ     $ - desc_4

desc_5 db   'Bound Range Exceed Error'
desc_len_5 equ     $ - desc_5

desc_6 db   'Invalid Opcode Error'
desc_len_6 equ     $ - desc_6

desc_7 db   'Device not available Error'
desc_len_7 equ     $ - desc_7

desc_8 db   'Double fault Error'
desc_len_8 equ     $ - desc_8

desc_9 db   'Coprocesssor overrun Error'
desc_len_9 equ     $ - desc_9

desc_10 db   'Invalid TSS Error'
desc_len_10 equ     $ - desc_10

desc_11 db   'Segment not present Error'
desc_len_11 equ     $ - desc_11

desc_12 db   'Stack Segment Fault Error'
desc_len_12 equ     $ - desc_12

desc_13 db    'General Protection'
desc_len_13 equ    $ - desc_13

desc_14 db   'Page Fault Error'
desc_len_14 equ     $ - desc_14

desc_15 db   'Intel Reserved Error'
desc_len_15 equ     $ - desc_15

desc_16 db   'FPU Error'
desc_len_16 equ     $ - desc_16

desc_17 db   'Alignment Check Error'
desc_len_17 equ     $ - desc_17

desc_18 db   'Machine Check Error'
desc_len_18 equ     $ - desc_18

desc_19 db   'SIMD Error'
desc_len_19 equ     $ - desc_19

BITS 32

sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_tick
extern sched_tarea_actual

;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 1
global _isr%1

_isr%1:
    mov eax, %1
    imprimir_texto_mp desc_%1, desc_len_%1, 0x07, 3, 0
    jmp $

%endmacro

;;
;; Datos

;; -------------------------------------------------------------------------- ;;
; Scheduler

;;
;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19
;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
