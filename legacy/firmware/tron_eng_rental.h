#ifndef __TRON_ENG_RENTAL_H__
#define __TRON_ENG_RENTAL_H__

#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
  const uint8_t address[35];
  const char *name;
} EnergyRentalProvider;

static const EnergyRentalProvider energy_rental_providers[] = {
    {
        .address = "TLgTYgG5bD9crpYqVED3MGvWuAUvdoFYEa",
        .name = "TrxRes",
    },
};

#define ENERGY_RENTAL_PROVIDERS_COUNT \
  (sizeof(energy_rental_providers) / sizeof(energy_rental_providers[0]))

const EnergyRentalProvider *get_energy_rental_provider(const uint8_t *address) {
  for (size_t i = 0; i < ENERGY_RENTAL_PROVIDERS_COUNT; i++) {
    if (memcmp(energy_rental_providers[i].address, address, 35) == 0) {
      return &energy_rental_providers[i];
    }
  }
  return NULL;
}

bool is_energy_rental_provider(const uint8_t *address) {
  return get_energy_rental_provider(address) != NULL;
}

#endif  // __TRON_ENG_RENTAL_H__
