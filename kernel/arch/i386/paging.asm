CR0PagingFlag equ 1 << 31
KernelOffset equ 0xC0000000

section .text
    global setCR3:function
    global getCR3:function
    global getCR2:function
    global enablePaging:function
    global invalidatePage:function

    setCR3:
        mov     eax, [esp + 4]
        mov     cr3, eax
        ret

    getCR3:
        mov     eax, cr3
        ret

    getCR2:
        mov     eax, cr2
        ret

    enablePaging:
        mov     eax, cr0
        or      eax, CR0PagingFlag
        mov     cr0, eax
        ret

    invalidatePage:
        mov     eax, [esp + 4]
        invlpg  [eax]
        ret
