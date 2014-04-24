#ifndef _ENTRY_H_
#define _ENTRY_H_

#include <glib.h>

typedef gint64 entry_id_t;


typedef struct _entry_t {
    entry_id_t id;
    int pos;
    int year, month, day;
    GString* date;
    GString* title;
    GString* tags;
    GString* file_path;
} entry_t;



void jld_entry_init(entry_t*);
void jld_entry_destroy(entry_t*);














#endif /* _ENTRY_H_ */
