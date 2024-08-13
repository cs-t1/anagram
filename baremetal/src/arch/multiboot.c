#include "../../include/stdio.h"
#include "../../include/tty.h"
#include "../../include/vga.h"
#include "../../include/multiboot.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern uint32_t multiboot_magic;
extern struct multiboot_info *multiboot_info_header;

/* This tutorial will only work for the 32-bit ix86 targets. */

void read_multiboot_info() {
  struct multiboot_tag *tag = multiboot_info_header->tags;
  struct mb_mem_tag *mm_tag;
  struct mb_cmdline *cmd_tag;
  struct mb_mem_map *mem_map;
  struct mb_mem_map_part *mem_map_part;
  uint32_t type, size, length;

  length = 0;
  while (true) {
    type = tag->type;
    size = tag->size;
    switch (type) {
    case 0:
      printf("List of tags over. Number of tags: %lx\n", (unsigned long)length);
      return;
    case 1:
      cmd_tag = (struct mb_cmdline *)tag;
      printf("kernel command-line: %s\n", cmd_tag->string);
      break;
    case 4:
      // Main memory tag
      mm_tag = (struct mb_mem_tag *)tag;
      printf("Lower: %lx\n", (unsigned long)mm_tag->mem_lower);
      printf("Upper: %lx\n", (unsigned long)mm_tag->mem_upper);
      break;
    case 6:
      // Memory map. There is a list
      mem_map = (struct mb_mem_map *)tag;
      printf("Found memory map tag\n");
      uint32_t number_maps =
          (size - sizeof(struct mb_mem_map)) / mem_map->entry_size;
      printf("Contains %"PRIx32" maps.\n", number_maps);

      // Loop over all memory maps
      for (uint32_t i = 0; i < number_maps; i++) {
        mem_map_part = &(mem_map->parts[i]);
        printf("Part @ %lx with size %lx, type %lx, reserved %lx\n",
               (unsigned long)mem_map_part->base_address,
               (unsigned long)mem_map_part->length,
               (unsigned long)mem_map_part->type,
               (unsigned long)mem_map_part->reserved);
      }
      break;

    default:
      printf("Unknown tag type: %lx with size: %lx\n", (unsigned long)type,
             (unsigned long)size);
    }
    length += 1;
    if (size % 8 != 0) {
      size += (8 - (size % 8));
    }
    tag = (struct multiboot_tag *)((size_t)tag + size);
  }
}

