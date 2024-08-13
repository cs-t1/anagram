#ifndef MULTIBOOT_H
#define MULTIBOOT_H 1

struct mb_mem_tag {
  uint32_t type;
  uint32_t size;
  uint32_t mem_lower;
  uint32_t mem_upper;
};

struct mb_cmdline {
  uint32_t type;
  uint32_t size;
  char *string;
};

struct mb_mem_map_part {
  uint64_t base_address;
  uint64_t length;
  uint32_t type;
  uint32_t reserved;
};

struct mb_mem_map {
  uint32_t type;
  uint32_t size;
  uint32_t entry_size;
  uint32_t entry_version;
  struct mb_mem_map_part parts[0];
};

struct multiboot_tag {
  uint32_t type;
  uint32_t size;
};

struct multiboot_info {
  uint32_t total_size;
  uint32_t reserved;
  struct multiboot_tag tags[0];
};

#endif

