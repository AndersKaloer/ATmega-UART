#include "usart.h"
#include "lib/ring_buffer/ringbuffer.h"

/**
 * @file
 * Prototypes for USART1 specific functions.
 */

#ifndef USART1_H
#define USART1_H

/**
 * Initializes USART1 and the ring buffers.
 */
void usart1_init(void);

/**
 * Returns the size of the receive queue
 */
ring_buffer_size_t usart1_recv_queue_size(void);

/**
 * Places the oldest element in the receive queue in <em>data</em>.
 * Returns 1 if data has been placed in <em>data</em>; 0 otherwise.
 * @param data A pointer to the byte at which the data shuld be placed.
 * @return 1 if data has been returned; 0 otherwise.
 */
ring_buffer_size_t usart1_recv_dequeue(char *data);

/**
 * Peeks the receive queue, i.e. returns the byte at index <em>index</em>
 * without removing it from the queue.
 * Returns 1 if data has been placed in <em>data</em>; 0 otherwise.
 * @param data A pointer to the byte at which the data shuld be placed.
 * @param index The index to peek. If 0 the oldest element will be returned.
 * @return 1 if data has been returned; 0 otherwise.
 */
ring_buffer_size_t usart1_recv_peek(char *data, ring_buffer_size_t index);

/**
 * Sends a byte using the USART.
 * The byte is be placed in the send queue which will
 * be sent in the order in which they are queued.
 * If the queue is empty, the byte will be sent immediately.
 * The function returns immediately.
 * @param data The byte to place in the buffer.
 */
void usart1_send_(char data);

/**
 * Sends an array of bytes.
 * The bytes are added to the queue and sent one by one.
 * The function returns immediately.
 * @param data A pointer to the start of the byte array.
 * @param size The length of the byte array.
 */
void usart1_send_arr(const char *data, ring_buffer_size_t size);
/**
 * Returns a usart_desc_t structure for USART1.
 * @return A pointer to the usart_desc_t structure for USART1.
 */
usart_desc_t *get_usart1_descriptor(void);

#endif /* USART1_H */
