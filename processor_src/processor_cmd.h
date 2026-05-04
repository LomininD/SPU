#ifndef PROCESSOR_CMD_H
#define PROCESSOR_CMD_H

#include "../shared/processor_properties.h"
#include "stack/stack.h"
#include "processor_dump.h"

err_t       proc_push(proc_info* proc);
err_t       proc_pushreg(proc_info* proc);
err_t       proc_popreg(proc_info* proc);
err_t       proc_pushm(proc_info* proc);
err_t       proc_popm(proc_info* proc);
err_t       proc_calc_binary(proc_info* proc);
err_t       proc_calc_unary(proc_info* proc);
err_t       proc_in(proc_info* proc);
err_t       get_number(int* number, md_t debug_mode);
err_t       proc_out(proc_info* proc);
err_t       proc_jmp(proc_info* proc);
err_t       proc_cond_jmp(proc_info* proc);
err_t       proc_dmp(proc_info* proc);
err_t       proc_dmpm(proc_info* proc);
err_t       proc_call(proc_info* proc);
err_t       proc_ret(proc_info* proc);
err_t       proc_draw(proc_info* proc);
err_t       proc_hlt(proc_info* proc);
bool        check_condition(proc_commands cmd, int a, int b);
const char* decode_cmd (proc_commands cmd);

#endif
