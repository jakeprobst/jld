#include "settings.h"
#include <stdio.h>

settings_t jld_settings = {0};



void _jld_init_xdg()
{
    jld_settings.config_path = g_string_new(g_get_user_config_dir());
    g_string_append(jld_settings.config_path, "jld/");
    jld_settings.data_path = g_string_new(g_get_user_data_dir());
    g_string_append(jld_settings.data_path, "jld/");
}


void _jld_create_config_files()
{
    if (!g_file_test(jld_settings.config_path->str, G_FILE_TEST_EXISTS)) {
        g_mkdir_with_parents(jld_settings.config_path->str, 0755);
    }
    if (!g_file_test(jld_settings.data_path->str, G_FILE_TEST_EXISTS)) {
        g_mkdir_with_parents(jld_settings.data_path->str, 0755);
    }
}


void _jld_settings_load()
{
    
}

void jld_settings_init()
{
    _jld_init_xdg();
    _jld_create_config_files();
    _jld_settings_load();
}




void jld_settings_save()
{
}
