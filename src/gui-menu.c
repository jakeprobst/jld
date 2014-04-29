#include "gui-menu.h"







void jld_gui_menu_init(jld_gui_menu_t* menu)
{
    menu->menu_bar = gtk_menu_bar_new();
    
    GtkWidget* file_menu = gtk_menu_new();
    GtkWidget* file = gtk_menu_item_new_with_mnemonic("_File");
    menu->new_entry = gtk_menu_item_new_with_mnemonic("_New Entry");
    menu->save = gtk_menu_item_new_with_mnemonic("_Save");
    menu->quit = gtk_menu_item_new_with_mnemonic("_Quit");
    
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu->new_entry);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu->save);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), gtk_separator_menu_item_new());
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu->quit);
    
    
    
    
    
    
    gtk_menu_shell_append(GTK_MENU_SHELL(menu->menu_bar), file);
}
