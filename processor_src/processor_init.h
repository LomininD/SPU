#ifndef PROCESSOR_INIT_H
#define PROCESSOR_INIT_H

#include "stack/stack.h"
#include "stack/font_styles.h"
#include "stack/stack_dump.h"
#include "../shared/processor_properties.h"

err_t parse_args(int argc, char* argv[], proc_info* proc);
err_t parse_flags(char flag_str[], proc_info* proc);
void  launch_proc_help(void);
err_t open_file(char* file_name, proc_info* proc, bool* got_byte_code_file);

err_t prepare_file(proc_info* proc, md_t debug_mode);
err_t check_file_signature(FILE* fp, md_t debug_mode);
err_t check_file_version(FILE* fp, md_t debug_mode);
err_t initialise_stack(size_t capacity, st_t* st, md_t debug_mode);

#endif
