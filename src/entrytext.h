#ifndef _ENTRYTEXT_H_
#define _ENTRYTEXT_H_


#include <gtk/gtk.h>

typedef struct _entry_text_t {
    GtkTextBuffer* entry_buffer;
    GtkWidget* entry;
    
    /*GtkTextTag* tag_bold;
    GtkTextTag* tag_italic;
    GtkTextTag* tag_underline;
    GtkTextTag* tag_invisible;
    GtkTextTag* tag_big1;
    GtkTextTag* tag_big2;
    GtkTextTag* tag_big3;*/
    
    
    
} entry_text_t;



gchar* jld_entry_text_get(entry_text_t*);
void jld_entry_text_set(entry_text_t*, gchar*);


void jld_entry_text_init(entry_text_t*);
void jld_entry_text_destroy(entry_text_t*);



















#endif /* _ENTRYTEXT_H_ */
