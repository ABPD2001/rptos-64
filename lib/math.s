.section .text

built_in_floor:
    mov s0,x0 @ set s0 (simd/neos & fpu registers) to x0 (parameter).
    fcvtms  x0, s0 @ floor s0 and store on x0.
    ret @ return

built_in_ceil:
    mov s0,x0 @ set s0 (simd/neos & fpu registers) to x0 (parameter).
    fcvtps x0, s0 @ ceil s0 and store on x0.
    ret @ return

built_in_max:
    cmp x0,x1 @ compare a and b numbers.
    csel x0,x0,x1,GE @ set x0 to (x0 if greater else x1).
    ret @ return

built_in_min:
    cmp x0,x1 @ compare a and b numbers.
    csel x0,x0,x1,LS @ set x0 to (x0 if greater else x1).
    ret @ return