from __future__ import print_function
import sys
import os
from pycparser import parse_file, c_parser, c_generator,c_ast
import copy 
import subprocess
import re 
sys.path.extend(['.', '..'])

def preprocess_file(filename):
    fo = open(filename, "r")
    lines = list(fo)
    header = ""
    src = ""
    long_comment = False
    for i in lines:
        if(i[0] == '#'):
            header = header + i
            #print(header)
        else:
            for j in range(len(i)-1):
                
                if(long_comment == False):
                    if ((i[j]+i[j+1])!='//' and  (i[j]+i[j+1])!='/*' and i[j]!= '\r'):
                        src = src + i[j]
                    elif((i[j]+i[j+1])=='/*'):
                        long_comment = True
                    elif((i[j]+i[j+1])=='//'):
                        break
                else:
                    if ((i[j]+i[j+1])=='*/'):
                        long_comment = False
                        break
            if src != "":
                break
            
    preprocessed = subprocess.check_output("cpp "+filename, shell=True).splitlines()
    is_func_body = False
    for i in preprocessed:
        if(is_func_body == True):
            src = src + i.decode('ascii')+'\n'

        if src == i.decode('ascii'):
            is_func_body = True
    return header, src   



def convert_to_ast(src_string):
        parser = c_parser.CParser()
        ast = parser.parse(src_string)
        return ast



def global_log(ast_tree):
    global_log = c_ast.Decl(
        'global_log', [], [],[], 
        c_ast.TypeDecl('global_log',[], c_ast.Struct('Global_Log',None))
        ,None,None)
    temp_ls1 = []
    temp_ls1.append(global_log)
    temp_ls2 = []
    temp_ls2 = copy.deepcopy(ast_tree.ext)
    global_log = c_ast.FileAST(temp_ls1+temp_ls2)
    return global_log




def add_records(ast_tree,ext_index,bock_item_index,var_name, func_name, var_size, func_size,type):

    Cast = c_ast.Cast(c_ast.Typename(None, [],c_ast.PtrDecl(
        [],c_ast.TypeDecl(
            None, [], c_ast.IdentifierType(['void'])
        )
    )), c_ast.ID(var_name))

    new_node = c_ast.FuncCall(
        c_ast.ID('add_record'),
        c_ast.ExprList(
            [c_ast.UnaryOp('&',c_ast.ID('global_log')),
            c_ast.Constant('string','"'+var_name+'"'),
            c_ast.Constant('string','"'+func_name+'"'),
            c_ast.Constant('int',str(var_size)),
            c_ast.Constant('int',str(func_size)),
            Cast,
            c_ast.Constant('char', '\''+type+'\'')
            ]
        )
    )
    return merge_ast_tree(ast_tree,ext_index,bock_item_index, new_node, "insert_after")





def update_value(ast_tree,ext_index,bock_item_index,var_name, func_name, var_size, func_size):
    Cast = c_ast.Cast(c_ast.Typename(None, [],c_ast.PtrDecl(
        [],c_ast.TypeDecl(
            None, [], c_ast.IdentifierType(['void'])
        )
    )), c_ast.ID(var_name)
    )

    new_node = c_ast.FuncCall(
        c_ast.ID('update_value'),
        c_ast.ExprList(
            [c_ast.UnaryOp('&',c_ast.ID('global_log')),
            Cast,
            c_ast.Constant('string','"'+var_name+'"'),
            c_ast.Constant('string','"'+func_name+'"'),
            c_ast.Constant('int',str(var_size)),
            c_ast.Constant('int',str(func_size))]
        )
    )

    return merge_ast_tree(ast_tree,ext_index,bock_item_index, new_node, "insert_after")





def check_value(ast_tree,ext_index,bock_item_index,var_name, func_name):
    Cast = c_ast.Cast(c_ast.Typename(None, [],c_ast.PtrDecl(
        [],c_ast.TypeDecl(
            None, [], c_ast.IdentifierType(['void'])
        )
    )), c_ast.ID(var_name))

    func_call = c_ast.FuncCall(
        c_ast.ID('check_value'),
        c_ast.ExprList(
            [c_ast.UnaryOp('&',c_ast.ID('global_log')),
            Cast,
            c_ast.Constant('string','"'+var_name+'"'),
            c_ast.Constant('string','"'+func_name+'"'),
            c_ast.Constant('int',str(len(var_name))),
            c_ast.Constant('int',str(len(func_name)))
            ]
        )
    )

    new_node = c_ast.If(
        c_ast.BinaryOp('==',func_call,c_ast.Constant('int','0')),
        c_ast.Compound([
            c_ast.FuncCall(
            c_ast.ID('printf'),c_ast.ExprList([c_ast.Constant('string','"%s\\n"'),c_ast.Constant('string','"Attack Detected"')])
            ), 
            c_ast.FuncCall(
            c_ast.ID('exit'),c_ast.ExprList([c_ast.Constant('int','1')])
        )])
        ,
        None
    )
    return merge_ast_tree(ast_tree,ext_index,bock_item_index, new_node,"insert_before")



def set_outdated(ast_tree,ext_index,bock_item_index, func_name):
    new_node = c_ast.FuncCall(
        c_ast.ID('set_outdated'),
        c_ast.ExprList(
            [c_ast.UnaryOp('&',c_ast.ID('global_log')),
            c_ast.Constant('string','"'+func_name+'"'),
            c_ast.Constant('int',str(len(func_name)))
            ]
        )
    )
    return merge_ast_tree(ast_tree,ext_index,bock_item_index, new_node,"insert_before")










def merge_ast_tree(ast_tree,ext_index,bock_item_index, new_node, mode):
    
    ext_after = []
    block_items_after = []
    block_items_before = []
    ext_before = ast_tree.ext[:ext_index]
    if(len(ast_tree.ext)>1):
        ext_after = ast_tree.ext[ext_index+1:]

    func_def_decl = ast_tree.ext[ext_index].decl 
    para_decl = ast_tree.ext[ext_index].param_decls 
    if (bock_item_index+1 <= len(ast_tree.ext[ext_index].body.block_items)):
        if(mode == "insert_after"):
            block_items_before = ast_tree.ext[ext_index].body.block_items[:bock_item_index+1]
    if(mode == "insert_before"):
        block_items_before = ast_tree.ext[ext_index].body.block_items[:bock_item_index]

    if(len(ast_tree.ext[ext_index].body.block_items)>1):
        if(mode == "insert_after"):
            block_items_after = ast_tree.ext[ext_index].body.block_items[bock_item_index+1:]
    if(mode == "insert_before"):
        block_items_after = ast_tree.ext[ext_index].body.block_items[bock_item_index:]
                
    block_items_before.append(new_node)
    changed_body = c_ast.Compound(block_items_before+block_items_after)
    changed_ext = c_ast.FuncDef(func_def_decl,para_decl, changed_body )
    ext_before.append(changed_ext)
    ast_tree = c_ast.FileAST(ext_before + ext_after)
    return ast_tree






def on_func_ptr_decl(ast, func_name):
    global func_def_decl
    if(ast.ext!=None):
        for i in range(len(ast.ext)):
            if(type(ast.ext[i]) == c_ast.FuncDef and func_name == ast.ext[i].decl.name):
                func_name_len = len(func_name)
                func_block_items = ast.ext[i].body.block_items
                if(func_block_items!=None):
                    for j in range(len(func_block_items)):
                        if(type(func_block_items[j])==c_ast.Decl):
                            one_decl = func_block_items[j]
                            if(type(one_decl.type) == c_ast.PtrDecl):
                                one_ptr_decl = one_decl.type
                                if(type(one_ptr_decl.type) == c_ast.FuncDecl):
                                    one_func_ptr_decl = one_ptr_decl.type.type
                                    var_name = one_func_ptr_decl.declname
                                    var_name_len = len(var_name)
                                    func_def_decl.append(var_name)
                                    ast = add_records(ast,i, j, var_name, func_name,var_name_len, func_name_len, 'f' )
    return ast



def on_assign_ptr_value(ast,func_name):
    global func_def_decl
    if(ast.ext!=None):
        for i in range(len(ast.ext)):
            if(type(ast.ext[i]) == c_ast.FuncDef and func_name == ast.ext[i].decl.name):
                func_name_len = len(func_name)
                func_block_items = ast.ext[i].body.block_items
                if(func_block_items!=None):
                    for j in range(len(func_block_items)):
                        if(type(func_block_items[j])==c_ast.Assignment):
                            var_name = func_block_items[j].lvalue.name
                            var_name_len = len(var_name)
                            if(var_name in func_def_decl):
                                ast = update_value(ast,i, j, var_name, func_name,var_name_len, func_name_len )
    return ast 


def on_use_func_ptr(ast):
    if(ast.ext!=None):
        for i in range(len(ast.ext)):
                if(type(ast.ext[i]) == c_ast.FuncDef):
                    func_block_items = ast.ext[i].body.block_items
                    func_name = ast.ext[i].decl.name 
                    #print(func_name)
                    if(func_block_items!=None):
                        for j in range(len(func_block_items)):
                            if(type(func_block_items[j])==c_ast.FuncCall):
                                one_func_call = func_block_items[j]
                                if (type(one_func_call.name)==c_ast.UnaryOp):
                                    var_name = one_func_call.name.expr.name
                                    ast = check_value(ast,i, j, var_name, func_name)
                            
    return ast 

def on_func_return(ast,func_name):
    global func_def_decl
    func_def_decl = []
    if(ast.ext!=None):
        for i in range(len(ast.ext)):
            if(type(ast.ext[i]) == c_ast.FuncDef and func_name == ast.ext[i].decl.name):
                    func_block_items = ast.ext[i].body.block_items
                    if(func_block_items!=None):
                        for j in range(len(func_block_items)):
                            if(type(func_block_items[j])==c_ast.Return):
                                ast = set_outdated(ast,i, j, func_name)
                            
    return ast 






func_decl_ptr = []
header, src = preprocess_file(str(sys.argv[1]))
header = header + "#include \"log.h\"\n"
generator = c_generator.CGenerator()
ast = convert_to_ast(src)
#print(ast)

if(ast.ext!=None):
        for i in range(len(ast.ext)):
            if(type(ast.ext[i]) == c_ast.FuncDef):
                func_name = ast.ext[i].decl.name
                ast = on_func_ptr_decl(ast, func_name)
                ast = on_assign_ptr_value(ast, func_name)
                ast = on_func_return(ast, func_name)
ast = global_log(ast)
ast = on_use_func_ptr(ast)
c_code = generator.visit(ast)
#print(header)
f= open(str(sys.argv[1]),"w+")
f.write(header + c_code)
