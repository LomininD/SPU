#include <math.h>
#include "stack/stack_dump.h"
#include "processor_cmd.h"
#include "processor.h"

// think about refactoring this file

err_t proc_push(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began push\n");
    int number = proc->code[++proc->ip];
    st_return_err pushed = st_push(&proc->st, number);

    if (pushed != no_error)
    {
        printf_err(debug_mode, "[from execute_cmd] -> push failed\n");
        return error;
    }

    st_dump(&proc->st);
    printf_log_msg(debug_mode, "execute_cmd: push succeeded\n");
    proc->ip++;
    return ok;
}


err_t proc_popreg(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began popreg\n");

    int reg = proc->code[++proc->ip];

    if (reg < 0 || reg >= register_amount)
    {
        printf_err(debug_mode, "[from execute_cmd] -> popreg failed (got non-existent register index %d)\n", reg);
        return error;
    }

    int number = 0;
    st_return_err got_number = st_pop(&proc->st, &number);

    if (got_number != no_error)
    {
        printf_err(debug_mode, "[from execute_cmd] -> popreg failed (not enough data in stack)\n");
        return error;
    }

    proc->registers[reg] = number;
    printf_log_msg(debug_mode, "execute_cmd: popreg succeeded\n");
    st_dump(&proc->st);
    proc->ip++;

    return ok;
}


err_t proc_pushreg(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began pushreg\n");

    int reg = proc->code[++proc->ip];

    if (reg < 0 || reg >= register_amount)
    {
        printf_err(debug_mode, "[from execute_cmd] -> pushreg failed (got non-existent register index %d)\n", reg);
        return error;
    }

    st_return_err pushed = st_push(&proc->st, proc->registers[reg]);

    if (pushed != no_error)
    {
        printf_err(debug_mode, "[from execute_cmd] -> pushreg failed (could not push data to stack)\n");
        return error;
    }

    printf_log_msg(debug_mode, "execute_cmd: pushreg succeeded\n");
    st_dump(&proc->st);
    proc->ip++;

    return ok;
}


err_t proc_popm(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began popm\n");

    int reg = proc->code[++proc->ip];

    if (reg < 0 || reg >= register_amount)
    {
        printf_err(debug_mode, "[from execute_cmd] -> popm failed (got non-existent register index %d)\n", reg);
        return error;
    }

    int ind = proc->registers[reg];

    if (ind < 0 || ind >= ram_size)
    {
        printf_err(debug_mode, "[from execute_cmd] -> popm failed (got non-existent ram index %d)\n", ind);
        return error;
    }

    int el = 0;
    st_return_err popped = st_pop(&proc->st, &el);

    if (popped != no_error)
    {
        printf_err(debug_mode, "[from execute_cmd] -> popm failed (not enough data in stack)\n");
        return error;
    }

    proc->RAM[ind] = el;

    printf_log_msg(debug_mode, "execute_cmd: popm succeeded\n");
    st_dump(&proc->st);
    proc->ip++;

    return ok;
}


err_t proc_pushm(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began pushm\n");

    int reg = proc->code[++proc->ip];

    if (reg < 0 || reg >= register_amount)
    {
        printf_err(debug_mode, "[from execute_cmd] -> pushm failed (got non-existent register index %d)\n", reg);
        return error;
    }

    int ind = proc->registers[reg];

    if (ind < 0 || ind >= ram_size)
    {
        printf_err(debug_mode, "[from execute_cmd] -> pushm failed (got non-existent ram index %d)\n", ind);
        return error;
    }

    st_return_err pushed = st_push(&proc->st, proc->RAM[ind]);

    if (pushed != no_error)
    {
        printf_err(debug_mode, "[from execute_cmd] -> pushm failed (could not push data to stack)\n");
        return error;
    }

    printf_log_msg(debug_mode, "execute_cmd: pushm succeeded\n");
    st_dump(&proc->st);
    proc->ip++;

    return ok;
}


err_t proc_calc_binary(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;
    proc_commands cmd = proc->current_cmd;

    const char* cmd_name = decode_cmd(cmd);
    printf_log_msg(debug_mode, "execute_cmd: began %s \n", cmd_name);

    int a = 0;
    int b = 0;
    st_return_err got_a = st_pop(&proc->st, &a);
    st_return_err got_b = st_pop(&proc->st, &b);

    int res = 0;

    switch(cmd)
    {
        case ADD:
            res = a + b;
            break;

        case SUB:
            res = b - a;
            break;

        case MULT:
            res = a * b;
            if (proc->proc_modes.float_mode == on)
                res = res / 1000;
            break;

        case DIV:
            if (a == 0)
            {
                printf_err(debug_mode, "[from execute_cmd] -> div failed - division by zero\n");
                return error;
            }

            if (proc->proc_modes.float_mode == on)
                b = b * accuracy;

            res = b / a;
            break;
    }

    if (got_a == no_error && got_b == no_error)
    {
        st_return_err pushed = st_push(&proc->st, res);

        if (pushed != no_error)
        {
            printf_err(debug_mode, "[from execute_cmd] -> %s failed (failed to push the result)\n", cmd_name);
            return error;
        }

        st_dump(&proc->st);
        printf_log_msg(debug_mode, "execute_cmd: %s succeeded, result = %d\n", cmd_name, res);
        proc->ip++;
        return ok;
    }

    printf_err(debug_mode, "[from execute_cmd] -> %s failed (not enough data in stack)\n", cmd_name);
    return error;
}


err_t proc_calc_unary(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;
    proc_commands cmd = proc->current_cmd;

    const char* cmd_name = decode_cmd(cmd);
    printf_log_msg(debug_mode, "execute_cmd: began %s \n", cmd_name);

    int a = 0;
    st_return_err got_a = st_pop(&proc->st, &a);

    int res = 0;

    switch(cmd)
    {
        case SQRT:
            if (a < 0)
            {
                printf_err(debug_mode, "[from execute_cmd] -> sqrt failed - cannot determine root of negative number\n");
                return error;
            }

            if (proc->proc_modes.float_mode == on)
                res = (int) (sqrt((double) a / (double) accuracy) * accuracy);
            else
                res = sqrt(a);
            break;
    }
    if (got_a == no_error)
    {
        st_return_err pushed = st_push(&proc->st, res);

        if (pushed != no_error)
        {
            printf_err(debug_mode, "[from execute_cmd] -> %s failed (failed to push the result)\n", cmd_name);
            return error;
        }

        st_dump(&proc->st);
        printf_log_msg(debug_mode, "execute_cmd: %s succeeded, result = %d\n", cmd_name, res);
        proc->ip++;
        return ok;
    }

    printf_err(debug_mode, "[from execute_cmd] -> %s failed (not enough data in stack)\n", cmd_name);
    return error;
}


err_t proc_in(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began in\n");

    int number = 0;

    err_t got_number = get_number(&number, debug_mode);

    if (got_number != ok)
    {
        printf_log_msg(debug_mode, "execute_cmd: in failed\n");
        return error;
    }

    st_return_err pushed = st_push(&proc->st, number);

    if (pushed != no_error)
    {
        printf_err(debug_mode, "[from execute_cmd] -> in failed (could not push read value to stack)\n");
        return error;
    }

    st_dump(&proc->st);
    printf_log_msg(debug_mode, "execute_cmd: in succeeded\n");
    proc->ip++;
    return ok;
}


err_t get_number(int* number, md_t debug_mode)
{
    char str_number[11] = {};
    // max number len is 9 symbols 10 symbols are read to check if number is overflowed
    char bad_symbols[11] = {};
    fgets(str_number, 11, stdin);
    int scanned = sscanf(str_number, "%d%s", number, bad_symbols);

    if (str_number[9] != '\0' && str_number[9] != '\n')
    {
        printf_err(debug_mode, "[from get_number] -> number is too big\n");
        return error;
    }

    if (scanned != 1)
    {
        printf_err(debug_mode, "[from get_number] -> the input value is not a number\n");
        return error;
    }

    printf_log_msg(debug_mode, "get_number: scanned number  = %d\n", *number);
    return ok;
}


err_t proc_out(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began out\n");

    int el = 0;
    st_return_err got_el = st_pop(&proc->st, &el);

    if (got_el != no_error)
    {
        printf_err(debug_mode, "[from execute_cmd] -> out failed (not enough data in stack)\n");
        return error;
    }

    printf("%d\n", el);
    printf_log_msg(debug_mode, "execute_cmd: out succeeded (%d)\n", el);
    proc->ip++;
    return ok;
}


err_t proc_jmp(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began jmp\n");

    size_t new_pointer = proc->code[++proc->ip];

    if (new_pointer >= proc->prg_size || new_pointer < 0)
    {
        printf_err(debug_mode, "[from proc_jmp] -> new IP points on a non-existed position in code\n");
        return error;
    }

    proc->ip = new_pointer;

    printf_log_msg(debug_mode, "execute_cmd: done jmp (new_position = %zu)\n", new_pointer);
    return ok;
}


err_t proc_cond_jmp(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;
    proc_commands cmd = proc->current_cmd;

    const char* cmd_name = decode_cmd(cmd);

    printf_log_msg(debug_mode, "execute_cmd: began conditional jmp (%s)\n", cmd_name);

    int a = 0;
    int b = 0;
    st_return_err got_a = st_pop(&proc->st, &a);
    st_return_err got_b = st_pop(&proc->st, &b);

    if (got_a != no_error || got_b != no_error)
    {
        printf_err(debug_mode, "[from execute_cmd] -> %s failed (not enough data in stack)\n", cmd_name);
        return error;
    }

    bool is_fulfilled = check_condition(cmd, a, b);

    if (is_fulfilled)
    {
        printf_log_msg(debug_mode, "proc_cond_jmp: condition is fulfilled\n");
        err_t jumped = proc_jmp(proc);
        if (jumped != ok)
            return error;
    }
    else
    {
        printf_log_msg(debug_mode, "proc_cond_jmp: condition is not fulfilled\n");
        proc->ip += 2;
    }

    printf_log_msg(debug_mode, "execute_cmd: done conditional jmp (%s)\n", cmd_name);
    return ok;
}


err_t proc_dmp(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began printing dump\n");

    spu_dump(proc);

    printf_log_msg(debug_mode, "execute_cmd: done printing dump\n");

    proc->ip += 1;
    return ok;
}


err_t proc_dmpm(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began printing memory dump\n");

    memory_dump(proc);

    printf_log_msg(debug_mode, "execute_cmd: done printing memory dump\n");

    proc->ip += 1;
    return ok;
}


err_t proc_call(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began call\n");

    st_return_err pushed = st_push(&proc->ret_st, proc->ip + 2);
    if (pushed != no_error)
        return error;

    err_t jumped = proc_jmp(proc);
    if (jumped != ok)
        return error;

    printf_log_msg(debug_mode, "execute_cmd: done call\n");
    return ok;
}

err_t proc_ret(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began ret\n");

    int new_pointer = 0;
    st_return_err popped = st_pop(&proc->ret_st, &new_pointer);

    if (popped != no_error)
        return error;

    if (new_pointer >= proc->prg_size || new_pointer < 0)
    {
        printf_err(debug_mode, "[from proc_ret] -> new IP points on a non-existed position in code\n");
        return error;
    }

    proc->ip = new_pointer;

    printf_log_msg(debug_mode, "execute_cmd: done ret (returned to ip %d)\n", new_pointer);
    return ok;
}


err_t proc_draw(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began draw\n");

    int* ram = proc->RAM;

    for (int i = 0; i < ram_size; i++)
    {
        printf("%c", ram[i]);
        if ((i + 1) % width == 0)
            printf("\n");
    }

    printf_log_msg(debug_mode, "execute_cmd: done draw\n");

    proc->ip += 1;
    return ok;
}


err_t proc_hlt(proc_info* proc)
{
    assert(proc != NULL);

    md_t debug_mode = proc->proc_modes.debug_mode;

    printf_log_msg(debug_mode, "execute_cmd: began hlt\n");

    err_t destroyed = proc_dtor(proc);

    if (destroyed != ok)
        return error;

    printf_log_msg(debug_mode, "execute_cmd: done hlt\n");

    proc->ip += 1;
    return ok;
}


bool check_condition(proc_commands cmd, int a, int b)
{
    switch(cmd)
    {
        case JB:
            return b < a;
            break;
        case JBE:
            return b <= a;
            break;
        case JA:
            return b > a;
            break;
        case JAE:
            return b >= a;
            break;
        case JE:
            return b == a;
            break;
        case JNE:
            return b != a;
            break;
        default:
            return false;
            break;
    };
}


const char* decode_cmd (proc_commands cmd)
{
    for (int i = 0; i < _cmd_count; i++)
        if (cmd == possible_cmd[i].cmd_code)
            return possible_cmd[i].name;
    return "unknown cmd";
}
