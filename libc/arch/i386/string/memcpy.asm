section .text
    global memcpy:function

    memcpy:
        mov     edi, [esp + 4] ; dest
        mov     eax, edi ; function returns dest
        mov     esi, [esp + 8] ; src
        mov     ecx, [esp + 12] ; size
        cld
        rep movsb ; move byte [esi] to [edi] ecx times
        ret
