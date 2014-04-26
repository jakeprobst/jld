#include "gui.h"



gint _jld_gui_sort_tree_by_pos(GtkTreeModel* model, GtkTreeIter* a, GtkTreeIter* b, gpointer param)
{
    int apos;
    gtk_tree_model_get(model, a, COL_POS, &apos, -1);
    int bpos;
    gtk_tree_model_get(model, b, COL_POS, &bpos, -1);

    return bpos - apos;
}

gint _jld_gui_sort_tree_by_date(GtkTreeModel* model, GtkTreeIter* a, GtkTreeIter* b, gpointer param)
{
    char* adate;
    int apos;
    gtk_tree_model_get(model, a, COL_DATE, &adate, COL_POS, &apos, -1);
    
    char* bdate;
    int bpos;
    gtk_tree_model_get(model, b, COL_DATE, &bdate, COL_POS, &bpos, -1);
    
    gint ret = 0;
    if (g_strcmp0(adate, bdate) == 0) {
        ret = bpos - apos;
    }
    else {
        ret = g_strcmp0(adate, bdate);
    }
    
    g_free(adate);
    g_free(bdate);
    
    return ret;
}


void _jld_gui_connect_signals(jld_gui_t* gui)
{
    g_signal_connect(gui->window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
}

void _jld_gui_setup_models(jld_gui_t* gui)
{
    GtkCellRenderer* renderer;
    
    gui->calendar_model = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT64, G_TYPE_INT);
    gtk_tree_view_set_model(GTK_TREE_VIEW(gui->calendar_entry), GTK_TREE_MODEL(gui->calendar_model));
    gtk_tree_sortable_set_default_sort_func(GTK_TREE_SORTABLE(gui->calendar_model), _jld_gui_sort_tree_by_pos, NULL, NULL);
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(gui->calendar_model), GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID, GTK_SORT_DESCENDING);
    
    renderer = gtk_cell_renderer_text_new();
    g_object_set(G_OBJECT(renderer), "editable", TRUE, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(gui->calendar_entry), -1, "Title", renderer, "text", COL_TITLE, NULL);
    
    gui->search_model = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
    gtk_tree_view_set_model(GTK_TREE_VIEW(gui->search_entry), GTK_TREE_MODEL(gui->search_model));
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(gui->search_entry), -1, "Date", renderer, "text", COL_DATE, NULL);
    renderer = gtk_cell_renderer_text_new();
    g_object_set(G_OBJECT(renderer), "editable", TRUE, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(gui->search_entry), -1, "Title", renderer, "text", COL_TITLE, NULL);

    gui->all_model = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
    gtk_tree_view_set_model(GTK_TREE_VIEW(gui->all_entry), GTK_TREE_MODEL(gui->all_model));
    gtk_tree_sortable_set_default_sort_func(GTK_TREE_SORTABLE(gui->all_model), _jld_gui_sort_tree_by_date, NULL, NULL);
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(gui->all_model), GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID, GTK_SORT_DESCENDING);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(gui->all_entry), -1, "Date", renderer, "text", COL_DATE, NULL);
    renderer = gtk_cell_renderer_text_new();
    g_object_set(G_OBJECT(renderer), "editable", TRUE, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(gui->all_entry), -1, "Title", renderer, "text", COL_TITLE, NULL);
}


void _jld_gui_create_widgets(jld_gui_t* gui)
{
    gui->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(gui->window), "jounallogdiary");
    gtk_window_set_default_size(GTK_WINDOW(gui->window), 1000, 600);
    
    GtkNotebook* notebook = GTK_NOTEBOOK(gtk_notebook_new());
    
    //calendar tab
    GtkBox* box1 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    
    gui->calendar = gtk_calendar_new();
    gtk_box_pack_start(box1, gui->calendar, FALSE, TRUE, 0);
    
    GtkWidget* scroll1 = gtk_scrolled_window_new(NULL, NULL);
    gui->calendar_entry = gtk_tree_view_new();
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(gui->calendar_entry), TRUE);
    gtk_container_add(GTK_CONTAINER(scroll1), GTK_WIDGET(gui->calendar_entry));
    gtk_box_pack_start(box1, scroll1, TRUE, TRUE, 0);
    
    gui->add_entry = gtk_button_new_from_icon_name("list-add", 0);
    gtk_box_pack_start(box1, GTK_WIDGET(gui->add_entry), FALSE, FALSE, 0);
    
    GtkWidget* label1 = gtk_label_new("Calendar");
    gtk_notebook_append_page(notebook, GTK_WIDGET(box1), label1);
    
    // search tab
    GtkBox* box2 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    
    
    gui->search_bar = gtk_search_entry_new();
    gtk_box_pack_start(box2, gui->search_bar, FALSE, FALSE, 0);
    
    GtkWidget* scroll2 = gtk_scrolled_window_new(NULL, NULL);
    gui->search_entry = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(scroll2), GTK_WIDGET(gui->search_entry));
    gtk_box_pack_start(box2, scroll2, TRUE, TRUE, 0);
    
    GtkWidget* label2 = gtk_label_new("Search");
    gtk_notebook_append_page(notebook, GTK_WIDGET(box2), label2);
    
    // all entries tab
    GtkBox* box3 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));

    GtkWidget* scroll3 = gtk_scrolled_window_new(NULL, NULL);
    gui->all_entry = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(scroll3), GTK_WIDGET(gui->all_entry));
    gtk_box_pack_start(box3, scroll3, TRUE, TRUE, 0);

    GtkWidget* label3 = gtk_label_new("All");
    gtk_notebook_append_page(notebook, GTK_WIDGET(box3), label3);    
    
    
    // text entry 
    GtkBox* box4 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    
    gui->title_label = gtk_label_new("");
    gtk_misc_set_alignment(GTK_MISC(gui->title_label),0,0);
    
    gui->entry = gtk_text_view_new();
    gui->entry_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui->entry));
    gtk_box_pack_start(box4, gui->title_label, FALSE, TRUE, 0);
    gtk_box_pack_start(box4, gui->entry, TRUE, TRUE, 0);
    gtk_widget_set_sensitive(gui->entry, FALSE);
    
    
    // bundle it all up
    GtkBox* box5 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    gtk_box_pack_start(box5, GTK_WIDGET(box4), TRUE, TRUE, 0);
    gtk_box_pack_start(box5, GTK_WIDGET(notebook), FALSE, FALSE, 0);
    
    gtk_container_add(GTK_CONTAINER(gui->window), GTK_WIDGET(box5));
    
    gtk_widget_show_all(gui->window);
}

void _jld_gui_create_context_menus(jld_gui_t* gui)
{
    // calendar entry list popup menu
    gui->model_context_menu = gtk_menu_new();
    
    gui->model_menu_up = gtk_menu_item_new_with_label("Move Up");
    gtk_menu_shell_append(GTK_MENU_SHELL(gui->model_context_menu), gui->model_menu_up);
    
    gui->model_menu_down = gtk_menu_item_new_with_label("Move Down");
    gtk_menu_shell_append(GTK_MENU_SHELL(gui->model_context_menu), gui->model_menu_down);

    gtk_menu_shell_append(GTK_MENU_SHELL(gui->model_context_menu), gtk_separator_menu_item_new());
    
    gui->model_menu_delete = gtk_menu_item_new_with_label("Delete");
    gtk_menu_shell_append(GTK_MENU_SHELL(gui->model_context_menu), gui->model_menu_delete);
    
    gtk_widget_show_all(gui->model_context_menu);
    
    // other entry list popup menu
    /*gui->other_model_context_menu = gtk_menu_new();
    
    gtk_menu_shell_append(GTK_MENU_SHELL(gui->other_model_context_menu), gui->model_menu_delete);
    
    gtk_widget_show_all(gui->other_model_context_menu);*/
}

void jld_gui_init(jld_gui_t* gui)
{
    _jld_gui_create_widgets(gui);
    _jld_gui_create_context_menus(gui);
    _jld_gui_setup_models(gui);
    _jld_gui_connect_signals(gui);
    
}


void jld_gui_destroy(jld_gui_t* gui)
{

}


void jld_gui_add_entry(jld_gui_t* gui, model_id id, entry_t* entry)
{
    GtkTreeIter iter;
    GtkListStore* model;
    
    if (id == MODEL_CALENDAR) {
        model = gui->calendar_model;
    }
    else if (id == MODEL_SEARCH) {
        model = gui->search_model;
    }
    else if (MODEL_ALL) {
        model = gui->all_model;
    }
    else {
        return;
    }
    
    gtk_list_store_append(model, &iter);
    gtk_list_store_set(model, &iter, COL_DATE, entry->date->str, COL_TITLE, entry->title->str, COL_ID, entry->id, COL_POS, entry->pos, -1);
}



void jld_gui_set_header(jld_gui_t* gui, gchar* date, gchar* title)
{
    GString* header = g_string_new("");
    g_string_sprintf(header, "<span font='15' gravity='west'>%s: %s</span>", date, title);
    gtk_label_set_markup(GTK_LABEL(gui->title_label), header->str);
    g_string_free(header, TRUE);
}



gchar* jld_gui_get_entry_text(jld_gui_t* gui)
{
    GtkTextIter start;
    GtkTextIter end;
    
    gtk_text_buffer_get_start_iter(gui->entry_buffer, &start);
    gtk_text_buffer_get_end_iter(gui->entry_buffer, &end);
    
    return gtk_text_buffer_get_text(gui->entry_buffer, &start, &end, TRUE);
}


void jld_gui_set_entry_text(jld_gui_t* gui, gchar* data)
{
    gtk_text_buffer_set_text(gui->entry_buffer, data, -1);
}





