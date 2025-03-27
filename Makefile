CC = g++

CFLAGS = -Wall -Wextra -c

LDFLAGS =  -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system

TARGET = sfml-app

SOURCES = main.cpp

OBJECTS = main.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
