#include <avr/io.h>
#include "lib/ring_buffer/ringbuffer.h"

/**
 * @file
 * Generalized usart properties.
 */

#ifndef USART_H
#define USART_H
/**
 * USART Descriptor.
 * This struct contains pointers to usart methods
 * and can be used to ensure portability between different
 * USARTs.
*/
typedef struct {
  /** Returns the size of the receive buffer */
  ring_buffer_size_t (*usart_recv_queue_size)(void);
  /** Returns and removes the first element in the receive buffer */
  ring_buffer_size_t (*usart_recv_dequeue)(char *data);
  /** Returns, without removing, the first element in the receive buffer */
  ring_buffer_size_t (*usart_recv_peek)(char *data, ring_buffer_size_t index);
  /** Sends the byte 'data' */
  void (*usart_send)(char data);
  /** Sends the array of size 'size' pointed to by 'data' */
  void (*usart_send_arr)(const char *data, ring_buffer_size_t size);
} usart_desc_t;
#endif /* USART_H */
