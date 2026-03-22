#include "./memory.h"

volatile struct memframe_t *alloc_page(u64_t task)
{
    const u64_t pages_count = (3.5 * (1073741824 /* 1 GB*/)) / (!memory_paging_settings->page_sizing ? 4096 : memory_paging_settings->page_sizing == 1 ? 16384
                                                                                                                                                       : 65536);

    for (u64_t i = 0; i < pages_count; i++)
    {
        if (!memory_frames[i].owner_task_id) // if wasnt allocated.
        {
            memory_frames[i].owner_task_id = task; // set owner task.
            return &memory_frames[i];              // return memory frame.
        }
    }
    return 0; // return 0 as error.
}

void free_page(volatile struct memframe_t *page)
{
    page->owner_task_id = 0;   // clear owner task.
    page->lower_attribute = 0; // clear lower attributes.
    page->upper_attribute = 0; // clear upper attributes.
    page->next_frame = NULL;   // clear pointer to next.
}