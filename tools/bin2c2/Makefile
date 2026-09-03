CC 		:= gcc
LD		:= g++
CFLAGS  := -s -Wall -O3
LDFLAGS := -s
TARGET  := bin2c2
OBJECTS := bin2c2.o
LIBS	:= 

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

clean:
	rm -f $(TARGET) *.o

