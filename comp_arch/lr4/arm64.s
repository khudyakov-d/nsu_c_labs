ex(int):
        sub     sp, sp, #48
        str     w0, [sp, 12]
        fmov    d0, 1.0e+0
        str     d0, [sp, 32]
        fmov    d0, 1.0e+0
        str     d0, [sp, 24]
        mov     x0, 1
        str     x0, [sp, 40]
.L3:
        ldr     x1, [sp, 40]
        mov     x0, 25855
        movk    x0, 0x1dcd, lsl 16
        cmp     x1, x0
        bhi     .L2
        ldr     w0, [sp, 12]
        scvtf   d1, w0
        ldr     d0, [sp, 24]
        fmul    d1, d1, d0
        ldr     x0, [sp, 40]
        ucvtf   d0, x0
        fdiv    d0, d1, d0
        str     d0, [sp, 24]
        ldr     d1, [sp, 32]
        ldr     d0, [sp, 24]
        fadd    d0, d1, d0
        str     d0, [sp, 32]
        ldr     x0, [sp, 40]
        add     x0, x0, 1
        str     x0, [sp, 40]
        b       .L3
.L2:
        ldr     d0, [sp, 32]
        add     sp, sp, 48
        ret
main:
        stp     x29, x30, [sp, -32]!
        add     x29, sp, 0
        mov     w0, 10
        str     w0, [x29, 28]
        ldr     w0, [x29, 28]
        bl      ex(int)
        str     d0, [x29, 16]
        mov     w0, 0
        ldp     x29, x30, [sp], 32
        ret
