extern exceptionHandler
extern doubleFault
extern generalProtectionFault
extern irq0
extern irq1
extern irq2
extern irq3
extern irq4
extern irq5
extern irq6
extern irq7
extern irq8
extern irq9
extern irq10
extern irq11
extern irq12
extern irq13
extern irq14
extern irq15
extern int80

section .rodata
    interruptHandlers:
        dd      exceptionHandler ; Division
        dd      exceptionHandler ; Single step
        dd      exceptionHandler ; NMI
        dd      exceptionHandler ; Breakpoint
        dd      exceptionHandler ; into instruction found overflow
        dd      exceptionHandler ; Bound range exceeded
        dd      exceptionHandler ; Illegal instruction
        dd      exceptionHandler ; CPU extension not available
        dd      doubleFault ; Double fault
        dd      exceptionHandler ; Reserved
        dd      exceptionHandler ; Invalid TSS
        dd      exceptionHandler ; Segment not present
        dd      exceptionHandler ; Stack fault
        dd      generalProtectionFault ; General protection fault
        dd      exceptionHandler ; Page fault
        dd      exceptionHandler ; ?
        dd      exceptionHandler ; Coprocessor error
        dd      exceptionHandler ; Alignment check
        dd      exceptionHandler ; Machine check
        dd      exceptionHandler ; Not used by x86 itself
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      irq0
        dd      irq1
        dd      irq2
        dd      irq3
        dd      irq5
        dd      irq5
        dd      irq6
        dd      irq7
        dd      irq8
        dd      irq9
        dd      irq10
        dd      irq11
        dd      irq12
        dd      irq13
        dd      irq14
        dd      irq15
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      exceptionHandler
        dd      int80
    .end:

    idtrInfo:
        dw      (idtBuffer.end - idtBuffer) - 1 ; size
        dd      idtBuffer ; address

section .bss
    idtBuffer:
        ; since addition is commutative, this should all add up to the correct
        ; number of bytes:
        resq    256
        ; resw    (interruptHandlers.end - interruptHandlers) / 4 ; offsetLow
        ; resw    (interruptHandlers.end - interruptHandlers) / 4 ; GDT index
        ; resb    (interruptHandlers.end - interruptHandlers) / 4 ; unused (x86)/ist(amd64)
        ; resb    (interruptHandlers.end - interruptHandlers) / 4 ; type and attributes
        ; resw    (interruptHandlers.end - interruptHandlers) / 4 ; offsetHigh(x86)/offsetMid(amd64)
        ; resd    (interruptHandlers.end - interruptHandlers) / 4 ; offsetHigh(amd64)
        ; resd    (interruptHandlers.end - interruptHandlers) / 4 ; unused (amd64)
    .end:


section .text
    global initIDT:function (initIDT.end - initIDT)

    initIDT:
        xor     ecx, ecx ; ecx to 0
        mov     edx, idtBuffer ; offset into the idt
    .loop:
        ; set up eax
        mov     eax, 4 ; 4 bytes per interrupt handler
        mul     ecx ; multiply eax by the next index
        adc     eax, interruptHandlers ; and add the interrupt handler offset
        ; now set up the idt
        mov     word [edx], ax
        mov     word [edx + 2], 0x08 ; kernel code
        mov     byte [edx + 4], 0
        mov     byte [edx + 5], 0x8E ; 0b1000_1110
        shr     eax, 16
        mov     word [edx + 6], ax
        ; finished
        inc     ecx ; increment ecx
        adc     edx, 8 ; 16 on amd64, go to next idt entry
        cmp     edx, (idtBuffer.end - idtBuffer)
        jb      .loop ; if edx is less than length of idt, go again

        ;finally, load the idt
        lidt    [idtrInfo]
        ret
    .end:
