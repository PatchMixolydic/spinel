section .text
    global haltCPU:function

    haltCPU:
        hlt
        ret
