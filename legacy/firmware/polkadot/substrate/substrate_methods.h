#ifndef __POLKADOT_SUBSTRATE_METHODS_H__
#define __POLKADOT_SUBSTRATE_METHODS_H__

#include <stddef.h>
#include <stdint.h>

#define GET_PD_CALL(CALL, VERSION) (PD_CALL_##CALL##_V##VERSION)

#include "substrate_methods_V18.h"
#include "substrate_methods_V19.h"
#include "substrate_types_V18.h"
#include "substrate_types_V19.h"

typedef union {
  pd_Method_V19_t V19;
  pd_Method_V18_t V18;
} pd_Method_t;

typedef union {
  pd_MethodNested_V19_t V19;
  pd_MethodNested_V18_t V18;
} pd_MethodNested_t;

#endif
