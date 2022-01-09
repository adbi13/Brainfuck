CFLAGS += -std=c99 -Wall -Wextra

TARGET = brainfuck
LIB = lib$(TARGET).so
INTERPRETER_SOURCE = $(TARGET)Interpreter.c
INTERPRETER_OBJECT = $(TARGET)Interpreter.o
LIB_SOURCE = $(TARGET)Lib.c
LIB_OBJECT = $(TARGET)Lib.o

.PHONY: all clean dist-clean

all: $(LIB) $(TARGET)
$(TARGET): $(INTERPRETER_OBJECT)
$(INTERPRETER_OBJECT): $(INTERPRETER_SOURCE)

$(LIB): $(LIB_OBJECT)
	$(CC) -shared -o $@ $^

$(LIB_OBJECT): $(LIB_SOURCE)
	$(CC) -fPIC -c -o $@ $^

clean:
	-$(RM) $(INTERPRETER_OBJECT) $(LIB_OBJECT)

distclean: clean
	-$(RM) $(TARGET) $(LIB)