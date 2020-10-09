
CC= gcc

all: link alarm send receive

link: link_layer.c
	$(CC) -c -o link_layer link_layer.c

alarm: alarm.c
	$(CC) -c -o alarm alarm.c

send: send.c
	$(CC) -o send send.c link_layer.c alarm.c

receive: receive.c
	$(CC) -o receive receive.c link_layer.c alarm.c

clean:
	rm -f *.o *.d receive send link_layer