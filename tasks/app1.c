#include "../std-lib/header.h"

void app1()
{
    char buffer[14] = "hello, world!"; // text.
    buffer[13] = '\0';                 // just in case.

    serial_put(buffer, 14); // print.
    terminate(NULL, NULL);  // end to process with 0 code (success).
}