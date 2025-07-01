CC = gcc

OBJS = basics.c vec2d.c transform.c cvec.c ceo.c 

COMPILER_FLAGS_RELEASE = -w -Wl,-subsystem,windows
COMPILER_FLAGS_QUICK = -w
COMPILER_FLAGS_DEBUG = -fmax-errors=3 -Waddress -Warray-bounds=1 -Wbool-compare -Wformat -Wimplicit -Wlogical-not-parentheses -Wmaybe-uninitialized -Wmemset-elt-size -Wmemset-transposed-args -Wmissing-braces -Wmultistatement-macros -Wopenmp-simd -Wparentheses -Wpointer-sign -Wrestrict -Wreturn-type -Wsequence-point -Wsizeof-pointer-div -Wsizeof-pointer-memaccess -Wstrict-aliasing -Wstrict-overflow=1 -Wtautological-compare -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wvolatile-register-var -Wcast-function-type -Wmissing-field-initializers -Wmissing-parameter-type -Woverride-init -Wsign-compare -Wtype-limits -Wshift-negative-value
COMPILER_FLAGS_MAX = -Wall -Wextra -Werror -O2 -std=c99 -pedantic

ifeq ($(OS),Windows_NT) # Windows_NT is the identifier for all versions of Windows
	DETECTED_OS := Windows
else
	DETECTED_OS := $(shell uname)
endif

ifeq ($(DETECTED_OS),Windows)
	INCLUDE_PATHS = -IC:/SDL/SDL3-3.2.16/x86_64-w64-mingw32/include/SDL3
	INCLUDE_PATHS += -IC:/SDL/SDL3-3.2.16/x86_64-w64-mingw32/include
	INCLUDE_PATHS += -IC:/SDL/SDL3_image-3.2.4/x86_64-w64-mingw32/include/SDL3_image
#	INCLUDE_PATHS += -IC:/SDL/SDL3_ttf-3.2.2/x86_64-w64-mingw32/include/SDL3

	LIBRARY_PATHS = -LC:/SDL/SDL3-3.2.16/x86_64-w64-mingw32/lib
	LIBRARY_PATHS += -LC:/SDL/SDL3_image-3.2.4/x86_64-w64-mingw32/lib
#	LIBRARY_PATHS += -LC:/SDL/SDL3_ttf-3.2.2/x86_64-w64-mingw32/lib

	LINKER_FLAGS = -lSDL3 -lSDL3_image #-lSDL3_ttf
else
	INCLUDE_PATHS = -I/usr/include/SDL3
	LINKER_FLAGS = -lm -lSDL3 -lSDL3_image #-lSDL3_ttf
endif

OBJ_NAME = CEO_Clash

release : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_RELEASE) $(LINKER_FLAGS) -o $(OBJ_NAME)
quick : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_QUICK) $(LINKER_FLAGS) -o $(OBJ_NAME)
debug : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_DEBUG) $(LINKER_FLAGS) -o $(OBJ_NAME)
max : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_MAX) $(LINKER_FLAGS) -o $(OBJ_NAME)