#include "assembler.h"
#include "code_reader.h"
#include "../shared/debug.h"
#include "string.h"


err_t asm_ctor(assembler_info* asm_data, debug_info* debug)
{
    assert(asm_data != NULL);
    assert(debug != NULL);

    md_t debug_mode = asm_data->debug_mode;

    printf_log_msg(debug_mode, "asm_ctor: began initialising\n");

    asm_data->len = 50;
    asm_data->str = (char*) calloc(asm_data->len + 1, sizeof(char));

    for(int i = 0; i < max_labels_number; i++)
        asm_data->labels[i] = -1;

    asm_data->pos = 0;
    asm_data->end = false;
    asm_data->cmd = UNKNOWN;

    debug->current_line = 0;
    debug->got_hlt = false;
    debug->got_output = false;
    debug->not_empty = false;

    printf_log_msg(debug_mode, "asm_ctor: finished initialising\n");
    return ok;
}


err_t fill_file_preamble(assembler_info* asm_data)
{
    assert(asm_data != NULL);

    asm_data->code[0] = 'L';
    asm_data->code[1] = 'M';
    asm_data->code[2] = 'D';
    asm_data->code[3] = version;
    asm_data->code[4] = asm_data->pos;

    // if changed update preamble_size in assembler_properties.h

    return ok;
}


err_t process_code(const files_info* files, assembler_info* asm_data, debug_info* debug)
{
    assert(files != NULL);
    assert(asm_data != NULL);
    assert(debug != NULL);

    md_t debug_mode = asm_data->debug_mode;

    printf_log_msg(debug_mode, "\n");

    while (!asm_data->end)
    {
        debug->current_line++;
        readline(asm_data, files->input_file);
        if (asm_data->end)
            break;

        printf_log_msg(debug_mode, "current pos in byte_code = %d\n", asm_data->pos);
        printf_log_msg(debug_mode, "assembler: got str: %s\n", asm_data->str);

        int scanned = sscanf(asm_data->str, RAW_CMD_FORMAT, asm_data->raw_cmd);
        if (scanned == -1)
        {
            printf_log_msg(debug_mode, "\n");
            continue;
        }

        printf_log_msg(debug_mode, "assembler: got instruction: %s\n", asm_data->raw_cmd);

        err_t recognized = determine_cmd(files->input_file_name, asm_data, debug->current_line);
        if (recognized != ok)
            return error;

        code_check(debug, asm_data->cmd);

        if (check_prg_size(asm_data->pos, debug_mode) != ok)
            return error;

        printf_log_msg(debug_mode, "\n");

        if (debug_mode == on)
            getchar();
    }
    return ok;
}


void reset_data(assembler_info* asm_data, int* current_line, FILE* input_file)
{
    assert(asm_data != NULL);
    assert(current_line != NULL);
    assert(input_file != NULL);

    asm_data->pos = 0;
    *current_line = 0;
    asm_data->end = 0;
    rewind(input_file);
}


err_t determine_cmd(const char* file_name, assembler_info* asm_data, int current_line)
{
    assert(file_name != NULL);
    assert(asm_data != NULL);

    md_t debug_mode = asm_data->debug_mode;

    for(int i = 0; i < _cmd_count; i++)
    {
        if (strcmp(possible_cmd[i].name, asm_data->raw_cmd) == 0)
        {
            printf_log_msg(debug_mode, "determine_cmd: recognized %s\n", possible_cmd[i].name);

            asm_data->cmd = possible_cmd[i].cmd_code;
            asm_data->code[asm_data->pos + preamble_size] = possible_cmd[i].cmd_code;
            printf_log_msg(debug_mode, "determine_cmd: code[%zu] = %d\n", asm_data->pos + preamble_size, possible_cmd[i].cmd_code);

            asm_data->pos++;

            err_t is_read = read_arg(file_name, asm_data, current_line, possible_cmd[i].arg_type);
            return is_read;
        }
    }

    if (asm_data->raw_cmd[0] == ':')
    {
        printf_log_msg(debug_mode, "determine_cmd: recognized label\n");
        return read_label(file_name, asm_data, current_line);
    }

    printf_log_msg(debug_mode, "\n");
    printf_err(debug_mode, "[%s:%d] -> determine_cmd: invalid command (%s)\n", file_name, current_line, asm_data->raw_cmd);
    asm_data->cmd = UNKNOWN;
    return error;
}


err_t asm_dtor(files_info* files, assembler_info* asm_data)
{
    assert(files != NULL);
    assert(asm_data != NULL);

    md_t debug_mode = asm_data->debug_mode;

    printf_log_msg(debug_mode, "asm_dtor: began termination\n");

    if (!files->output_defined)
        free(files->output_file_name);
    free(asm_data->str);
    fclose(files->input_file);
    fclose(files->output_file);

    printf_log_msg(debug_mode, "asm_dtor: done termination\n");

    return ok;
}
