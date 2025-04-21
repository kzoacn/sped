/*
 *  SPDX-License-Identifier: MIT
 */

#ifndef INSTANCES_H
#define INSTANCES_H

#include "macros.h"

#include <stddef.h>
#include <stdint.h>

#define MAX_LAMBDA 512
#define MAX_LAMBDA_BYTES (MAX_LAMBDA / 8)
#define MAX_DEPTH 12
#define MAX_TAU 128
#define UNIVERSAL_HASH_B_BITS 16
#define UNIVERSAL_HASH_B (UNIVERSAL_HASH_B_BITS / 8)
#define IV_SIZE 16

FAEST_BEGIN_C_DECL

typedef enum faest_paramid_t {
  PARAMETER_SET_INVALID   = 0,
  FAEST_128S              = 1,
  FAEST_128F              = 2,
  FAEST_192S              = 3,
  FAEST_192F              = 4,
  FAEST_256S              = 5,
  FAEST_256F              = 6,
  FAEST_320F              = 7,
  FAEST_512F              = 8,
  PARAMETER_SET_MAX_INDEX = 13
} faest_paramid_t;

typedef struct faest_param_t {
  uint32_t lambda;
  uint32_t Nwd;
  uint32_t l;
  
  uint32_t tau;
  uint32_t k0;
  uint32_t k1;
  uint32_t t0;
  uint32_t t1;
  
  uint32_t sigSize;
  uint32_t pkSize;

  uint32_t m;
  uint32_t n;
  uint32_t w;
  uint32_t d;
} faest_param_t;

typedef struct faest_paramset_t {
  faest_param_t faest_param;
  faest_paramid_t faest_paramid;
} faest_paramset_t;

const char* ATTR_CONST faest_get_param_name(faest_paramid_t paramid);
faest_paramset_t ATTR_CONST faest_get_paramset(faest_paramid_t paramid);

FAEST_END_C_DECL

#endif
