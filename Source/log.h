#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct Record 
{ 
    char type;
    char var_name[507];
    char func_name[507];
    void* var_address;
    int is_out_dated;
}; 
struct Global_Log
{
    int next_slot;
    int log_full;//If it was filled
    struct Record records[1024];
};
void add_record(struct Global_Log* global_log, char* var_name, char* func_name, int var_size, int func_size, void* addr, char type);
void set_next_slot(struct Global_Log* global_log);
void set_var_name(struct Global_Log* global_log, char* var_name, int size);

void set_func_name(struct Global_Log* global_log, char* func_name, int size);
void set_var_address(struct Global_Log* global_log,  void* var_addr);
void set_type(struct Global_Log* global_log,char type);
void set_outdated(struct Global_Log* global_log, char* func_name, int func_length);
void update_value(struct Global_Log* global_log, void* new_value, char* var_name, char* func_name, int var_length, int func_length);
int check_value(struct Global_Log* global_log, void* current_value, char* var_name, char* func_name, int var_length, int func_length);
int search_outdated_record(struct Global_Log* global_log);
int is_string_equal(char* string1, char* string2, int string2_length);
void print_log(struct Global_Log global_log, int lines);



void add_record(struct Global_Log* global_log, char* var_name, char* func_name, int var_size, int func_size, void* addr, char type)
{
    set_var_name(global_log, var_name, var_size);
    set_func_name(global_log, func_name, func_size);
    set_var_address(global_log,  addr);
    set_type(global_log,type);
    global_log->records[global_log->next_slot].is_out_dated= 0;
}



//**********************************Setters***********************************************
void set_next_slot(struct Global_Log* global_log)
{
    if (global_log->next_slot == 1023 && global_log->log_full == 0)
    {
        global_log->log_full = 1;
        global_log->next_slot = search_outdated_record(global_log);
        if(global_log->next_slot == -1)
            exit(1);
    }
    else if(global_log->log_full == 1)
    {
        global_log->next_slot = search_outdated_record(global_log);
        if(global_log->next_slot == -1)
            exit(1);
        
    }
    else
    {
        global_log->next_slot++;
        
    }
    
}
void set_var_name(struct Global_Log* global_log, char* var_name, int size)
{
    for(int i = 0 ; i < size; i++)
        global_log->records[ global_log->next_slot].var_name[i] = var_name[i];
    
}

void set_func_name(struct Global_Log* global_log, char* func_name, int size)
{
    for(int i = 0 ; i < size; i++)
        global_log->records[global_log->next_slot].func_name[i] = func_name[i];
    
}

void set_var_address(struct Global_Log* global_log,  void* var_addr)
{
    global_log->records[global_log->next_slot].var_address= var_addr;
    

}

void set_type(struct Global_Log* global_log, char type)
{
    global_log->records[global_log->next_slot].type = type; 
    
}

void set_outdated(struct Global_Log* global_log, char* func_name, int func_length)
{
    int func_name_eq = 0;
    for(int i = 0 ;i < 1024; i++)
    {
        func_name_eq = is_string_equal(global_log->records[i].func_name,func_name,func_length);
        if (func_name_eq)
        {
            global_log->records[i].is_out_dated = 1;
            
        }

    }
    
}


//**********************************Other Functions****************************************
void update_value(struct Global_Log* global_log, void* new_value, char* var_name, char* func_name, int var_length, int func_length)
{   
    int is_func_name_eq = 0;
    int is_var_name_eq = 0;
    for(int i = 0; i<1024; i++)
    {
        is_var_name_eq = is_string_equal(global_log->records[i].var_name,var_name, var_length );
        is_func_name_eq = is_string_equal(global_log->records[i].func_name,func_name, func_length );
        if(is_func_name_eq == 1&&is_var_name_eq == 1)
        {
            global_log->records[i].var_address = new_value;
            return;
        }
    }
    
    
}

int check_value(struct Global_Log* global_log, void* current_value, char* var_name, char* func_name, int var_length, int func_length)
{
    for(int i = 0; i<1024; i++)
    {
        //printf("checking");
        if(global_log->records[i].is_out_dated == 0 &&
            is_string_equal(global_log->records[i].var_name,var_name, var_length )&&
            is_string_equal(global_log->records[i].func_name,func_name, func_length ))
        {
            if (global_log->records[i].var_address == current_value )
            {
                //printf("%p\n",current_value );
                //printf("%p\n", global_log->records[i].var_address);
                return 1;
            }
            else
            {
                //printf("%p\n",current_value );
                //printf("%p\n", global_log->records[i].var_address);
                return 0;
            }
            
            
        }
    }
    return 1;
}
int search_outdated_record(struct Global_Log* global_log)
{
    for(int i = 0; i < 1024; i++)
    {
        if(global_log->records[i].is_out_dated == 1)
            return i;
    }
    return -1;
}

int is_string_equal(char* string1, char* string2, int string2_length)
{
    
    for(int i = 0; i<string2_length; i++)
    {
        //printf("%c", string1[i]);
        //printf("%c ", string2[i]);
        if (string1[i]!=string2[i])
        {
            //printf("returned");
            return 0;
        }
        
    }
    for (int i = string2_length; i < 507; i++)
    {
        if (string1[i]!=0)
        {
            //printf("%c",string1[i]);
            return 0;
        }
        
    }
    return 1;
    
}

void print_log(struct Global_Log global_log, int lines)
{
    for(int i = 0; i<lines; i++ )
    {
        printf("This is the record # %d\n", i);
        printf("function it is in: %s\n", global_log.records[i].func_name);        
        printf("variable name is: %s\n", global_log.records[i].var_name);
        printf("addr: %p\n", global_log.records[i].var_address);
        printf("the type: %c\n", global_log.records[i].type);
        printf("is_out_dated%d\n\n", global_log.records[i].is_out_dated);

    }
}
