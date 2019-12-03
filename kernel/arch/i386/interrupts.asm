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

    isrNoError 0
    isrNoError 1
    isrNoError 2
    isrNoError 3
    isrNoError 4
    isrNoError 5
    isrNoError 6
    isrNoError 7
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
    isrNoError 18
    isrNoError 19
    isrNoError 20
    isrNoError 21
    isrNoError 22
    isrNoError 23
    isrNoError 24
    isrNoError 25
    isrNoError 26
    isrNoError 27
    isrNoError 28
    isrNoError 29
    isrError 30
    isrNoError 31
    irq 0
    irq 1
    irq 2
    irq 3
    irq 4
    irq 5
    irq 6
    irq 7
    irq 8
    irq 9
    irq 10
    irq 11
    irq 12
    irq 13
    irq 14
    irq 15
    isrNoError 80
