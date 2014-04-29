#ifndef _GUI_MENU_H_
#define _GUI_MENU_H_

#include <gtk/gtk.h>


typedef struct _jld_gui_menu_t {
    GtkWidget* menu_bar;
    
    GtkWidget* new_entry;
    GtkWidget* save;
    GtkWidget* quit;
    
    GtkWidget* bold;
    GtkWidget* italic;
    GtkWidget* strike;
    
    GtkWidget* show_markup;
    
} jld_gui_menu_t;






void jld_gui_menu_init(jld_gui_menu_t*);











#endif /* _GUI_MENU_H_ */
