; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "imprimir.mac"

global start

extern GDT_DESC
extern KERNEL
extern IDT_DESC
extern idt_inicializar
extern screen_inicializar
extern mmu_inicializar_dir_kernel
extern mmu_inicializar
extern screen_pintar_nombre
extern resetear_pic
extern habilitar_pic
extern game_inicializar

;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
iniciando_mr_msg db     'Iniciando kernel (Modo Real)...'
iniciando_mr_len equ    $ - iniciando_mr_msg

iniciando_mp_msg db     'Iniciando kernel (Modo Protegido)...'
iniciando_mp_len equ    $ - iniciando_mp_msg

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    ; Deshabilitar interrupciones
    cli

    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    imprimir_texto_mr iniciando_mr_msg, iniciando_mr_len, 0x07, 0, 0
    ; Habilitar A20
    call habilitar_A20
    xchg bx, bx
    ; Cargar la GDT
    lgdt [GDT_DESC]
    ; Setear el bit PE del registro CR0
    xchg bx, bx
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    ; Saltar a modo protegido
    xchg bx, bx
    jmp 0x58:mp
    ; Establecer selectores de segmentos
BITS 32
mp:
    xchg bx, bx
    xor eax, eax
    mov ax, 1001000b    ;data segment nivel 0
    mov ds, ax
    xchg bx, bx
    mov es, ax
    mov gs, ax
    mov ss, ax
    mov ax, 1100000b    ;video
    mov fs, ax
    ; Establecer la base de la pila
    xchg bx, bx
    mov ebp, 0x27000       ;setear la pila
    mov esp, 0x27000
    xchg bx, bx
    ; Imprimir mensaje de bienvenida
    imprimir_texto_mp iniciando_mp_msg, iniciando_mp_len, 0x07, 2, 0
    xchg bx, bx
    ; Inicializar el juego
    call game_inicializar
    ; Inicializar pantalla
    call screen_inicializar
    call screen_pintar_nombre
    xchg bx, bx
    ; Inicializar el manejador de memoria
    call mmu_inicializar
    xchg bx, bx
    ; Inicializar el directorio de paginas
    call mmu_inicializar_dir_kernel
    xchg bx, bx
    call mmu_inicializar_dir_pirata
    xchg bx, bx
    ; Cargar directorio de paginas
    xor eax, eax
    mov eax, 0x27000
    mov cr3, eax
    xchg bx, bx
    ; Habilitar paginacion
    xor eax, eax
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    xchg bx, bx
    ; Inicializar tss

    ; Inicializar tss de la tarea Idle

    ; Inicializar el scheduler

    ; Inicializar la IDT
    call idt_inicializar
    xchg bx, bx
    ; Cargar IDT
    lidt[IDT_DESC]
    xchg bx, bx
    ; Configurar controlador de interrupciones
    call resetear_pic
    xchg bx, bx
    call habilitar_pic
    xchg bx, bx
    ; Cargar tarea inicial

    ; Habilitar interrupciones
    sti
    xchg bx, bx
    ; Saltar a la primera tarea: Idle

    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
