#include "search.h"

#define SEARCHES_PER_STEP 10










gboolean _jld_search_search(search_t* search)
{
    int i;
    for(i = 0; search->entry_index != NULL && i < SEARCHES_PER_STEP; search->entry_index = search->entry_index->next, i++) {
        entry_t* entry = (entry_t*)search->entry_index->data;
        GString* text = jld_database_get_entry_data(search->db, entry);
        
        if (g_strrstr(text->str, search->query.all->str) == NULL) {
            goto fail;
        }
        
        /*if (g_strrstr(entry->title->str, search->query.all->str) == NULL) {
            goto fail;
        }*/
        
        jld_gui_add_entry(search->gui, MODEL_SEARCH, entry);
        
        fail:
        g_string_free(text, TRUE);
        search->entry_count++;
    }
    
    gtk_entry_set_progress_fraction(GTK_ENTRY(search->gui->search_bar), search->entry_count/search->entry_total);
    if (search->entry_count == search->entry_total) {
        return FALSE;
    }
    return TRUE;
}

void _jld_search_finished(search_t* search)
{
    gtk_entry_set_progress_fraction(GTK_ENTRY(search->gui->search_bar), 0);
}

void _jld_search_activated(GtkSearchEntry* entry, search_t* search)
{
    if (search->search_id != 0) {
        g_source_remove(search->search_id);
        jld_search_query_destroy(&search->query);
        gtk_list_store_clear(search->gui->search_model);
    }
    
    search->entries = jld_database_get_all_entries(search->db);
    search->entry_index = search->entries;
    search->entry_total = jld_database_count_all_entries(search->db);
    search->entry_count = 0;
    jld_search_query_init(&search->query);
    jld_search_query_parse(&search->query, gtk_entry_get_text(GTK_ENTRY(entry)));
    
    search->search_id = g_timeout_add_full(G_PRIORITY_HIGH_IDLE, 100, (GSourceFunc)_jld_search_search, search, (GDestroyNotify)_jld_search_finished);
}


void jld_search_init(search_t* search, jld_gui_t* gui, database_t* db)
{
    search->gui = gui;
    search->db = db;
    search->search_id = 0;
    
    g_signal_connect(search->gui->search_bar, "activate", G_CALLBACK(_jld_search_activated), search);
    g_signal_connect(search->gui->search_bar, "search-changed", G_CALLBACK(_jld_search_activated), search);
}


void jld_search_destroy(search_t* search)
{
    
}
