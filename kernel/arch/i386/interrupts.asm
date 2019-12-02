section .text
    global loadIDT:function (loadIDT.end - loadIDT)
    global enableInterrupts:function (enableInterrupts.end - enableInterrupts)
    global disableInterrupts:function (disableInterrupts.end - disableInterrupts)

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

    global exceptionHandler:function (exceptionHandler.end - exceptionHandler)
    global doubleFault:function (doubleFault.end - doubleFault)
    global generalProtectionFault:function (generalProtectionFault.end - generalProtectionFault)
    global irq0:function (irq0.end - irq0)
    global irq1:function (irq1.end - irq1)
    global irq2:function (irq2.end - irq2)
    global irq3:function (irq3.end - irq3)
    global irq4:function (irq4.end - irq4)
    global irq5:function (irq5.end - irq5)
    global irq6:function (irq6.end - irq6)
    global irq7:function (irq7.end - irq7)
    global irq8:function (irq8.end - irq8)
    global irq9:function (irq9.end - irq9)
    global irq10:function (irq10.end - irq10)
    global irq11:function (irq11.end - irq11)
    global irq12:function (irq12.end - irq12)
    global irq13:function (irq13.end - irq13)
    global irq14:function (irq14.end - irq14)
    global irq15:function (irq15.end - irq15)
    extern cException
    extern cDoubleFault
    extern cGeneralProtectionFault
    extern cIRQ0
    extern cIRQ1
    extern cIRQ2
    extern cIRQ3
    extern cIRQ4
    extern cIRQ5
    extern cIRQ6
    extern cIRQ7
    extern cIRQ8
    extern cIRQ9
    extern cIRQ10
    extern cIRQ11
    extern cIRQ12
    extern cIRQ13
    extern cIRQ14
    extern cIRQ15

    exceptionHandler:
        pushad
        cld
        call cException
        popad
        iretd
    .end:

    doubleFault:
        pushad
        cld
        call cDoubleFault
        popad
        iretd
    .end:

    generalProtectionFault:
        pushad
        cld
        call cGeneralProtectionFault
        popad
        iretd
    .end:

    irq0:
        pushad
        cld
        call cIRQ0
        popad
        iretd
    .end:

    irq1:
        pushad
        cld
        call cIRQ1
        popad
        iretd
    .end:

    irq2:
        pushad
        cld
        call cIRQ2
        popad
        iretd
    .end:

    irq3:
        pushad
        cld
        call cIRQ3
        popad
        iretd
    .end:

    irq4:
        pushad
        cld
        call cIRQ4
        popad
        iretd
    .end:

    irq5:
        pushad
        cld
        call cIRQ5
        popad
        iretd
    .end:

    irq6:
        pushad
        cld
        call cIRQ6
        popad
        iretd
    .end:

    irq7:
        pushad
        cld
        call cIRQ7
        popad
        iretd
    .end:

    irq8:
        pushad
        cld
        call cIRQ8
        popad
        iretd
    .end:

    irq9:
        pushad
        cld
        call cIRQ9
        popad
        iretd
    .end:

    irq10:
        pushad
        cld
        call cIRQ10
        popad
        iretd
    .end:

    irq11:
        pushad
        cld
        call cIRQ11
        popad
        iretd
    .end:

    irq12:
        pushad
        cld
        call cIRQ12
        popad
        iretd
    .end:

    irq13:
        pushad
        cld
        call cIRQ13
        popad
        iretd
    .end:

    irq14:
        pushad
        cld
        call cIRQ14
        popad
        iretd
    .end:

    irq15:
        pushad
        cld
        call cIRQ15
        popad
        iretd
    .end:
