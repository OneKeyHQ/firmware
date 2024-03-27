#include "bignum.h"
#include "coin.h"
#include "common_defs.h"
#include "crypto_helper.h"
#include "parser_impl.h"
#include "parser_txdef.h"
#include "substrate/substrate_dispatch.h"
#include "substrate/substrate_types.h"

extern char polkadot_network[32];
uint16_t __polkadot_dicimal = COIN_AMOUNT_DECIMAL_PLACES;
char __polkadot_ticker[8] = {0};

static parser_error_t parser_init_context(parser_context_t *ctx,
                                          const uint8_t *buffer,
                                          uint16_t bufferSize) {
  ctx->offset = 0;
  ctx->buffer = NULL;
  ctx->bufferLen = 0;

  if (bufferSize == 0 || buffer == NULL) {
    // Not available, use defaults
    return parser_init_context_empty;
  }

  ctx->buffer = buffer;
  ctx->bufferLen = bufferSize;
  return parser_ok;
}

parser_error_t polkadot_parser_init(parser_context_t *ctx,
                                    const uint8_t *buffer,
                                    uint16_t bufferSize) {
  CHECK_PARSER_ERR(parser_init_context(ctx, buffer, bufferSize))
  return parser_ok;
}

const char *polkadot_parser_getErrorDescription(parser_error_t err) {
  switch (err) {
    // General errors
    case parser_ok:
      return "No error";
    case parser_no_data:
      return "No more data";
    case parser_init_context_empty:
      return "Initialized empty context";
    case parser_display_idx_out_of_range:
      return "display_idx_out_of_range";
    case parser_display_page_out_of_range:
      return "display_page_out_of_range";
    // Coin specific
    case parser_spec_not_supported:
      return "Spec version not supported";
    case parser_tx_version_not_supported:
      return "Txn version not supported";
    case parser_not_allowed:
      return "Not allowed";
    case parser_not_supported:
      return "Not supported";
    case parser_unexpected_buffer_end:
      return "Unexpected buffer end";
    case parser_unexpected_value:
      return "Unexpected value";
    case parser_value_out_of_range:
      return "Value out of range";
    case parser_value_too_many_bytes:
      return "Value too many bytes";
    case parser_unexpected_module:
      return "Unexpected module index";
    case parser_unexpected_callIndex:
      return "Method not supported";
    case parser_unexpected_unparsed_bytes:
      return "Unexpected unparsed bytes";
    case parser_print_not_supported:
      return "Value cannot be printed";
    case parser_tx_nesting_not_supported:
      return "Call nesting not supported";
    case parser_tx_nesting_limit_reached:
      return "Max nested calls reached";
    case parser_tx_call_vec_too_large:
      return "Call vector exceeds limit";
    default:
      return "Unrecognized error code";
  }
}

GEN_DEF_READFIX_UNSIGNED(8)

GEN_DEF_READFIX_UNSIGNED(16)

GEN_DEF_READFIX_UNSIGNED(32)

GEN_DEF_READFIX_UNSIGNED(64)

parser_error_t _substrate_readBool(parser_context_t *c, pd_bool_t *v) {
  CHECK_INPUT()

  const uint8_t p = *(c->buffer + c->offset);
  CTX_CHECK_AND_ADVANCE(c, 1)

  switch (p) {
    case 0x00:
      *v = false;
      break;
    case 0x01:
      *v = true;
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readCompactInt(parser_context_t *c, compactInt_t *v) {
  CHECK_INPUT()

  v->ptr = c->buffer + c->offset;
  const uint8_t mode =
      *v->ptr & 0x03u;  // get mode from two least significant bits

  uint64_t tmp;
  switch (mode) {
    case 0:  // single byte
      v->len = 1;
      CTX_CHECK_AND_ADVANCE(c, v->len)
      _getValue(v, &tmp);
      break;
    case 1:  // 2-byte
      v->len = 2;
      CTX_CHECK_AND_ADVANCE(c, v->len)
      _getValue(v, &tmp);
      break;
    case 2:  // 4-byte
      v->len = 4;
      CTX_CHECK_AND_ADVANCE(c, v->len)
      _getValue(v, &tmp);
      break;
    case 3:  // bigint
      v->len = (*v->ptr >> 2u) + 4 + 1;
      CTX_CHECK_AND_ADVANCE(c, v->len)
      break;
    default:
      // this is actually impossible
      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _getValue(const compactInt_t *c, uint64_t *v) {
  *v = 0;

  switch (c->len) {
    case 1:
      *v = (*c->ptr) >> 2u;
      break;
    case 2:
      *v = (*c->ptr) >> 2u;
      *v += *(c->ptr + 1) << 6u;
      if (*v < 64) {
        return parser_value_out_of_range;
      }
      break;
    case 4:
      *v = (*c->ptr) >> 2u;
      *v += *(c->ptr + 1) << 6u;
      *v += *(c->ptr + 2) << (8u + 6u);
      *v += *(c->ptr + 3) << (16u + 6u);
      if (*v < 16383) {
        return parser_value_out_of_range;
      }
      break;
    default:
      return parser_value_out_of_range;
  }

  return parser_ok;
}

parser_error_t _toStringCompactInt(const compactInt_t *c, uint8_t decimalPlaces,
                                   bool trimTrailingZeros, char postfix[],
                                   char prefix[], char *outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t *pageCount) {
  char bufferUI[200];
  memset(outValue, 0, outValueLen);
  memset(bufferUI, 0, sizeof(bufferUI));
  *pageCount = 1;

  if (c->len <= 4) {
    uint64_t v;
    _getValue(c, &v);
    if (uint64_to_str(bufferUI, sizeof(bufferUI), v) != NULL) {
      return parser_unexpected_value;
    }
  } else {
    // This is longer number
    uint8_t bcdOut[100];
    const uint16_t bcdOutLen = sizeof(bcdOut);

    bignumLittleEndian_to_bcd(bcdOut, bcdOutLen, c->ptr + 1, c->len - 1);
    if (!bignumLittleEndian_bcdprint(bufferUI, sizeof(bufferUI), bcdOut,
                                     bcdOutLen))
      return parser_unexpected_buffer_end;
  }

  // Format number
  if (intstr_to_fpstr_inplace(bufferUI, sizeof(bufferUI), decimalPlaces) == 0) {
    return parser_unexpected_value;
  }

  if (trimTrailingZeros) {
    number_inplace_trimming(bufferUI, 0);
    const size_t len = strlen(bufferUI);
    if (bufferUI[len - 1] == '.') bufferUI[len - 1] = '\0';
  }

  if (z_str3join(bufferUI, sizeof(bufferUI), prefix, postfix) != zxerr_ok) {
    return parser_unexpected_buffer_end;
  }

  pageString(outValue, outValueLen, bufferUI, pageIdx, pageCount);

  return parser_ok;
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

parser_error_t _readCallIndex(parser_context_t *c, pd_CallIndex_t *v) {
  CHECK_INPUT()

  CHECK_ERROR(_preadUInt8(c, &v->moduleIdx))
  CHECK_ERROR(_preadUInt8(c, &v->idx))
  return parser_ok;
}

parser_error_t _readEra(parser_context_t *c, pd_ExtrinsicEra_t *v) {
  CHECK_INPUT()
  //  https://github.com/paritytech/substrate/blob/fc3adc87dc806237eb7371c1d21055eea1702be0/core/sr-primitives/src/generic/era.rs#L117

  v->type = eEraImmortal;
  v->period = 0;
  v->phase = 0;

  uint8_t first;
  CHECK_ERROR(_preadUInt8(c, &first))
  if (first == 0) {
    return parser_ok;
  }

  v->type = eEraMortal;
  uint64_t encoded = first;
  CHECK_ERROR(_preadUInt8(c, &first))
  encoded += (uint64_t)first << 8u;

  v->period = 2U << (encoded % (1u << 4u));
  uint64_t quantize_factor = (v->period >> 12u);
  quantize_factor = (quantize_factor == 0 ? 1 : quantize_factor);

  v->phase = (encoded >> 4u) * quantize_factor;

  if (v->period >= 4 && v->phase < v->period) {
    return parser_ok;
  }

  return parser_unexpected_value;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

parser_error_t _readCompactIndex(parser_context_t *c, pd_CompactIndex_t *v) {
  CHECK_INPUT()
  CHECK_ERROR(_readCompactInt(c, &v->index))
  return parser_ok;
}

parser_error_t _readCompactBalance(parser_context_t *c,
                                   pd_CompactBalance_t *v) {
  CHECK_INPUT()
  CHECK_ERROR(_readCompactInt(c, &v->value))
  return parser_ok;
}

parser_error_t _toStringCompactIndex(const pd_CompactIndex_t *v, char *outValue,
                                     uint16_t outValueLen, uint8_t pageIdx,
                                     uint8_t *pageCount) {
  return _toStringCompactInt(&v->index, 0, true, "", "", outValue, outValueLen,
                             pageIdx, pageCount);
}

parser_error_t _toStringCompactBalance(const pd_CompactBalance_t *v,
                                       char *outValue, uint16_t outValueLen,
                                       uint8_t pageIdx, uint8_t *pageCount) {
  CHECK_ERROR(_toStringCompactInt(&v->value, __polkadot_dicimal, true,
                                  __polkadot_ticker, "", outValue, outValueLen,
                                  pageIdx, pageCount))
  return parser_ok;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

parser_error_t _checkVersions(parser_context_t *c) {
  // Methods are not length delimited so in order to retrieve the specVersion
  // it is necessary to parse from the back.
  // The transaction is expect to end in
  // [4 bytes] specVersion
  // [4 bytes] transactionVersion
  // [32 bytes] genesisHash
  // [32 bytes] blockHash
  const uint16_t specOffsetFromBack = 4 + 4 + 32 + 32;
  if (c->bufferLen < specOffsetFromBack) {
    return parser_unexpected_buffer_end;
  }

  uint8_t *p = (uint8_t *)(c->buffer + c->bufferLen - specOffsetFromBack);
  uint32_t specVersion = 0;
  specVersion += (uint32_t)p[0] << 0u;
  specVersion += (uint32_t)p[1] << 8u;
  specVersion += (uint32_t)p[2] << 16u;
  specVersion += (uint32_t)p[3] << 24u;

  p += 4;
  uint32_t transactionVersion = 0;
  transactionVersion += (uint32_t)p[0] << 0u;
  transactionVersion += (uint32_t)p[1] << 8u;
  transactionVersion += (uint32_t)p[2] << 16u;
  transactionVersion += (uint32_t)p[3] << 24u;

  // transactionVersion
  // if (transactionVersion != (SUPPORTED_TX_VERSION_CURRENT) &&
  //     transactionVersion != (SUPPORTED_TX_VERSION_PREVIOUS)) {
  //   return parser_tx_version_not_supported;
  // }

  // if (specVersion < SUPPORTED_MINIMUM_SPEC_VERSION) {
  //   return parser_spec_not_supported;
  // }

  c->tx_obj->specVersion = specVersion;
  c->tx_obj->transactionVersion = transactionVersion;

  return parser_ok;
}

uint16_t __address_type;

uint16_t _getAddressType() { return __address_type; }

uint16_t _detectAddressType(const parser_context_t *c) {
  (void)c;
  memset(__polkadot_ticker, 0, sizeof(__polkadot_ticker));
  if (!strncmp(polkadot_network, "polkadot", 8)) {
    __polkadot_dicimal = COIN_AMOUNT_DECIMAL_PLACES;
    memcpy(__polkadot_ticker, COIN_TICKER, 4);
    return 0;
  } else if (!strncmp(polkadot_network, "kusama", 6)) {
    __polkadot_dicimal = COIN_AMOUNT_DECIMAL_PLACES_12;
    memcpy(__polkadot_ticker, KUSAMA_COIN_TICKER, 4);
    return 2;
  } else if (!strncmp(polkadot_network, "astar", 5)) {
    __polkadot_dicimal = COIN_AMOUNT_DECIMAL_PLACES_18;
    memcpy(__polkadot_ticker, ASTAR_COIN_TICKER, 5);
    return 5;
  } else if (!strncmp(polkadot_network, "westend", 7)) {
    __polkadot_dicimal = COIN_AMOUNT_DECIMAL_PLACES_12;
    memcpy(__polkadot_ticker, WESTEND_COIN_TICKER, 4);
    return 42;
  } else if (!strncmp(polkadot_network, "joystream", 9)) {
    __polkadot_dicimal = COIN_AMOUNT_DECIMAL_PLACES;
    memcpy(__polkadot_ticker, JOY_COIN_TICKER, 4);
    return 126;
  } else if (!strncmp(polkadot_network, "manta", 5)) {
    __polkadot_dicimal = COIN_AMOUNT_DECIMAL_PLACES_18;
    memcpy(__polkadot_ticker, MANTA_COIN_TICKER, 6);
    return 77;
  }

  return 42;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

parser_error_t _readAddress(parser_context_t *c, pd_Address_t *v) {
  CHECK_INPUT()
  // Based on
  // https://github.com/paritytech/substrate/blob/fc3adc87dc806237eb7371c1d21055eea1702be0/srml/indices/src/address.rs#L66

  uint8_t tmp;
  CHECK_ERROR(_preadUInt8(c, &tmp))

  switch (tmp) {
    case 0xFF: {
      v->type = eAddressId;
      v->idPtr = c->buffer + c->offset;
      CTX_CHECK_AND_ADVANCE(c, 32)
      break;
    }
    case 0xFE: {
      compactInt_t ci;
      CHECK_ERROR(_readCompactInt(c, &ci))

      v->type = eAddressIndex;
      CHECK_ERROR(_getValue(&ci, &v->idx))

      if (v->idx <= 0xffffffffu) {
        return parser_unexpected_value;
      }
      break;
    }
    case 0xFD: {
      uint32_t tmpval;
      CHECK_ERROR(_preadUInt32(c, &tmpval))
      v->type = eAddressIndex;
      v->idx = tmpval;
      if (v->idx <= 0xFFFF) {
        return parser_unexpected_value;
      }
      break;
    }
    case 0xFC: {
      uint16_t tmpval;
      CHECK_ERROR(_preadUInt16(c, &tmpval))
      v->type = eAddressIndex;
      v->idx = tmpval;
      if (v->idx <= 0xEF) {
        return parser_unexpected_value;
      }
      break;
    }
    default:
      if (tmp <= 0xEF) {
        v->type = eAddressIndex;
        v->idx = tmp;
        return parser_ok;
      }

      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _toStringPubkeyAsAddress(const uint8_t *pubkey, char *outValue,
                                        uint16_t outValueLen, uint8_t pageIdx,
                                        uint8_t *pageCount) {
  char bufferUI[200];

  if (crypto_SS58EncodePubkey((uint8_t *)bufferUI, sizeof(bufferUI),
                              __address_type, pubkey) == 0) {
    return parser_no_data;
  }

  pageString(outValue, outValueLen, bufferUI, pageIdx, pageCount);
  if (pageIdx >= *pageCount) {
    return parser_no_data;
  }
  return parser_ok;
}
