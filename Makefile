# This is the name of your final .nex file without the .nex extension
EXEC_OUTPUT=build/zxn_sprite_1

# List all your source files here
SOURCES = main.c layer2.c sprites.c

# Maybe you'll need to edit this
CRT=1

# You don't need to edit below here, have a nice day.

MKDIR = mkdir -p
CC=zcc
AS=zcc
TARGET=+zxn -subtype=nex #-I/Users/mike/src/zxnext_layer2/include -I/Users/mike/src/stuff/z88dk/include 
VERBOSITY=-vn -SO3 
OUT_DIR=build bin
PRAGMA_FILE=zpragma.inc

OBJECTS=$(SOURCES:.*=.o)
OBJS=$(patsubst %, src/%, $(OBJECTS))

C_OPT_FLAGS=-SO3 --max-allocs-per-node200000 --opt-code-size

CFLAGS=$(TARGET) $(VERBOSITY) -c $(C_OPT_FLAGS) -compiler sdcc -clib=new -pragma-include:$(PRAGMA_FILE)
LDFLAGS=$(TARGET) $(VERBOSITY) --list -m -s -clib=new -pragma-include:$(PRAGMA_FILE) -L/Users/mike/src/zxnext_layer2/lib/sccz80/ 
#-lzxnext_layer2
ASFLAGS=$(TARGET) $(VERBOSITY) -c --list -m -s

EXEC=$(EXEC_OUTPUT).nex

%.o: %.c $(PRAGMA_FILE)
	$(CC) $(CFLAGS) -o @ $<

%.o: %.asm
	$(AS) $(ASFLAGS) -o @ $<

all : dirs $(EXEC)

$(EXEC) : $(OBJS)
	$(CC) $(LDFLAGS) --list -startup=$(CRT) $(OBJS) -o $(EXEC_OUTPUT) -create-app

.PHONY: clean dirs install

install: all
	mv $(EXEC) bin

clean:
	rm -rf $(OUT_DIR) /tmp/tmpXX*
	rm -f src/*.lis src/*.sym src/*.o

dirs: $(OUT_DIR)

$(OUT_DIR):
	$(MKDIR) $(OUT_DIR)