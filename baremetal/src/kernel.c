#include "../include/stdio.h"
#include "../include/tty.h"
#include "../include/vga.h"
#include "../include/cache_driver.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MMIO_MEM_SIZE 0x200

size_t strlen(const char *str) {
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}

void test_abas_poll(volatile unsigned char *offset, uint32_t size) {
  select_channel(0);
  for (uint32_t i = 0; i < size; i++) {
      offset[i] = i;
      asm volatile("nop" ::: "memory");
  }

  set_a_model(0b1000000, true);

  terminal_setcolor(VGA_COLOR_WHITE);
  printf("00 ");

  for (uint32_t i = 0; i < size; i++) {
      unsigned char found = offset[i];
      if (i == 16) {
          terminal_setcolor(VGA_COLOR_WHITE);
          printf(" ABAS POLL");
      }
      if (i % 16 == 0 && i != 0) {
          printf("\n");
          terminal_setcolor(VGA_COLOR_WHITE);
          printf("%x ", i);
      }

      if ((uint32_t)found == i % 0x100) {
          terminal_setcolor(VGA_COLOR_GREEN);
      } else {
          terminal_setcolor(VGA_COLOR_RED);
      }

      printf("%x ", found);
      asm volatile("nop" ::: "memory");
  }
}

void test_abas_flush(volatile unsigned char *offset, uint32_t size) {
  select_channel(0);
  for (uint32_t i = 0; i < size; i++) {
      offset[i] = i;
      asm volatile("nop" ::: "memory");
  }

  flush_caches();

  set_a_model(0b1000000, true);

  terminal_setcolor(VGA_COLOR_WHITE);
  printf("00 ");

  for (uint32_t i = 0; i < size; i++) {
      unsigned char found = offset[i];
      if (i == 16) {
          terminal_setcolor(VGA_COLOR_WHITE);
          printf(" ABAS FLUSH");
      }
      if (i % 16 == 0 && i != 0) {
          printf("\n");
          terminal_setcolor(VGA_COLOR_WHITE);
          printf("%x ", i);
      }

      if ((uint32_t)found == i % 0x100) {
          terminal_setcolor(VGA_COLOR_GREEN);
      } else {
          terminal_setcolor(VGA_COLOR_RED);
      }

      printf("%x ", found);
      asm volatile("nop" ::: "memory");
  }
}

void test_dq_poll(volatile unsigned char *offset, uint32_t size) {
  select_channel(0);
  for (uint32_t i = 0; i < size; i++) {
      offset[i] = 0x42;
      asm volatile("nop" ::: "memory");
  }

  // Force read the first charaters to keep them in cache
  volatile uint8_t a;
  for (int i = 0; i < 64; i++) {
      a = offset[i];
      asm volatile("nop" ::: "memory");
  }

  uint64_t db = 0b1101111010101101101111101110111111011110101011011011111011101111;
  set_dq_model(db, true);
  set_dq_model(~db, false);

  terminal_setcolor(VGA_COLOR_WHITE);
  printf("00 ");

  for (uint32_t i = 0; i < size; i++) {
      unsigned char found = offset[i];
      if (i == 16) {
          terminal_setcolor(VGA_COLOR_WHITE);
          printf(" DQ POLL");
      }
      if (i % 16 == 0 && i != 0) {
          printf("\n");
          terminal_setcolor(VGA_COLOR_WHITE);
          printf("%x ", i);
      }

      if ((uint32_t)found == 0x42) {
          terminal_setcolor(VGA_COLOR_GREEN);
      } else {
          terminal_setcolor(VGA_COLOR_RED);
      }

      printf("%x ", found);
      asm volatile("nop" ::: "memory");
  }
}

// Test with DQ modified, cache flush
void test_dq_flush(volatile unsigned char *offset, uint32_t size) {
  select_channel(0);
  for (uint32_t i = 0; i < size; i++) {
      offset[i] = 0x42;
      asm volatile("nop" ::: "memory");
  }

  // Force read the first charaters to keep them in cache
  volatile uint8_t a;
  for (int i = 0; i < 64; i++) {
      a = offset[i];
      asm volatile("nop" ::: "memory");
  }

  uint64_t db = 0b1101111010101101101111101110111111011110101011011011111011101111;
  set_dq_model(db, true);
  set_dq_model(~db, false);

  flush_caches();

  terminal_setcolor(VGA_COLOR_WHITE);
  printf("00 ");

  for (uint32_t i = 0; i < size; i++) {
      unsigned char found = offset[i];
      if (i == 16) {
          terminal_setcolor(VGA_COLOR_WHITE);
          printf(" DQ FLUSH");
      }
      if (i % 16 == 0 && i != 0) {
          printf("\n");
          terminal_setcolor(VGA_COLOR_WHITE);
          printf("%x ", i);
      }

      if ((uint32_t)found == 0x42) {
          terminal_setcolor(VGA_COLOR_GREEN);
      } else {
          terminal_setcolor(VGA_COLOR_RED);
      }

      printf("%x ", found);
      asm volatile("nop" ::: "memory");
  }
}

void config_caches() {
    enable_caches();
    configure_l1i(true, 16, 2, 8);
    configure_l1d(true, 16, 2, 8);
    configure_l2(false, 64, 2, 8);
    configure_l3(true, 16, 2, 8);

    reset_caches();
}

void wait() {
    volatile uint64_t a = 0;
    for (int i = 0; i < 1000000000; i++) {
        a += 1;
        asm volatile ("nop" ::: "memory");
    }
}

void reset_state(volatile unsigned char *ptr, size_t size) {
    set_a_model(0, true);
    set_a_model(0, false);

    set_ba_model(0, true);
    set_ba_model(0, false);

    set_s_model(0, true);
    set_s_model(0, false);

    set_dq_model(0, true);
    set_dq_model(0, false);

    for (size_t i = 0; i < size; i++) {
        ptr[i] = 0;
        asm volatile ("nop" ::: "memory");
    }

    flush_caches();
    wait();
    terminal_initialize();
}

void kernel_main(void) {
  /* Initialize terminal interface */
  terminal_initialize();

  //config_caches();

  volatile unsigned char *mem_offset = (volatile unsigned char *)MMIO_MEM_START;

  while(true) {
      test_dq_poll(mem_offset, 0x180);
      reset_state(mem_offset, 0x180);

      test_dq_flush(mem_offset, 0x180);
      reset_state(mem_offset, 0x180);

      test_abas_poll(mem_offset, 0x180);
      reset_state(mem_offset, 0x180);

      test_abas_flush(mem_offset, 0x180);
      reset_state(mem_offset, 0x180);

  }
}
