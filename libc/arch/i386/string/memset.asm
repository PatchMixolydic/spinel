section .text
    global memset:function

    memset:
        mov     edi, [esp + 4] ; bufptr
        mov     eax, edi ; function returns bufptr
        mov     al, [esp + 8] ; value
        mov     ecx, [esp + 12] ; size
        cld
        rep stosb ; store byte ecx times
        ret
