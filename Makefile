TARGET = prog
LIBS = -lm
CC = gcc
# -O0: no optimization
# -p: generate profiling information
# -g: include debugging information
# -Wall: enable all warnings
CFLAGS = -O0 -pg -g -Wall
BUILD_DIR = build

.PHONY: default all clean

default: $(TARGET)
all: default

SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))
HEADERS = $(wildcard *.h)

$(BUILD_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -rf $(BUILD_DIR)
	-rm -f $(TARGET)

# verify core dump limit using 
# ulimit -c
# this will probably return a 0 (bytes) initially
# to set the limit to unlimited use
# ulimit -c unlimited
