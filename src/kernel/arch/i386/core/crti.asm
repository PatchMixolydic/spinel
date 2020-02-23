section .init
    global _init:function (_init.end - _init)
    _init:
        push    ebp
        mov     ebp, esp
        ; crtbegin.o's init section goes here
    .end:

section .fini
    global _fini:function (_fini.end - _fini)
    _fini:
        push   ebp
        mov    ebp, esp
        ; crtbegin.o's fini section goes here
    .end:
