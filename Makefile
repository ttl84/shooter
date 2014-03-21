CC:=g++
#compile and link flags
ifeq ($(OS), Windows_NT)
	RM = del
	BIN = a.exe
	LDFLAGS += -lMingw32
else
	RM = rm
	BIN = a.out
endif

LDFLAGS+=-L"../SDL2/lib" -lSDL2main -lSDL2
CFLAGS+=-std=c++0x -pedantic-errors -Wstrict-aliasing=0 -Wall -g -DDEBUG -I"../SDL2/include"

#file names and directories
DIR=src src/container
SRC=$(wildcard $(DIR:%=%/*.cpp))
OBJ=$(SRC:.cpp=.o)
DEP=$(SRC:.cpp=.d)

#rules
$(BIN): $(OBJ)
	$(CC) $^ $(LDFLAGS) -o $(BIN)
-include $(DEP)
%.o: %.cpp
	$(CC) $< $(CFLAGS) -c -o $@ -MMD -MP 

run: $(BIN)
	$(BIN)
clean:
	$(RM) $(OBJ) $(DEP) $(BIN)
pack: clean
	-vi changelog.txt
	7z a shooter-`date --rfc-3339=date`.7z ./src ./data ./save ./Makefile ./changelog.txt
.PHONY: all run pack clean
