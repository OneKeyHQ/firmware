//clang-format off

#ifndef _EMMC_COMMANDS_MACROS_H_
  #define _EMMC_COMMANDS_MACROS_H_

  #include <stdio.h>
  #include <string.h>

  #define MSG_INIT(NAME, TYPE) TYPE NAME = TYPE##_init_default

  #define MSG_ASSIGN_CALLBACK_ENCODE(NAME, FIELD, CB_ENCODE, ARGUMENT) \
    {                                                                  \
      NAME.FIELD.funcs.encode = CB_ENCODE;                             \
      NAME.FIELD.arg = (void*)ARGUMENT;                                \
    }
  #define MSG_ASSIGN_CALLBACK_DECODE(NAME, FIELD, CB_DECODE, ARGUMENT) \
    {                                                                  \
      NAME.FIELD.funcs.decode = CB_DECODE;                             \
      NAME.FIELD.arg = (void*)ARGUMENT;                                \
    }
  #define MSG_ASSIGN_REQUIRED_VALUE(NAME, FIELD, VALUE) \
    {                                                   \
      NAME.FIELD = VALUE;                               \
    }
  #define MSG_ASSIGN_VALUE(NAME, FIELD, VALUE)       \
    {                                                \
      NAME.has_##FIELD = true;                       \
      MSG_ASSIGN_REQUIRED_VALUE(NAME, FIELD, VALUE); \
    }
  #define MSG_ASSIGN_REQUIRED_STRING(NAME, FIELD, VALUE)  \
    {                                                     \
      memzero(NAME.FIELD, sizeof(NAME.FIELD));            \
      strncpy(NAME.FIELD, VALUE, sizeof(NAME.FIELD) - 1); \
    }
  #define MSG_ASSIGN_STRING(NAME, FIELD, VALUE)       \
    {                                                 \
      NAME.has_##FIELD = true;                        \
      MSG_ASSIGN_REQUIRED_STRING(NAME, FIELD, VALUE); \
    }
  #define MSG_ASSIGN_REQUIRED_BYTES(NAME, FIELD, VALUE, LEN)               \
    {                                                                      \
      memzero(NAME.FIELD.bytes, sizeof(NAME.FIELD.bytes));                 \
      memcpy(NAME.FIELD.bytes, VALUE, MIN(LEN, sizeof(NAME.FIELD.bytes))); \
      NAME.FIELD.size = MIN(LEN, sizeof(NAME.FIELD.bytes));                \
    }
  #define MSG_ASSIGN_BYTES(NAME, FIELD, VALUE, LEN)       \
    {                                                     \
      MSG_ASSIGN_REQUIRED_BYTES(NAME, FIELD, VALUE, LEN); \
    }

  #define MSG_RECV_RET_ON_ERR(NAME, TYPE)                                        \
    if ( !recv_msg_async_parse(iface_num, msg_size, buf, TYPE##_fields, &NAME) ) \
      return -1;
  #define MSG_RECV_NOCHECK(NAME, TYPE) recv_msg_async_parse(iface_num, msg_size, buf, TYPE##_fields, &NAME)
  #define MSG_SEND(NAME, TYPE) \
    send_msg(iface_num, MessageType_MessageType_##TYPE, TYPE##_fields, &NAME, false)
  #define MSG_SEND_NOCHECK(NAME, TYPE) \
    send_msg(iface_num, MessageType_MessageType_##TYPE, TYPE##_fields, &NAME, true)

#endif //_EMMC_COMMANDS_MACROS_H_

//clang-format on