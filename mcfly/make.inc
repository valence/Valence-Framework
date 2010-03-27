all: $(OBJS)

%.o: %.c
	$(CC) -c $< -o $@ -I$(INCLUDE) $(FLAGS) $(CFLAGS) $(ADDITIONAL_CFLAGS)

clean::
	@rm -rfv $(OBJS) $(EXTRA_CLEAN)

distclean::
	@rm -v Makefile
