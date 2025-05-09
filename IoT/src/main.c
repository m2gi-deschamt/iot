/*
 * Copyright: Olivier Gruber (olivier dot gruber at acm dot org)
 *
 * This program is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */
#include "main.h"
#include "uart.h"
#include "isr.h"
#include "ring.h"

extern uint32_t irq_stack_top;
extern uint32_t stack_top;

void check_stacks() {
  void *memsize = (void*)MEMORY;
  void *addr;
  addr = &stack_top;
  if (addr >= memsize)
    panic();

  addr = &irq_stack_top;
  if (addr >= memsize)
    panic();
}

char line[MAX_CHARS];
uint32_t nchars = 0;

void process_ring() {
  uint8_t code;
  while (!ring_empty()) {
    code = ring_get();
    line[nchars++]=(char)code;
    uart_send(UART0, code);
  }
}

void uart_irq_handler(uint32_t irq, void *cookie) {
  char c;
  uart_receive(UART0, &c);

  while (c)
  {
    if (ring_full())
      panic();
    ring_put(c);
    uart_receive(UART0, &c);
  }
}

/**
 * This is the C entry point,
 * upcalled once the hardware has been setup properly
 * in assembly language, see the startup.s file.
 */
void _start(void) {
  char c;
  check_stacks();
  uarts_init();
  uart_enable(UART0);
  vic_setup_irqs();
  vic_enable_irq(UART0_IRQ, uart_irq_handler, NULL);
  for (;;)
  {
    process_ring();
    core_disable_irqs();
    if (ring_empty())
    {
      core_halt();
    }
    core_enable_irqs();
  }
}
void panic() {
  for(;;)
    ;
}