# ifndef DEBUG_UTILS_H
# define DEBUG_UTILS_H

#include "../shared/assembler_properties.h"

void  code_check(debug_info* debug, proc_commands cmd);
err_t check_prg_size(int pos, md_t debug_mode);
void  check_warnings(debug_info* debug, char* input_file_name, md_t debug_mode);
void  output_labels(int* labels, md_t debug_mode);
void  output_code(FILE* fp, int* code, int pos, md_t debug_mode);

#endif
