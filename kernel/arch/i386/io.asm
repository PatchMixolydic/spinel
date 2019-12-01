section text:
    global inb:function (inb.end - inb)
    global outb:function (outb.end - outb)

    inb:
        mov     dx, [esp + 4] ; address
        in      al, dx
        ret
    .end:

    outb:
        mov     al, [esp + 8] ; data
        mov     dx, [esp + 4] ; address
        out     dx, al
        ret
    .end:
