#include "entrytext.h"

#include <string.h>



// cursor position doesnt take into account utf8 encoding, so the point in the
// raw data ends up not matching the point in the widget.
int _normalize_length(int length, guchar* data)
{
    int modify = 0;
    int i;
    for(i = 0; i < length; i++) {
        if (data[i] > 0xF0) {
            modify += 3;
        }
        else if (data[i] > 0xE0) {
            modify += 2;
        }
        else if (data[i] > 0xC0) {
            modify += 1;
        }
    }
    return length-modify;
}


gchar* jld_entry_text_get(entry_text_t* entry_text)
{
    GtkTextIter start;
    GtkTextIter end;
    
    gtk_text_buffer_get_start_iter(entry_text->entry_buffer, &start);
    gtk_text_buffer_get_end_iter(entry_text->entry_buffer, &end);
    
    return gtk_text_buffer_get_text(entry_text->entry_buffer, &start, &end, TRUE);
}


void jld_entry_text_set(entry_text_t* entry_text, gchar* data)
{    
    gtk_text_buffer_set_text(entry_text->entry_buffer, data, -1);
}

void _jld_entry_text_changed(GtkTextBuffer* buffer, entry_text_t* entry_text)
{
    GtkTextIter iter_start;
    GtkTextIter iter_end;
    
    gtk_text_buffer_get_start_iter(entry_text->entry_buffer, &iter_start);
    gtk_text_buffer_get_end_iter(entry_text->entry_buffer, &iter_end);
    gtk_text_buffer_remove_all_tags(buffer, &iter_start, &iter_end);
    
    struct _markup {
        char* str;
        char* tag;
    } markup[] = {{"**", "bold"},
                  {"//", "italic"},
                  {"~~", "strike"},
                  {"++", "h3"},
                  {"==", "h2"},
                  {"^^", "h1"},
                  {NULL, NULL}};


    void _tag_text(gchar* data, int offset_begin, int offset_end) {
        int i;
        for(i = offset_begin; data[i] != '\0' && i < offset_end; i++) {
            int m;
            for(m = 0; markup[m].str != NULL; m++) {
                if (strncmp(markup[m].str, data+i, strlen(markup[m].str)) == 0) {
                    int sub_i;
                    for(sub_i = i+strlen(markup[m].str); data[sub_i] != '\0' && sub_i < offset_end; sub_i++) {
                        if (strncmp(markup[m].str, data+sub_i, strlen(markup[m].str)) == 0) {
                            break;
                        }
                    }
                    if (data[sub_i] == '\0') { // no ending tag
                        continue;
                    }
                    _tag_text(data, i+strlen(markup[m].str), sub_i);
                    
                    int start = _normalize_length(i, (guchar*)data);
                    int end = _normalize_length(sub_i, (guchar*)data);

                    gtk_text_buffer_get_iter_at_offset(entry_text->entry_buffer, &iter_start, start+strlen(markup[m].str));
                    gtk_text_buffer_get_iter_at_offset(entry_text->entry_buffer, &iter_end, end);
                    gtk_text_buffer_apply_tag_by_name(entry_text->entry_buffer, markup[m].tag, &iter_start, &iter_end);
                    
                    int cursor_pos;
                    g_object_get(entry_text->entry_buffer, "cursor-position", &cursor_pos, NULL);
                    
                    char* markup_tag = NULL;
                    if (entry_text->show_markup || (start <= cursor_pos && cursor_pos <= end+strlen(markup[m].str))) {
                        markup_tag = "grey-out";
                    }
                    else {
                        markup_tag = "invisible";
                    }
                    
                    gtk_text_buffer_get_iter_at_offset(entry_text->entry_buffer, &iter_start, start);
                    gtk_text_buffer_get_iter_at_offset(entry_text->entry_buffer, &iter_end, start+strlen(markup[m].str));
                    gtk_text_buffer_apply_tag_by_name(entry_text->entry_buffer, markup_tag, &iter_start, &iter_end);
                    
                    gtk_text_buffer_get_iter_at_offset(entry_text->entry_buffer, &iter_start, end);
                    gtk_text_buffer_get_iter_at_offset(entry_text->entry_buffer, &iter_end, end+strlen(markup[m].str));
                    gtk_text_buffer_apply_tag_by_name(entry_text->entry_buffer, markup_tag, &iter_start, &iter_end);
                    
                    i = sub_i;
                }
            }
        }
    }

    gchar* data = jld_entry_text_get(entry_text);
    _tag_text(data, 0, strlen(data));
    g_free(data);
}



void _jld_entry_text_cursor_moved(GtkTextBuffer* buffer, GParamSpec* spec, entry_text_t* entry)
{
    _jld_entry_text_changed(buffer, entry);
}

/*gboolean _jld_entry_text_in_style(entry_text_t* entry_text, char* markup, int end)
{
    gchar* data = jld_entry_text_get(entry_text);
    int count = 0;
    int i;
    for(i = 0; data[i] != '\0' && i < end; i++) {
        if (strncmp(markup,data+i,strlen(markup)) == 0) {
            count += 1;
        }
    }
    
    g_free(data);
    
    // if its even, it means all tags are matched so it is not currently in a tag
    if (count % 2 == 0) {
        return FALSE;
    }
    return TRUE;
}*/

int _jld_entry_text_contains_markup(gchar* data, char* markup)
{
    int i;
    for(i = 0; data[i] != '\0'; i++) {
        printf("'%s' '%s', %d\n", data+i, markup, strncmp(data+i, markup, strlen(markup)));
        if (strncmp(data+i, markup, strlen(markup)) == 0) {
            return i;
        }
    }
    return 0;
}

gboolean _jld_entry_text_index_in_markup(gchar* data, char* markup, int index)
{
    gboolean in_markup = FALSE;
    int i;
    for(i = 0; data[i] != '\0' && i < index; i++) {
        if (strncmp(data+i, markup, strlen(markup)) == 0) {
            in_markup = !in_markup;
        }
    }
    return in_markup;
}

void _jld_entry_text_apply_style_to_selection(entry_text_t* entry_text, char* markup)
{
    GtkTextIter start;
    GtkTextIter end;
    
    if (gtk_text_buffer_get_selection_bounds(entry_text->entry_buffer, &start, &end)) {
        gchar* all_text = jld_entry_text_get(entry_text);
        gchar* text = gtk_text_buffer_get_text(entry_text->entry_buffer, &start, &end, TRUE);
        
        int start_offset = gtk_text_iter_get_offset(&start);
        int end_offset = gtk_text_iter_get_offset(&end);
        
        gboolean start_in_markup = _jld_entry_text_index_in_markup(all_text, markup, start_offset);
        gboolean end_in_markup = _jld_entry_text_index_in_markup(all_text, markup, end_offset);
        
        if (strncmp(all_text+start_offset-strlen(markup), markup, strlen(markup)) == 0
            &&  strncmp(all_text+end_offset, markup, strlen(markup)) == 0) { // selection exactly surrounded by tags
                gtk_text_iter_set_offset(&start, start_offset-strlen(markup));
                gtk_text_iter_set_offset(&end, end_offset+strlen(markup));
                
                gtk_text_buffer_delete(entry_text->entry_buffer, &start, &end);
                                gtk_text_buffer_insert(entry_text->entry_buffer, &start, text, -1);
        }
        else if (start_in_markup && end_in_markup) {
            while (g_strrstr(text, markup) != NULL) {
                gchar* offset = g_strrstr(text, markup);
                memmove(offset, offset+strlen(markup), strlen(offset));
            }
            
            gtk_text_buffer_delete(entry_text->entry_buffer, &start, &end);
            
            int newlen = strlen(text)+strlen(markup)*2;
            gchar* newtext = g_malloc(newlen);
            sprintf(newtext, "%s%s%s", markup, text, markup);
        
            gtk_text_buffer_insert(entry_text->entry_buffer, &start, newtext, -1);
            g_free(newtext);
        }
        else if (start_in_markup && !end_in_markup) {
            while (g_strrstr(text, markup) != NULL) {
                gchar* offset = g_strrstr(text, markup);
                memmove(offset, offset+strlen(markup), strlen(offset));
            }
            
            gtk_text_buffer_delete(entry_text->entry_buffer, &start, &end);
            
            strncat(text, markup, strlen(markup));
            gtk_text_buffer_insert(entry_text->entry_buffer, &start, text, -1);
            
        }
        else if (!start_in_markup && end_in_markup) {
            while (g_strrstr(text, markup) != NULL) {
                gchar* offset = g_strrstr(text, markup);
                memmove(offset, offset+strlen(markup), strlen(offset));
            }
            
            gtk_text_buffer_delete(entry_text->entry_buffer, &start, &end);
            
            int newlen = strlen(text)+strlen(markup)+1;
            gchar* newtext = g_malloc(newlen);
            snprintf(newtext, newlen, "%s%s", markup, text);
            
            gtk_text_buffer_insert(entry_text->entry_buffer, &start, newtext, -1);
            g_free(newtext);
        }
        else if (!start_in_markup && !end_in_markup) {
            while (g_strrstr(text, markup) != NULL) {
                gchar* offset = g_strrstr(text, markup);
                memmove(offset, offset+strlen(markup), strlen(offset));
            }
            
            gtk_text_buffer_delete(entry_text->entry_buffer, &start, &end);
            
            int newlen = strlen(text)+strlen(markup)*2+1;
            gchar* newtext = g_malloc(newlen);
            snprintf(newtext, newlen, "%s%s%s", markup, text, markup);
            
            gtk_text_buffer_insert(entry_text->entry_buffer, &start, newtext, -1);
            g_free(newtext);
        }
        
        g_free(text);
    }
}


void _jld_entry_text_bold(GtkMenuItem* item, entry_text_t* entry_text)
{
    _jld_entry_text_apply_style_to_selection(entry_text, "**");
}
void _jld_entry_text_italic(GtkMenuItem* item, entry_text_t* entry_text)
{
    _jld_entry_text_apply_style_to_selection(entry_text, "//");
}
void _jld_entry_text_strike(GtkMenuItem* item, entry_text_t* entry_text)
{
    _jld_entry_text_apply_style_to_selection(entry_text, "~~");
}
void _jld_entry_text_header1(GtkMenuItem* item, entry_text_t* entry_text)
{
    _jld_entry_text_apply_style_to_selection(entry_text, "^^");
}
void _jld_entry_text_header2(GtkMenuItem* item, entry_text_t* entry_text)
{
    _jld_entry_text_apply_style_to_selection(entry_text, "==");
}

void _jld_entry_text_header3(GtkMenuItem* item, entry_text_t* entry_text)
{
    _jld_entry_text_apply_style_to_selection(entry_text, "++");
}
void _jld_entry_text_connect_signals(entry_text_t* entry_text, jld_gui_menu_t* menu)
{
    g_signal_connect(menu->bold, "activate", G_CALLBACK(_jld_entry_text_bold), entry_text);
    g_signal_connect(menu->italic, "activate", G_CALLBACK(_jld_entry_text_italic), entry_text);
    g_signal_connect(menu->strike, "activate", G_CALLBACK(_jld_entry_text_strike), entry_text);
    g_signal_connect(menu->header1, "activate", G_CALLBACK(_jld_entry_text_header1), entry_text);
    g_signal_connect(menu->header2, "activate", G_CALLBACK(_jld_entry_text_header2), entry_text);
    g_signal_connect(menu->header3, "activate", G_CALLBACK(_jld_entry_text_header3), entry_text);
    
}

void _jld_entry_text_add_accelerators(entry_text_t* entry_text, jld_gui_menu_t* menu)
{
    gtk_widget_add_accelerator(menu->bold, "activate", menu->accel_group,
                            GDK_KEY_b, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(menu->italic, "activate", menu->accel_group,
                            GDK_KEY_i, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(menu->strike, "activate", menu->accel_group,
                            GDK_KEY_minus, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(menu->header1, "activate", menu->accel_group,
                            GDK_KEY_1, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(menu->header2, "activate", menu->accel_group,
                            GDK_KEY_2, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(menu->header3, "activate", menu->accel_group,
                            GDK_KEY_3, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
}

void jld_entry_text_init(entry_text_t* entry_text, jld_gui_menu_t* menu)
{
    entry_text->show_markup = TRUE;
    
    
    entry_text->entry = gtk_text_view_new();
    entry_text->entry_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(entry_text->entry));
    gtk_widget_set_sensitive(entry_text->entry, FALSE);
    
    gtk_text_buffer_create_tag(entry_text->entry_buffer, "invisible", "invisible", TRUE, NULL);
    gtk_text_buffer_create_tag(entry_text->entry_buffer, "grey-out", "foreground", "grey", NULL);
    gtk_text_buffer_create_tag(entry_text->entry_buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_buffer_create_tag(entry_text->entry_buffer, "italic", "style", PANGO_STYLE_ITALIC, NULL);
    gtk_text_buffer_create_tag(entry_text->entry_buffer, "strike", "strikethrough", TRUE, NULL);
    gtk_text_buffer_create_tag(entry_text->entry_buffer, "h1", "scale", PANGO_SCALE_LARGE, NULL);
    gtk_text_buffer_create_tag(entry_text->entry_buffer, "h2", "scale", PANGO_SCALE_X_LARGE, NULL);
    gtk_text_buffer_create_tag(entry_text->entry_buffer, "h3", "scale", PANGO_SCALE_XX_LARGE, NULL);
    
    g_signal_connect(entry_text->entry_buffer, "changed", G_CALLBACK(_jld_entry_text_changed), entry_text);
    g_signal_connect(entry_text->entry_buffer, "notify::cursor-position", G_CALLBACK(_jld_entry_text_cursor_moved), entry_text);
    
    _jld_entry_text_connect_signals(entry_text, menu);
    _jld_entry_text_add_accelerators(entry_text, menu);
}

void jld_entry_text_destroy(entry_text_t* entry_text)
{
    
}




