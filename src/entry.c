#include "entry.h"







void jld_entry_init(entry_t* entry)
{
    entry->id = -1;
    entry->pos = -1;
    entry->year = -1;
    entry->month = -1;
    entry->day = -1;
    entry->date = g_string_new("");
    entry->title = g_string_new("");
    entry->tags = g_string_new("");
    entry->file_path = g_string_new("");
}



void jld_entry_destroy(entry_t* entry)
{
    entry->id = -1;
    entry->pos = -1;
    entry->year = -1;
    entry->month = -1;
    entry->day = -1;
    g_string_free(entry->date, TRUE);
    g_string_free(entry->title, TRUE);
    g_string_free(entry->tags, TRUE);
    g_string_free(entry->file_path, TRUE);
}
