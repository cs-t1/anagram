#include "../include/cache_driver.h"
#include "../include/shared.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

SingleCacheConfigRequest *l1i_req = (SingleCacheConfigRequest *)(size_t)(CACHE_CONFIG_BASE + 32);
SingleCacheConfigRequest *l1d_req = (SingleCacheConfigRequest *)(size_t)(CACHE_CONFIG_BASE + 64);
SingleCacheConfigRequest *l2_req = (SingleCacheConfigRequest *)(size_t)(CACHE_CONFIG_BASE + 96);
SingleCacheConfigRequest *l3_req = (SingleCacheConfigRequest *)(size_t)(CACHE_CONFIG_BASE + 128);

void enable_caches() {
    uint8_t *ptr = CACHE_CONFIG_BASE;
    *ptr = 1;
    *(ptr + 1) = 1;
}

void configure_l1i(bool enable, uint64_t size, uint8_t assoc,
                  uint32_t block_size) {
    l1i_req->enable = enable;
    l1i_req->size = size;
    l1i_req->assoc = assoc;
    l1i_req->block_size = block_size;
}

void configure_l1d(bool enable, uint64_t size, uint8_t assoc,
                  uint32_t block_size) {
    l1d_req->enable = enable;
    l1d_req->size = size;
    l1d_req->assoc = assoc;
    l1d_req->block_size = block_size;
}

void configure_l2(bool enable, uint64_t size, uint8_t assoc,
                  uint32_t block_size) {
    l2_req->enable = enable;
    l2_req->size = size;
    l2_req->assoc = assoc;
    l2_req->block_size = block_size;
}

void configure_l3(bool enable, uint64_t size, uint8_t assoc,
                  uint32_t block_size) {
    l3_req->enable = enable;
    l3_req->size = size;
    l3_req->assoc = assoc;
    l3_req->block_size = block_size;
}

void flush_caches() {
  volatile uint8_t *flush_reg = (uint8_t *)(CACHE_CONFIG_BASE + 3);

  *flush_reg = 1;
}

void reset_caches() {
  volatile uint8_t *reset_reg = (uint8_t *)(CACHE_CONFIG_BASE + 2);

  *reset_reg = 1;
}

static inline uint64_t read_hits(uint8_t cache_idx) {
    volatile uint64_t *reg = (uint64_t *)(size_t)(CACHE_METRICS_BASE + 16 * cache_idx);

    return *reg;
}

static inline uint64_t read_misses(uint8_t cache_idx) {
    volatile uint64_t *reg = (uint64_t *)(size_t)(CACHE_METRICS_BASE + 16 * cache_idx + 8);

    return *reg;
}

uint64_t get_il1_hits() {
    return read_hits(0);
}

uint64_t get_il1_misses() {
    return read_misses(0);
}

uint64_t get_dl1_hits() {
    return read_hits(1);
}

uint64_t get_dl1_misses() {
    return read_misses(1);
}

uint64_t get_l2_hits() {
    return read_hits(2);
}

uint64_t get_l2_misses() {
    return read_misses(2);
}

uint64_t get_l3_hits() {
    return read_hits(3);
}

uint64_t get_l3_misses() {
    return read_misses(3);
}

// Fault model configuration methods
inline void select_channel(uint8_t idx) {
     volatile uint8_t *ptr = (uint8_t *)FAULT_MODEL_CONFIG_BASE;
     *ptr = idx;
}
inline void set_dq_model(uint64_t val, bool is_up) {
     volatile uint64_t *ptr = (uint64_t *)(size_t)(FAULT_MODEL_CONFIG_BASE + 8 + (is_up ? 0 : 8));
     *ptr = val;
}
inline void set_a_model(uint16_t val, bool is_up) {
     volatile uint16_t *ptr = (uint16_t *)(size_t)(FAULT_MODEL_CONFIG_BASE + 8 + 2 * 8 + (is_up ? 0 : 2));
     *ptr = val;
}
inline void set_ba_model(uint8_t val, bool is_up) {
     volatile uint8_t *ptr = (uint8_t *)(size_t)(FAULT_MODEL_CONFIG_BASE + 8 + 2 * 8 + 2 * 2 + (is_up ? 0 : 1));
     *ptr = val;
}
inline void set_s_model(uint8_t val, bool is_up) {
     volatile uint8_t *ptr = (uint8_t *)(size_t)(FAULT_MODEL_CONFIG_BASE + 8 + 2 * 8 + 2 * 2 + 2 * 1 + (is_up ? 0 : 1));
     *ptr = val;
}

void change_wp(WritePolicy wp) {
    volatile uint8_t *ptr = (uint8_t *)(size_t)(CACHE_CONFIG_BASE + 12);
    *ptr = wp == WRITE_BACK ? 0 : 1;
}

void change_rp(ReplacementPolicy rp) {
    volatile uint8_t *ptr = (uint8_t *)(size_t)(CACHE_CONFIG_BASE + 16);
    *ptr = (uint8_t)rp;
}
