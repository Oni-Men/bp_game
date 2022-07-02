TARGET  := run
CC := hgcc
CFLAGS := -W -Wall
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o,$(SRCS))

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

test: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm ./$(TARGET)
	rm ./*.o
