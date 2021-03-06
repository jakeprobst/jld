#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <glib.h>

#include "jld.h"
#include "settings.h"



void _jld_mark_calendar(jld_t* jld)
{
    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(jld->gui.calendar), &year, &month, &day);
    
    GList* elist = jld_database_get_entry_by_date(&jld->db, year, month+1, -1);
    if (elist == NULL)
        return;
    
    
    gtk_calendar_clear_marks(GTK_CALENDAR(jld->gui.calendar));
    GList* l;
    for(l = elist; l != NULL; l = l->next) {
        entry_t* entry = l->data;
        
        gtk_calendar_mark_day(GTK_CALENDAR(jld->gui.calendar), entry->day);
    }
    
    g_list_free(elist);
}

void _jld_save_entry(jld_t* jld)
{
    if (jld->current_entry == NULL)
        return;
    
    gchar* buf = jld_gui_get_entry_text(&jld->gui);
    
    jld_database_write_entry(&jld->db, jld->current_entry, buf);
    g_free(buf);
}

void _jld_save_entry_cb(GtkMenuItem* item, jld_t* jld)
{
    _jld_save_entry(jld);
}

void _jld_select_today(GtkMenuItem* item, jld_t* jld)
{
    time_t t = time(NULL);
    struct tm tim;
    localtime_r(&t, &tim);
    
    gtk_calendar_select_month(GTK_CALENDAR(jld->gui.calendar), tim.tm_mon, tim.tm_year+1900);
    gtk_calendar_select_day(GTK_CALENDAR(jld->gui.calendar), tim.tm_mday);
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
    if (entry == NULL) {
        return;
    }
    
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
                gtk_list_store_set(stores[i], &iter, COL_TITLE, entry->title->str, COL_POS, entry->pos, -1);
                jld_gui_set_header(&jld->gui, entry->date->str, entry->title->str);
                break;
            }
        } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(stores[i]), &iter));
    }
}

void _jld_delete_entry(jld_t* jld, entry_t* entry)
{
    GtkTreeIter iter;
    GtkListStore* stores[] = {jld->gui.calendar_model, jld->gui.search_model, jld->gui.all_model, NULL};
    
    int i;
    for(i = 0; stores[i] != NULL; i++) {
        if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(stores[i]), &iter)) {
            continue;
        }
        do {
            entry_id_t eid;
            int pos;
            gtk_tree_model_get(GTK_TREE_MODEL(stores[i]), &iter, COL_ID, &eid, COL_POS, &pos, -1);
            if (eid == entry->id) {
                gtk_list_store_remove(stores[i], &iter);
                break;
            }
        } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(stores[i]), &iter));
    }
    jld_database_delete_entry(&jld->db, entry);
    if (jld->current_entry == entry) {
        jld->current_entry = NULL;
        gtk_widget_set_sensitive(jld->gui.entry_text.entry, FALSE);
    }
    _jld_mark_calendar(jld);
}


void _jld_delete_entry_menu(GtkMenuItem* item, jld_t* jld)
{
    if (jld->context_menu_entry == NULL)
        return;
    
    _jld_delete_entry(jld, jld->context_menu_entry);
}

void _jld_delete_entry_selected(GtkMenuItem* item, jld_t* jld)
{    
    GtkTreeView* view = jld_gui_get_current_tree_view(&jld->gui);
    GtkTreeModel* model = gtk_tree_view_get_model(view);
    GtkTreeSelection* sel = gtk_tree_view_get_selection(view);

    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(sel, &model, &iter)) {
        entry_id_t eid;
        gtk_tree_model_get(model, &iter, COL_ID, &eid, -1);
        
        entry_t* entry = jld_database_get_entry(&jld->db, eid);
        
        _jld_delete_entry(jld, entry);
    }
}

void _jld_rename_entry_selected(GtkMenuItem* item, jld_t* jld)
{
    if (jld->current_entry == NULL) {
        return;
    }
    
    gtk_widget_show(jld->gui.title_entry);
    gtk_widget_hide(jld->gui.title_label);
    gtk_entry_set_text(GTK_ENTRY(jld->gui.title_entry), gtk_label_get_text(GTK_LABEL(jld->gui.title_label)));
    gtk_widget_grab_focus(jld->gui.title_entry);
}

void _jld_rename_entry_activate(GtkEntry* entry, jld_t* jld)
{
    const gchar* new_text = gtk_entry_get_text(entry);
    if (strlen(new_text) != 0) {
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
        
        if (!name_collision) {
            g_string_assign(jld->current_entry->title, new_text);
        
            _jld_save_entry(jld);
            _jld_reload_entry(jld, jld->current_entry);
        }
    }
    
    gtk_widget_show(jld->gui.title_label);
    gtk_widget_hide(jld->gui.title_entry);
}


void _jld_move_entry_up(GtkMenuItem* item, jld_t* jld)
{
    if (jld->context_menu_entry->pos == 0) {
        return;
    }
    GList* entries = jld_database_get_entry_by_date(&jld->db, jld->context_menu_entry->year, 
                                                    jld->context_menu_entry->month, 
                                                    jld->context_menu_entry->day);
    GList* l;
    for(l = entries; l != NULL; l = l->next) {
        entry_t* entry = l->data;
        if (entry->pos == jld->context_menu_entry->pos - 1) {
            entry->pos++;
            jld_database_write_entry(&jld->db, entry, NULL);
            _jld_reload_entry(jld, entry);
            break;
        }
    }

    jld->context_menu_entry->pos--;
    jld_database_write_entry(&jld->db, jld->context_menu_entry, NULL);
    _jld_reload_entry(jld, jld->context_menu_entry);

    g_list_free(entries);
}


void _jld_move_entry_down(GtkMenuItem* item, jld_t* jld)
{
    if (jld->context_menu_entry->pos == jld_database_count_entry_by_date(&jld->db, 
                                                    jld->context_menu_entry->year, 
                                                    jld->context_menu_entry->month, 
                                                    jld->context_menu_entry->day) - 1) {
        return;
    }
    
    GList* entries = jld_database_get_entry_by_date(&jld->db, jld->context_menu_entry->year, 
                                                    jld->context_menu_entry->month, 
                                                    jld->context_menu_entry->day);
    GList* l;
    for(l = entries; l != NULL; l = l->next) {
        entry_t* entry = l->data;
        if (entry->pos == jld->context_menu_entry->pos+1) {
            entry->pos--;
            jld_database_write_entry(&jld->db, entry, NULL);
            _jld_reload_entry(jld, entry);
            break;
        }
    }
    
    jld->context_menu_entry->pos++;
    jld_database_write_entry(&jld->db, jld->context_menu_entry, NULL);
    _jld_reload_entry(jld, jld->context_menu_entry);

    g_list_free(entries);
}


void _jld_select_entry(jld_t* jld, entry_t* entry)
{
    gtk_widget_set_sensitive(jld->gui.entry_text.entry, TRUE);
        
    _jld_save_entry(jld);
    jld->current_entry = entry;
    
    GString* data = jld_database_get_entry_data(&jld->db, jld->current_entry);
    
    jld_gui_set_header(&jld->gui, jld->current_entry->date->str, jld->current_entry->title->str);
    jld_gui_set_entry_text(&jld->gui, data->str);
        
    g_string_free(data, TRUE);
}

void _jld_create_entry(GtkWidget* button, jld_t* jld)
{
    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(jld->gui.calendar), &year, &month, &day);
    month += 1;
    
    entry_t* entry = jld_database_create_entry(&jld->db, year, month, day);
  
    jld_gui_add_entry(&jld->gui, MODEL_CALENDAR, entry);
    jld_gui_add_entry(&jld->gui, MODEL_ALL, entry);
    gtk_calendar_mark_day(GTK_CALENDAR(jld->gui.calendar), day);
    
    GtkTreeSelection* sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(jld->gui.calendar_entry));
    GtkTreeIter iter;
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(jld->gui.calendar_model), &iter);
    int n = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(jld->gui.calendar_model), NULL);
    gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(jld->gui.calendar_model), &iter, NULL, n-1);
    gtk_tree_selection_select_iter(sel, &iter);
    _jld_select_entry(jld, entry);
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
    
    _jld_mark_calendar(jld);
}

gboolean _jld_model_clicked(GtkWidget* treeview, GdkEventButton* event, jld_t* jld)
{
    if (event->type == GDK_BUTTON_PRESS && event->button == 3) { // right clicked, do context menu
        gtk_menu_popup(GTK_MENU(jld->gui.model_context_menu), NULL, NULL, NULL, NULL,  
                       (event != NULL) ? event->button : 0, gdk_event_get_time((GdkEvent*)event));
            
        GtkTreePath* path;
        if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), event->x, event->y, &path, NULL, NULL, NULL)) {
            
            GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
            GtkTreeIter iter;
            gtk_tree_model_get_iter(model, &iter, path);
            
            entry_id_t eid;
            gtk_tree_model_get(model, &iter, COL_ID, &eid, -1);
            
            entry_t* entry = jld_database_get_entry(&jld->db, eid);
            
            jld->context_menu_entry = entry;
        }
        else {
            jld->context_menu_entry = NULL;
        }
        return TRUE;
    }
    if (event->type == GDK_BUTTON_PRESS && event->button == 1) { // left clicked, select entry to show
        GtkTreePath* path;
        if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), event->x, event->y, &path, NULL, NULL, NULL)) {
            
            GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
            GtkTreeIter iter;
            gtk_tree_model_get_iter(model, &iter, path);
            
            GtkTreeSelection* sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
            gtk_tree_selection_select_iter(sel, &iter);
            
            entry_id_t eid;
            gtk_tree_model_get(model, &iter, COL_ID, &eid, -1);
            entry_t* entry = jld_database_get_entry(&jld->db, eid);
            _jld_select_entry(jld, entry);
        }
        return TRUE;
    }

    return FALSE;
}

gboolean _jld_quit(GtkWidget* w, GdkEvent* event, jld_t* jld)
{
    _jld_save_entry(jld);
    return FALSE;
}

void _jld_quit_menu(GtkMenuItem* item, jld_t* jld)
{
    _jld_quit(NULL, NULL, jld);
}


void _jld_connect_signals(jld_t* jld)
{
    g_signal_connect(jld->gui.window, "delete-event", G_CALLBACK (_jld_quit), jld);
    g_signal_connect(jld->gui.add_entry, "clicked", G_CALLBACK(_jld_create_entry), jld);
    g_signal_connect(jld->gui.calendar, "day-selected", G_CALLBACK(_jld_change_date), jld);
    g_signal_connect(jld->gui.calendar, "month-changed", G_CALLBACK(_jld_change_month), jld);
    g_signal_connect(jld->gui.calendar_entry, "button-press-event", G_CALLBACK(_jld_model_clicked), jld);
    g_signal_connect(jld->gui.search_entry, "button-press-event", G_CALLBACK(_jld_model_clicked), jld);
    g_signal_connect(jld->gui.all_entry, "button-press-event", G_CALLBACK(_jld_model_clicked), jld);
    
    g_signal_connect(jld->gui.title_entry, "activate", G_CALLBACK(_jld_rename_entry_activate), jld);
    
    g_signal_connect(jld->gui.model_menu_delete, "activate", G_CALLBACK(_jld_delete_entry_menu), jld);
    g_signal_connect(jld->gui.model_menu_up, "activate", G_CALLBACK(_jld_move_entry_up), jld);
    g_signal_connect(jld->gui.model_menu_down, "activate", G_CALLBACK(_jld_move_entry_down), jld);
    
    g_signal_connect(jld->gui.menu.new_entry, "activate", G_CALLBACK(_jld_create_entry), jld);
    g_signal_connect(jld->gui.menu.delete_entry, "activate", G_CALLBACK(_jld_delete_entry_selected), jld);
    g_signal_connect(jld->gui.menu.rename_entry, "activate", G_CALLBACK(_jld_rename_entry_selected), jld);
    g_signal_connect(jld->gui.menu.today, "activate", G_CALLBACK(_jld_select_today), jld);
    g_signal_connect(jld->gui.menu.save, "activate", G_CALLBACK(_jld_save_entry_cb), jld);
    g_signal_connect(jld->gui.menu.quit, "activate", G_CALLBACK(_jld_quit_menu), jld);
}


void _jld_add_accelerators(jld_t* jld)
{
    gtk_widget_add_accelerator(jld->gui.menu.new_entry, "activate", jld->gui.menu.accel_group, 
                               GDK_KEY_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(jld->gui.menu.delete_entry, "activate", jld->gui.menu.accel_group, 
                               GDK_KEY_Delete, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(jld->gui.menu.rename_entry, "activate", jld->gui.menu.accel_group, 
                               GDK_KEY_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(jld->gui.menu.today, "activate", jld->gui.menu.accel_group, 
                               GDK_KEY_t, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(jld->gui.menu.save, "activate", jld->gui.menu.accel_group, 
                               GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(jld->gui.menu.quit, "activate", jld->gui.menu.accel_group, 
                               GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
}

void jld_init(jld_t* jld)
{
    jld_database_init(&jld->db);
    jld_gui_init(&jld->gui);
    jld_search_init(&jld->search, &jld->gui, &jld->db);
    
    jld->current_entry = NULL;
    _jld_connect_signals(jld);
    _jld_add_accelerators(jld);
    _jld_load_entries(jld);
}

void jld_run(jld_t* jld)
{
    gtk_main();
}

void jld_destroy(jld_t* jld)
{
    jld_search_destroy(&jld->search);
    jld_gui_destroy(&jld->gui);
    jld_database_destroy(&jld->db);
}





