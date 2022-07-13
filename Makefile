TARGET  := run
CC := hgcc
CFLAGS :=
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o,$(SRCS))

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o:%.c %.h
	$(CC) $(CFLAGS) -o $@ -c $<

test: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	-rm ./$(TARGET)
	-rm ./*.o
