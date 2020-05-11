#include <stdio.h>
#include <gcc-plugin.h>
#include <coretypes.h>
#include "gcc-common.h"
//#include "gcc-common.h"
//#include <plugin-version.h>
#include <context.h>
#include <tree.h>
#include <tree-pass.h>
#include <tree-dump.h>
#include <gimple.h>
#include <rtl.h>
#include <memmodel.h>
#include <emit-rtl.h>

#define P(...) {printf("[+] " __VA_ARGS__); printf("\n");}
#ifdef DEBUG
#define D(...) P(__VA_ARGS__)
#else
#define D(...) /* Nothing */
#endif


int plugin_is_GPL_compatible;
void * _Unwind_Resume =0;
void * ret_addr = NULL;


static bool ra_gate(void)
{
    return true;
}

static void setup_basic(rtx insn);
static void finish_basic(rtx insn);
static void setup_main(rtx insn);
static void finish_main(rtx insn);

typedef void(* rad)(rtx);
struct {rad setup; rad finish;} rads[]=
{
    {setup_main,  finish_main},
    {setup_basic, finish_basic},
};



static void setup_main(rtx insn)
{
    /* 	
	subq	$8192, %rsp					# void* ret_addr[1024]
	leaq	8184(%rsp), %r10			# use register %r10 to store the next available slot which is ret_addr[0], this is a rarely used register 	
	pushq	%rax						# save %rax value
	movq	8(%rbp), %rax				# the return address of function after prologue is stored at %rax temporarily bc you can't do 
										# mem to mem assignment
	movq	%rax, (%r10)				# assign next_slot the return address, which is the memory location where %r15 points to
	addq	$8, %r10					# now next_slot is ret_addrr[1]
	popq	%rax						# restore the value of %rax
    */

    P("In main() prologue");
    rtx rbp, rax, r10, val, sub, add, lea;
    rtx dec, mem, psh, inc, mov, pop;

    rax = gen_rtx_REG(DImode, AX_REG);
    r10 = gen_rtx_REG(DImode, R10_REG);
    rbp = gen_rtx_REG(DImode, BP_REG);

    /* subq %rsp, 8192*/
    val = gen_rtx_PLUS(Pmode, stack_pointer_rtx, GEN_INT(-8192));
    sub = gen_rtx_SET(stack_pointer_rtx, val);
    emit_insn_before(sub, insn);
    
    /* leaq	8184(%rsp), %r10 */
    add = gen_rtx_PLUS(Pmode, stack_pointer_rtx, GEN_INT(8184));
    lea = gen_rtx_SET(r10, add);
    emit_insn_before(lea, insn);




    /* pushq %rax */
    dec = gen_rtx_PRE_DEC(DImode, stack_pointer_rtx);
    mem = gen_rtx_MEM(DImode, dec);
    psh = gen_rtx_SET(mem, rax);
    emit_insn_before(psh, insn);



    /* movq	8(%rbp), %rax */
    inc = gen_rtx_PLUS(Pmode, rbp, GEN_INT(8));
    mem = gen_rtx_MEM(DImode, inc);
    mov = gen_rtx_SET(rax, mem);
    emit_insn_before(mov, insn);

    /* movq	%rax, (%r10) */
    mem = gen_rtx_MEM(DImode, r10);
    mov = gen_rtx_SET(mem, rax);
    emit_insn_before(mov, insn);

    /* subq	$8, %r10 */
    val = gen_rtx_PLUS(Pmode, r10, GEN_INT(-8));
    add = gen_rtx_SET(r10, val);
    emit_insn_before(add, insn);





    /* movq	(%rbp), %rax */
    mem = gen_rtx_MEM(DImode, rbp);
    mov = gen_rtx_SET(rax, mem);
    emit_insn_before(mov, insn);

    /* movq	%rax, (%r10) */
    mem = gen_rtx_MEM(DImode, r10);
    mov = gen_rtx_SET(mem, rax);
    emit_insn_before(mov, insn);

    /* subq	$8, %r10 */
    val = gen_rtx_PLUS(Pmode, r10, GEN_INT(-8));
    add = gen_rtx_SET(r10, val);
    emit_insn_before(add, insn);


    /* popq	%rax */
    inc = gen_rtx_POST_INC(DImode, stack_pointer_rtx);
    mem = gen_rtx_MEM(DImode, inc);
    pop = gen_rtx_SET(rax, mem);
    emit_insn_before(pop, insn);
  
}

static void finish_main(rtx insn)
{
    P("In main() epilogue");

    /* --------------------------restore %rsp---------------------------- */

    /*
    	addq	$8192, %rsp  
    */

    rtx val, add;

    val = gen_rtx_PLUS(Pmode, stack_pointer_rtx, GEN_INT(8192));
    add = gen_rtx_SET(stack_pointer_rtx, val);
    emit_insn_after(add, insn);
}


static void setup_basic(rtx insn)
{
    /*
        pushq	%rax						# save %rax value
        movq	8(%rbp), %rax				# the return address of function after prologue is stored at %rax temporarily bc you can't do 
                                            # mem to mem assignment
        movq	%rax, (%r10)				# assign next_slot the return address, which is the memory location where %r15 points to
        subq	$8, %r10					# now next_slot is ret_addrr[1]
        popq	%rax						# restore the value of %rax
    */

    P("In function prologue");

    rtx rax, r10, rbp, dec, mem, psh, inc, mov, val, add, pop;

    rax = gen_rtx_REG(DImode, AX_REG);
    r10 = gen_rtx_REG(DImode, R10_REG);
    rbp = gen_rtx_REG(DImode, BP_REG);


    /* pushq %rax */
    dec = gen_rtx_PRE_DEC(DImode, stack_pointer_rtx);
    mem = gen_rtx_MEM(DImode, dec);
    psh = gen_rtx_SET(mem, rax);
    emit_insn_before(psh, insn);




    /* movq	8(%rbp), %rax */
    inc = gen_rtx_PLUS(Pmode, rbp, GEN_INT(8));
    mem = gen_rtx_MEM(DImode, inc);
    mov = gen_rtx_SET(rax, mem);
    emit_insn_before(mov, insn);

    /* movq	%rax, (%r10) */
    mem = gen_rtx_MEM(DImode, r10);
    mov = gen_rtx_SET(mem, rax);
    emit_insn_before(mov, insn);

    /* subq	$8, %r10 */
    val = gen_rtx_PLUS(Pmode, r10, GEN_INT(-8));
    add = gen_rtx_SET(r10, val);
    emit_insn_before(add, insn);





    /* movq	(%rbp), %rax */
    mem = gen_rtx_MEM(DImode, rbp);
    mov = gen_rtx_SET(rax, mem);
    emit_insn_before(mov, insn);

    /* movq	%rax, (%r10) */
    mem = gen_rtx_MEM(DImode, r10);
    mov = gen_rtx_SET(mem, rax);
    emit_insn_before(mov, insn);

    /* subq	$8, %r10 */
    val = gen_rtx_PLUS(Pmode, r10, GEN_INT(-8));
    add = gen_rtx_SET(r10, val);
    emit_insn_before(add, insn);


    /* popq	%rax */
    inc = gen_rtx_POST_INC(DImode, stack_pointer_rtx);
    mem = gen_rtx_MEM(DImode, inc);
    pop = gen_rtx_SET(rax, mem);
    emit_insn_before(pop, insn);

}

static void finish_basic(rtx insn)
{
    P("In function epilogue");
    /*
    add $8, %r10
    push %rbx, %rcx
    mov (%r10), %rbx
    mov 8(%rbp), %rcx
    cmp %rbx, %rcx
    jeq   
    call abort() / mov %rbx, 8(%rbp) 
    pop %rcx, %rbx

    */
    rtx r10, rax, rbx, rbp, val, add, mem, mov, inc, dec, psh, pop;
    rtx cmp, label, jmp;
    //rtx call;
    rtx_insn * last;


    
    //--------------------------------------
    //Check Base Pointer first 
    


    rax = gen_rtx_REG(DImode, AX_REG);
    rbx = gen_rtx_REG(DImode, BX_REG);
    //rcx = gen_rtx_REG(DImode, CX_REG);
    r10 = gen_rtx_REG(DImode, R10_REG);
    rbp = gen_rtx_REG(DImode, BP_REG);


    /* push %rbx, %rax */
    dec = gen_rtx_PRE_DEC(DImode, stack_pointer_rtx);
    mem = gen_rtx_MEM(DImode, dec);
    psh = gen_rtx_SET(mem, rbx);
    last = emit_insn_after(psh, insn);
    psh = gen_rtx_SET(mem, rax);
    last = emit_insn_after(psh, last);





    /* addq	$8, %r10 */
    val = gen_rtx_PLUS(Pmode, r10, GEN_INT(8));
    add = gen_rtx_SET(r10, val);
    last = emit_insn_after(add, last);

    /* mov (%r10), %rbx */
    mem = gen_rtx_MEM(DImode, r10);
    mov = gen_rtx_SET(rbx, mem);
    last = emit_insn_after(mov, last);

    /* movq	(%rbp), %rax */
    mem = gen_rtx_MEM(DImode, rbp);
    mov = gen_rtx_SET(rax, mem);
    last = emit_insn_after(mov, last);

    /* cmp %rbx, %rax */
    cmp = gen_rtx_COMPARE(CCmode, rbx, rax);
    cmp = gen_rtx_SET(gen_rtx_REG(CCmode, FLAGS_REG), cmp);
    last = emit_insn_after(cmp, last);

    /* jeq */
    label = gen_label_rtx();
    jmp = gen_rtx_EQ(VOIDmode, gen_rtx_REG(CCmode, FLAGS_REG), const0_rtx);
    jmp = gen_rtx_IF_THEN_ELSE(VOIDmode,
        jmp,
        gen_rtx_LABEL_REF(VOIDmode, label),
        pc_rtx);
    jmp = gen_rtx_SET(pc_rtx, jmp);
    last = emit_insn_after(jmp, last);
    JUMP_LABEL(last) = label;

    /* Call abort() */
    /*
    call = gen_rtx_SYMBOL_REF(Pmode, "abort");
    call = gen_rtx_CALL(Pmode, gen_rtx_MEM(QImode, call), const0_rtx);
    last = emit_insn_after(call, last);
    emit_label_after(label, last);
    */
    /* restore (%rbp) */
    /* mov   %rbx , 8(%rbp) */

    mem = gen_rtx_MEM(DImode, rbp);
    mov = gen_rtx_SET(mem, rbx);
    last = emit_insn_after(mov, last);
    last = emit_label_after(label, last);


    
    //----------------------------------------
    //Then check Return Address
    


    /* addq	$8, %r10 */
    val = gen_rtx_PLUS(Pmode, r10, GEN_INT(8));
    add = gen_rtx_SET(r10, val);
    last = emit_insn_after(add, last);

    /* mov (%r10), %rbx */
    mem = gen_rtx_MEM(DImode, r10);
    mov = gen_rtx_SET(rbx, mem);
    last = emit_insn_after(mov, last);

    /* movq	8(%rbp), %rax */
    inc = gen_rtx_PLUS(Pmode, rbp, GEN_INT(8));
    mem = gen_rtx_MEM(DImode, inc);
    //mov = gen_rtx_SET(DImode, rax, mem);
    mov = gen_rtx_SET(rax, mem);
    last = emit_insn_after(mov, last);

    /* cmp %rbx, %rax */
    cmp = gen_rtx_COMPARE(CCmode, rbx, rax);
    cmp = gen_rtx_SET(gen_rtx_REG(CCmode, FLAGS_REG), cmp);
    last = emit_insn_after(cmp, last);

    /* jeq */
    label = gen_label_rtx();
    jmp = gen_rtx_EQ(VOIDmode, gen_rtx_REG(CCmode, FLAGS_REG), const0_rtx);
    jmp = gen_rtx_IF_THEN_ELSE(VOIDmode,
        jmp,
        gen_rtx_LABEL_REF(VOIDmode, label),
        pc_rtx);
    jmp = gen_rtx_SET(pc_rtx, jmp);
    last = emit_insn_after(jmp, last);
    JUMP_LABEL(last) = label;

    /* Call abort() */
    /*
    call = gen_rtx_SYMBOL_REF(Pmode, "abort");
    call = gen_rtx_CALL(Pmode, gen_rtx_MEM(QImode, call), const0_rtx);
    last = emit_insn_after(call, last);
    emit_label_after(label, last);
    */
    /* mov   %rbx , 8(%rbp) */
    inc = gen_rtx_PLUS(Pmode, rbp, GEN_INT(8));
    mem = gen_rtx_MEM(DImode, inc);
    mov = gen_rtx_SET(mem, rbx);
    last = emit_insn_after(mov, last);
    last = emit_label_after(label, last);





    /* pop %rax, %rbx */
    inc = gen_rtx_POST_INC(DImode, stack_pointer_rtx);
    mem = gen_rtx_MEM(DImode, inc);
    pop = gen_rtx_SET(rax, mem);
    last = emit_insn_after(pop, last);
    pop = gen_rtx_SET(rbx, mem);
    emit_insn_after(pop, last);

}



static unsigned ra_exec(void)
{
    rtx_insn * insn;
    const char * name = get_name(cfun->decl);
    P("Dealing with: %s", name);


    // insn = get_insns();
    // if (name[0] == 'm' && name[1] == 'a' && name[2] == 'i' && name[3] == 'n')
    //     /* main() prologue begin, allocate space for a return address/ base pointer table */
    //     rads[0].setup(insn);


    for (insn=get_insns(); insn; insn=NEXT_INSN(insn))
    {
        if (NOTE_P(insn) && (NOTE_KIND(insn) == NOTE_INSN_PROLOGUE_END))
        {
            if (name[0] == 'm' && name[1] == 'a' && name[2] == 'i' && name[3] == 'n')
                /* in the prologue of main() */
                rads[0].setup(insn);
            else
                /* in the prologue of other functions */
                rads[1].setup(insn);
        }

        else if (NOTE_P(insn) && (NOTE_KIND(insn) == NOTE_INSN_EPILOGUE_BEG))
        {   
            if (name[0] == 'm' && name[1] == 'a' && name[2] == 'i' && name[3] == 'n')
                /* in the epilogue of main() */
                rads[0].finish(insn);
            else
                /* in the epilogue of other functions */
                rads[1].finish(insn);
        }
    }

#ifdef DEBUG
    print_rtl(stdout, get_insns());
#endif
    return 0;
}


const pass_data my_pass_data = 
{
    .type = RTL_PASS,
    .name = "ra_check",
    .optinfo_flags = OPTGROUP_NONE,
    .tv_id = TV_NONE,
    .properties_required = 0, 
    .properties_provided = 0, 
    .properties_destroyed = 0,
    .todo_flags_start = 0,    
    .todo_flags_finish = TODO_dump_func,   
};

struct my_pass : rtl_opt_pass
{
public:
    my_pass(gcc::context *ctx) : rtl_opt_pass(my_pass_data,ctx) {}

    virtual unsigned int execute(function* fun) override
    {
        printf("%s\n", function_name(fun));
        ra_exec();
        return 0;
    }

    virtual bool gate(function* fun) override
    {
        //printf("%s\n", function_name(fun));
        return ra_gate();
        
    }

    virtual my_pass* clone() override {return this; }
};

static struct register_pass_info my_pass_info = 
{
    .pass = new my_pass(g),
    .reference_pass_name = "pro_and_epilogue",
    .ref_pass_instance_number = 0,
    .pos_op = PASS_POS_INSERT_AFTER,
};


int plugin_init(
    struct plugin_name_args   *info,  /* Argument info  */
    struct plugin_gcc_version *ver)   /* Version of GCC */
{
    if (!plugin_default_version_check(ver, &gcc_version)) {
        printf("incompatible gcc/plugin versions\n");
        return 1;
    }

    register_callback("ra_check", PLUGIN_PASS_MANAGER_SETUP, NULL, &my_pass_info);


    return 0;
}
