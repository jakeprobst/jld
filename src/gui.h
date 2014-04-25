#ifndef _JLD_GUI_H_
#define _JLD_GUI_H_

#include <gtk/gtk.h>

#include "entry.h"

enum {
    COL_DATE,
    COL_TITLE,
    COL_ID,
    COL_POS
};

typedef enum _model_id {
    MODEL_CALENDAR,
    MODEL_SEARCH,
    MODEL_ALL
} model_id;


typedef struct _jld_gui_t {
    GtkWidget* window;
    
    GtkWidget* calendar;
    GtkWidget* calendar_entry;
    GtkWidget* add_entry;
    
    GtkWidget* search_bar;
    GtkWidget* search_entry;
    
    GtkWidget* all_entry;
    
    GtkWidget* title_label;
    GtkTextBuffer* entry_buffer;
    GtkWidget* entry;
    
    GtkWidget* model_context_menu;
    GtkWidget* model_menu_up;
    GtkWidget* model_menu_down;
    GtkWidget* model_menu_delete;
    
    GtkListStore* calendar_model;
    GtkListStore* search_model;
    GtkListStore* all_model;
    
} jld_gui_t;






void jld_gui_init(jld_gui_t*);
void jld_gui_destroy(jld_gui_t*);


void jld_gui_add_entry(jld_gui_t*, model_id id, entry_t*);
void jld_gui_set_header(jld_gui_t*, gchar*, gchar*);









#endif /* _JLD_GUI_H_ */
