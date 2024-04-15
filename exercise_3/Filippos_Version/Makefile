# Define the compiler
CC = gcc

# Define compiler flags
CFLAGS = -Wall

# Define the target executable name
TARGET = out

# Define the source files
SRC = new.c

# Define the default make target
all: $(TARGET)

# Rule to link the program
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Clean command to remove the executable
clean:
	rm -f $(TARGET)

# Prevent make from doing something with a file named clean
.PHONY: clean

