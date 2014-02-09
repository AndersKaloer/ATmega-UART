#include <assert.h>
#include <stdio.h>
#include "../usart.h"
#include "../usart0.h"

int main(void) {

  char buf;
  
  usart0_init();
  usart0_send_arr("Ready", 5);
  while(1) {
    if(usart0_recv_dequeue(&buf)) {
      usart0_send(buf);
    }
  }
  
  return 0;
}
