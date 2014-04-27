#ifndef _SEARCH_H_
#define _SEARCH_H_

#include <gtk/gtk.h>
#include "gui.h"
#include "database.h"
#include "search-query.h"

typedef struct _search_t {
    
    //GtkWidget* search_bar;
    //GtkListStore* search_model;
    jld_gui_t* gui;
    database_t* db;
    
    gdouble entry_total, entry_count;
    GList* entries;
    GList* entry_index;
    
    search_query_t query;
    guint search_id;
    
} search_t;


void jld_search_init(search_t*, jld_gui_t*, database_t*);
void jld_search_destroy(search_t*);










#endif /* _SEARCH_H_ */
