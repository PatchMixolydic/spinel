section .text
    global haltCPU:function
    global inByte:function
    global outByte:function
    global inWord:function
    global outWord:function
    global inDWord:function
    global outDWord:function
    global loadGDT:function
    global loadIDT:function
    global getESP:function
    global enableInterrupts:function
    global disableInterrupts:function

    haltCPU:
        hlt
        ret

    inByte:
        mov     dx, [esp + 4] ; address
        in      al, dx
        ret

    outByte:
        mov     al, [esp + 4] ; data
        mov     dx, [esp + 4] ; address
        out     dx, al
        ret

    inWord:
        mov     dx, [esp + 4] ; address
        in      ax, dx
        ret

    outWord:
        mov     ax, [esp + 4] ; data
        mov     dx, [esp + 4] ; address
        out     dx, ax
        ret

    inDWord:
        mov     dx, [esp + 4] ; address
        in      eax, dx
        ret

    outDWord:
        mov     eax, [esp + 4] ; data
        mov     dx, [esp + 4] ; address
        out     dx, eax
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

    loadIDT:
        mov     edx, [esp + 4]
        lidt    [edx]
        ret

    getESP:
        mov     eax, esp
        ret

    enableInterrupts:
        sti
        in      eax, 0x70
        or      eax, 0x80 ; turn on NMI
        out     0x70, eax
        ret

    disableInterrupts:
        cli
        in      eax, 0x70
        and     eax, 0x7F ; turn off NMI
        out     0x70, eax
        ret
