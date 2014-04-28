#include "search-query.h"







void jld_search_query_init(search_query_t* query)
{
    query->text = g_string_new("");
    query->title = g_string_new("");
    query->all = g_string_new("");
    query->year = query->month = query->day = -1;
}

void jld_search_query_destroy(search_query_t* query)
{
    g_string_free(query->text, TRUE);
    g_string_free(query->title, TRUE);
    g_string_free(query->all, TRUE);
    query->year = query->month = query->day = -1;
}

void jld_search_query_parse(search_query_t* query, const char* str)
{
    g_string_assign(query->all, str);
}



gboolean jld_search_query_match(search_query_t* query, entry_t* entry, gchar* data)
{
    gboolean retval = TRUE;
    

    if (g_strrstr(data, query->all->str) == NULL) {
        retval = FALSE;
    }
    
    
    return retval;
}














