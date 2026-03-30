#include "./kmem.h"

volatile struct kmem_page_t *alloc_organized_frame(u8_t size)
{
    const u32_t sizes[5] = {2048, 8192, 32768, 131072, 524288};
    volatile struct kmem_page_t *output = NULL;

    if (size > 4)
        return 0; // 0 as error (fail).

    // gain mutex for kernel memory system.
    while (!gain_mutex(kmem_lock))
    {
        spinwait_mutex(kmem_lock);
    }

    for (u64_t i = 0; i < 93; i++)
    {

        if (!kernel_pages[i].id & (0xF << 28))
        {
            kernel_pages[i].id |= (size & 0xf) << 28; // only 4 bits.
            kernel_pages[i].size = sizes[size];       // set size.
            kernel_pages[i].start_address = 0;        // set start address. (test value)
            output = kernel_pages + i;                // return pointer.
            break;
        }
        output = 0; // set as full.
    }

    for (u64_t i = 0; i < 93; i++)
    {
        if (!kernel_frames[i].allocated && kernel_frames[i].size == size) // if same size and doesnt allocated
        {
            kernel_frames[i].allocated = 1; // set allocated.
            output->start_address = kernel_frames[i].start_address;
        }
    }
    if (!output->start_address) // if wasnt found any same size.
        for (u64_t i = 0; i < 93; i++)
        {
            if (!kernel_frames[i].allocated && kernel_frames[i].size >= size) // if same size and doesnt allocated
            {
                kernel_frames[i].allocated = 1; // set allocated.
                output->start_address = kernel_frames[i].start_address;
            }
        }

    release_mutex(kmem_lock); // release mutex kernel memory lock.
    return output;
}

u64_t available_organized_frame(u8_t size)
{
    const u32_t sizes[5] = {2048, 8192, 32768, 131072, 524288};
    u64_t output = false;

    if (size > 4)
        return 0; // 0 as error (fail).

    // gain mutex for kernel memory system.
    while (!gain_mutex(kmem_lock))
    {
        spinwait_mutex(kmem_lock);
    }

    for (u64_t i = 0; i < 93; i++)
    {
        if (!kernel_frames[i].allocated && kernel_frames[i].size == size) // if same size and doesnt allocated
        {
            output = true; // set availblity.
        }
    }

    release_mutex(kmem_lock); // release mutex kernel memory lock.
    return output;
}

u64_t free_organized_frame(u32_t id)
{
    // gain mutex for kernel memory system.
    while (!gain_mutex(kmem_lock))
    {
        spinwait_mutex(kmem_lock);
    }

    for (u64_t i = 0; i < 93; i++)
    {
        if (kernel_pages[i].id == id)
        {
            kernel_pages[i].id &= ~(0xF << 28);
            return 0;
        }
    }

    release_mutex(kmem_lock); // release mutex kernel memory lock.
    return 1;                 // 1 as not found.
}

volatile struct kmem_page_t *alloc_kframe(u32_t size)
{
    volatile struct kmem_page_t *output = NULL;

    const u32_t diffs[5] = {2048 - size, 8192 - size, 32768 - size, 131072 - size, 524288 - size};
    const u32_t sizes[5] = {2048, 8192, 32768, 131072, 524288};
    u32_t least_diff = 0;

    for (u64_t i = 0; i < 5; i++)
    {
        if (!least_diff)
            least_diff = i;
        else if (diffs[i] < diffs[least_diff])
            least_diff = i;
    }

    // gain mutex for kernel memory system.
    while (!gain_mutex(kmem_lock))
    {
        spinwait_mutex(kmem_lock);
    }

    for (u64_t i = 0; i < 93; i++)
    {

        if (!kernel_pages[i].id & (0xF << 28))
        {
            kernel_pages[i].id |= (size & 0xf) << 28; // only 4 bits.
            kernel_pages[i].size = sizes[size];       // set size.
            kernel_pages[i].start_address = 0;        // set start address. (test value)
            output = kernel_pages + i;                // return pointer.
            break;
        }
        output = 0; // set as full.
    }

    for (u64_t i = 0; i < 93; i++)
    {
        if (!kernel_frames[i].allocated && kernel_frames[i].size == size) // if same size and doesnt allocated
        {
            kernel_frames[i].allocated = 1; // set allocated.
            output->start_address = kernel_frames[i].start_address;
        }
    }
    if (!output->start_address) // if wasnt found any same size.
    {
        switch (least_diff)
        {
        case KMEM_SIZE_ORG_2KB:
            break;
        case KMEM_SIZE_ORG_8KB:
            u64_t compatible_frames = 0;
            for (u64_t i = 0; i < 93; i++)
            {
                if (compatible_frames == 4)
                {
                    output->start_address = kernel_frames[i - 4].start_address;
                    output->id |= (1 << 27); // enable multi-frame bit.
                    output->size = 8192;     // 8KB.
                }
                if (kernel_frames[i].size == KMEM_SIZE_ORG_2KB && !kernel_frames[i].allocated)
                    compatible_frames++; // increment compatible frames.
                else
                    compatible_frames = 0; // undo progress.
            }
            break;
        case KMEM_SIZE_ORG_32KB:
            u64_t compatible_frames = 0;
            for (u64_t i = 0; i < 93; i++)
            {
                if (compatible_frames == 4)
                {
                    output->start_address = kernel_frames[i - 4].start_address;
                    output->size = 8192; // 8KB.
                }
                if (kernel_frames[i].size == KMEM_SIZE_ORG_8KB && !kernel_frames[i].allocated)
                    compatible_frames++; // increment compatible frames.
                else
                    compatible_frames = 0; // undo progress.
            }
            break;
        case KMEM_SIZE_ORG_128KB:
            u64_t compatible_frames = 0;
            for (u64_t i = 0; i < 93; i++)
            {
                if (compatible_frames == 4)
                {
                    output->start_address = kernel_frames[i - 4].start_address;
                    output->size = 8192; // 8KB.
                }
                if (kernel_frames[i].size == KMEM_SIZE_ORG_32KB && !kernel_frames[i].allocated)
                    compatible_frames++; // increment compatible frames.
                else
                    compatible_frames = 0; // undo progress.
            }
            break;
        case KMEM_SIZE_ORG_512KB:
            u64_t compatible_frames = 0;
            for (u64_t i = 0; i < 93; i++)
            {
                if (compatible_frames == 4)
                {
                    output->start_address = kernel_frames[i - 4].start_address;
                    output->size = 8192; // 8KB.
                }
                if (kernel_frames[i].size == KMEM_SIZE_ORG_128KB && !kernel_frames[i].allocated)
                    compatible_frames++; // increment compatible frames.
                else
                    compatible_frames = 0; // undo progress.
            }
            break;
        }
        if (!output->start_address)

            for (u64_t i = 0; i < 93; i++)
            {
                if (!kernel_frames[i].allocated && kernel_frames[i].size >= size) // if same size and doesnt allocated
                {
                    kernel_frames[i].allocated = 1; // set allocated.
                    output->start_address = kernel_frames[i].start_address;
                }
            }
    }

    release_mutex(kmem_lock); // release mutex kernel memory lock.
    return output;
}

u64_t free_frame(u32_t id)
{
    for (u64_t i = 0; i < 93; i++)
    {
        if (kernel_pages[i].id == id)
        {
            kernel_pages[i].size = 0;          // clear size.
            kernel_pages[i].start_address = 0; // clear start address.
            kernel_pages[i].id &= 0xff;        // clear all flags.

            return 0; // done.
        }
    }
    return 1; // as error.
}

void clear_mem_incr(u64_t start_address, u64_t size)
{
    volatile u8_t *_byte_ = start_address;
    for (u64_t i = 0; i < size; i++)
    {
        *_byte_ = 0; // clear byte.
        _byte_++;    // increment pointer.
    }
}

void clear_mem_decr(u64_t start_address, u64_t size)
{
    volatile u8_t *_byte_ = start_address;
    for (u64_t i = 0; i < size; i++)
    {
        *_byte_ = 0; // clear byte.
        _byte_--;    // decrement pointer.
    }
}

void clear_frame(u64_t start_address, u8_t size)
{
    const u32_t sizes[5] = {2048, 8192, 32768, 131072, 524288};
    volatile u8_t *_byte_ = start_address;

    for (u64_t i = 0; i < sizes[size]; i++)
    {
        *_byte_ = 0; // clear byte.
        _byte_++;    // increment pointer.
    }
}

s64_t determine_frame_id(u64_t address)
{
    for (u64_t i = 0; i < 93; i++)
    {
        if (address >= kernel_pages[i].start_address && address <= kernel_pages[i].start_address + kernel_pages[i].size) // if it was in range of current page.
            return kernel_pages[i].id;
    }

    return -1; // -1 as not found.
}