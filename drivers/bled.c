#include "./bled.h"

void initialize_bleds()
{

    gpfunction(GPIO_FSEL4, 2, GPIO_OUTPUT); // set GPIO42 to output.
    gpfunction(GPIO_FSEL4, 5, GPIO_OUTPUT); // set GPIO45 to output.

    gpclear(GPIO_CLR4, 10); // clear GPIO42 (just in case).
    gpclear(GPIO_CLR4, 13); // clear GPIO45 (just in case).
}

void brled_on()
{
    gpset(GPIO_SET4, 13); // set GPIO45.
}
void bgled_on()
{
    gpset(GPIO_SET4, 10); // set GPIO42.
}
void brled_off()
{
    gpclear(GPIO_CLR4, 13); // clear GPIO45 (just in case).
}
void bgled_off()
{
    gpclear(GPIO_CLR4, 10); // clear GPIO42 (just in case).
}
void brled_set(u8_t value)
{
    if (value)
        gpclear(GPIO_CLR4, 13);
    else
        gpset(GPIO_SET4, 13);
}
void bgled_set(u8_t value)
{
    if (value)
        gpset(GPIO_SET4, 10);
    else
        gpclear(GPIO_CLR4, 10);
}