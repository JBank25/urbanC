TARGET = prog
LIBS = -lm
CC = gcc
CFLAGS = -g -Wall
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
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -rf $(BUILD_DIR)
	-rm -f $(TARGET)