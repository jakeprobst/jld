#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <glib.h>



typedef struct _settings_t {
    GString* config_path;
    GString* data_path;
    GString* entry_path;
    int win_x, win_y, win_w, win_h;
    
    
} settings_t;


extern settings_t jld_settings;



void jld_settings_init();



void jld_settings_save();









#endif /* _SETTINGS_H_ */
