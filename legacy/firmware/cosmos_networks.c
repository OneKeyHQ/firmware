#include "cosmos_networks.h"
#include <string.h>

const CosmosNetworkType cosmos_networks[COSMOS_NETWORK_COUNT] = {
    {"cosmoshub-4", "Cosmos Hub", "ATOM", "uatom", 6},
    {"osmosis-1", "Osmosis", "OSMO", "uosmo", 6},
    {"secret-4", "Secret Network", "SCRT", "uscrt", 6},
    {"akashnet-2", "Akash", "AKT", "uakt", 6},
    {"crypto-org-chain-mainnet-1", "Crypto.org", "CRO", "basecro", 8},
    {"iov-mainnet-ibc", "Starname", "IOV", "uiov", 6},
    {"sifchain-1", "Sifchain", "ROWAN", "rowan", 18},
    {"shentu-2.2", "Shentu", "CTK", "uctk", 6},
    {"irishub-1", "IRISnet", "IRIS", "uiris", 6},
    {"regen-1", "Regen", "REGEN", "uregen", 6},
    {"core-1", "Persistence", "XPRT", "uxprt", 6},
    {"sentinelhub-2", "Sentinel", "DVPN", "udvpn", 6},
    {"ixo-4", "ixo", "IXO", "uixo", 6},
    {"emoney-3", "e-Money", "NGM", "ungm", 6},
    {"agoric-3", "Agoric", "BLD", "ubld", 6},
    {"bostrom", "Bostrom", "BOOT", "boot", 0},
    {"juno-1", "Juno", "JUNO", "ujuno", 6},
    {"stargaze-1", "Stargaze", "STARS", "ustars", 6},
    {"axelar-dojo-1", "Axelar", "AXL", "uaxl", 6},
    {"sommelier-3", "Sommelier", "SOMM", "usomm", 6},
    {"umee-1", "Umee", "UMEE", "uumee", 6},
    {"gravity-bridge-3", "Gravity Bridge", "GRAV", "ugraviton", 6},
    {"tgrade-mainnet-1", "Tgrade", "TGD", "utgd", 6},
    {"stride-1", "Stride", "STRD", "ustrd", 6},
    {"evmos_9001-2", "Evmos", "EVMOS", "aevmos", 18},
    {"injective-1", "Injective", "INJ", "inj", 18},
    {"kava_2222-10", "Kava", "KAVA", "ukava", 6},
    {"quicksilver-1", "Quicksilver", "QCK", "uqck", 6},
};

const CosmosNetworkType *cosmosnetworkByChainId(const char *chain_id) {
  for (int i = 0; i < COSMOS_NETWORK_COUNT; i++) {
    if (memcmp(chain_id, cosmos_networks[i].chain_id, strlen(chain_id)) == 0) {
      return &(cosmos_networks[i]);
    }
  }
  return NULL;
}
