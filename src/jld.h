#ifndef _JLD_H_
#define _JLD_H_


#include "gui.h"
#include "database.h"

typedef struct _jld_t {
    jld_gui_t gui;
    database_t db;

    entry_t* current_entry;
} jld_t;










void jld_init(jld_t*);
void jld_run(jld_t*);
void jld_destroy(jld_t*);















#endif /* _JLD_H_ */
