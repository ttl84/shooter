CC:=g++
#compile and link flags
ifeq ($(OS), Windows_NT)
	RM = del
	BIN = a.exe
	LDFLAGS += -lMingw32
	SDL2MAIN:=-lSDL2main
else
	RM = rm
	BIN = a.out
	SDL2MAIN:=
endif
INCLUDE_FLAGS := -I"include" \
	-I"../SDL2/include"

LDFLAGS+=-L"../SDL2/lib" $(SDL2MAIN) -lSDL2 \

CFLAGS+=-std=c++11 -pedantic-errors -Wstrict-aliasing=0 -Wall -g -DDEBUG $(INCLUDE_FLAGS)

#file names and directories
DIR=src
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
	7z a shooter-`date --rfc-3339=date`.7z ./*
.PHONY: all run pack clean
