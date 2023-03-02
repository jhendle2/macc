CC:=gcc
CFLAGS:=-Wall -Wno-unused-function

SRC:=./src
OBJ:=./obj
HDR:=-I$(SRC)

CFLAGS+= $(HDR)

SRCS:=$(wildcard $(SRC)/*.c)
OBJS:=$(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))

APP:=macc

DBG:=false
# DBG:=true
ifeq ($(DBG), true)
	CFLAGS+= -DENABLE_DEBUG_FLAG
endif

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -g -O0 -c -o $@ $<

.PHONY: debug
all: debug
debug: $(APP).c $(OBJS)
	$(CC) $(CFLAGS) -g -O0 -o $(APP) $(APP).c $(OBJS)

release: $(APP).c $(SRCS)
	$(CC) $(CFLAGS) -O2 -o $(APP) $(APP).c $(SRCS)

EXAMPLE:=./examples/hello_world.c
test:
	./$(APP) $(EXAMPLE)

valgrind:
	valgrind -s --leak-check=full --track-origins=yes ./$(APP) $(EXAMPLE)

valgrind-debug: debug valgrind
valgrind-release: release valgrind

clean:
	@rm $(APP) $(OBJ)/*.o