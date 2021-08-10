#include "flash_enc.h"
#include "otp.h"
#include "util.h"
#include "w25qxx.h"

#include "aes/aes.h"

static uint8_t aes_key[32] = {0};

void flash_enc_init(void) {
  flash_otp_read(FLASH_OTP_RANDOM_KEY, 0, aes_key, FLASH_OTP_BLOCK_SIZE);
}

bool flash_write_enc(uint8_t *buffer, uint32_t address, uint32_t len) {
  uint8_t enc_buffer[AES_BLOCK_SIZE] = {0xff};
  uint32_t pre_remain = 0, next_remain = 0;
  aes_encrypt_ctx enc_ctx = {0};

  if (address % AES_BLOCK_SIZE) {
    pre_remain = AES_BLOCK_SIZE - address % AES_BLOCK_SIZE;
  }
  if (len > pre_remain) {
    next_remain = (address + len) % AES_BLOCK_SIZE;
    len -= pre_remain + next_remain;
  } else {
    pre_remain = len;
    len = 0;
  }

  aes_encrypt_key128(aes_key, &enc_ctx);
  if (pre_remain) {
    w25qxx_read_bytes(enc_buffer, address - address % AES_BLOCK_SIZE,
                      AES_BLOCK_SIZE);
    memcpy(enc_buffer + address % AES_BLOCK_SIZE, buffer, pre_remain);
    aes_ecb_encrypt(enc_buffer, enc_buffer, AES_BLOCK_SIZE, &enc_ctx);
    if (!w25qxx_write_buffer(enc_buffer, address - address % AES_BLOCK_SIZE,
                             AES_BLOCK_SIZE)) {
      return false;
    }
  }
  if (len) {
    aes_ecb_encrypt(buffer + pre_remain, buffer + pre_remain, len, &enc_ctx);
    if (!w25qxx_write_buffer(buffer + pre_remain, address + pre_remain, len)) {
      return false;
    }
  }
  if (next_remain) {
    w25qxx_read_bytes(enc_buffer, address + pre_remain + len, AES_BLOCK_SIZE);
    memcpy(enc_buffer, buffer + pre_remain + len, next_remain);
    aes_ecb_encrypt(enc_buffer, enc_buffer, AES_BLOCK_SIZE, &enc_ctx);
    if (!w25qxx_write_buffer(enc_buffer, address + pre_remain + len,
                             AES_BLOCK_SIZE)) {
      return false;
    }
  }
  return true;
}

bool flash_read_enc(uint8_t *buffer, uint32_t address, uint32_t len) {
  uint8_t dec_buffer[AES_BLOCK_SIZE] = {0};
  uint32_t pre_remain = 0, next_remain = 0;
  aes_decrypt_ctx dec_ctx = {0};

  if (w25qxx_read_bytes(buffer, address, len)) {
    if (check_all_ones(buffer, len)) {
      return true;
    } else {
      if (address % AES_BLOCK_SIZE) {
        pre_remain = AES_BLOCK_SIZE - address % AES_BLOCK_SIZE;
      }

      if (len > pre_remain) {
        next_remain = (address + len) % AES_BLOCK_SIZE;
        len -= pre_remain + next_remain;
      } else {
        pre_remain = len;
        len = 0;
      }

      aes_decrypt_key128(aes_key, &dec_ctx);

      if (pre_remain) {
        w25qxx_read_bytes(dec_buffer, address - address % AES_BLOCK_SIZE,
                          AES_BLOCK_SIZE);
        aes_ecb_decrypt(dec_buffer, dec_buffer, AES_BLOCK_SIZE, &dec_ctx);
        memcpy(buffer, dec_buffer + address % AES_BLOCK_SIZE, pre_remain);
      }
      if (len) {
        w25qxx_read_bytes(buffer + pre_remain, address + pre_remain, len);
        aes_ecb_decrypt(buffer + pre_remain, buffer + pre_remain, len,
                        &dec_ctx);
      }
      if (next_remain) {
        w25qxx_read_bytes(dec_buffer, address + pre_remain + len,
                          AES_BLOCK_SIZE);
        aes_ecb_decrypt(dec_buffer, dec_buffer, AES_BLOCK_SIZE, &dec_ctx);
        memcpy(buffer + pre_remain + len, dec_buffer, next_remain);
      }
    }
    return true;
  }
  return false;
}
