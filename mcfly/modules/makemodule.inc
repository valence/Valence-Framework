ADDITIONAL_CFLAGS = -fPIC
INCLUDE = ../../include

$(LIBNAME): $(OBJS)
	$(CC) -o libmcfly_`basename $@`.so $@.o -shared $(CLIBS)

include ../../make.inc
