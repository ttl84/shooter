#compile and link flags
ifeq ($(OS), Windows_NT)
	BIN = a.exe
	LMINGW32:= -lMingw32
	LSDL2MAIN:= -lSDL2main
else
	BIN = a.out
endif
INCLUDE_FLAGS := -I include

LDFLAGS:= $(LMINGW32) $(LSDL2MAIN) -lSDL2 -lSDL2_mixer

CFLAGS+=-std=c++11 -pedantic-errors -Wstrict-aliasing=0 -Wall -g -DDEBUG

# This is a list of modules of the project.
# Modules are siblings, but may have inter module dependencies.
# Inter module dependcy is done through the include files in include/$module/*.h
MODULES:=main filereader video util

# Compile a list of sources files from the modules.
# src/*/*.cpp
SRC:=$(wildcard $(MODULES:%=src/%/*.cpp))

# Each source file has a matching .d dependency file and an .o object file.
DEP:=$(SRC:.cpp=.d)
OBJ:=$(SRC:.cpp=.o)


# Create the executable from object files.
# Causes object files to be built.
$(BIN): $(OBJ)
	$(CXX) $^ $(LDFLAGS) -o $(BIN)

# Create the object files and dependency files from source files.
# Modules can include their own public headers without full qualified name.
# Example:
# In src/A/A.cpp, #include "A.h"
# In src/B/B.cpp, #include "A/A.h"
%.o: %.cpp
	$(CXX) $< $(CFLAGS) -I include -I $(subst src/, include/, $(dir $<)) -c -o $@ -MMD -MP

# Include all the dependency files to invoke the object file rule.
-include $(DEP)

run: $(BIN)
	$(BIN)
clean:
	$(RM) $(OBJ) $(DEP) $(BIN)
pack: clean
	7z a shooter-`date --rfc-3339=date`.7z ./*
.PHONY: all run pack clean
