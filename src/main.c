#include <stdlib.h>
#include "jld.h"
#include "settings.h"







int main(int argc, char** argv)
{
    jld_settings_init();
    gtk_init(&argc, &argv);
    
    jld_t jld;
    jld_init(&jld);
    jld_run(&jld);
    jld_destroy(&jld);
    return 0;
}
