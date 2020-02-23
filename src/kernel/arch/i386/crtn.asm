section .init
    ; crtend.o's init section goes here
    pop ebp
    ret

section .fini
    ; crtend.o's fini section goes here
    pop ebp
    ret
