plugin_func_ptr:
	@python gen_fun_overflow.py
	@g++ -I '/usr/lib/gcc/x86_64-linux-gnu/7/plugin/include' -std=c++11 -Wall -fno-rtti -Wno-literal-suffix -fPIC -c -o ast_plugin.o  ast_plugin.cc;
	@ld -dynamiclib -pie -fPIC -shared -o ast_plugin.so ast_plugin.o;
	@gcc -g -fplugin=./ast_plugin.so func_ptr_overflow_test.c -o func_ptr_overflow_test.o -z execstack -fno-stack-protector;
plugin_ret_addr:
	@g++ -I '/usr/lib/gcc/x86_64-linux-gnu/7/plugin/include' -std=c++11 -Wall -fno-rtti -Wno-literal-suffix -fPIC -c -o plugin_rtl.o  plugin_rtl.cc;
	@ld -dynamiclib -pie -fPIC -shared -o plugin_rtl.so plugin_rtl.o;
	@gcc -g -fplugin=./plugin_rtl.so vuln.c -o vuln.o -z execstack -fno-stack-protector;

func_ptr:
	@python gen_fun_overflow.py 
	@gcc -g func_ptr_overflow_test.c -o func_ptr_overflow_test.o -z execstack -fno-stack-protector;
ret_addr:
	@gcc -g vuln.c -o vuln.o -z execstack -fno-stack-protector;

