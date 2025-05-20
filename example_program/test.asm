
; This is an example program

    .section .text
    .global _start
_start:
    mov r4, #0x11
    mov r1, r4
    mov r3, r1
    mov ES:!0xfe20, #0x69 ; Random place in ram
    mov r0, ES:!0xfe20
    inc r0
    mov PSW, #0x03
    mov A, PSW
    nop
    xch A, r0
    oneb A
    clrb r0
    add a, #0x03
    movw AX, #loop
loop:
    nop
    br AX ; infinite loop