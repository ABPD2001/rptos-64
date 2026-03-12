.section .text
.balign 4

.global mxcreate,mxwrite,mxread

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