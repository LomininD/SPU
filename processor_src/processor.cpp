#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "processor.h"
#include "processor_init.h"




err_t proc_ctor(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "proc_ctor: began initialising processor\n");

    size_t st_capacity = 10;
    size_t ret_st_capacity = 10;
    err_t st_initialised = initialise_stack(st_capacity, &(proc->st), debug_mode);
    err_t ret_st_initialised = initialise_stack(ret_st_capacity, &(proc->ret_st), debug_mode);

    if (st_initialised != ok || ret_st_initialised != ok)
        return error;

    proc->ip = 0;
    proc->prg_size = 0;
    proc->current_cmd = UNKNOWN;

    err_t prepared = prepare_file(proc, debug_mode);
    if (prepared != ok)
        return error;

    proc->code = (int*) calloc(proc->prg_size, sizeof(int));

    printf_log_msg(debug_mode, "proc_ctor: processor initialised\n\n");
    return ok;
}


err_t read_byte_code(proc_info* proc)
{
    assert(proc != NULL);

    FILE* fp = proc->byte_code_file;

    assert(fp != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "started reading\n");

    int cmd = 0;
    int i = 0;
    int scanned = 0;

    while (true)
    {
        scanned = fscanf(fp, "%d", &cmd);

        if (scanned == -1)
            break;

        if (i == max_byte_code_len)
        {
            printf_err(debug_mode, "[from read_byte_code] -> max byte code len exceeded\n");
            return error;
        }

        printf_log_msg(debug_mode, "scanned_cmd: %d\n", cmd);
        //proc->prg_size++;
        proc->code[i] = cmd;
        i++;
    }

    return ok;
}


err_t execute_cmd(proc_info* proc, proc_commands cmd)
{
    assert(proc != NULL);

    err_t executed = error;
    bool recognized = false;

    proc->current_cmd = cmd;

    for (int i = 0; i < _cmd_count; i++)
    {
        if (cmd == possible_cmd[i].cmd_code)
        {
            executed = (*(possible_cmd[i].cmd_func))(proc);
            recognized = true;
            break;
        }
    }

    if (!recognized)
        printf_err(proc->proc_modes.debug_mode, "[from execute_cmd] -> unknown command (cmd = %d, ip = %zu), cannot execute\n", cmd, proc->ip);

    return executed;
}


err_t proc_dtor(proc_info* proc)
{
    assert(proc != NULL);

    FILE* fp = proc->byte_code_file;

    assert(fp != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    if (debug_mode == on)
    {
        spu_dump(proc);
        memory_dump(proc);
    }

    printf_log_msg(debug_mode, "proc_dtor: began termination\n");

    st_return_err terminated_st = st_dtor(&proc->st);
    st_return_err terminated_ret_st = st_dtor(&proc->ret_st);

    if (terminated_st == no_error && terminated_ret_st == no_error)
    {
        printf_log_msg(debug_mode, "proc_dtor: stack destroyed\n");
    }
    else
    {
        printf_err(debug_mode, "[from execute_cmd] -> dtor failed\n");
        return error;
    }

    fclose(fp);
    free(proc->code);
    printf_log_msg(debug_mode, "proc_dtor: done termination\n");

    if (proc->proc_modes.debug_mode == on)
        fclose(log_ptr);

    return ok;
}

