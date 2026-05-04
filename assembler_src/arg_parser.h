# ifndef ARG_PARSER
# define ARG_PARSER

#include "../shared/assembler_properties.h"
#include <string.h>

#define OUT_EXT ".out"
#define ASM_EXT ".asm"

struct parser_struct
{
    bool input_file_determined;
    bool output_file_determined;
    bool got_o_flag;
};

err_t parse_args(int argc, char* argv[], files_info* files, assembler_info* asm_data);
err_t parse_flags(const char flag_str[], assembler_info* asm_data, parser_struct* verification);
err_t parse_file_name(files_info* files, const char* files_str, parser_struct* verification);
char* generate_output_name(const char* input_file_name);
void  launch_asm_help(void);
err_t verify_file_names(const files_info* files, const parser_struct* verification);
err_t open_files(files_info* files, const parser_struct* verification, assembler_info* asm_data);

#endif
