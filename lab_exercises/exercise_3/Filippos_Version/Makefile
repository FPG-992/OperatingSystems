# Define compiler
CC=gcc

# Compiler flags
CFLAGS=-Wall

# Target executable name
TARGET=out

# Source files
SOURCES=exercise_3.c

# Object files
OBJECTS=$(SOURCES:.c=.o)

# Default target
all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

# Rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
