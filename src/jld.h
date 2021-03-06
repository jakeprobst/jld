#ifndef _JLD_H_
#define _JLD_H_


#include "gui.h"
#include "database.h"
#include "search.h"

typedef struct _jld_t {
    jld_gui_t gui;
    database_t db;
    search_t search;

    entry_t* current_entry;
    entry_t* context_menu_entry; // for passing to move and delete
    
} jld_t;










void jld_init(jld_t*);
void jld_run(jld_t*);
void jld_destroy(jld_t*);















#endif /* _JLD_H_ */
