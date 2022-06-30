TARGET  := run
CC := hgcc
CFLAGS := -W -Wall
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o,$(SRCS))

$(TARGET): main.o input.o button.o space.o map.o texture.o entity.o status.o
	$(CC) -o $(TARGET) main.o input.o button.o space.o map.o texture.o status.o

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

test: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm ./$(TARGET)
	rm ./*.o
