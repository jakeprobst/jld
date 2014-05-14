#include "gui.h"
#include "entrytext.h"


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


gboolean _jld_gui_title_label_clicked(GtkEventBox* eventbox, GdkEventButton* event, jld_gui_t* gui)
{
    gtk_widget_show(gui->title_entry);
    gtk_widget_hide(gui->title_label);
    gtk_entry_set_text(GTK_ENTRY(gui->title_entry), gtk_label_get_text(GTK_LABEL(gui->title_label)));
    gtk_widget_grab_focus(gui->title_entry);
    return FALSE;
}

gboolean _jld_gui_title_entry_focus_out(GtkEntry* entry, GdkEventButton* event, jld_gui_t* gui)
{
    gtk_widget_show(gui->title_label);
    gtk_widget_hide(gui->title_entry);
    return FALSE;
}

void _jld_gui_connect_signals(jld_gui_t* gui)
{
    g_signal_connect(gui->window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect(gui->title_eventbox, "button-press-event", G_CALLBACK (_jld_gui_title_label_clicked), gui);
    g_signal_connect(gui->title_entry, "focus-out-event", G_CALLBACK (_jld_gui_title_entry_focus_out), gui);
}

void _jld_gui_add_accelerators(jld_gui_t* gui)
{
    
    
    
    
    
}

void _jld_gui_setup_models(jld_gui_t* gui)
{
    GtkCellRenderer* renderer;
    
    gui->calendar_model = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT64, G_TYPE_INT);
    gtk_tree_view_set_model(GTK_TREE_VIEW(gui->calendar_entry), GTK_TREE_MODEL(gui->calendar_model));
    gtk_tree_sortable_set_default_sort_func(GTK_TREE_SORTABLE(gui->calendar_model), _jld_gui_sort_tree_by_pos, NULL, NULL);
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(gui->calendar_model), GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID, GTK_SORT_DESCENDING);
    
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(gui->calendar_entry), -1, "Title", renderer, "text", COL_TITLE, NULL);
    
    gui->search_model = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
    gtk_tree_view_set_model(GTK_TREE_VIEW(gui->search_entry), GTK_TREE_MODEL(gui->search_model));
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(gui->search_entry), -1, "Date", renderer, "text", COL_DATE, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(gui->search_entry), -1, "Title", renderer, "text", COL_TITLE, NULL);

    gui->all_model = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
    gtk_tree_view_set_model(GTK_TREE_VIEW(gui->all_entry), GTK_TREE_MODEL(gui->all_model));
    gtk_tree_sortable_set_default_sort_func(GTK_TREE_SORTABLE(gui->all_model), _jld_gui_sort_tree_by_date, NULL, NULL);
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(gui->all_model), GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID, GTK_SORT_DESCENDING);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(gui->all_entry), -1, "Date", renderer, "text", COL_DATE, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(gui->all_entry), -1, "Title", renderer, "text", COL_TITLE, NULL);
}




void _jld_gui_create_widgets(jld_gui_t* gui)
{
    gui->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(gui->window), "jounallogdiary");
    gtk_window_set_default_size(GTK_WINDOW(gui->window), 1000, 600);
    gtk_window_add_accel_group(GTK_WINDOW(gui->window), gui->menu.accel_group);
    
    gui->notebook = GTK_NOTEBOOK(gtk_notebook_new());
    
    //calendar tab
    GtkBox* box1 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    
    gui->calendar = gtk_calendar_new();
    gtk_box_pack_start(box1, gui->calendar, FALSE, TRUE, 0);
    
    GtkWidget* scroll1 = gtk_scrolled_window_new(NULL, NULL);
    gui->calendar_entry = gtk_tree_view_new();
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(gui->calendar_entry), TRUE);
    gtk_container_add(GTK_CONTAINER(scroll1), GTK_WIDGET(gui->calendar_entry));
    gtk_box_pack_start(box1, scroll1, TRUE, TRUE, 0);
    
    gui->add_entry = gtk_button_new_from_icon_name("list-add", GTK_ICON_SIZE_MENU);
    gtk_box_pack_start(box1, GTK_WIDGET(gui->add_entry), FALSE, FALSE, 0);
    
    GtkWidget* label1 = gtk_label_new("Calendar");
    gtk_notebook_append_page(gui->notebook, GTK_WIDGET(box1), label1);
    
    // search tab
    GtkBox* box2 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    
    
    gui->search_bar = gtk_search_entry_new();
    gtk_box_pack_start(box2, gui->search_bar, FALSE, FALSE, 0);
    
    GtkWidget* scroll2 = gtk_scrolled_window_new(NULL, NULL);
    gui->search_entry = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(scroll2), GTK_WIDGET(gui->search_entry));
    gtk_box_pack_start(box2, scroll2, TRUE, TRUE, 0);
    
    GtkWidget* label2 = gtk_label_new("Search");
    gtk_notebook_append_page(gui->notebook, GTK_WIDGET(box2), label2);
    
    // all entries tab
    GtkBox* box3 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));

    GtkWidget* scroll3 = gtk_scrolled_window_new(NULL, NULL);
    gui->all_entry = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(scroll3), GTK_WIDGET(gui->all_entry));
    gtk_box_pack_start(box3, scroll3, TRUE, TRUE, 0);

    GtkWidget* label3 = gtk_label_new("All");
    gtk_notebook_append_page(gui->notebook, GTK_WIDGET(box3), label3);    
    
    
    // text entry 
    GtkBox* box4 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    
    GtkBox* box4_1 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    gui->date_label = gtk_label_new("");
    gtk_misc_set_alignment(GTK_MISC(gui->date_label),0,0);
    gui->title_label = gtk_label_new("");
    gtk_misc_set_alignment(GTK_MISC(gui->title_label),0,0);
    gui->title_entry = gtk_entry_new();
    gtk_box_pack_start(box4_1, gui->date_label, FALSE, FALSE, 0);
    gtk_box_pack_start(box4_1, gui->title_label, FALSE, FALSE, 0);
    gtk_box_pack_start(box4_1, gui->title_entry, TRUE, TRUE, 0);
    
    gui->title_eventbox = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(gui->title_eventbox), GTK_WIDGET(box4_1));
    
    gtk_box_pack_start(box4, gui->title_eventbox, FALSE, TRUE, 0);
    gtk_box_pack_start(box4, gui->entry_text.entry, TRUE, TRUE, 0);    
    
    // bundle it all up
    GtkBox* box5 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    gtk_box_pack_start(box5, GTK_WIDGET(box4), TRUE, TRUE, 0);
    gtk_box_pack_start(box5, GTK_WIDGET(gui->notebook), FALSE, FALSE, 0);
    
    GtkBox* box6 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_box_pack_start(box6, gui->menu.menu_bar, FALSE, FALSE, 0);
    gtk_box_pack_start(box6, GTK_WIDGET(box5), TRUE, TRUE, 0);
    
    gtk_container_add(GTK_CONTAINER(gui->window), GTK_WIDGET(box6));
    
    gtk_widget_show_all(gui->window);
    gtk_widget_hide(gui->title_entry);
}



void _jld_gui_create_context_menus(jld_gui_t* gui)
{
    gui->model_context_menu = gtk_menu_new();
    
    gui->model_menu_up = gtk_menu_item_new_with_label("Move Up");
    gtk_menu_shell_append(GTK_MENU_SHELL(gui->model_context_menu), gui->model_menu_up);
    
    gui->model_menu_down = gtk_menu_item_new_with_label("Move Down");
    gtk_menu_shell_append(GTK_MENU_SHELL(gui->model_context_menu), gui->model_menu_down);

    gtk_menu_shell_append(GTK_MENU_SHELL(gui->model_context_menu), gtk_separator_menu_item_new());
    
    gui->model_menu_delete = gtk_menu_item_new_with_label("Delete");
    gtk_menu_shell_append(GTK_MENU_SHELL(gui->model_context_menu), gui->model_menu_delete);
    
    gtk_widget_show_all(gui->model_context_menu);
}

void jld_gui_init(jld_gui_t* gui)
{
    jld_gui_menu_init(&gui->menu);
    jld_entry_text_init(&gui->entry_text, &gui->menu);
    _jld_gui_create_widgets(gui);
    _jld_gui_create_context_menus(gui);
    _jld_gui_setup_models(gui);
    _jld_gui_connect_signals(gui);
    _jld_gui_add_accelerators(gui);
}


void jld_gui_destroy(jld_gui_t* gui)
{
    jld_entry_text_destroy(&gui->entry_text);
}


void jld_gui_add_entry(jld_gui_t* gui, model_id_t id, entry_t* entry)
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
    g_string_sprintf(header, "<span font='15' gravity='west'>%s: </span>", date);
    gtk_label_set_markup(GTK_LABEL(gui->date_label), header->str);
    
    g_string_sprintf(header, "<span font='15' gravity='west'>%s</span>", title);
    gtk_label_set_markup(GTK_LABEL(gui->title_label), header->str);
    
    g_string_free(header, TRUE);
}



gchar* jld_gui_get_entry_text(jld_gui_t* gui)
{
    return jld_entry_text_get(&gui->entry_text);
}


void jld_gui_set_entry_text(jld_gui_t* gui, gchar* data)
{
    jld_entry_text_set(&gui->entry_text, data);
}

GtkTreeView* jld_gui_get_current_tree_view(jld_gui_t* gui)
{
    if (gtk_notebook_get_current_page(gui->notebook) == 0) {
        return GTK_TREE_VIEW(gui->calendar_entry);
    }
    if (gtk_notebook_get_current_page(gui->notebook) == 1) {
        return GTK_TREE_VIEW(gui->search_entry);
    }
    if (gtk_notebook_get_current_page(gui->notebook) == 2) {
        return GTK_TREE_VIEW(gui->all_entry);
    }
    return NULL;
}



