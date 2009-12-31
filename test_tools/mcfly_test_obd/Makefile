CC = gcc
APP = mcfly_test_obd
CFLAGS = -g3 -Wall
INCLUDE = -I../../mcfly/include/
LIBS = -lmcfly
LIBPATH = ../../mcfly/srclib

all: $(APP)

$(APP):
	$(CC) -o $@ mcfly_test_obd.c $(INCLUDE) $(CFLAGS) -L$(LIBPATH) $(LIBS)

test: $(APP)
	LD_LIBRARY_PATH=$(LIBPATH) ./$(APP)

clean:
	rm -rfv $(APP)

brandon: clean all test
