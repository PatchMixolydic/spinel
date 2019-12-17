section .text
    global haltCPU:function
    global getESP:function

    haltCPU:
        hlt
        ret

    loadGDT:
        mov     eax, [esp + 4]
        lgdt    [eax] ; load GDT
        ; reloading cs requires this:
        jmp     0x8:.loadCS
    .loadCS:
        mov     ax, 0x10
        mov     ds, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax
        mov     ss, ax
        ret

    getESP:
        mov     eax, esp
        ret
