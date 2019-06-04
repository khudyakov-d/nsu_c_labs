_Z2exi:
        push    {r3, r4, r5, r6, r7, r8, r9, r10, fp, lr}
        bl      __aeabi_i2d
        mov     r6, #0
        ldr     r7, .L6
        mov     r10, r0
        mov     fp, r1
        mov     r8, r6
        mov     r9, r7
        mov     r4, #1
        ldr     r5, .L6+4
.L2:
        mov     r2, r6
        mov     r3, r7
        mov     r0, r10
        mov     r1, fp
        bl      __aeabi_dmul
        mov     r6, r0
        mov     r0, r4
        mov     r7, r1
        bl      __aeabi_ui2d
        mov     r2, r0
        mov     r3, r1
        mov     r0, r6
        mov     r1, r7
        bl      __aeabi_ddiv
        mov     r2, r0
        mov     r3, r1
        mov     r6, r0
        mov     r7, r1
        mov     r0, r8
        mov     r1, r9
        bl      __aeabi_dadd
        add     r4, r4, #1
        cmp     r4, r5
        mov     r8, r0
        mov     r9, r1
        bne     .L2
        pop     {r3, r4, r5, r6, r7, r8, r9, r10, fp, lr}
        bx      lr
.L6:
        .word   1072693248
        .word   500000000
main:
        mov     r0, #0
        bx      lr
