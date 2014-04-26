#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "database.h"
#include "settings.h"





char* readline(FILE* fd)
{
    int len = 0;
    int start = ftell(fd);
    
    while(TRUE) {
        int c = fgetc(fd);
        if (c == '\n' || c == EOF) {
            break;
        }
        len++;
    }
    
    if (len == 0) {
        return NULL;
    }
    
    char* buf = g_malloc(len+1);
    fseek(fd, start, SEEK_SET);
    fread(buf, len, 1, fd);
    buf[len] = '\0';
    
    fgetc(fd); // get that linebreak out of there
        
    return buf;
}

gboolean split_key_value(char* str, char** key, char** value)
{
    char* split = strstr(str, ":");
    if (split == NULL) {
        return FALSE;
    }
    *key = str;
    *split = '\0';
    *value = split + 1;
    for(;*value[0] == ' ' || *value[0] == '\t'; *value += 1) {};
    
    return TRUE;
}

gint _jld_database_entry_tree_cmp(int* a, int* b)
{    
    if (*a > *b) {
        return 1;
    }
    if (*a < *b) {
        return -1;
    }
    return 0;
}

gboolean _jld_database_load_file(database_t* db, char* path)
{
    FILE* fd = fopen(path, "r");
    if (fd == NULL) {
        printf("panic\n");
        return FALSE;
    }
    
    entry_t* entry = g_malloc(sizeof(entry_t));
    jld_entry_init(entry);
    
    char *str, *key, *value;
    while((str = readline(fd)) != NULL) {
        split_key_value(str, &key, &value);
        
        if (g_strcmp0(key, "id") == 0) {
            entry->id = atoll(value);
            db->id = MAX(entry->id, db->id);
        }
        else if (g_strcmp0(key, "date") == 0) {
            g_string_assign(entry->date, value);
            sscanf(entry->date->str, "%d-%d-%d", &entry->year, &entry->month, &entry->day);
        }
        else if (g_strcmp0(key, "pos") == 0) {
            entry->pos = atoi(value);
        }
        else if (g_strcmp0(key, "title") == 0) {
            g_string_assign(entry->title, value);
        }
        
        g_free(str);
        
    }
    
    g_string_assign(entry->file_path, path);
    
    g_tree_insert(db->entries, &entry->id, entry);
    
    fclose(fd);
    
    return TRUE;
}

void _jld_database_load_entries(database_t* db)
{
    db->entries = g_tree_new((GCompareFunc)_jld_database_entry_tree_cmp);
    
    DIR* dir = opendir(db->entry_path->str);
    if (dir == NULL) {
        printf("no directory for some reason\n");
    }
    
    struct dirent* dp;
    while(TRUE) {
        dp = readdir(dir);
        if (dp == NULL) {
            break;
        }
        
        GString* fullpath = g_string_new(db->entry_path->str);
        g_string_append(fullpath, dp->d_name);
        _jld_database_load_file(db, fullpath->str);
        
        g_string_free(fullpath, TRUE);
    }
    closedir(dir);    
}


void jld_database_init(database_t* db)
{
    db->id = 0;
    db->entry_path = g_string_new(jld_settings.data_path->str);
    g_string_append(db->entry_path, "entries/");
    if (!g_file_test(db->entry_path->str, G_FILE_TEST_EXISTS)) {
        g_mkdir_with_parents(db->entry_path->str, 0755);
    }
    
    _jld_database_load_entries(db);
}



void jld_database_destroy(database_t* db)
{
    GList* entries = jld_database_get_all_entries(db);
    GList* l;
    for(l = entries; l != NULL; l = l->next) {
        jld_entry_destroy((entry_t*)l->data);
    }
    g_list_free(entries);
    g_tree_destroy(db->entries);
}


entry_t* jld_database_create_entry(database_t* db, int year, int month, int day)
{
    db->id++;
    
    entry_t* entry = g_malloc(sizeof(entry_t));
    jld_entry_init(entry);
    entry->id = db->id;
    entry->year = year;
    entry->month = month;
    entry->day = day;
    entry->pos = jld_database_count_entry_by_date(db, year, month, day);
    
    g_string_sprintf(entry->date, "%d-%.2d-%.2d", year, month, day);
    g_string_assign(entry->title, "new entry");
    g_string_sprintf(entry->file_path, "%s%s %s.txt", db->entry_path->str, entry->date->str, entry->title->str);
    int title_index = 1;
    while(g_file_test(entry->file_path->str, G_FILE_TEST_EXISTS)) {
        g_string_sprintf(entry->title, "new entry [%d]", title_index++);
        g_string_sprintf(entry->file_path, "%s%s %s.txt", db->entry_path->str, entry->date->str, entry->title->str);
    }
    
    
    g_tree_insert(db->entries, &entry->id, entry);
    
    
    FILE* fd = fopen(entry->file_path->str, "w");
    
    GString* output = g_string_new("");
    g_string_sprintf(output, "id: %ld\ndate: %s\npos: %d\ntitle: %s\n\n", entry->id, entry->date->str, entry->pos, entry->title->str);
    fwrite(output->str, output->len, 1, fd);
    
    g_string_free(output, TRUE);
    fclose(fd);
    
    return entry;
}


entry_t* jld_database_get_entry(database_t* db, entry_id_t id)
{
    return (entry_t*) g_tree_lookup(db->entries, &id);
}




GList* jld_database_get_entry_by_date(database_t* db, int year, int month, int day)
{
    GList* elist = NULL;
    
    gboolean __list_by_date(gpointer id, entry_t* entry, gpointer param)
    {       
        if (year != -1 && year != entry->year) {
            return FALSE;
        }
        if (month != -1 && month != entry->month) {
            return FALSE;
        }
        if (day != -1 && day != entry->day) {
            return FALSE;
        }
        elist = g_list_append(elist, entry);
        return FALSE;
    }

    g_tree_foreach(db->entries, (GTraverseFunc)__list_by_date, NULL);
    return elist;
}



int jld_database_count_entry_by_date(database_t* db, int year, int month, int day)
{
    int ecount = 0;
    
    gboolean __count_by_date(gpointer id, entry_t* entry, gpointer param)
    {       
        if (year != -1 && year != entry->year) {
            return FALSE;
        }
        if (month != -1 && month != entry->month) {
            return FALSE;
        }
        if (day != -1 && day != entry->day) {
            return FALSE;
        }
        ecount++;
        return FALSE;
    }

    g_tree_foreach(db->entries, (GTraverseFunc)__count_by_date, NULL);
    return ecount;
    
}

gboolean _jld_database_tree_to_list(gpointer id, gpointer entry, gpointer param)
{
    GList** list = (GList**)param;
    *list = g_list_append(*list, entry);
    
    return FALSE;
}

GList* jld_database_get_all_entries(database_t* db)
{
    GList* elist = NULL;
    g_tree_foreach(db->entries, _jld_database_tree_to_list, &elist);
    return elist;
}



GString* jld_database_get_entry_data(database_t* db, entry_t* entry)
{
    FILE* fd = fopen(entry->file_path->str, "r");
    
    while (TRUE) { // check for double line breaks
        int c = fgetc(fd);
        if (c == '\n') {
            c = fgetc(fd);
            if (c == '\n') {
                break;
            }
        }
    }
    
    int start = ftell(fd);
    fseek(fd, 0, SEEK_END);
    int end = ftell(fd);
    fseek(fd, start, SEEK_SET);

    char* buf = g_malloc(end-start+1);
    fread(buf, end-start, 1, fd);
    buf[end-start] = '\0';
        
    GString* data = g_string_new(buf);
    
    g_free(buf);
    fclose(fd);
    
    return data;
}

void jld_database_write_entry(database_t* db, entry_t* entry, char* data)
{
    GString* sdata = NULL;
    if (data == NULL) {
        sdata = jld_database_get_entry_data(db, entry);
        data = sdata->str;
    }
    
    remove(entry->file_path->str);
    g_string_sprintf(entry->file_path, "%s%s %s.txt", db->entry_path->str, entry->date->str, entry->title->str);
    
    FILE* fd = fopen(entry->file_path->str, "w");
    GString* output = g_string_new("");
    g_string_sprintf(output, "id: %ld\ndate: %s\npos: %d\ntitle: %s\n\n%s", entry->id, entry->date->str, entry->pos, entry->title->str, data);
    fwrite(output->str, output->len, 1, fd);
    
    g_string_free(output, TRUE);
    fclose(fd);
    if (sdata) {
        g_string_free(sdata, TRUE);
    }
}

void jld_database_delete_entry(database_t* db, entry_t* entry)
{
    remove(entry->file_path->str);
    g_tree_remove(db->entries, entry);
    
    GList* entries = jld_database_get_entry_by_date(db, entry->year, entry->month, entry->day);
    GList* l;
    for(l = entries; l != NULL; l = l->next) {
        entry_t* e = l->data;
        if (e->pos > entry->pos) {
            e->pos--;
            jld_database_write_entry(db, e, NULL);
        }
    }
    
    g_list_free(entries);
    
    jld_entry_destroy(entry);
    g_free(entry);
}







