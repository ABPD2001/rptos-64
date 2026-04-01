#include "./serial.h"

u64_t printf(u8_t *buffer)
{
    u64_t content_pt1 = 0b1; // set write mode.
    u32_t length = 0;

    while (!buffer[length])
    {
        length++;
    }

    content_pt1 |= length << 1;               // set length.
    mxwrite(0, content_pt1, buffer, true, 0); // send a request to mailbox for owner.
}

u64_t scanf(u8_t *buffer, u32_t length, u8_t delimiter)
{
    u64_t content_pt1 = 0; // set read mode.
    if (!delimiter)
        delimiter = '\n'; // if wasnt set, set default.

    content_pt1 |= length << 1;               // set length.
    content_pt1 |= delimiter << 33;           // set delimiter.
    mxwrite(0, content_pt1, buffer, true, 0); // send a request to mailbox for owner.
}