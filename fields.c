/*
 *  SPDX-License-Identifier: MIT
 */

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include "fields.h" 

// GF(2^8) with X^8 + X^4 + X^3 + X^1 + 1
#define bf8_modulus (UINT8_C((1 << 4) | (1 << 3) | (1 << 1) | 1))
// GF(2^64) with X^64 + X^4 + X^3 + X^1 + 1
#define bf64_modulus (UINT64_C((1 << 4) | (1 << 3) | (1 << 1) | 1))
// GF(2^128) with X^128 + X^7 + X^2 + X^1 + 1
#define bf128_modulus (UINT64_C((1 << 7) | (1 << 2) | (1 << 1) | 1))
// GF(2^192) with X^192 + X^7 + X^2 + X^1 + 1
#define bf192_modulus (UINT64_C((1 << 7) | (1 << 2) | (1 << 1) | 1))
// GF(2^256) with X^256 + X^10 + X^5 + X^2 + 1
#define bf256_modulus (UINT64_C((1 << 10) | (1 << 5) | (1 << 2) | 1))
// GF(2^320) with x^320 + x^4 + x^3 + x + 1
#define bf320_modulus (UINT64_C((1 << 4) | (1 << 3) | (1 << 1) | 1))
// GF(2^512) with x^512 + x^8 + x^5 + x^2 + 1
#define bf512_modulus (UINT64_C((1 << 8) | (1 << 5) | (1 << 2) | 1))


// GF(2^8) implementation
 

bf8_t bf8_mul(bf8_t lhs, bf8_t rhs) {
  bf8_t result = 0;
  for (unsigned int idx = 8; idx; --idx, rhs >>= 1) {
    result ^= (-(rhs & 1)) & lhs;
    const uint8_t mask = -((lhs >> 7) & 1);
    lhs                = (lhs << 1) ^ (mask & bf8_modulus);
  }
  return result;
}

bf8_t bf8_inv(bf8_t in) {
  bf8_t t1 = in;
  bf8_t t2 = in;
  for (size_t i = 0; i < 8 - 2; i++) {
    t2 = bf8_mul(t2, t2);
    t1 = bf8_mul(t1, t2);
  }
  return bf8_mul(t1, t1);
}

// GF(2^64) implementation
 

bf64_t bf64_mul(bf64_t lhs, bf64_t rhs) {
  bf64_t result = 0;
  for (unsigned int idx = 64; idx; --idx, rhs >>= 1) {
    result ^= (-(rhs & 1)) & lhs;
    const uint64_t mask = -((lhs >> 63) & 1);
    lhs                 = (lhs << 1) ^ (mask & bf64_modulus);
  }
  return result;
}

bf64_t bf64_inv(bf64_t in) {
  bf64_t t1 = in;
  bf64_t t2 = in;
  for (size_t i = 0; i < 64 - 2; i++) {
    t2 = bf64_mul(t2, t2);
    t1 = bf64_mul(t1, t2);
  }
  return bf64_mul(t1, t1);
}

// GF(2^128) implementation
 

ATTR_CONST
static inline bf128_t bf128_and(bf128_t lhs, bf128_t rhs) {
  for (unsigned int i = 0; i != ARRAY_SIZE(lhs.values); ++i) {
    lhs.values[i] &= rhs.values[i];
  }
  return lhs;
}

ATTR_CONST
static inline bf128_t bf128_shift_left_1(bf128_t value) {
  value.values[1] = (value.values[1] << 1) | (value.values[0] >> 63);
  value.values[0] = value.values[0] << 1;
  return value;
}

ATTR_CONST
static inline uint64_t bf128_bit_to_uint64_mask(bf128_t value, unsigned int bit) {
  const unsigned int byte_idx = bit / 64;
  const unsigned int bit_idx  = bit % 64;

  return -((value.values[byte_idx] >> bit_idx) & 1);
}

ATTR_CONST
static inline bf128_t bf128_bit_to_mask(bf128_t value, unsigned int bit) {
  bf128_t ret;
  ret.values[0] = ret.values[1] = bf128_bit_to_uint64_mask(value, bit);
  return ret;
}

bf128_t bf128_mul(bf128_t lhs, bf128_t rhs) {
  bf128_t result = {0};
  for (unsigned int idx = 0; idx != 128; ++idx) {
    result = bf128_add(result, bf128_and(bf128_bit_to_mask(rhs, idx), lhs));

    const uint64_t mask = bf128_bit_to_uint64_mask(lhs, 127);
    lhs                 = bf128_shift_left_1(lhs);
    lhs.values[0] ^= (mask & bf128_modulus);
  }
  return result;
}

ATTR_CONST
static inline bf128_t bf128_bit_to_mask_1(uint8_t bit) {
  bf128_t ret;
  ret.values[0] = ret.values[1] = -(bit & 1);
  return ret;
}

 

bf128_t bf128_inv(bf128_t in) {
  bf128_t t1 = in;
  bf128_t t2 = in;
  for (size_t i = 0; i < 128 - 2; i++) {
    t2 = bf128_mul(t2, t2);
    t1 = bf128_mul(t1, t2);
  }
  return bf128_mul(t1, t1);
}

bf128_t bf128_sum_poly(const bf128_t* xs) {
  bf128_t ret   = bf128_zero();
  bf128_t alpha = bf128_from_bf64(1);
  for (size_t i = 0; i < 128; ++i, alpha = bf128_shift_left_1(alpha)) {
    ret = bf128_add(ret, bf128_mul(alpha, xs[i]));
  }
  return ret;
}

// GF(2^192) implementation 

ATTR_CONST
static inline bf192_t bf192_and(bf192_t lhs, bf192_t rhs) {
  for (unsigned int i = 0; i != ARRAY_SIZE(lhs.values); ++i) {
    lhs.values[i] &= rhs.values[i];
  }
  return lhs;
}

ATTR_CONST
static inline bf192_t bf192_shift_left_1(bf192_t value) {
  value.values[2] = (value.values[2] << 1) | (value.values[1] >> 63);
  value.values[1] = (value.values[1] << 1) | (value.values[0] >> 63);
  value.values[0] = value.values[0] << 1;
  return value;
}

ATTR_CONST
static inline uint64_t bf192_bit_to_uint64_mask(bf192_t value, unsigned int bit) {
  const unsigned int byte_idx = bit / 64;
  const unsigned int bit_idx  = bit % 64;

  return -((value.values[byte_idx] >> bit_idx) & 1);
}

ATTR_CONST
static inline bf192_t bf192_bit_to_mask(bf192_t value, unsigned int bit) {
  bf192_t ret;
  ret.values[0] = ret.values[1] = ret.values[2] = bf192_bit_to_uint64_mask(value, bit);
  return ret;
}

bf192_t bf192_mul(bf192_t lhs, bf192_t rhs) {
  bf192_t result = {0};
  for (unsigned int idx = 0; idx != 192; ++idx) {
    result = bf192_add(result, bf192_and(bf192_bit_to_mask(rhs, idx), lhs));

    const uint64_t mask = bf192_bit_to_uint64_mask(lhs, 191);
    lhs                 = bf192_shift_left_1(lhs);
    lhs.values[0] ^= (mask & bf192_modulus);
  }
  return result;
}

ATTR_CONST
static bf192_t bf192_bit_to_mask_1(uint8_t bit) {
  bf192_t ret;
  ret.values[0] = ret.values[1] = ret.values[2] = -(bit & 1);
  return ret;
}

 
bf192_t bf192_inv(bf192_t in) {
  bf192_t t1 = in;
  bf192_t t2 = in;
  for (size_t i = 0; i < 192 - 2; i++) {
    t2 = bf192_mul(t2, t2);
    t1 = bf192_mul(t1, t2);
  }
  return bf192_mul(t1, t1);
}

bf192_t bf192_sum_poly(const bf192_t* xs) {
  bf192_t ret   = bf192_zero();
  bf192_t alpha = bf192_from_bf64(1);
  for (size_t i = 0; i < 192; ++i, alpha = bf192_shift_left_1(alpha)) {
    ret = bf192_add(ret, bf192_mul(alpha, xs[i]));
  }
  return ret;
}

// GF(2^256) implementation 
ATTR_CONST
static inline bf256_t bf256_and(bf256_t lhs, bf256_t rhs) {
  for (unsigned int i = 0; i != ARRAY_SIZE(lhs.values); ++i) {
    lhs.values[i] &= rhs.values[i];
  }
  return lhs;
}

ATTR_CONST
static inline bf256_t bf256_shift_left_1(bf256_t value) {
  value.values[3] = (value.values[3] << 1) | (value.values[2] >> 63);
  value.values[2] = (value.values[2] << 1) | (value.values[1] >> 63);
  value.values[1] = (value.values[1] << 1) | (value.values[0] >> 63);
  value.values[0] = value.values[0] << 1;
  return value;
}

ATTR_CONST
static inline uint64_t bf256_bit_to_uint64_mask(bf256_t value, unsigned int bit) {
  const unsigned int byte_idx = bit / 64;
  const unsigned int bit_idx  = bit % 64;

  return -((value.values[byte_idx] >> bit_idx) & 1);
}

ATTR_CONST
static inline bf256_t bf256_bit_to_mask(bf256_t value, unsigned int bit) {
  bf256_t ret;
  ret.values[0] = ret.values[1] = ret.values[2] = ret.values[3] =
      bf256_bit_to_uint64_mask(value, bit);
  return ret;
}

bf256_t bf256_mul(bf256_t lhs, bf256_t rhs) {
  bf256_t result = {0};
  for (unsigned int idx = 0; idx != 256; ++idx) {
    result = bf256_add(result, bf256_and(bf256_bit_to_mask(rhs, idx), lhs));

    const uint64_t mask = bf256_bit_to_uint64_mask(lhs, 255);
    lhs                 = bf256_shift_left_1(lhs);
    lhs.values[0] ^= (mask & bf256_modulus);
  }
  return result;
}

ATTR_CONST
static inline bf256_t bf256_bit_to_mask_1(uint8_t bit) {
  bf256_t ret;
  ret.values[0] = ret.values[1] = ret.values[2] = ret.values[3] = -(bit & 1);
  return ret;
}

 

bf256_t bf256_inv(bf256_t in) {
  bf256_t t1 = in;
  bf256_t t2 = in;
  for (size_t i = 0; i < 256 - 2; i++) {
    t2 = bf256_mul(t2, t2);
    t1 = bf256_mul(t1, t2);
  }
  return bf256_mul(t1, t1);
}

bf256_t bf256_sum_poly(const bf256_t* xs) {
  bf256_t ret   = bf256_zero();
  bf256_t alpha = bf256_from_bf64(1);
  for (size_t i = 0; i < 256; ++i, alpha = bf256_shift_left_1(alpha)) {
    ret = bf256_add(ret, bf256_mul(alpha, xs[i]));
  }
  return ret;
}


// GF(2^320) implementation 
ATTR_CONST
static inline bf320_t bf320_and(bf320_t lhs, bf320_t rhs) {
  for (unsigned int i = 0; i != ARRAY_SIZE(lhs.values); ++i) {
    lhs.values[i] &= rhs.values[i];
  }
  return lhs;
}

ATTR_CONST
static inline bf320_t bf320_shift_left_1(bf320_t value) {
  value.values[4] = (value.values[4] << 1) | (value.values[3] >> 63);
  value.values[3] = (value.values[3] << 1) | (value.values[2] >> 63);
  value.values[2] = (value.values[2] << 1) | (value.values[1] >> 63);
  value.values[1] = (value.values[1] << 1) | (value.values[0] >> 63);
  value.values[0] = value.values[0] << 1;
  return value;
}

ATTR_CONST
static inline uint64_t bf320_bit_to_uint64_mask(bf320_t value, unsigned int bit) {
  const unsigned int byte_idx = bit / 64;
  const unsigned int bit_idx  = bit % 64;

  return -((value.values[byte_idx] >> bit_idx) & 1);
}

ATTR_CONST
static inline bf320_t bf320_bit_to_mask(bf320_t value, unsigned int bit) {
  bf320_t ret;
  ret.values[0] = ret.values[1] = ret.values[2] = ret.values[3] = ret.values[4] =
      bf320_bit_to_uint64_mask(value, bit);
  return ret;
}

bf320_t bf320_mul(bf320_t lhs, bf320_t rhs) {
  bf320_t result = {0};
  for (unsigned int idx = 0; idx != 320; ++idx) {
    result = bf320_add(result, bf320_and(bf320_bit_to_mask(rhs, idx), lhs));

    const uint64_t mask = bf320_bit_to_uint64_mask(lhs, 319);
    lhs                 = bf320_shift_left_1(lhs);
    lhs.values[0] ^= (mask & bf320_modulus);
  }
  return result;
}

ATTR_CONST
static inline bf320_t bf320_bit_to_mask_1(uint8_t bit) {
  bf320_t ret;
  ret.values[0] = ret.values[1] = ret.values[2] = ret.values[3] = ret.values[4] = -(bit & 1);
  return ret;
}

 

bf320_t bf320_inv(bf320_t in) {
  bf320_t t1 = in;
  bf320_t t2 = in;
  for (size_t i = 0; i < 320 - 2; i++) {
    t2 = bf320_mul(t2, t2);
    t1 = bf320_mul(t1, t2);
  }
  return bf320_mul(t1, t1);
}

bf320_t bf320_sum_poly(const bf320_t* xs) {
  bf320_t ret   = bf320_zero();
  bf320_t alpha = bf320_from_bf64(1);
  for (size_t i = 0; i < 320; ++i, alpha = bf320_shift_left_1(alpha)) {
    ret = bf320_add(ret, bf320_mul(alpha, xs[i]));
  }
  return ret;
}



// GF(2^512) implementation 
ATTR_CONST
static inline bf512_t bf512_and(bf512_t lhs, bf512_t rhs) {
  for (unsigned int i = 0; i != ARRAY_SIZE(lhs.values); ++i) {
    lhs.values[i] &= rhs.values[i];
  }
  return lhs;
}

ATTR_CONST
static inline bf512_t bf512_shift_left_1(bf512_t value) {
  value.values[7] = (value.values[7] << 1) | (value.values[6] >> 63);
  value.values[6] = (value.values[6] << 1) | (value.values[5] >> 63);
  value.values[5] = (value.values[5] << 1) | (value.values[4] >> 63);
  value.values[4] = (value.values[4] << 1) | (value.values[3] >> 63);
  value.values[3] = (value.values[3] << 1) | (value.values[2] >> 63);
  value.values[2] = (value.values[2] << 1) | (value.values[1] >> 63);
  value.values[1] = (value.values[1] << 1) | (value.values[0] >> 63);
  value.values[0] = value.values[0] << 1;
  return value;
}

ATTR_CONST
static inline uint64_t bf512_bit_to_uint64_mask(bf512_t value, unsigned int bit) {
  const unsigned int byte_idx = bit / 64;
  const unsigned int bit_idx  = bit % 64;

  return -((value.values[byte_idx] >> bit_idx) & 1);
}

ATTR_CONST
static inline bf512_t bf512_bit_to_mask(bf512_t value, unsigned int bit) {
  bf512_t ret;
  ret.values[0] = ret.values[1] = ret.values[2] = ret.values[3] = ret.values[4] = ret.values[5] = ret.values[6] = ret.values[7] =
      bf512_bit_to_uint64_mask(value, bit);
  return ret;
}

bf512_t bf512_mul(bf512_t lhs, bf512_t rhs) {
  bf512_t result = {0};
  for (unsigned int idx = 0; idx != 512; ++idx) {
    result = bf512_add(result, bf512_and(bf512_bit_to_mask(rhs, idx), lhs));

    const uint64_t mask = bf512_bit_to_uint64_mask(lhs, 511);
    lhs                 = bf512_shift_left_1(lhs);
    lhs.values[0] ^= (mask & bf512_modulus);
  }
  return result;
}

ATTR_CONST
static inline bf512_t bf512_bit_to_mask_1(uint8_t bit) {
  bf512_t ret;
  ret.values[0] = ret.values[1] = ret.values[2] = ret.values[3] = ret.values[4] = ret.values[5] = ret.values[6] = ret.values[7] = -(bit & 1);
  return ret;
}

 

bf512_t bf512_inv(bf512_t in) {
  bf512_t t1 = in;
  bf512_t t2 = in;
  for (size_t i = 0; i < 512 - 2; i++) {
    t2 = bf512_mul(t2, t2);
    t1 = bf512_mul(t1, t2);
  }
  return bf512_mul(t1, t1);
}

bf512_t bf512_sum_poly(const bf512_t* xs) {
  bf512_t ret   = bf512_zero();
  bf512_t alpha = bf512_from_bf64(1);
  for (size_t i = 0; i < 512; ++i, alpha = bf512_shift_left_1(alpha)) {
    ret = bf512_add(ret, bf512_mul(alpha, xs[i]));
  }
  return ret;
}
