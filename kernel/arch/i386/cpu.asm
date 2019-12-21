section .text
    global haltCPU:function
    global getESP:function
    global contextSwitch:function
    global saveRegisters:function

    haltCPU:
        hlt
        ret

    getESP:
        mov     eax, esp
        ret
