#include "parser_impl.h"
#include "bignum.h"
#include "coin.h"
#include "crypto_helper.h"
#include "parser_common.h"
#include "parser_txdef.h"
#include "substrate/substrate_dispatch.h"
#include "substrate/substrate_types.h"

extern uint16_t __address_type;

parser_error_t _readTx(parser_context_t *c, parser_tx_t *v) {
  CHECK_INPUT()
  //  Reverse parse to retrieve spec before forward parsing
  CHECK_ERROR(_checkVersions(c))

  // Now forward parse
  CHECK_ERROR(_readCallIndex(c, &v->callIndex))
  CHECK_ERROR(
      _readMethod(c, v->callIndex.moduleIdx, v->callIndex.idx, &v->method))
  CHECK_ERROR(_readEra(c, &v->era))
  CHECK_ERROR(_readCompactIndex(c, &v->nonce))
  CHECK_ERROR(_readCompactBalance(c, &v->tip))
  CHECK_ERROR(_preadUInt32(c, &v->specVersion))
  CHECK_ERROR(_preadUInt32(c, &v->transactionVersion))
  CHECK_ERROR(_readHash(c, &v->genesisHash))
  CHECK_ERROR(_readHash(c, &v->blockHash))

  if (c->offset < c->bufferLen) {
    return parser_unexpected_unparsed_bytes;
  }

  if (c->offset > c->bufferLen) {
    return parser_unexpected_buffer_end;
  }

  __address_type = _detectAddressType(c);

  return parser_ok;
}
