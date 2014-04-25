#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "entry.h"


typedef struct _database_t {
    GString* entry_path;
    GTree* entries; // entry_id_t id -> entry_t entry
    entry_id_t id;
    
} database_t;




void jld_database_init(database_t*);
void jld_database_destroy(database_t*);

entry_t* jld_database_create_entry(database_t*, int,int,int);

entry_t* jld_database_get_entry(database_t*, entry_id_t);
GList* jld_database_get_entry_by_date(database_t*, int,int,int);
GList* jld_database_get_all_entries(database_t*);

int jld_database_count_entry_by_date(database_t*, int,int,int);
int jld_database_count_all_entries(database_t*);

GString* jld_database_get_entry_data(database_t*, entry_t*);

//only writes title and data information
void jld_database_write_entry_data(database_t*, entry_t*, char*);
void jld_database_delete_entry(database_t*, entry_t*);
















#endif /* _DATABASE_H_ */
