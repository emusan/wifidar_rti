include Makefile.common

all: src
	$(CC) -o $(PROGRAM) $(CFLAGS) src/$(PROGRAM).c

.PHONY:src clean

clean:
	$(MAKE) -C src $@
	rm -f $(PROGRAM)

