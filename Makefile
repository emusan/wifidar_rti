include Makefile.common

all: src
	#$(CC) -o $(PROGRAM) $(CFLAGS) src/main.c -Wl,-rpath -Wl,/usr/local/lib `PKG_CONFIG_PATH="/usr/local/lib/pkgconfig" pkg-config  --cflags --libs plplotd`
	$(CC) -o $(PROGRAM) $(CFLAGS) src/main.c -g -Wl,-rpath -Wl,"/usr/lib64" -I/usr/include/plplot -L/usr/lib64 -lplplot -lltdl -lm -lcsirocsa -lqsastime

.PHONY:src clean

clean:
	rm -f $(PROGRAM)

