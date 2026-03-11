.section .text
.balign 4

.global serial_put

serial_put:
    svc #0 @ set a writing operation.
    mov x0,#0 @ set wait reason for mini-uart (tx).
    mov x1,#0 @ clear instruction of wait.
    svc #27 @ wait

    ret @ return.