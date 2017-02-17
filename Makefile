.PHONY: all
all: sztest-static sztest-dynamic

# change as needed
SZ_INSTALL_PATH = /home/bda/lib

sztest-static: sztest.c
	gcc -std=gnu99 -lm -g -o sztest-static sztest.c -I $(SZ_INSTALL_PATH)/include $(SZ_INSTALL_PATH)/lib/libsz.a $(SZ_INSTALL_PATH)/lib/libzlib.a

sztest-dynamic: sztest.c
	gcc -std=gnu99 -lm -g -o sztest-dynamic sztest.c -I $(SZ_INSTALL_PATH)/include -L $(SZ_INSTALL_PATH)/lib -lsz -lzlib

.PHONY: clean
clean:
	rm sztest-static sztest-dynamic
