#include "tron_tokens.h"
#include <string.h>

#define TRON_TOKENS_COUNT 14
const TronTokenType tron_tokens[TRON_TOKENS_COUNT] = {
    {"TR7NHqjeKQxGTCi8q8ZY4pL8otSzgjLj6t", " USDT", 6},
    {"TNUC9Qb1rRpS5CbWLmNMxXBjyFoydXjWFR", " WTRX", 6},
    {"TKfjV9RNKJJCqPvBtK8L7Knykh7DNWvnYt", " WBTT", 6},
    {"TCFLL5dx5ZJdKnWuesXxi1VPwjLVmWZZy9", " JST", 18},
    {"TLa2f6VPqDgRE67v1736s7bJ8Ray5wYjU7", " WIN", 6},
    {"TSSMHYeV2uE9qYH95DqyoCuNCzEL1NvU3S", " SUN", 18},
    {"TXpw8XeWYeTUd4quDskoUqeQPowRh4jY65", " WBTC", 8},
    {"THb4CqiFdwNHsWsQCs4JhzwjMWys4aqCbF", " ETH", 18},
    {"TEkxiTehnzSmSe2XqrBj4w32RUN966rdz8", " USDC", 6},
    {"TMwFHYXLJaRUPeW6421aqXL4ZEzPRFGkGT", " USDJ", 18},
    {"TUpMhErZL2fhh4sVNULAbNKLokS4GjC1F4", " TUSD", 18},
    {"TFczxzPhnThNSqr5by8tvxsdCFRRz6cPNq", " NFT", 6},
    {"TDyvndWuvX5xTBwHPYJi7J3Yq8pq8yh62h", " HT", 18},
    {"THbVQp8kMjStKNnf2iCY6NEzThKMK5aBHg", " DOGE", 8},
};

ConstTronTokenPtr get_tron_token_by_address(const char *address) {
  for (int i = 0; i < TRON_TOKENS_COUNT; i++) {
    if (memcmp(address, tron_tokens[i].address, 35) != 0) continue;
    return &tron_tokens[i];
  }

  return NULL;
}
