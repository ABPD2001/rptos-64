#include "../std-lib/header.h"

void app2()
{
    char buffer[30];
    char *qeust = "Enter your name: ";
    char *prefix = "hello ";

    printf(qeust);           // print question.
    scanf(buffer, 30, NULL); // read from serial.

    printf(prefix); // print prefix.
    printf(buffer); // print name.
    printf("!");    // print '!'.

    terminate(0, 0);
}