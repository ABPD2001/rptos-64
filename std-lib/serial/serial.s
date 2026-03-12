.section .text
.balign 4

.global serial_put

serial_put:
    svc #0 @ set a writing operation.
    mov x0,#0 @ set wait reason for mini-uart (tx).
    mov x1,#0 @ clear instruction of wait.
    svc #27 @ wait.

    ret @ return.

serial_read:
    svc #1
    mov x0,#1 @ set wait reson for mini-uart (rx).
    mov x1,0 @ clear instructio of wait.
    svc #27 @ wait.

    ret @ return.

allocate_serial:
    svc #7 @ try to allocate.
    ret @ return.

release_serial:
    svc #6 @ try to allocate.
    ret @ return.

serial_available:
    svc #5 @ try to allocate.
    ret @ return.