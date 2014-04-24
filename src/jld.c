#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib.h>

#include "jld.h"
#include "settings.h"



void _jld_create_entry(GtkWidget* button, jld_t* jld)
{
    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(jld->gui.calendar), &year, &month, &day);
    month += 1;
    
    entry_t* entry = jld_database_create_entry(&jld->db, year, month, day);
  
    jld_gui_add_entry(&jld->gui, MODEL_CALENDAR, entry);
    jld_gui_add_entry(&jld->gui, MODEL_ALL, entry);
    gtk_calendar_mark_day(GTK_CALENDAR(jld->gui.calendar), day);
}

void _jld_save_entry(jld_t* jld)
{
    if (jld->current_entry == NULL)
        return;
    GtkTextIter start;
    GtkTextIter end;
    
    gtk_text_buffer_get_start_iter(jld->gui.entry_buffer, &start);
    gtk_text_buffer_get_end_iter(jld->gui.entry_buffer, &end);
    
    gchar* buf = gtk_text_buffer_get_text(jld->gui.entry_buffer, &start, &end, TRUE);    
    
    jld_database_write_entry_data(&jld->db, jld->current_entry, buf);
    g_free(buf);
}


void _jld_load_entries(jld_t* jld)
{
    GList* elist = jld_database_get_all_entries(&jld->db);
    if (elist == NULL)
        return;
    
    gtk_list_store_clear(GTK_LIST_STORE(jld->gui.all_model));
    
    GList* l;
    for(l = elist; l != NULL; l = l->next) {
        entry_t* entry = l->data;
        jld_gui_add_entry(&jld->gui, MODEL_ALL, entry);
    }
    
    /* to load today's stuff */
    g_signal_emit_by_name(jld->gui.calendar, "day-selected");
    g_signal_emit_by_name(jld->gui.calendar, "month-changed");
    
    g_list_free(elist);
}

void _jld_reload_entry(jld_t* jld, entry_t* entry)
{
    GtkTreeIter iter;
    GtkListStore* stores[] = {jld->gui.calendar_model, jld->gui.search_model, jld->gui.all_model, NULL};
    
    int i;
    for(i = 0; stores[i] != NULL; i++) {
        if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(stores[i]), &iter)) {
            continue;
        }
        do {
            entry_id_t id;
            gtk_tree_model_get(GTK_TREE_MODEL(stores[i]), &iter, COL_ID, &id, -1);
            if (id == entry->id) {
                gtk_list_store_set(stores[i], &iter, COL_TITLE, entry->title->str, -1);
                jld_gui_set_header(&jld->gui, entry->date->str, entry->title->str);
                break;
            }
        } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(stores[i]), &iter));
    }
}


void _jld_select_entry(GtkTreeSelection* sel, jld_t* jld)
{
    GtkTreeIter iter;
    GtkTreeModel* model;
    
    if (gtk_tree_selection_get_selected(sel, &model, &iter)) {
        gtk_widget_set_sensitive(jld->gui.entry, TRUE);
        
        _jld_save_entry(jld);
        
        entry_id_t eid;
        gtk_tree_model_get(model, &iter, COL_ID, &eid, -1);
        jld->current_entry = jld_database_get_entry(&jld->db, eid);
        
        jld_gui_set_header(&jld->gui, jld->current_entry->date->str, jld->current_entry->title->str);
        
        GString* data = jld_database_get_entry_data(&jld->db, jld->current_entry);
        
        gtk_text_buffer_set_text(jld->gui.entry_buffer, data->str, -1);
        g_string_free(data, TRUE);
    }
    else {
        /*gtk_widget_set_sensitive(jld->gui.entry, FALSE);
        gtk_text_buffer_set_text(jld->gui.entry_buffer, "", -1);
        jld_gui_set_header(&jld->gui, " ", " ");*/
    }
    
}

void _jld_change_date(GtkCalendar* cal, jld_t* jld)
{
    gtk_list_store_clear(jld->gui.calendar_model);
    
    guint year, month, day;
    gtk_calendar_get_date(cal, &year, &month, &day);
    GList* elist = jld_database_get_entry_by_date(&jld->db, year, month+1, day);
    if (elist == NULL)
        return;
    
    GList* l;
    for(l = elist; l != NULL; l = l->next) {
        entry_t* entry = l->data;
        jld_gui_add_entry(&jld->gui, MODEL_CALENDAR, entry);
    }
    
    g_list_free(elist);
}

void _jld_change_month(GtkCalendar* cal, jld_t* jld)
{
    gtk_calendar_clear_marks(cal);
    
    guint year, month, day;
    gtk_calendar_get_date(cal, &year, &month, &day);
    
    GList* elist = jld_database_get_entry_by_date(&jld->db, year, month+1, -1);
    if (elist == NULL)
        return;
    
    GList* l;
    for(l = elist; l != NULL; l = l->next) {
        entry_t* entry = l->data;
        
        gtk_calendar_mark_day(cal, entry->day);
    }
    
    g_list_free(elist);
}

void _jld_entry_title_edited(GtkCellRendererText* render, gchar* path, gchar* new_text, gpointer param)
{
    jld_t* jld = (jld_t*)param;
    
    gboolean name_collision = FALSE;
    GtkTreeIter iter;
    if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(jld->gui.all_model), &iter)) {
        return;
    }
    do {
        char* date;
        char* title;
        gtk_tree_model_get(GTK_TREE_MODEL(jld->gui.all_model), &iter, COL_DATE, &date, COL_TITLE, &title, -1);
        
        if (g_strcmp0(jld->current_entry->date->str, date) == 0 && g_strcmp0(title, new_text) == 0) {
            name_collision = TRUE;
        }
        
        g_free(date);
        g_free(title);
    } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(jld->gui.all_model), &iter) && !name_collision);
    
    if (name_collision) {
        return;
    }
    
    g_string_assign(jld->current_entry->title, new_text);
    
    _jld_save_entry(jld);
    _jld_reload_entry(jld, jld->current_entry);
}


void _jld_connect_signals(jld_t* jld)
{
    g_signal_connect(jld->gui.add_entry, "clicked", G_CALLBACK(_jld_create_entry), jld);
    g_signal_connect(jld->gui.calendar, "day-selected", G_CALLBACK(_jld_change_date), jld);
    g_signal_connect(jld->gui.calendar, "month-changed", G_CALLBACK(_jld_change_month), jld);
    g_signal_connect(gtk_tree_view_get_selection(GTK_TREE_VIEW(jld->gui.calendar_entry)), "changed", G_CALLBACK(_jld_select_entry), jld);
    g_signal_connect(gtk_tree_view_get_selection(GTK_TREE_VIEW(jld->gui.search_entry)), "changed", G_CALLBACK(_jld_select_entry), jld);
    g_signal_connect(gtk_tree_view_get_selection(GTK_TREE_VIEW(jld->gui.all_entry)), "changed", G_CALLBACK(_jld_select_entry), jld);
    
    GtkTreeViewColumn* col;
    GList* render_list;
    col = gtk_tree_view_get_column(GTK_TREE_VIEW(jld->gui.calendar_entry), 0); // first renderer is COL_TITLE
    render_list = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(col));
    g_signal_connect(render_list->data, "edited", G_CALLBACK(_jld_entry_title_edited), jld);
    g_list_free(render_list);
    col = gtk_tree_view_get_column(GTK_TREE_VIEW(jld->gui.search_entry), 1); // second renderer is COL_TITLE
    render_list = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(col));
    g_signal_connect(render_list->data, "edited", G_CALLBACK(_jld_entry_title_edited), jld);
    g_list_free(render_list);
    col = gtk_tree_view_get_column(GTK_TREE_VIEW(jld->gui.all_entry), 1); // second renderer is COL_TITLE
    render_list = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(col));
    g_signal_connect(render_list->data, "edited", G_CALLBACK(_jld_entry_title_edited), jld);
    g_list_free(render_list);
}



void jld_init(jld_t* jld)
{
    jld_database_init(&jld->db);
    jld_gui_init(&jld->gui);
    
    jld->current_entry = NULL;
    _jld_connect_signals(jld);
    _jld_load_entries(jld);
}



void jld_run(jld_t* jld)
{
    gtk_main();
}

void jld_destroy(jld_t* jld)
{
    jld_gui_destroy(&jld->gui);
    jld_database_destroy(&jld->db);
}





































































































































































