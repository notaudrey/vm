CC = clang
CFLAGS = -std=c11 -g
TARGET = lang 

CFLAGS += -Wall -Wextra -pipe -lreadline

all: $(TARGET)

$(TARGET): versionheader
	# Curse you __FILE__
	cd src && $(CC) $(CFLAGS) -o ../$(TARGET) *.c

versionheader: clean
	bash gen_version_h.sh

clean:
	rm -f *.a *.o *.la *.lo *.so *.so.* *.out $(TARGET)
