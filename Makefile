OBJECT	= input.h input.c main.c
TARGET  = run
CC = hgcc
CFLAGS = -W -Wall

$(TARGET): main.o input.o button.o space.o map.o texture.o entity.o status.o
	$(CC) -o $(TARGET) main.o input.o button.o space.o map.o texture.o status.o

main.o: main.c
	$(CC) -c main.c

input.o: input.c input.h
	$(CC) -c input.c

button.o: button.c button.h space.h
	$(CC) -c button.c

space.o: space.c space.h
	$(CC) -c space.c

map.o: map.c map.h
	$(CC) -c map.c

texture.o: texture.c texture.h
	$(CC) -c texture.c

entity.o: entity.c entity.h
	$(CC) -c entity.c

status.o: status.c status.h
	$(CC) -c status.c

test: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm ./$(TARGET)
	rm ./*.o
