#ifndef PROCESSOR_PROPERTIES_H
#define PROCESSOR_PROPERTIES_H

#include "../processor_src/stack/stack.h"
#include "../processor_src/stack/stack_dump.h"
#include "debug.h"

// if file is updated UPDATE VERSION

const int    version = 15;
const int    max_byte_code_len = 10000;
const int    width = 400; // 127
const int    height = 250; // 78
const size_t ram_size = width * height;
const int    register_amount = 8;
const int    accuracy = 1000;

struct proc_modes_type
{
    md_t debug_mode;
    md_t float_mode;
};

enum proc_commands
{
    UNKNOWN = -1,
    PUSH,
    PUSHREG,
    POPREG,
    PUSHM,
    POPM,
    ADD,
    SUB,
    DIV,
    MULT,
    SQRT,
    IN,
    OUT,
    JMP,
    JB,
    JBE,
    JA,
    JAE,
    JE,
    JNE,
    DMP,
    DMPM,
    CALL,
    RET,
    DRAW,
    HLT,

    _cmd_count
};

struct proc_info
{
    proc_modes_type   proc_modes;
    FILE*             byte_code_file;
    proc_commands     current_cmd;
    int*              code;
    size_t            prg_size;
    st_t              st;
    st_t              ret_st;
    int               registers[register_amount];
    int               RAM[ram_size];
    size_t            ip;
};

enum err_t
{
    error,
    ok,
    help
};

enum arg_t
{
    none,
    number,
    string,
    ind,
    label
};

struct cmd_struct
{
    char            name[10];
    proc_commands   cmd_code;
    arg_t           arg_type;
    err_t           (*cmd_func) (proc_info*);
};

extern cmd_struct possible_cmd[];

#endif
