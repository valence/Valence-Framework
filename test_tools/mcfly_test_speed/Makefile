CC = gcc
APP = mcfly_test_speed
CFLAGS = -g3 -Wall
INCLUDE = -I../../mcfly/include/
LIBS = -lmcfly
LIBPATH = ../../mcfly/srclib

all: $(APP) test

$(APP):
	$(CC) -o $@ mcfly_test_speed.c $(INCLUDE) $(CFLAGS) -L$(LIBPATH) $(LIBS)

test: $(APP)
	LD_LIBRARY_PATH=$(LIBPATH) ./$(APP)

clean:
	rm -rfv $(APP)
