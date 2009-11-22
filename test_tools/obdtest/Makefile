CC = gcc
APP = obdtest
CFLAGS = -g3 -Wall
OBJS = obd.o main.o

all: $(APP)

$(APP): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rfv $(OBJS) $(APP)
