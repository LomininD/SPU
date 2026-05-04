#include "processor_dump.h"
#include "stdlib.h"


void memory_dump(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "====================================RAM DUMP====================================\n");

    printf_log_bold(debug_mode, " RAM", NULL);
    printf_log_msg(debug_mode, "  [%p]\n", proc->RAM);

    for (int i = 0; i < ram_size; ++i)
    {
        put_number(proc->RAM[i], debug_mode, false);

        if ((i + 1) % dump_width == 0)
            printf_log_msg(debug_mode, "\n");
    }

    if (ram_size % 10 != 0)
        printf_log_msg(debug_mode, "\n");

    printf_log_msg(debug_mode, "================================================================================\n\n");
}


void spu_dump(proc_info* proc) // launches only if debug mode on
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "====================================SPU DUMP====================================\n");
    printf_log_bold(debug_mode, "spu", NULL);
    printf_log_msg(debug_mode, "  [%p]\n", proc);
    printf_log_msg(debug_mode, "{\n");
    printf_log_msg(debug_mode, "\tregister amount = %d\n", register_amount);
    printf_log_msg(debug_mode, "\tbyte_code_file [%p]\n\n", proc->byte_code_file);
    printf_log_msg(debug_mode, "\tdebug_mode = %d\n", debug_mode);
    printf_log_msg(debug_mode, "\tfloat_mode = %d\n\n", proc->proc_modes.float_mode);
    printf_log_msg(debug_mode, "\tcurrent ip = %zu\n", proc->ip);
    printf_log_msg(debug_mode, "}\n\n");

    print_code(proc);

    print_registers(proc);

    printf_log_bold(debug_mode, "MAIN STACK\n", NULL);

    st_dump(&proc->st);

    printf_log_bold(debug_mode, "RET STACK\n", NULL);

    st_dump(&proc->ret_st);

    printf_log_msg(debug_mode, "================================================================================\n\n");
}


void print_code(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;
    size_t ip = proc->ip;

    printf_log_msg(debug_mode, "--------------------------------------CODE--------------------------------------\n");

    for (int i = 0; i < proc->prg_size; ++i)
    {
        if (i == ip)
            put_number(proc->code[i], debug_mode, true);
        else
            put_number(proc->code[i], debug_mode, false);

        if ((i + 1) % dump_width == 0)
            printf_log_msg(debug_mode, "\n");
    }

    if (proc->prg_size % 10 != 0)
        printf_log_msg(debug_mode, "\n");

    printf_log_msg(debug_mode, "--------------------------------------------------------------------------------\n\n");

}

void print_registers(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "--------------------------------REGISTER  VALUES--------------------------------\n");

    for (int i = 0; i < register_amount; i++)
    {
        printf_log_msg(debug_mode, "\t%cX: %d\n", (char) 'A' + i, proc->registers[i]);
    }

    printf_log_msg(debug_mode, "--------------------------------------------------------------------------------\n");
}


void put_number(int number, md_t debug_mode, bool highlight)
{
    char str[max_number_size + 1] = {};
	for (int i = 0; i < max_number_size ; i++)
		str[i] = '0';

    bool is_negative = number < 0;

	if (is_negative)
	{
        str[0] = '-';
		number *= -1;
	}

    for (int i = max_number_size - 1; i >= is_negative; i--)
    {
		str[i] = (char) (48 + number % 10);
        number /= 10;
    }

    if (highlight)
        printf_log_bold(debug_mode, ">%s<", str);
    else
        printf_log_grey(debug_mode, " %s ", str);
}
