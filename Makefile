CC = clang
CFLAGS = -g -D__DEBUG_BUILD
VM = vm
COMPILER = compiler
SCRATCH = scratch

CFLAGS += -Wall -Wextra -pipe -lreadline -lbsd -std=c11

all: $(VM) $(COMPILER)

$(COMPILER): versionheader
	#
	# Making compiler
	#
	cd src/compiler && $(CC) $(CFLAGS) -o ../../$(COMPILER) *.c ../common/*.c

$(SCRATCH): $(COMPILER)
	#
	# Making scratch
	#
	cd src/scratch && $(CC) $(CFLAGS) -o ../../$(SCRATCH) *.c ../common/*.c
	./compiler
	#valgrind -v --track-origins=yes --leak-check=full --show-leak-kinds=all ./scratch test.adri
	./scratch test.adri

valgrind: $(VM)
	valgrind -v --track-origins=yes --leak-check=full ./$(VM) test.adri

$(VM): compiler
	#
	# Making VM
	#
	cd src/vm && $(CC) $(CFLAGS) -o ../../$(VM) *.c ../common/*.c

versionheader: clean
	bash gen_version_h.sh

clean:
	rm -f *.a *.o *.la *.lo *.so *.so.* *.out *.adri $(VM) $(COMPILER) $(SCRATCH)
