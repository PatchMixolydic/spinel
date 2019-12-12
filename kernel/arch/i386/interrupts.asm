%macro isrNoError 1
    ; args: number
    global isr%1:function (isr%1.end - isr%1)
    isr%1:
        pushad
        cld
        push    byte %1
        call    interruptHandler
        add     esp, 4 ; remove interrupt number
        popad
        iretd
    .end:
%endmacro

%macro isrError 1
    ; args: number
    global isr%1:function (isr%1.end - isr%1)
    isr%1:
        pushad
        cld
        push    byte %1
        call    interruptHandler
        add     esp, 4 ; remove interrupt number
        popad
        add     esp, 4 ; remove error code
        iretd
    .end:
%endmacro

%macro irq 1
    ; args: number
    global irq%1:function (irq%1.end - irq%1)
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
    global loadIDT:function (loadIDT.end - loadIDT)
    global enableInterrupts:function (enableInterrupts.end - enableInterrupts)
    global disableInterrupts:function (disableInterrupts.end - disableInterrupts)
    extern interruptHandler

    loadIDT:
        mov     edx, [esp + 4]
        lidt    [edx]
        ret
    .end:

    enableInterrupts:
        sti
        ret
    .end:

    disableInterrupts:
        cli
        ret
    .end:

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
