/*
 *  SPDX-License-Identifier: MIT
 */

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include "instances.h"
#include "parameters.h"

const char* faest_get_param_name(faest_paramid_t paramid) {
  switch (paramid) {
  case PARAMETER_SET_INVALID:
    return "PARAMETER_SET_INVALID";
  case FAEST_128S:
    return "FAEST_128S";
  case FAEST_128F:
    return "FAEST_128F";
  default:
    return "PARAMETER_SET_MAX_INDEX";
  }
}

#define FAEST_128S_PARAMS                                                                          \
  {                                                                                                \
    FAEST_128S_LAMBDA, FAEST_128S_Nwd,                \
        FAEST_128S_L,  FAEST_128S_TAU, FAEST_128S_K0,              \
        FAEST_128S_K1, FAEST_128S_T0, FAEST_128S_T1, FAEST_128S_SIG_SIZE, FAEST_128S_PK_SIZE,       \
        FAEST_128S_M, FAEST_128S_N, FAEST_128S_W, FAEST_128S_D       \
  }
#define FAEST_128F_PARAMS                                                                          \
  {                                                                                                \
    FAEST_128F_LAMBDA, FAEST_128F_Nwd,                \
        FAEST_128F_L,  FAEST_128F_TAU, FAEST_128F_K0,              \
        FAEST_128F_K1, FAEST_128F_T0, FAEST_128F_T1, FAEST_128F_SIG_SIZE, FAEST_128F_PK_SIZE,       \
        FAEST_128F_M, FAEST_128F_N, FAEST_128F_W, FAEST_128F_D       \
  }
#define FAEST_INVALID_PARAMS                                                                       \
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0 }

faest_paramset_t faest_get_paramset(faest_paramid_t paramid) {
  switch (paramid) {
  case FAEST_128S:
    return (faest_paramset_t){FAEST_128S_PARAMS, FAEST_128S};
  case FAEST_128F:
    return (faest_paramset_t){FAEST_128S_PARAMS, FAEST_128F};
  default:
    return (faest_paramset_t){FAEST_INVALID_PARAMS, PARAMETER_SET_INVALID};
  }
}
