#include "gui-menu.h"







void jld_gui_menu_init(jld_gui_menu_t* menu)
{
    menu->accel_group = gtk_accel_group_new();
    
    menu->menu_bar = gtk_menu_bar_new();
    
    GtkWidget* file_menu = gtk_menu_new();
    GtkWidget* file = gtk_menu_item_new_with_mnemonic("_File");
    menu->new_entry = gtk_menu_item_new_with_mnemonic("_New Entry");
    menu->delete_entry = gtk_menu_item_new_with_mnemonic("_Delete Entry");
    menu->rename_entry = gtk_menu_item_new_with_mnemonic("_Rename Entry");
    menu->save = gtk_menu_item_new_with_mnemonic("_Save");
    menu->quit = gtk_menu_item_new_with_mnemonic("_Quit");
    
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu->new_entry);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu->delete_entry);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu->rename_entry);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu->save);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), gtk_separator_menu_item_new());
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu->quit);
    
    GtkWidget* format_menu = gtk_menu_new();
    GtkWidget* format = gtk_menu_item_new_with_mnemonic("Format");
    menu->bold = gtk_menu_item_new_with_mnemonic("_Bold");
    menu->italic = gtk_menu_item_new_with_mnemonic("_Italic");
    menu->strike = gtk_menu_item_new_with_mnemonic("_Strike");
    menu->header1 = gtk_menu_item_new_with_mnemonic("Header _1");
    menu->header2 = gtk_menu_item_new_with_mnemonic("Header _2");
    menu->header3 = gtk_menu_item_new_with_mnemonic("Header _3");
    menu->show_markup_cursor = gtk_check_menu_item_new_with_label("Show Markup Under Cursor");
    menu->show_markup = gtk_check_menu_item_new_with_label("Show Markup");
    
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(format), format_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), menu->bold);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), menu->italic);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), menu->strike);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), menu->header1);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), menu->header2);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), menu->header3);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), gtk_separator_menu_item_new());
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), menu->show_markup_cursor);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), menu->show_markup);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(menu->menu_bar), file);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu->menu_bar), format);
}
