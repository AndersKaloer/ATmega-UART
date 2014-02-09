#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>
#include "usart1.h"

/**
 * @file
 * Implementation of USART1 specific functions.
 */

/**
 * Ring buffer for received data.
 */
ring_buffer_t usart1_recv_ring_buf;
/**
 * Ring buffer for data to send.
 */
ring_buffer_t usart1_send_ring_buf;


void usart1_init(void) {
  /* Disable interrupts */
  cli();
  /* Intialize ring buffers */
  ring_buffer_init(&usart1_recv_ring_buf);
  ring_buffer_init(&usart1_send_ring_buf);
  
  /* Used for enabling interrupts etc. */
  UCSR1A = 0;
  /* Enable USART1 TX and RX */
  UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);
  /* Async USART, 8bit, no parity and 1 stop bit */
  UCSR1C = (1 << UCSZ10) | (1 << UCSZ11);
  /* 9600 Baud Rate at 16.00000 MHz */
  UBRR1L = 103;
  UBRR1H = 0;
  /* Enable interrupts */
  sei();
}

ring_buffer_size_t usart1_recv_queue_size(void) {
  ring_buffer_size_t result;
  /* Prevent race conditions */
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    result = ring_buffer_num_items(&usart1_recv_ring_buf);
  }
  return result;
}

ring_buffer_size_t usart1_recv_dequeue(char *data) {
  ring_buffer_size_t result;
  /* Prevent race conditions */
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    result = ring_buffer_dequeue(&usart1_recv_ring_buf, data);
  }
  return result;
}

ring_buffer_size_t usart1_recv_peek(char *data, ring_buffer_size_t index) {
  ring_buffer_size_t result;
  /* Prevent race conditions */
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    result = ring_buffer_peek(&usart1_recv_ring_buf, data, index);
  }
  return result;
}

void usart1_send(char data) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    /* Add to queue */
    ring_buffer_queue(&usart1_send_ring_buf, data);
    /* Enable data register empty interrupt */
    UCSR1B |= (1 << UDRIE1);
  }
}

void usart1_send_arr(const char *data, ring_buffer_size_t size) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    ring_buffer_queue_arr(&usart1_send_ring_buf, data, size);
    /* Enable data register empty interrupt */
    UCSR1B |= (1 << UDRIE1);
  }
}


usart_desc_t *get_usart1_descriptor(void) {
  static usart_desc_t descriptor =
    {
      .usart_recv_queue_size = usart1_recv_queue_size,
      .usart_recv_dequeue = usart1_recv_dequeue,
      .usart_recv_peek = usart1_recv_peek,
      .usart_send = usart1_send,
      .usart_send_arr = usart1_send_arr
    };
  return &descriptor;
}

/**
 * The USART receive interrupt service routine.
 * The received buffer is placed in the ring buffer.
 */
ISR(USART1_RX_vect) {
  /* Read received data */
  char received_data = UDR1;
  /* Place data in ring buffer */
  /* As interrupts are disabled race conditions cannot occur here */
  ring_buffer_queue(&usart1_recv_ring_buf, received_data);
}

/**
 * The USART transmit register empty interrupt.
 * Pops the oldest element from the queue to the send register.
 */
ISR(USART1_UDRE_vect) {
  char data;
  /* Check for data in queue */
  /* As interrupts are disabled race conditions cannot occur here */
  if(ring_buffer_dequeue(&usart1_send_ring_buf, &data) > 0) {
    /* Send oldest byte */
    UDR1 = data;
  } else {
    /* Nothing to send */
    /* Disable interrupt */
    UCSR1B &= ~(1 << UDRIE1);
  }
}
