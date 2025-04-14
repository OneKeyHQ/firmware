#ifndef H_CARDANO_APP_CBOR
#define H_CARDANO_APP_CBOR

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// temporary handy values
enum {
  CBOR_MT_UNSIGNED = 0,
  CBOR_MT_NEGATIVE = 1,
  CBOR_MT_BYTES = 2,
  CBOR_MT_TEXT = 3,
  CBOR_MT_ARRAY = 4,
  CBOR_MT_MAP = 5,
  CBOR_MT_TAG = 6,
  CBOR_MT_PRIMITIVES = 7,

  CBOR_VALUE_MASK = 0b00011111,
  CBOR_TYPE_MASK = 0b11100000,
  CBOR_INDEF = CBOR_VALUE_MASK,
  CBOR_NULL = 22
};

typedef enum {
  // raw tags
  CBOR_TYPE_UNSIGNED = CBOR_MT_UNSIGNED << 5,
  CBOR_TYPE_NEGATIVE = CBOR_MT_NEGATIVE << 5,
  CBOR_TYPE_BYTES = CBOR_MT_BYTES << 5,
  CBOR_TYPE_TEXT = CBOR_MT_TEXT << 5,
  CBOR_TYPE_ARRAY = CBOR_MT_ARRAY << 5,
  CBOR_TYPE_MAP = CBOR_MT_MAP << 5,
  CBOR_TYPE_TAG = CBOR_MT_TAG << 5,
  CBOR_TYPE_PRIMITIVES = CBOR_MT_PRIMITIVES << 5,

  // tag extensions
  CBOR_TYPE_ARRAY_INDEF = CBOR_TYPE_ARRAY + CBOR_INDEF,
  CBOR_TYPE_INDEF_END = CBOR_TYPE_PRIMITIVES + CBOR_INDEF,
  CBOR_TYPE_NULL = CBOR_TYPE_PRIMITIVES + CBOR_NULL,
  CBOR_TYPE_FALSE = CBOR_TYPE_PRIMITIVES + 0x14,
  CBOR_TYPE_TRUE = CBOR_TYPE_PRIMITIVES + 0x15,
} cbor_type_tag_t;

enum {
  CBOR_TAG_EMBEDDED_CBOR_BYTE_STRING = 24,
  CBOR_SET_TAG = 0x102,  // 258
};

typedef struct {
  uint8_t type;
  uint8_t width;  // Contains number of *additional* bytes carrying the value
  uint64_t value;
} cbor_token_t;

typedef cbor_token_t token_t;  // legacy

// Serializes token into buffer, returning number of written bytes
size_t cbor_writeToken(uint8_t type, uint64_t value, uint8_t* buffer,
                       size_t bufferSize);

cbor_token_t cbor_parseToken(const uint8_t* buf, size_t size);

bool cbor_mapKeyFulfillsCanonicalOrdering(const uint8_t* previousBuffer,
                                          size_t previousSize,
                                          const uint8_t* nextBuffer,
                                          size_t nextSize);

#endif  // H_CARDANO_APP_CBOR
