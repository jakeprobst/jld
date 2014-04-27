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
                  {NULL, NULL}};

    
    gchar* data = jld_entry_text_get(entry_text);
    int i;
    for(i = 0; data[i] != '\0'; i++) {
        int m;
        for(m = 0; markup[m].str != NULL; m++) {
            if (strncmp(markup[m].str, data+i, strlen(markup[m].str)) == 0) {
                int sub_i;
                for(sub_i = i+strlen(markup[m].str); data[sub_i] != '\0'; sub_i++) {
                    if (strncmp(markup[m].str, data+sub_i, strlen(markup[m].str)) == 0) {
                        break;
                    }
                }
                if (data[sub_i] == '\0') { // no ending tag
                    continue;
                }
                int start = _normalize_length(i, (guchar*)data);
                int end = _normalize_length(sub_i, (guchar*)data);

                gtk_text_buffer_get_iter_at_offset(entry_text->entry_buffer, &iter_start, start+strlen(markup[m].str));
                gtk_text_buffer_get_iter_at_offset(entry_text->entry_buffer, &iter_end, end);
                gtk_text_buffer_apply_tag_by_name(entry_text->entry_buffer, markup[m].tag, &iter_start, &iter_end);
                
                int cursor_pos;
                g_object_get(entry_text->entry_buffer, "cursor-position", &cursor_pos, NULL);
                
                char* markup_tag = NULL;
                if (start <= cursor_pos && cursor_pos <= end+strlen(markup[m].str)) {
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
        
    g_free(data);
}



void _jld_entry_text_cursor_moved(GtkTextBuffer* buffer, GParamSpec* spec, entry_text_t* entry)
{
    _jld_entry_text_changed(buffer, entry);
}

void jld_entry_text_init(entry_text_t* entry_text)
{
    entry_text->entry = gtk_text_view_new();
    entry_text->entry_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(entry_text->entry));
    gtk_widget_set_sensitive(entry_text->entry, FALSE);
    
    gtk_text_buffer_create_tag(entry_text->entry_buffer, "invisible", "invisible", TRUE, NULL);
    gtk_text_buffer_create_tag(entry_text->entry_buffer, "grey-out", "foreground", "grey", NULL);
    gtk_text_buffer_create_tag(entry_text->entry_buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_buffer_create_tag(entry_text->entry_buffer, "italic", "style", PANGO_STYLE_ITALIC, NULL);
    gtk_text_buffer_create_tag(entry_text->entry_buffer, "strike", "strikethrough", TRUE, NULL);
    
    
    g_signal_connect(entry_text->entry_buffer, "changed", G_CALLBACK(_jld_entry_text_changed), entry_text);
    g_signal_connect(entry_text->entry_buffer, "notify::cursor-position", G_CALLBACK(_jld_entry_text_cursor_moved), entry_text);
}

void jld_entry_text_destroy(entry_text_t* entry_text)
{
    
}




