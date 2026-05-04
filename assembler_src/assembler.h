# ifndef ASSEMBLER_H
# define ASSEMBLER_H

#include <string.h>
#include "../shared/processor_properties.h"
#include "../processor_src/stack/font_styles.h"
#include "../shared/assembler_properties.h"
#include "debug_utils.h"

// if no argument for output is given, output to file with the same name but .out extention
// solve problem with unexpected symbols
// debug messages in console (in debug mode) - user can redirect them to file
// debug messages to log file (in debug mode)
// errors and introductory information in console (always)

extern cmd_struct possible_cmd[];
//extern FILE* log_ptr;

err_t asm_ctor(assembler_info* asm_data, debug_info* debug);
err_t fill_file_preamble(assembler_info* asm_data);
err_t process_code(const files_info* files, assembler_info* asm_data, debug_info* debug);
void  reset_data(assembler_info* asm_data, int* current_line, FILE* input_file);
err_t determine_cmd(const char* file_name, assembler_info* asm_data, int current_line);
err_t asm_dtor(files_info* files, assembler_info* asm_data);

# define RAW_CMD_FORMAT "%31s"


#endif
