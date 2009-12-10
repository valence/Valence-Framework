all: $(OBJS)

%.o: %.c
	$(CC) -c $< -o $@ -I$(INCLUDE) $(CFLAGS) $(ADDITIONAL_CFLAGS)

clean:
	@rm -rfv $(OBJS) $(EXTRA_CLEAN)

distclean:
	@rm -v Makefile
