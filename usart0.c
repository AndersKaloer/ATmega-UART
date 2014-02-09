#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>
#include "usart0.h"

/**
 * @file
 * Implementation of USART0 specific functions.
 */

/**
 * Ring buffer for received data.
 */
ring_buffer_t usart0_recv_ring_buf;
/**
 * Ring buffer for data to send.
 */
ring_buffer_t usart0_send_ring_buf;


void usart0_init(void) {
  /* Disable interrupts */
  cli();
  /* Initialize ring buffers */
  ring_buffer_init(&usart0_recv_ring_buf);
  ring_buffer_init(&usart0_send_ring_buf);
  
  /* Used for enabling interrupts etc. */
  UCSR0A = 0;
  /* Enable USART0 TX and RX */
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
  /* Async USART, 8bit, no parity and 1 stop bit */
  UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
  /* 9600 Baud Rate at 16.00000 MHz */
  UBRR0L = 103;
  UBRR0H = 0;
  /* Enable interrupts */
  sei();
}

ring_buffer_size_t usart0_recv_queue_size(void) {
  ring_buffer_size_t result;
  /* Prevent race conditions */
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    result = ring_buffer_num_items(&usart0_recv_ring_buf);
  }
  return result;
}

ring_buffer_size_t usart0_recv_dequeue(char *data) {
  ring_buffer_size_t result;
  /* Prevent race conditions */
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    result = ring_buffer_dequeue(&usart0_recv_ring_buf, data);
  }
  return result;
}

ring_buffer_size_t usart0_recv_peek(char *data, ring_buffer_size_t index) {
  ring_buffer_size_t result;
  /* Prevent race conditions */
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    result = ring_buffer_peek(&usart0_recv_ring_buf, data, index);
  }
  return result;
}

void usart0_send(char data) {
  /* Prevent race conditions */
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    /* Add to queue */
    ring_buffer_queue(&usart0_send_ring_buf, data);
    /* Enable data register empty interrupt */
    UCSR0B |= (1 << UDRIE0);
  }
}

void usart0_send_arr(const char *data, ring_buffer_size_t size) {
  /* Prevent race conditions */
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    ring_buffer_queue_arr(&usart0_send_ring_buf, data, size);
    /* Enable data register empty interrupt */
    UCSR0B |= (1 << UDRIE0);
  }
}

usart_desc_t *get_usart0_descriptor(void) {
  static usart_desc_t descriptor =
    {
      .usart_recv_queue_size = usart0_recv_queue_size,
      .usart_recv_dequeue = usart0_recv_dequeue,
      .usart_recv_peek = usart0_recv_peek,
      .usart_send = usart0_send,
      .usart_send_arr = usart0_send_arr
    };
  return &descriptor;
}

/**
 * The USART receive interrupt service routine.
 * The received buffer is placed in the ring buffer.
 */
ISR(USART0_RX_vect) {
  /* Read received data */
  char received_data = UDR0;
  /* Place data in ring buffer */
  /* As interrupts are disabled race conditions cannot occur here */
  ring_buffer_queue(&usart0_recv_ring_buf, received_data);
}

/**
 * The USART transmit register empty interrupt.
 * Pops the oldest element from the queue to the send register.
 */
ISR(USART0_UDRE_vect) {
  char data;
  /* Check for data in queue */
  /* As interrupts are disabled race conditions cannot occur here */
  if(ring_buffer_dequeue(&usart0_send_ring_buf, &data) > 0) {
    /* Send oldest byte */
    UDR0 = data;
  } else {
    /* Nothing to send */
    /* Disable interrupt */
    UCSR0B &= ~(1 << UDRIE0);
  }
}
