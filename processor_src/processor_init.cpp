#include "processor_init.h"


err_t parse_args(int argc, char* argv[], proc_info* proc)
{
    assert(argv != NULL);
    assert(proc != NULL);

    bool got_byte_code_file = false;

    proc->proc_modes.debug_mode = off;
    proc->proc_modes.float_mode = off;

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            err_t got_flags = parse_flags(argv[i], proc);
            if (got_flags != ok)
                return got_flags;
        }
        else
        {
            err_t opened = open_file(argv[i], proc, &got_byte_code_file);
            if (opened != ok)
                return error;
        }
    }

    if (proc->proc_modes.debug_mode == on)
    {
        log_ptr = fopen("spu_log.log", "w");
    }

    if (!got_byte_code_file)
    {
        printf(MAKE_BOLD_RED("ERROR:") " required file was not provided\n");
        printf(MAKE_GREY("Note: for more info add -h flag.\n"));
        return error;
    }

    if (proc->byte_code_file == NULL || (proc->proc_modes.debug_mode == on && log_ptr == NULL))
    {
        printf(MAKE_BOLD_RED("ERROR:") " could not open the file\n");
        printf(MAKE_GREY("Note: file name may be incorrect\n"));
        return error;
    }

    return ok;
}

err_t parse_flags(char flag_str[], proc_info* proc)
{
    assert(proc != NULL);

    int ind = 1;
    while(flag_str[ind] != '\0')
    {
        if (flag_str[ind] == 'd')
        {
            proc->proc_modes.debug_mode = on;
        }
        else if (flag_str[ind] == 'h')
        {
            launch_proc_help();
            return help;
        }
        else if (flag_str[ind] == 'f')
        {
            proc->proc_modes.float_mode = on;;
        }
        else
        {
            printf(MAKE_BOLD_RED("ERROR:") " wrong flag(s)\n");
            printf(MAKE_GREY("Note: for more info add -h flag.\n"));
            return error;
        }
        ind++;
    }
    return ok;
}


err_t open_file(char* file_name, proc_info* proc, bool* got_byte_code_file)
{
    assert(file_name != NULL);
    assert(proc != NULL);
    assert(got_byte_code_file != NULL);

    if (!*got_byte_code_file)
    {
        proc->byte_code_file = fopen(file_name, "r");
        *got_byte_code_file = true;
    }
    else
    {
        printf(MAKE_BOLD_RED("ERROR:") "got too many files\n");
        printf(MAKE_GREY("Note: for more info add -h flag.\n"));
        return error;
    }
    return ok;
}


void launch_proc_help(void)
{
    printf(MAKE_BOLD("===PROCESSOR HELP===\n\n"));
    printf("Byte code file name is required for processor.\n\n");
    printf("Possible flags:\n");
    printf("-d: debug mode (log stored in spu_log.log)\n");
    printf("-f: float mode (all input numbers should be multiplied by 1000)\n");
    printf("-h: prints help information\n");
}


err_t prepare_file(proc_info* proc, md_t debug_mode)
{
    assert(proc != NULL);

    FILE* fp = proc->byte_code_file;

    err_t signature_ok = check_file_signature(fp, debug_mode);

    if (signature_ok != ok)
        return error;

    err_t version_ok = check_file_version(fp, debug_mode);

    if (version_ok != ok)
        return error;

    fscanf(fp, "%zu", &proc->prg_size);

    return ok;
}


err_t check_file_signature(FILE* fp, md_t debug_mode)
{
    assert(fp != NULL);

    int c = 0;
    int correct_symbols = 0;

    fscanf(fp, "%d", &c);
    if (c == 'L')
        correct_symbols++;
    fscanf(fp, "%d", &c);
    if (c == 'M')
        correct_symbols++;
    fscanf(fp, "%d", &c);
    if (c == 'D')
        correct_symbols++;

    if (correct_symbols == 3)
    {
        printf_log_msg(debug_mode, "check_file_signature: file signature verified\n");
        return ok;
    }
    else
    {
        printf_err(debug_mode, "[from check_file_signature] -> wrong file signature\n");
        return error;
    }
}


err_t check_file_version(FILE* fp, md_t debug_mode)
{
    assert(fp != NULL);

    int file_version = 0;
    fscanf(fp, "%d", &file_version);
    if (file_version < version)
    {
        printf_err(debug_mode, "[from check_file_version] -> assembly version is old\n");
        return error;
    }
    else if (file_version > version)
    {
        printf_err(debug_mode, "[from check_file_version] -> processor version is old\n");
        return error;
    }
    else
    {
        printf_log_msg(debug_mode, "check_file_version: assembly version verified\n");
        return ok;
    }
}


err_t initialise_stack(size_t capacity, st_t* st, md_t debug_mode)
{
    assert(st != NULL);

    st_return_err created = st_ctor(st, capacity, debug_mode);

    if (created == no_error)
    {
        printf_log_msg(debug_mode, "initialise_stack: stack created\n");
        return ok;
    }
    else
    {
        printf_err(debug_mode, "[from initialise_stack] -> problem occurred while creating stack\n");
        return error;
    }
}


