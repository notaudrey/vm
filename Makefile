CC = clang
CFLAGS = -g -D__DEBUG_BUILD
VM = vm
COMPILER = compiler

CFLAGS += -Wall -Wextra -pipe -lreadline -std=c11

all: $(VM) $(COMPILER)

$(COMPILER): versionheader
	#
	# Making compiler
	#
	cd src/compiler && $(CC) $(CFLAGS) -o ../../$(COMPILER) *.c ../common/*.c

valgrind: $(VM)
	valgrind -v --track-origins=yes --leak-check=full ./$(VM)

$(VM): versionheader
	#
	# Making VM
	#
	cd src/vm && $(CC) $(CFLAGS) -o ../../$(VM) *.c ../common/*.c

versionheader: clean
	bash gen_version_h.sh

clean:
	rm -f *.a *.o *.la *.lo *.so *.so.* *.out $(VM) $(COMPILER)
