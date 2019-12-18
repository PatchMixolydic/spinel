section .text
    global haltCPU:function
    global getESP:function

    haltCPU:
        hlt
        ret

    getESP:
        mov     eax, esp
        ret
