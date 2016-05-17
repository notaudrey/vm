CC = clang
CFLAGS = -g -D__DEBUG_BUILD
TARGET = lang 

CFLAGS += -Wall -Wextra -pipe -lreadline -std=c11

all: $(TARGET)

valgrind: $(TARGET)
	valgrind -v --track-origins=yes --leak-check=full ./$(TARGET)

$(TARGET): versionheader
	# Curse you __FILE__
	cd src && $(CC) $(CFLAGS) -o ../$(TARGET) *.c

versionheader: clean
	bash gen_version_h.sh

clean:
	rm -f *.a *.o *.la *.lo *.so *.so.* *.out $(TARGET)
