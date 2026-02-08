.section .text

built_in_floor:
    mov s0,x0 @ set s0 (simd/neos & fpu registers) to x0 (parameter).
    fcvtms  x0, s0 @ floor s0 and store on x0.
    ret @ return

built_in_ceil:
    mov s0,x0 @ set s0 (simd/neos & fpu registers) to x0 (parameter).
    fcvtms  x0, s0 @ ceil s0 and store on x0.
    ret @ return