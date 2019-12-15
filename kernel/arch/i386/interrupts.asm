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

%macro irq 1
    ; args: number
    global irq%1:function
    extern cIRQ%1
    irq%1:
        pushad
        cld
        call    cIRQ%1
        popad
        iretd
    .end:
%endmacro

section .text
    global loadIDT:function
    global enableInterrupts:function
    global disableInterrupts:function
    extern interruptHandler

    loadIDT:
        mov     edx, [esp + 4]
        lidt    [edx]
        ret

    enableInterrupts:
        sti
        ret

    disableInterrupts:
        cli
        ret

    isrCommon:
        cld
        push    ecx ; set to the interrupt number in macros
        call    interruptHandler
        add     esp, 4 ; remove interrupt number
        popad
        add     esp, 4 ; remove error code
        iretd

    %assign i 0
    %rep    8
        isrNoError i
        %assign i i+1
    %endrep
    isrError 8
    isrNoError 9
    isrError 10
    isrError 11
    isrError 12
    isrError 13
    isrError 14
    isrNoError 15
    isrNoError 16
    isrError 17
    %assign i 18
    %rep    12
        isrNoError i
        %assign i i+1
    %endrep
    isrError 30
    isrNoError 31
    %assign i 0
    %rep    16
        irq i
        %assign i i+1
    %endrep
    isrNoError 80
