CC             = avr-gcc
CFLAGS         = -Wall -g -O2 -std=c99 -DF_CPU=16000000UL -mmcu=atmega2560

simple:
	$(CC) $(CFLAGS) -o simple simple.c ../usart.h ../usart0.c ../usart1.c ../lib/ring_buffer/ringbuffer.c

clean:
	rm -f simple
