#include "cosmos_networks.h"
#include <string.h>

const CosmosNetworkType cosmos_networks[COSMOS_NETWORK_COUNT] = {
    {"cosmoshub-4", "cosmos", "Cosmos Hub", "ATOM", "uatom", 6},
    {"osmosis-1", "osmo", "Osmosis", "OSMO", "uosmo", 6},
    {"secret-4", "secret", "Secret Network", "SCRT", "uscrt", 6},
    {"akashnet-2", "akash", "Akash", "AKT", "uakt", 6},
    {"crypto-org-chain-mainnet-1", "cro", "Crypto.org", "CRO", "basecro", 8},
    {"iov-mainnet-ibc", "star", "Starname", "IOV", "uiov", 6},
    {"sifchain-1", "sif", "Sifchain", "ROWAN", "rowan", 18},
    {"shentu-2.2", "certik", "Shentu", "CTK", "uctk", 6},
    {"irishub-1", "iaa", "IRISnet", "IRIS", "uiris", 6},
    {"regen-1", "regen", "Regen", "REGEN", "uregen", 6},
    {"core-1", "persistence", "Persistence", "XPRT", "uxprt", 6},
    {"sentinelhub-2", "sent", "Sentinel", "DVPN", "udvpn", 6},
    {"ixo-4", "ixo", "ixo", "IXO", "uixo", 6},
    {"emoney-3", "emoney", "e-Money", "NGM", "ungm", 6},
    {"agoric-3", "agoric", "Agoric", "BLD", "ubld", 6},
    {"bostrom", "bostrom", "Bostrom", "BOOT", "boot", 0},
    {"juno-1", "juno", "Juno", "JUNO", "ujuno", 6},
    {"stargaze-1", "stars", "Stargaze", "STARS", "ustars", 6},
    {"axelar-dojo-1", "axelar", "Axelar", "AXL", "uaxl", 6},
    {"sommelier-3", "somm", "Sommelier", "SOMM", "usomm", 6},
    {"umee-1", "umee", "Umee", "UMEE", "uumee", 6},
    {"gravity-bridge-3", "gravity", "Gravity Bridge", "GRAV", "ugraviton", 6},
    {"tgrade-mainnet-1", "tgrade", "Tgrade", "TGD", "utgd", 6},
    {"stride-1", "stride", "Stride", "STRD", "ustrd", 6},
    {"evmos_9001-2", "evmos", "Evmos", "EVMOS", "aevmos", 18},
    {"injective-1", "inj", "Injective", "INJ", "inj", 18},
    {"kava_2222-10", "kava", "Kava", "KAVA", "ukava", 6},
    {"quicksilver-1", "quick", "Quicksilver", "QCK", "uqck", 6},
    {"fetchhub-4", "fetch", "Fetch.ai", "FET", "afet", 18},
};

const CosmosNetworkType *cosmosnetworkByChainId(const char *chain_id) {
  for (int i = 0; i < COSMOS_NETWORK_COUNT; i++) {
    if (memcmp(chain_id, cosmos_networks[i].chain_id, strlen(chain_id)) == 0) {
      return &(cosmos_networks[i]);
    }
  }
  return NULL;
}

const CosmosNetworkType *cosmosnetworkByHrp(const char *hrp) {
  for (int i = 0; i < COSMOS_NETWORK_COUNT; i++) {
    if (memcmp(hrp, cosmos_networks[i].hrp, strlen(hrp)) == 0) {
      return &(cosmos_networks[i]);
    }
  }
  return NULL;
}
