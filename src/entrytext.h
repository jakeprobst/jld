#ifndef _ENTRYTEXT_H_
#define _ENTRYTEXT_H_


#include <gtk/gtk.h>

#include "gui-menu.h"

typedef struct _entry_text_t {
    GtkTextBuffer* entry_buffer;
    GtkWidget* entry;
    
    gboolean show_markup_cursor;
    gboolean show_markup;
    
    GList* undo_stack;
    GList* undo_pointer;
    
        
} entry_text_t;



gchar* jld_entry_text_get(entry_text_t*);
void jld_entry_text_set(entry_text_t*, gchar*);


void jld_entry_text_init(entry_text_t*, jld_gui_menu_t*);
void jld_entry_text_destroy(entry_text_t*);



















#endif /* _ENTRYTEXT_H_ */
