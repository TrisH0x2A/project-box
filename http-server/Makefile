CC = gcc
CFLAGS = -Wall -Wextra -O2
SRC = server.c
TARGET = webserver


all: $(TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
run: $(TARGET)
	./$(TARGET)
clean:
	rm -f $(TARGET)
.PHONY: all run clean serve build-and-run