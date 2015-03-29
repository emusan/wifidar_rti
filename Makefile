include Makefile.common

all: src
	$(CC) -o $(PROGRAM) $(CFLAGS) src/main.c -Wl,-rpath -Wl,/usr/local/lib `PKG_CONFIG_PATH="/usr/local/lib/pkgconfig" pkg-config  --cflags --libs plplotd`

.PHONY:src clean

clean:
	rm -f $(PROGRAM)

