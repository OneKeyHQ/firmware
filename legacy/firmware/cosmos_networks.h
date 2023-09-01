#ifndef _COSMOS_NETWORK_H__
#define _COSMOS_NETWORK_H__

#include <stdint.h>

#define COSMOS_NETWORK_COUNT 29

typedef struct {
  const char *const chain_id;
  const char *const hrp;
  const char *const chain_name;
  const char *const coin_denom;
  const char *const coin_minimal_denom;
  int decimals;
} CosmosNetworkType;

extern const CosmosNetworkType cosmos_networks[COSMOS_NETWORK_COUNT];

const CosmosNetworkType *cosmosnetworkByChainId(const char *chain_id);
const CosmosNetworkType *cosmosnetworkByHrp(const char *hrp);

#endif
