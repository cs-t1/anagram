#ifndef CACHE_DRIVER_H
#define CACHE_DRIVER_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MMIO_MEM_START UINT32_C(0xffff000)
#define CACHE_CONFIG_BASE UINT32_C(0xfffec00)
#define CACHE_METRICS_BASE UINT32_C(0xfffee00)
// TODO: 
#define FAULT_MODEL_CONFIG_BASE  UINT32_C(0xfffed00)

// TODO: keep these in sync with their counterpart in the emulation code
typedef enum {
    RANDOM = 0,
    LRU = 1,
    MRU = 2,
    FIFO = 3,
    LIFO = 4,
} ReplacementPolicy;

typedef enum {
    WRITE_BACK = 0,
    WRITE_THROUGH = 1,
} WritePolicy;

void disable_all_caches();

// TODO:
void enable_caches();
void enable_l1();
void enable_l2();
void enable_l3();

// CONTRACT: These have to be powers of 2
// INFO: Maybe we can simply pass the power of 2 as args
void configure_l1i(bool enable, uint64_t size, uint8_t assoc, uint32_t block_size);
void configure_l1d(bool enable, uint64_t size, uint8_t assoc, uint32_t block_size);
void configure_l2(bool enable, uint64_t size, uint8_t assoc, uint32_t block_size);
void configure_l3(bool enable, uint64_t size, uint8_t assoc, uint32_t block_size);

// This reinits the caches, and updates their config to what is required
void reset_caches();

void flush_caches();

// TODO:
void change_wp(WritePolicy wp);
void change_rp(ReplacementPolicy rp);

// TODO: These should probably return uint128_t?
// How do we pass these accross the interface?
uint64_t get_il1_hits();
uint64_t get_il1_misses();

uint64_t get_dl1_hits();
uint64_t get_dl1_misses();

uint64_t get_l2_hits();
uint64_t get_l2_misses();

uint64_t get_l3_hits();
uint64_t get_l3_misses();


void select_channel(uint8_t);
void set_dq_model(uint64_t val, bool is_up);
void set_a_model(uint16_t val, bool is_up);
void set_ba_model(uint8_t val, bool is_up);
void set_s_model(uint8_t val, bool is_up);

#endif
