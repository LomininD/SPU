CC = g++

FLAGS = -g0 -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat \
-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion \
-Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
-Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith \
-Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2\
-Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused \
-Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast \
-Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector \
-fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -fPIE -Werror=vla \
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

DEPS = ../shared/debug.h debug_utils.h arg_parser.h assembler.h \
../shared/assembler_properties.h code_reader.h ../shared/processor_properties.h \
../processor_src/stack/font_styles.h ../processsor_src/processor_cmd.h

OBJ = assembler.o \
	main.o \
	code_reader.o \
	arg_parser.o \
	debug_utils.o \
	../shared/cmd_structures.o \
	../shared/debug.o \
	../processor_src/stack/stack.o \
	../processor_src/stack/stack_dump.o \
	../processor_src/stack/min_max.o \
	../processor_src/processor_cmd.o \
	../processor_src/processor_dump.o \
	../processor_src/processor.o \
	../processor_src/processor_init.o \

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(FLAGS)

../assembler: $(OBJ)
	$(CC) -o $@ $^ $(FLAGS)

clean:
	rm -rf *.o ../assembler
