#include "gcc-plugin.h"
#include "plugin-version.h"
//#include "tree-pass.h"
//#include "context.h"
#include "tree.h"
#include "log.h"
#include <stdio.h> 
#include <stdlib.h> 

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
int plugin_is_GPL_compatible;


// This is a callback function  (void* gcc_data, void* user_data)
void roll_back(void* event, void* my_data)
{
    system("cp .func_ptr_overflow_test.c func_ptr_overflow_test.c");
    system("rm  .func_ptr_overflow_test.c");


}

int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version/*, int argc ,struct plugin_argument *argv*/)
{
    if (!plugin_default_version_check(version, &gcc_version)) 
    {
        printf("incompatible gcc/plugin versions\n");
        return 1;
    }
    const char * const plugin_name = plugin_info->base_name;

    //scan the file to find return address
    //api: void register_callback (const char *plugin_name, int event, plugin_callback_func callback, void *user_data);
    //register_callback(plugin_name, PLUGIN_START_UNIT, finish_decl, &global_log);
    
    system("cp func_ptr_overflow_test.c .func_ptr_overflow_test.c");  
    system("python3.6 modify_ast.py func_ptr_overflow_test.c" ); 

    register_callback(plugin_name, PLUGIN_FINISH, roll_back, NULL);


    //printf("Hello world!\n");

    return 0;
} 
