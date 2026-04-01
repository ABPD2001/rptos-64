.section .text
.balign 4

.global mxcreate,mxwrite,mxread,mxaddress

mxcreate:
    svc #16
    ret
mxwrite:
    svc #17
    ret
mxread:
    svc #18
    ret
mxedit:
    svc #29
    ret

mxaddress_exit:
    mov x0,#0
    ret

mxadress:
    cmp x0,#64
    b.gt mxaddress_exit
    svc #31 