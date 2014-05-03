#ifndef _JLD_GUI_H_
#define _JLD_GUI_H_

#include <gtk/gtk.h>

#include "entry.h"
#include "entrytext.h"
#include "gui-menu.h"

enum {
    COL_DATE,
    COL_TITLE,
    COL_ID,
    COL_POS
};

typedef enum _model_id_t {
    MODEL_CALENDAR,
    MODEL_SEARCH,
    MODEL_ALL
} model_id_t;



typedef struct _jld_gui_t {
    GtkWidget* window;
    
    GtkAccelGroup* accel_group;
    jld_gui_menu_t menu;
    
    GtkNotebook* notebook;
    
    GtkWidget* calendar;
    GtkWidget* calendar_entry;
    GtkWidget* add_entry;
    
    GtkWidget* search_bar;
    GtkWidget* search_entry;
    
    GtkWidget* all_entry;
    
    GtkWidget* date_label;
    GtkWidget* title_label;
    GtkWidget* title_eventbox;
    GtkWidget* title_entry;
    entry_text_t entry_text;
    
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


void jld_gui_add_entry(jld_gui_t*, model_id_t id, entry_t*);
void jld_gui_set_header(jld_gui_t*, gchar*, gchar*);

gchar* jld_gui_get_entry_text(jld_gui_t*);
void jld_gui_set_entry_text(jld_gui_t*, gchar*);

GtkTreeView* jld_gui_get_current_tree_view(jld_gui_t*);





#endif /* _JLD_GUI_H_ */
