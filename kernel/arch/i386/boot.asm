MultibootModuleAlign equ 1 ; align modules on page boundaries
MultibootMemoryInfo equ 1 << 1 ; provide a memory map
MultibootMagic equ 0x1BADB002
MultibootFlags equ MultibootModuleAlign | MultibootMemoryInfo
MultibootChecksum equ -(MultibootMagic + MultibootFlags)

; Multiboot header
section .multiboot
align 4
    dd MultibootMagic
    dd MultibootFlags
    dd MultibootChecksum

; Read only data
section .rodata
    gdt:
    .null:
        dq      0
    .kernelCode:
        dw      0FFFFh ; Limit
        dw      0 ; Base
        dw      0 ; Base
        db      10011010b ; Present, privilege 0, code, readable, nonconforming
        db      11001111b ; 4kiB granularity
        db      0 ; Base
    .kernel:
        dw      0FFFFh ; Limit
        dw      0 ; Base
        dw      0 ; Base
        db      10010010b ; Present, privilege 0, data, expand down, writable, nonconforming
        db      11001111b ; 4kiB granularity
        db      0 ; Base
    .end:

    .desc:
        db      gdt.end - gdt
        dq      gdt

; Data, uninitialized
section .bss
align 16
    stackBottom:
        resb    16384
    stackTop:

; Code
section .text
    global _start:function (_start.end - _start)
    extern kernelMain
    extern _init

    _start:
        cli
        mov     esp, stackTop
        xor     ax, ax ; zero ax
        mov     ds, ax ; move 0 to data segment
        lgdt    [gdt.desc] ; load GDT
        call    _init
        call    kernelMain
        ; If kernelMain ever returns, spin forever
        cli
    .hang:
        hlt
        jmp     .hang
    .end:
