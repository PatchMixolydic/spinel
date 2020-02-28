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
    global getCR2:function
    global setCR3:function
    global getCR3:function
    global enableInterrupts:function
    global disableInterrupts:function
    global invalidatePage:function

    haltCPU:
        hlt
        ret

    inByte:
        mov     dx, [esp + 4] ; address
        in      al, dx
        ret

    outByte:
        mov     dx, [esp + 4] ; address
        mov     al, [esp + 8] ; data
        out     dx, al
        ret

    inWord:
        mov     dx, [esp + 4] ; address
        in      ax, dx
        ret

    outWord:
        mov     dx, [esp + 4] ; address
        mov     ax, [esp + 8] ; data
        out     dx, ax
        ret

    inDWord:
        mov     dx, [esp + 4] ; address
        in      eax, dx
        ret

    outDWord:
        mov     dx, [esp + 4] ; address
        mov     eax, [esp + 8] ; data
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

    getCR2:
        mov     eax, cr2
        ret

    setCR3:
        mov     edx, [esp + 4]
        mov     cr3, edx
        ret

    getCR3:
        mov     eax, cr3
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

    invalidatePage:
        mov     eax, [esp + 4]
        invlpg  [eax]
        ret
