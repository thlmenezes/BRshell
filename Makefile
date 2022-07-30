SRC_PATH = src
BIN_PATH = obj

sources  := $(wildcard $(SRC_PATH)/*.cpp)
objects  := $(sources:$(SRC_PATH)/%.cpp=$(BIN_PATH)/%.o)

##### Compiler Config
CC       := g++
CFLAGS   := -std=c++11 -Wall -pedantic -g
DFLAGS   := -ggdb -O0 -DDEBUG

##### Executable name
EXEC = BRsh
##### Remove directories
RMDIR = rm -rdf
##### Remove files
RM = rm -f

##### Internal commands, avoid PATH
.PHONY: folders debug clean

##### General rule
all: $(EXEC)

##### Generates executable
$(EXEC): $(objects)
	$(LINK.o) $^ -o $@

##### Generates object files
$(BIN_PATH)/%.o : $(SRC_PATH)/%.cpp | folders
	$(COMPILE.c) $(OUTPUT_OPTION) $<

debug: CFLAGS += $(DFLAGS)
debug: $(EXEC)

clean:
	-$(RMDIR) $(BIN_PATH)
	-$(RM) $(EXEC)

folders:
	@mkdir -p $(SRC_PATH) $(DEP_PATH) $(BIN_PATH)

help:
	@echo Available commands
	@echo - debug:    Builds debug version
	@echo - clean:    Cleans generated files
	@echo - help:     Shows help
	@echo - folders:  Generates project directories