#include "algo_asa.h"
#include <stdio.h>
#include "stdint.h"

#define ARRAY_SIZE(__arr) (sizeof(__arr) / sizeof(__arr[0]))

#define ALGO_ASA(__id, __name, __unit, __decimals) \
  { .assetId = __id, .decimals = __decimals, .unit = __unit, .name = __name, }

static const algo_asset_info_t algo_assets[] = {
    ALGO_ASA(438840, "Micro-Tesla", "M-TSLA ", 0),
    ALGO_ASA(438839, "Micro-Apple", "M-AAPL ", 0),
    ALGO_ASA(438838, "Micro-Google", "M-GOOGL ", 0),
    ALGO_ASA(438837, "Micro-Netflix", "M-NFLX ", 0),
    ALGO_ASA(438836, "Micro-Twitter", "M-TWTR ", 0),
    ALGO_ASA(438833, "Micro-Amazon", "M-AMZN ", 0),
    ALGO_ASA(438832, "Micro-Microsoft", "M-MSFT ", 0),
    ALGO_ASA(438831, "MESE Index Fund", "MESX ", 6),
    ALGO_ASA(438828, "MESE USD Exchange Token", "USD-MESE ", 6),
    ALGO_ASA(312769, "Tether USDt", "USDt ", 6),
    ALGO_ASA(31566704, "USDC", "USDC ", 6),
    ALGO_ASA(6587142, "Meld Silver", "MCAG ", 5),
    ALGO_ASA(6547014, "Meld Gold", "MCAU ", 5),
    ALGO_ASA(2838934, "Credit Opportunities Fund I", "VAL 1 ", 0),
    ALGO_ASA(2836760, "Liquid Mining Fund I", "RHO 1 ", 0),
    ALGO_ASA(2757561, "realioUSD", "RUSD ", 7),
    ALGO_ASA(2751733, "Realio Token", "RIO ", 7),
    ALGO_ASA(2725935, "Realio Security Token", "RST ", 7),
    ALGO_ASA(27165954, "PLANET", "PLANETS ", 6),
    ALGO_ASA(163650, "Asia Reserve Currency Coin", "ARCC ", 6),
    ALGO_ASA(137594422, "HEADLINE", "HDL ", 6),
};

const algo_asset_info_t *algo_asa_get(uint64_t id) {
  const algo_asset_info_t *p;
  const algo_asset_info_t *endp = algo_assets + ARRAY_SIZE(algo_assets);

  for (p = algo_assets; p && p < endp; p++) {
    if (p->assetId == id) {
      return p;
    }
  }
  return NULL;
}