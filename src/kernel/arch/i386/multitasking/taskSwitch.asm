struc ArchTask
    .esp:       resd 1
    .kernelESP: resd 1
    .cr3:       resd 1
endstruc

section .data
    extern currentArchTask

section .text
    global x86TaskSwitch:function
    extern setTSSKernelStack

    x86TaskSwitch:
        ; The stack thus far:
        ; The volatiles (EAX, ECX, EDX) are saved, if needed
        ; EIP is saved
        ; We need the nonvolatiles
        push    ebp
        push    esi
        push    edi
        push    ebx

        ; Get the ending task and save its stack pointer
        mov     ebx, [currentArchTask]
        mov     [ebx + ArchTask.esp], esp

        ; Now get the upcoming task
        ; The arithmetic here is compiled away,
        ; but is left for documentation's sake:
        ; Skip the four registers pushed, and grab the first argument
        mov     ebx, [esp + 4 * 4 + 4]
        mov     [currentArchTask], ebx

        ; Load the upcoming task's stack and cr3 registers
        mov     esp, [ebx + ArchTask.esp]
        mov     ecx, [ebx + ArchTask.kernelESP]
        mov     edx, [ebx + ArchTask.cr3]
        ; Get previous cr3
        mov     esi, cr3
        ; Check to see if a reload is necessary
        cmp     edx, esi
        je      .skipCR3
        mov     cr3, edx
    .skipCR3:
        ; load TSS0
        push    ecx
        call    setTSSKernelStack
        pop     ecx

        ; now, get this functions ebp, esi, edi, and edx and go to its EIP
        pop     ebx
        pop     edi
        pop     esi
        pop     ebp
        ret
