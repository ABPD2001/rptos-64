.section .text
.balign 4

.global taskid,terminate,sleep,wait

taskid:
    svc #8
    ret
sleep:
    svc #9
    ret
terminate:
    svc #10
    ret
wait:
    svc #27
    ret