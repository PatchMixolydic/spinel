section .text
    global haltCPU:function
    global inByte:function
    global outByte:function
    global inShort:function
    global outShort:function
    global inWord:function
    global outWord:function

    haltCPU:
        hlt
        ret

    inByte:
        mov     dx, [esp + 4] ; address
        in      al, dx
        ret
    .end:

    outByte:
        mov     al, [esp + 8] ; data
        mov     dx, [esp + 4] ; address
        out     dx, al
        ret
    .end:

    inShort:
        mov     dx, [esp + 4] ; address
        in      ax, dx
        ret
    .end:

    outShort:
        mov     ax, [esp + 8] ; data
        mov     dx, [esp + 4] ; address
        out     dx, ax
        ret
    .end:

    inWord:
        mov     dx, [esp + 4] ; address
        in      eax, dx
        ret
    .end:

    outWord:
        mov     eax, [esp + 8] ; data
        mov     dx, [esp + 4] ; address
        out     dx, eax
        ret
    .end:
