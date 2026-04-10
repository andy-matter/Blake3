#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


class BLAKE3 {
  public:

    const char* getVersion(void);

    void init(void);
    void init_keyed(const uint8_t key[32]);
    
    void update(const void *input, size_t input_len);

    void finalize(uint8_t *out, size_t out_len);
    void finalize_seek(uint64_t seek, uint8_t *out, size_t out_len);

    void reset(void);
 
    

  private:

    const char* BLAKE3_VERSION_STRING = "1.8.3";
    static const int BLAKE3_KEY_LEN = 32;
    static const int BLAKE3_OUT_LEN = 32;
    static const int BLAKE3_BLOCK_LEN = 64;
    static const int BLAKE3_CHUNK_LEN = 1024;
    static const int BLAKE3_MAX_DEPTH = 54;
    static const int MAX_SIMD_DEGREE = 1;
    static const int MAX_SIMD_DEGREE_OR_2 = 2;


    // internal flags
    enum blake3_flags {
      CHUNK_START         = 1 << 0,
      CHUNK_END           = 1 << 1,
      PARENT              = 1 << 2,
      ROOT                = 1 << 3,
      KEYED_HASH          = 1 << 4,
      DERIVE_KEY_CONTEXT  = 1 << 5,
      DERIVE_KEY_MATERIAL = 1 << 6,
    };


    typedef struct {
      uint32_t input_cv[8];
      uint64_t counter;
      uint8_t block[BLAKE3_BLOCK_LEN];
      uint8_t block_len;
      uint8_t flags;
    } output_t;

    typedef struct {
      uint32_t cv[8];
      uint64_t chunk_counter;
      uint8_t buf[BLAKE3_BLOCK_LEN];
      uint8_t buf_len;
      uint8_t blocks_compressed;
      uint8_t flags;
    } blake3_chunk_state;

    typedef struct {
      uint32_t key[8];
      blake3_chunk_state chunk;
      uint8_t cv_stack_len;
      uint8_t cv_stack[(BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN];
    } blake3_hasher;


    blake3_hasher int_hasher;




    constexpr static const uint32_t IV[8] = {0x6A09E667UL, 0xBB67AE85UL, 0x3C6EF372UL,
                                   0xA54FF53AUL, 0x510E527FUL, 0x9B05688CUL,
                                   0x1F83D9ABUL, 0x5BE0CD19UL};
    
    constexpr static const uint8_t MSG_SCHEDULE[7][16] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {2, 6, 3, 10, 7, 0, 4, 13, 1, 11, 12, 5, 9, 14, 15, 8},
        {3, 4, 10, 12, 13, 2, 7, 14, 6, 5, 9, 0, 11, 15, 8, 1},
        {10, 7, 12, 9, 14, 3, 13, 15, 4, 0, 11, 2, 5, 8, 1, 6},
        {12, 13, 9, 11, 15, 10, 14, 8, 7, 2, 5, 3, 0, 1, 6, 4},
        {9, 14, 11, 5, 8, 12, 15, 1, 13, 3, 0, 10, 2, 6, 4, 7},
        {11, 15, 5, 0, 1, 9, 8, 6, 14, 10, 2, 12, 3, 4, 7, 13},
    };


     static inline __attribute__((always_inline))  unsigned int highest_one(uint64_t x) {
      unsigned int c = 0;
      if(x & 0xffffffff00000000ULL) { x >>= 32; c += 32; }
      if(x & 0x00000000ffff0000ULL) { x >>= 16; c += 16; }
      if(x & 0x000000000000ff00ULL) { x >>=  8; c +=  8; }
      if(x & 0x00000000000000f0ULL) { x >>=  4; c +=  4; }
      if(x & 0x000000000000000cULL) { x >>=  2; c +=  2; }
      if(x & 0x0000000000000002ULL) {           c +=  1; }
      return c;
    }

     static inline __attribute__((always_inline))  unsigned int popcnt(uint64_t x) {
      unsigned int count = 0;
      while (x != 0) {
        count += 1;
        x &= x - 1;
      }
      return count;
    }

     static inline __attribute__((always_inline))  uint64_t round_down_to_power_of_2(uint64_t x) {
      return 1ULL << highest_one(x | 1);
    }

     static inline __attribute__((always_inline))  uint32_t counter_low(uint64_t counter) { return (uint32_t)counter; }

     static inline __attribute__((always_inline))  uint32_t counter_high(uint64_t counter) {
      return (uint32_t)(counter >> 32);
    }

     static inline __attribute__((always_inline))  uint32_t load32(const void *src) {
      const uint8_t *p = (const uint8_t *)src;
      return ((uint32_t)(p[0]) << 0) | ((uint32_t)(p[1]) << 8) |
             ((uint32_t)(p[2]) << 16) | ((uint32_t)(p[3]) << 24);
    }

     static inline __attribute__((always_inline))  void load_key_words(const uint8_t key[BLAKE3_KEY_LEN], uint32_t key_words[8]) {
      key_words[0] = load32(&key[0 * 4]);
      key_words[1] = load32(&key[1 * 4]);
      key_words[2] = load32(&key[2 * 4]);
      key_words[3] = load32(&key[3 * 4]);
      key_words[4] = load32(&key[4 * 4]);
      key_words[5] = load32(&key[5 * 4]);
      key_words[6] = load32(&key[6 * 4]);
      key_words[7] = load32(&key[7 * 4]);
    }

     static inline __attribute__((always_inline))  void load_block_words(const uint8_t block[BLAKE3_BLOCK_LEN], uint32_t block_words[16]) {
      for (size_t i = 0; i < 16; i++) {
          block_words[i] = load32(&block[i * 4]);
      }
    }

     static inline __attribute__((always_inline))  void store32(void *dst, uint32_t w) {
      uint8_t *p = (uint8_t *)dst;
      p[0] = (uint8_t)(w >> 0);
      p[1] = (uint8_t)(w >> 8);
      p[2] = (uint8_t)(w >> 16);
      p[3] = (uint8_t)(w >> 24);
    }

     static inline __attribute__((always_inline))  void store_cv_words(uint8_t bytes_out[32], uint32_t cv_words[8]) {
      store32(&bytes_out[0 * 4], cv_words[0]);
      store32(&bytes_out[1 * 4], cv_words[1]);
      store32(&bytes_out[2 * 4], cv_words[2]);
      store32(&bytes_out[3 * 4], cv_words[3]);
      store32(&bytes_out[4 * 4], cv_words[4]);
      store32(&bytes_out[5 * 4], cv_words[5]);
      store32(&bytes_out[6 * 4], cv_words[6]);
      store32(&bytes_out[7 * 4], cv_words[7]);
    }





    void blake3_compress_in_place(uint32_t cv[8],
                                  const uint8_t block[BLAKE3_BLOCK_LEN],
                                  uint8_t block_len, uint64_t counter,
                                  uint8_t flags);
    
    void blake3_compress_xof(const uint32_t cv[8],
                             const uint8_t block[BLAKE3_BLOCK_LEN],
                             uint8_t block_len, uint64_t counter, uint8_t flags,
                             uint8_t out[64]);
    
    void blake3_xof_many(const uint32_t cv[8],
                         const uint8_t block[BLAKE3_BLOCK_LEN],
                         uint8_t block_len, uint64_t counter, uint8_t flags,
                         uint8_t out[64], size_t outblocks);
    
    void blake3_hash_many(const uint8_t *const *inputs, size_t num_inputs,
                          size_t blocks, const uint32_t key[8], uint64_t counter,
                          bool increment_counter, uint8_t flags,
                          uint8_t flags_start, uint8_t flags_end, uint8_t *out);
    
    size_t blake3_simd_degree(void);
    
    size_t blake3_compress_subtree_wide(const uint8_t *input, size_t input_len,
                                        const uint32_t key[8],
                                        uint64_t chunk_counter, uint8_t flags,
                                        uint8_t *out);




inline __attribute__((always_inline))  void chunk_state_init(blake3_chunk_state *self, const uint32_t key[8],
                             uint8_t flags);


inline __attribute__((always_inline))  void chunk_state_reset(blake3_chunk_state *self, const uint32_t key[8],
                              uint64_t chunk_counter);


inline __attribute__((always_inline))  size_t chunk_state_len(const blake3_chunk_state *self);


static inline __attribute__((always_inline))  size_t chunk_state_fill_buf(blake3_chunk_state *self,
                                   const uint8_t *input, size_t input_len);


static inline __attribute__((always_inline))  uint8_t chunk_state_maybe_start_flag(const blake3_chunk_state *self);


 static inline __attribute__((always_inline))  output_t make_output(const uint32_t input_cv[8],
                            const uint8_t block[BLAKE3_BLOCK_LEN],
                            uint8_t block_len, uint64_t counter,
                            uint8_t flags);


 inline __attribute__((always_inline))  void output_chaining_value(const output_t *self, uint8_t cv[32]);


 inline __attribute__((always_inline))  void output_root_bytes(const output_t *self, uint64_t seek, uint8_t *out,
                              size_t out_len);


 inline __attribute__((always_inline))  void chunk_state_update(blake3_chunk_state *self, const uint8_t *input,
                               size_t input_len);


 static inline __attribute__((always_inline))  output_t chunk_state_output(const blake3_chunk_state *self);


 static inline __attribute__((always_inline))  output_t parent_output(const uint8_t block[BLAKE3_BLOCK_LEN],
                              const uint32_t key[8], uint8_t flags);


 static inline __attribute__((always_inline))  size_t left_subtree_len(size_t input_len);


 inline __attribute__((always_inline))  size_t compress_chunks_parallel(const uint8_t *input, size_t input_len,
                                       const uint32_t key[8],
                                       uint64_t chunk_counter, uint8_t flags,
                                       uint8_t *out);


 inline __attribute__((always_inline))  size_t compress_parents_parallel(const uint8_t *child_chaining_values,
                                        size_t num_chaining_values,
                                        const uint32_t key[8], uint8_t flags,
                                        uint8_t *out);


 inline __attribute__((always_inline))  void compress_subtree_to_parent_node(const uint8_t *input, size_t input_len,
                                const uint32_t key[8], uint64_t chunk_counter,
                                uint8_t flags, uint8_t out[2 * BLAKE3_OUT_LEN]);


inline __attribute__((always_inline))  void hasher_init_base(const uint32_t key[8], uint8_t flags);


 inline __attribute__((always_inline))  void hasher_merge_cv_stack(uint64_t total_len);


 inline __attribute__((always_inline))  void hasher_push_cv(uint8_t new_cv[BLAKE3_OUT_LEN],
                           uint64_t chunk_counter);


 inline __attribute__((always_inline))  void blake3_hasher_update_base(const void *input,
                                      size_t input_len);


 static inline __attribute__((always_inline))  uint32_t rotr32(uint32_t w, uint32_t c);


 static inline __attribute__((always_inline))  void g(uint32_t *state, size_t a, size_t b, size_t c, size_t d,
              uint32_t x, uint32_t y);


 static inline __attribute__((always_inline))  void round_fn(uint32_t state[16], const uint32_t *msg, size_t round);


 static inline __attribute__((always_inline))  void compress_pre(uint32_t state[16], const uint32_t cv[8],
                         const uint8_t block[BLAKE3_BLOCK_LEN],
                         uint8_t block_len, uint64_t counter, uint8_t flags);


 inline __attribute__((always_inline))  void hash_one(const uint8_t *input, size_t blocks,
                              const uint32_t key[8], uint64_t counter,
                              uint8_t flags, uint8_t flags_start,
                              uint8_t flags_end, uint8_t out[BLAKE3_OUT_LEN]);

};
