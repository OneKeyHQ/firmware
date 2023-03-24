#include "cbor.h"

// Note(ppershing): consume functions should either
// a) *consume* expected value, or
// b) *throw* but not consume anything from the stream

static const uint64_t VALUE_W1_UPPER_THRESHOLD = 24;
static const uint64_t VALUE_W2_UPPER_THRESHOLD = (uint64_t)1 << 8;
static const uint64_t VALUE_W4_UPPER_THRESHOLD = (uint64_t)1 << 16;
static const uint64_t VALUE_W8_UPPER_THRESHOLD = (uint64_t)1 << 32;

static inline void u1be_write(uint8_t* outBuffer, uint8_t value) {
  outBuffer[0] = value;
}
static inline void u2be_write(uint8_t* outBuffer, uint16_t value) {
  u1be_write(outBuffer, value >> 8);
  u1be_write(outBuffer + 1, value & 0xFF);
}
static inline void u4be_write(uint8_t* outBuffer, uint32_t value) {
  u2be_write(outBuffer, value >> 16);
  u2be_write(outBuffer + 2, value & 0xFFff);
}
static inline void u8be_write(uint8_t* outBuffer, uint64_t value) {
  u4be_write(outBuffer, (value >> 32));
  u4be_write(outBuffer + 4, value & 0xFFffFFff);
}
static inline uint8_t u1be_read(const uint8_t* inBuffer) { return inBuffer[0]; }

static inline uint16_t u2be_read(const uint8_t* inBuffer) {
  // STATIC_ASSERT(sizeof(uint32_t) == sizeof(unsigned), "bad unsigned size");

  // bitwise OR promotes unsigned types smaller than int to unsigned
  return (uint16_t)(((uint32_t)(u1be_read(inBuffer) << 8)) |
                    ((uint32_t)(u1be_read(inBuffer + 1))));
}

static inline uint32_t u4be_read(const uint8_t* inBuffer) {
  return ((uint32_t)u2be_read(inBuffer) << 16) |
         (uint32_t)(u2be_read(inBuffer + 2));
}

static inline uint64_t u8be_read(const uint8_t* inBuffer) {
  return ((uint64_t)u4be_read(inBuffer) << 32u) |
         (uint64_t)(u4be_read(inBuffer + 4));
}

cbor_token_t cbor_parseToken(const uint8_t* buf, size_t size) {
  const uint8_t tag = buf[0];
  cbor_token_t result;
  if (1 > size) {
    result.type = CBOR_TYPE_NULL;
    return result;
  }

  // tag extensions first
  if (tag == CBOR_TYPE_ARRAY_INDEF || tag == CBOR_TYPE_INDEF_END) {
    result.type = tag;
    result.width = 0;
    result.value = 0;
    return result;
  }

  result.type = tag & CBOR_TYPE_MASK;

  switch (result.type) {
    case CBOR_TYPE_UNSIGNED:
    case CBOR_TYPE_NEGATIVE:
    case CBOR_TYPE_BYTES:
    case CBOR_TYPE_ARRAY:
    case CBOR_TYPE_MAP:
    case CBOR_TYPE_TAG:
      break;
    default:
      // We don't know how to parse others
      // (particularly CBOR_TYPE_PRIMITIVES)
      result.type = CBOR_TYPE_NULL;
      return result;
  }

  const uint8_t val = (tag & CBOR_VALUE_MASK);
  if (val < 24) {
    result.width = 0;
    result.value = val;
    // return result;
  } else {
    // shift buffer
    // Holds minimum value for a given byte-width.
    // Anything below this is not canonical CBOR as
    // it could be represented by a shorter CBOR notation
    uint64_t limit_min;
    switch (val) {
      case 24:
        if (2 > size) {
          result.type = CBOR_TYPE_NULL;
          return result;
        }
        result.width = 1;
        result.value = u1be_read(buf + 1);
        limit_min = VALUE_W1_UPPER_THRESHOLD;
        break;
      case 25:
        if (3 > size) {
          result.type = CBOR_TYPE_NULL;
          return result;
        }
        result.width = 2;
        result.value = u2be_read(buf + 1);
        limit_min = VALUE_W2_UPPER_THRESHOLD;
        break;
      case 26:
        if (5 > size) {
          result.type = CBOR_TYPE_NULL;
          return result;
        }
        result.width = 4;
        result.value = u4be_read(buf + 1);
        limit_min = VALUE_W4_UPPER_THRESHOLD;
        break;
      case 27:
        if (9 > size) {
          result.type = CBOR_TYPE_NULL;
          return result;
        }
        result.width = 8;
        result.value = u8be_read(buf + 1);
        limit_min = VALUE_W8_UPPER_THRESHOLD;
        break;
      default:
        // Values above 27 are not valid in CBOR.
        // Exception is indefinite length marker
        // but this has been handled separately.
        result.type = CBOR_TYPE_NULL;
        return result;
    }

    if (result.value < limit_min) {
      // This isn't canonical CBOR
      result.type = CBOR_TYPE_NULL;
      return result;
    }
  }

  if (result.type == CBOR_TYPE_NEGATIVE) {
    if (result.value > INT64_MAX) {
      result.type = CBOR_TYPE_NULL;
      return result;
    }
    int64_t negativeValue;
    if (result.value < INT64_MAX) {
      negativeValue = -((int64_t)(result.value + 1));
    } else {
      negativeValue = INT64_MIN;
    }
    result.value = negativeValue;
  }

  return result;
}

size_t cbor_writeToken(uint8_t type, uint64_t value, uint8_t* buffer,
                       size_t bufferSize) {
  if (bufferSize > 1024) return 0;

  if (type == CBOR_TYPE_ARRAY_INDEF || type == CBOR_TYPE_INDEF_END ||
      type == CBOR_TYPE_NULL) {
    if (1 > bufferSize) {
      return 0;
    }
    buffer[0] = type;
    return 1;
  }

  if (type & CBOR_VALUE_MASK) {
    // type should not have any value
    return 0;
  }

  // Check sanity
  switch (type) {
    case CBOR_TYPE_NEGATIVE: {
      int64_t negativeValue;
      // reinterpret an actually negative value hidden in an unsigned in the
      // safe way
      memcpy(&negativeValue, &value, sizeof(value));
      if (negativeValue >= 0) {
        return 0;
      }
      value = (uint64_t)(-negativeValue) - 1;
      // intentional fallthrough
    }
    case CBOR_TYPE_UNSIGNED:
    case CBOR_TYPE_BYTES:
    case CBOR_TYPE_TEXT:
    case CBOR_TYPE_ARRAY:
    case CBOR_TYPE_MAP:
    case CBOR_TYPE_TAG:
      break;
    default:
      // not supported
      return 0;
  }

  // Warning(ppershing): It might be tempting but we don't want to call
  // stream_appendData() twice Instead we have to construct the whole buffer at
  // once to make append operation atomic.

  if (value < VALUE_W1_UPPER_THRESHOLD) {
    if (1 > bufferSize) {
      return 0;
    }
    u1be_write(buffer, (uint8_t)(type | value));
    return 1;
  } else if (value < VALUE_W2_UPPER_THRESHOLD) {
    if (2 > bufferSize) {
      return 0;
    }
    u1be_write(buffer, type | 24);
    u1be_write(buffer + 1, (uint8_t)value);
    return 1 + 1;
  } else if (value < VALUE_W4_UPPER_THRESHOLD) {
    if (3 > bufferSize) {
      return 0;
    }
    u1be_write(buffer, type | 25);
    u2be_write(buffer + 1, (uint16_t)value);
    return 1 + 2;
  } else if (value < VALUE_W8_UPPER_THRESHOLD) {
    if (5 > bufferSize) {
      return 0;
    }
    u1be_write(buffer, type | 26);
    u4be_write(buffer + 1, (uint32_t)value);
    return 1 + 4;
  } else {
    if (9 > bufferSize) {
      return 0;
    }
    u1be_write(buffer, type | 27);
    u8be_write(buffer + 1, value);
    return 1 + 8;
  }
}

bool cbor_mapKeyFulfillsCanonicalOrdering(const uint8_t* previousBuffer,
                                          size_t previousSize,
                                          const uint8_t* nextBuffer,
                                          size_t nextSize) {
  if (previousSize > 1024) return false;
  if (nextSize > 1024) return false;

  if (previousSize != nextSize) {
    return previousSize < nextSize;
  }
  for (size_t i = 0; i < previousSize; ++i) {
    if (*previousBuffer != *nextBuffer) {
      return *previousBuffer < *nextBuffer;
    }
    ++previousBuffer;
    ++nextBuffer;
  }
  // key duplication is an error
  return false;
}
