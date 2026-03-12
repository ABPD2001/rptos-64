.section .text
.balign 4

.global system_shutdown,system_reboot

system_shutdown:
    svc #25
    ret
svc_system_reboot:
    svc #26
    ret