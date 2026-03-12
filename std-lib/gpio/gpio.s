.section .text
.balign 4

.global __built_in_std_gpioalloc,__built_in_std_gpiofree,__built_in_std_gpioon,__built_in_std_gpiooff,__built_in_std_gpioval,__built_in_gpiofunction

gpiofunction:
    svc #28
    ret @ return.
gpioalloc:
    svc #11
    ret @ return.
gpiofree:
    svc #12
    ret @ return.
gpioset:
    svc #13
    ret @ return.
gpioclear:
    svc 14
    ret @ return.
gpiovalue:
    svc #15
    ret @ return.