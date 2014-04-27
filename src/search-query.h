#ifndef _SEARCH_QUERY_H_
#define _SEARCH_QUERY_H_


#include <glib.h>


typedef struct _search_query_t {
    GString* title;
    GString* text;
    int year, month, day;
    
    GString* all;
} search_query_t;





void jld_search_query_parse(search_query_t*, const char*);





void jld_search_query_init(search_query_t*);
void jld_search_query_destroy(search_query_t*);


















#endif /* _SEARCH_QUERY_H_ */
