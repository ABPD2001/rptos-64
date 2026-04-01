#include "../std-lib/header.h"

void app1() // serial mailbox id is 0 and power mailbox is 1.
{
    u64_t content_pt1 = 0;

    char buffer[14] = "hello, world!"; // text.
    buffer[13] = '\0';                 // just in case.

    content_pt1 |= 1;       // set write mode.
    content_pt1 |= 14 << 1; // set length.
    content_pt1 |= '\n'
                   << 33; // set delimiter.

    u64_t mailbox = mxaddress(0);                    // get mailbox physical address.
    mxwrite(mailbox, content_pt1, &buffer, true, 0); // send a serial write request.
}