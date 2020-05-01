 %macro isrNoError 1
    ; args: number
    global isr%1:function
    isr%1:
        push    dword 0 ; no error code
        pushad
        mov     ecx, %1
        jmp     isrCommon
%endmacro

%macro isrError 1
    ; args: number
    global isr%1:function
    isr%1:
        pushad
        mov     ecx, %1
        jmp     isrCommon
%endmacro

section .text
    extern mainInterruptHandler

    isrCommon:
        cld
        push    ecx ; set to the interrupt number in macros
        call    mainInterruptHandler
        add     esp, 4 ; remove interrupt number
        popad
        add     esp, 4 ; remove error code
        iretd

    %assign i 0
    %rep    8
        isrNoError i
        %assign i i+1
    %endrep
    isrError    8
    isrNoError  9
    %assign i 10
    %rep 5
        isrError i
        %assign i i+1
    %endrep
    isrNoError  15
    isrNoError  16
    isrError    17
    %assign i 18
    %rep    12
        isrNoError i
        %assign i i+1
    %endrep
    isrError    30
    isrNoError  31
    %assign i 32
    %rep    16
        isrNoError i
        %assign i i+1
    %endrep
    isrNoError  0x88
