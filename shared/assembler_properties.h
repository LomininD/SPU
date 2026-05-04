# ifndef ASSEMBLER_PROPERTIES_H
# define ASSEMBLER_PROPERTIES_H

#include <stdio.h>
#include "processor_properties.h"
#include "debug.h"

const size_t max_labels_number = 10;
const size_t max_cmd_len = 32;
const size_t preamble_size = 5;


struct files_info
{
    char* input_file_name;
    char* output_file_name;
    FILE* input_file;
    FILE* output_file;
    bool  output_defined;
};

struct assembler_info
{
    size_t          len;
    char*           str;
    char            raw_cmd[max_cmd_len];
    int             labels[max_labels_number];
    int             pos;
    bool            end;
    int             code[max_byte_code_len]; // do it as a dynamic array (after)
    proc_commands   cmd;
    md_t            debug_mode;
    md_t            listing_mode;
};

struct debug_info
{
    int  current_line;
    bool got_hlt;
    bool got_output;
    bool not_empty;
};

#endif
