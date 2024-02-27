/*
 * This file is part of the OneKey project, https://onekey.so/
 *
 * Copyright (C) 2021 OneKey Team <core@onekey.so>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "nervos.h"
#include <stdio.h>
#include <string.h>
#include "blake2b.h"
#include "buttons.h"
#include "config.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "secp256k1.h"
#include "schnorr_bch.h"

#define MAX_ADDRESS_LENGTH 100
#define CODE_INDEX_SECP256K1_SINGLE 0x00
#define FORMAT_TYPE_SHORT 0x01
// 在nervos.c的顶部添加这行声明
void bytes_to_hex_str(const uint8_t *bytes, size_t bytes_len, char *hex_str);

const char CHARSET[] = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

// 将字节数组转换为十六进制字符串
void bytes_to_hex_str(const uint8_t *bytes, size_t bytes_len, char *hex_str) {
  const char hex_chars[] = "0123456789abcdef";
  for (size_t i = 0; i < bytes_len; i++) {
    hex_str[2 * i] = hex_chars[(bytes[i] >> 4) & 0x0F];
    hex_str[2 * i + 1] = hex_chars[bytes[i] & 0x0F];
  }
  hex_str[2 * bytes_len] = '\0';  // Null-terminate the string
}

void print_hash_state(const char *stage, const uint8_t *hash,
                      size_t hash_size) {
  char hash_hex[2 * hash_size + 1];  // 每个字节两个字符，加上终止符
  bytes_to_hex_str(hash, hash_size, hash_hex);
  debugLog(0, "nervos", stage);
  debugLog(0, "nervos", hash_hex);
}

// 对公钥进行BLAKE2b哈希并只取前20字节（BLAKE160）
void ckb_blake160(void) {
  // 定义并初始化公钥
  const uint8_t public_key[] = {
      0x03, 0x39, 0x91, 0xde, 0xa5, 0xd7, 0x07, 0xaa, 0xc8, 0xf5, 0xd0,
      0xc7, 0xc7, 0xc1, 0x0f, 0xe0, 0x32, 0x24, 0x88, 0xc0, 0xda, 0x8f,
      0x70, 0x63, 0xb8, 0x36, 0x7c, 0xf8, 0x92, 0xa6, 0x42, 0x76, 0x03};
  size_t public_key_len = sizeof(public_key);
  uint8_t hash[32];  // 完整的BLAKE2b哈希大小为32字节
  blake2b_state S;
  const uint8_t personal[] = "ckb-default-hash";
  size_t outlen = 32;  // 输出哈希值的长度为32字节

  // 初始化哈希状态
  if (blake2b_InitPersonal(&S, outlen, personal, sizeof(personal) - 1) != 0) {
    debugLog(0, "nervos", "Failed to initialize hash state");
    return;
  }

  // 更新哈希状态
  if (blake2b_Update(&S, public_key, public_key_len) != 0) {
    debugLog(0, "nervos", "Failed to update hash state");
    return;
  }

  // 完成哈希计算
  if (blake2b_Final(&S, hash, outlen) != 0) {
    debugLog(0, "nervos", "Failed to finalize hash");
    return;
  }

  // 将哈希结果的前20字节转换为十六进制字符串
  char hash_hex[2 * 20 + 1];  // 每个字节转换为2个十六进制字符，加上结束符
  bytes_to_hex_str(hash, 20, hash_hex);

  // 打印最终的哈希值
  debugLog(0, "nervos", "Final hash 160:");
  debugLog(0, "nervos", hash_hex);
}

void ckb_blake160_1(const uint8_t *public_key, size_t public_key_len,
                    char *output) {
  // 定义并初始化公钥
  // const uint8_t public_key[] = {
  //     0x03, 0x39, 0x91, 0xde, 0xa5, 0xd7, 0x07, 0xaa,
  //     0xc8, 0xf5, 0xd0, 0xc7, 0xc7, 0xc1, 0x0f, 0xe0,
  //     0x32, 0x24, 0x88, 0xc0, 0xda, 0x8f, 0x70, 0x63,
  //     0xb8, 0x36, 0x7c, 0xf8, 0x92, 0xa6, 0x42, 0x76,
  //     0x03
  // };
  // size_t public_key_len = sizeof(public_key);
  uint8_t hash[32];  // 完整的BLAKE2b哈希大小为32字节
  blake2b_state S;
  const uint8_t personal[] = "ckb-default-hash";
  size_t outlen = 32;  // 输出哈希值的长度为32字节

  // 初始化哈希状态
  if (blake2b_InitPersonal(&S, outlen, personal, sizeof(personal) - 1) != 0) {
    debugLog(0, "nervos", "Failed to initialize hash state");
    return;
  }

  // 更新哈希状态
  if (blake2b_Update(&S, public_key, public_key_len) != 0) {
    debugLog(0, "nervos", "Failed to update hash state");
    return;
  }

  // 完成哈希计算
  if (blake2b_Final(&S, hash, outlen) != 0) {
    debugLog(0, "nervos", "Failed to finalize hash");
    return;
  }

  // 将哈希结果的前20字节转换为十六进制字符串
  char hash_hex[2 * 20 + 1];  // 每个字节转换为2个十六进制字符，加上结束符
  bytes_to_hex_str(hash, 20, hash_hex);

  snprintf(output, 2 * 20 + 3, "%s", hash_hex);  // 添加"0x"前缀到输出
  // 打印最终的哈希值
  debugLog(0, "nervos", "Final hash 160__new:");
  debugLog(0, "nervos", hash_hex);
  debugLog(0, "nervos", "Final hash 160__new:");
  debugLog(0, "nervos", output);
}

// uint32_t bech32_polymod(const uint8_t *values, size_t len) {
//   static const uint32_t generator[] = {0x3B6A57B2, 0x26508E6D, 0x1EA119FA,
//                                        0x3D4233DD, 0x2A1462B3};
//   uint32_t chk = 1;
//   for (size_t i = 0; i < len; ++i) {
//     uint8_t top = chk >> 25;
//     chk = (chk & 0x1FFFFFF) << 5 ^ values[i];
//     for (int j = 0; j < 5; ++j) {
//       if (top & (1 << j)) {
//         chk ^= generator[j];
//       }
//     }
//   }
//   return chk;
// }

uint32_t bech32_polymod(const uint8_t *values, size_t len) {
    static const uint32_t generator[] = {0x3b6a57b2, 0x26508e6d, 0x1ea119fa, 0x3d4233dd, 0x2a1462b3};
    uint32_t chk = 1;
    for (size_t i = 0; i < len; ++i) {
        uint8_t top = chk >> 25;
        chk = ((chk & 0x1ffffff) << 5) ^ values[i];
        for (int j = 0; j < 5; ++j) {
            if ((top >> j) & 1) {
                chk ^= generator[j];
            }
        }
    }
    return chk;
}


void bech32_hrp_expand(const char *hrp, uint8_t *expanded) {
  size_t hrp_len = strlen(hrp);
  for (size_t i = 0; i < hrp_len; ++i) {
    expanded[i] = hrp[i] >> 5;
  }
  expanded[hrp_len] = 0;
  for (size_t i = 0; i < hrp_len; ++i) {
    expanded[hrp_len + 1 + i] = hrp[i] & 31;
  }
}

// int convertbits(const uint8_t *data, size_t datalen, uint8_t *out, int frombits,
//                 int tobits, int pad) {
//   uint32_t acc = 0;
//   int bits = 0;
//   int retlen = 0;
//   const uint32_t maxv = (1 << tobits) - 1;
//   for (size_t i = 0; i < datalen; ++i) {
//     if (data[i] >> frombits) return 0;  // Value out of range
//     acc = (acc << frombits) | data[i];
//     bits += frombits;
//     while (bits >= tobits) {
//       bits -= tobits;
//       out[retlen++] = (acc >> bits) & maxv;
//     }
//   }
//   if (pad) {
//     if (bits) out[retlen++] = (acc << (tobits - bits)) & maxv;
//   } else if (bits >= frombits || ((acc << (tobits - bits)) & maxv)) {
//     return 0;  // Cannot convert
//   }
//   return retlen;
// }


int convertbits(const uint8_t *data, size_t datalen, uint8_t *out, int frombits, int tobits, int pad) {
    uint32_t acc = 0;
    int bits = 0;
    size_t retlen = 0;
    const uint32_t maxv = (1 << tobits) - 1;
    const uint32_t max_acc = (1 << (frombits + tobits - 1)) - 1;
    for (size_t i = 0; i < datalen; ++i) {
        if (data[i] >> frombits) {
            return -1; // Value out of range
        }
        acc = ((acc << frombits) | data[i]) & max_acc;
        bits += frombits;
        while (bits >= tobits) {
            bits -= tobits;
            out[retlen++] = (acc >> bits) & maxv;
        }
    }
    if (pad) {
        if (bits) {
            out[retlen++] = (acc << (tobits - bits)) & maxv;
        }
    } else if (bits >= frombits || ((acc << (tobits - bits)) & maxv)) {
        return -1; // Cannot convert
    }
    return retlen; // 返回转换后的数据长度
}








void debug_print_hex(const uint8_t *data, size_t data_len, const char *label) {
  // 假设最大长度不超过256个字节
  char hex_str[512 + 1];  // 每个字节转换为两个字符，额外一个字符用于'\0'
  bytes_to_hex_str(data, data_len, hex_str);
  debugLog(0, label, hex_str);
}

void print_public_key_with_debugLog(const uint8_t *public_key,
                                    size_t public_key_len) {
  // 为了安全起见，确保十六进制字符串有足够的空间
  // 每个字节转换为两个十六进制字符，加上一个字符的空终止符
  char hex_str[public_key_len * 2 + 1];
  char *ptr = hex_str;

  for (size_t i = 0; i < public_key_len; ++i) {
    // 将每个字节转换为十六进制字符串
    ptr += sprintf(ptr, "%02x", public_key[i]);
  }
  debugLog(0, "Public Key BLAKE2b Hash", "nervos test final  111");
  // 使用debugLog打印十六进制表示的公钥
  debugLog(0, "Public Key", hex_str);
}

int hex_char_to_int(char c) {
    if ('0' <= c && c <= '9') return c - '0';
    if ('a' <= c && c <= 'f') return c - 'a' + 10;
    if ('A' <= c && c <= 'F') return c - 'A' + 10;
    return -1;
}

void hex_str_to_bytes(const char *hex_str, uint8_t *bytes, size_t bytes_len) {
    for (size_t i = 0; i < bytes_len; ++i) {
        int hi = hex_char_to_int(hex_str[2*i]);
        int lo = hex_char_to_int(hex_str[2*i + 1]);
        if (hi == -1 || lo == -1) {
            printf("Invalid hex string");
            return;
        }
        bytes[i] = (hi << 4) + lo;
    }
}



void nervos_get_address_from_public_key(const uint8_t *public_key,
                                        char *address) {
  size_t public_key_le = 33;

  print_public_key_with_debugLog(public_key, public_key_le);

  // 假设 public_key 是33字节，包含了SECP256K1公钥
  // char hash[2 * 20 + 3];
  ckb_blake160();   // 对公钥进行Blake160哈希
  char output[43];  // 哈希输出缓冲区
  ckb_blake160_1(public_key, public_key_le, output);

  debugLog(0, "", "nerveos hash 160");
  debugLog(0, "nervos address", output);

  uint8_t
      payload[22];  // 格式类型(1) + 代码索引(1) + 哈希(20) => 转换后的大小
  payload[0] = FORMAT_TYPE_SHORT;
  payload[1] = CODE_INDEX_SECP256K1_SINGLE;
  //memcpy(payload + 2, output, 20);
  hex_str_to_bytes(output, payload + 2, 20); 

  debugLog(0, "nervos", "Final hash payload:");
  debug_print_hex(payload, sizeof(payload) / sizeof(payload[0]),
                  "My Data");
  // debugLog(0, "nervos", data_part);

  uint8_t data_part[36];  // 转换后的数据大小足够容纳转换结果
  convertbits(payload, 22, data_part, 8, 5, 1);
  size_t data_len = sizeof(data_part) / sizeof(data_part[0]);


  debugLog(0, "nervos", "Final hash data_part:");
  debug_print_hex(data_part, sizeof(data_part) / sizeof(data_part[0]),
                  "data_part");

  uint8_t data_part1[37];  // 转换后的数据大小足够容纳转换结果
  convertbits(payload, 22, data_part1, 8, 5, 1);
  //size_t data_len1 = sizeof(data_part1) / sizeof(data_part1[0]);


  debugLog(0, "nervos", "Final hash data_part1:");
  debug_print_hex(data_part1, sizeof(data_part1) / sizeof(data_part1[0]),
                  "data_part");
  // debugLog(0, "nervos", converted);

  // HRP扩展  //////////////////////////
  uint8_t expanded[7];     // HRP扩展后的大小
  const char *hrp = "ckt";  // 测试网络HRP，主网为"ckb"
  bech32_hrp_expand(hrp, expanded);
  debugLog(0, "nervos", "Final hash expanded:");
  debug_print_hex(expanded, sizeof(expanded) / sizeof(expanded[0]), "expanded");

  ////////////////////////////////////////////////

  size_t hrp_exp_len = strlen(hrp) * 2 + 1;

      // 合并数组
    uint8_t values[49]; // 确保这个数组足够大
    memcpy(values, expanded, hrp_exp_len);
    memcpy(values + hrp_exp_len, data_part, data_len);
    size_t values_len = hrp_exp_len + data_len;

    debugLog(0, "nervos", "Final hash data_values:");
    debug_print_hex(values, sizeof(values) / sizeof(values[0]),
                  "data_part");


    // 添加6个0值作为占位符
    for (int i = 0; i < 6; ++i) {
        values[values_len + i] = 0;
    }
    values_len += 6;



    // 计算polymod
    uint32_t polymod = bech32_polymod(values, values_len) ^ 1;
    debugLog(0, "nervos", "Final hash polymod:");
    char buffer[3]; // 确保缓冲区足够大以容纳十六进制或十进制表示的uint32_t值
    snprintf(buffer, sizeof(buffer), "%x", polymod); // 十六进制格式
    debugLog(0, "nervos", buffer);


    uint8_t checksum[6];
    for (int i = 0; i < 6; ++i) {
    checksum[i] = (polymod >> 5 * (5 - i)) & 31;
      }  

    debugLog(0, "nervos", "Final hash checksum:");
    debug_print_hex(checksum, sizeof(checksum) / sizeof(checksum[0]),
                  "data_part");

    size_t combined_len = 36 + 6;
    uint8_t combined[combined_len];
    memcpy(combined, data_part, 36);
    memcpy(combined + 36, checksum, 6);

    debugLog(0, "nervos", "Final hash combined:");
    debug_print_hex(combined, sizeof(combined) / sizeof(combined[0]),
                  "data_part");

    //char addr[strlen(hrp) + 1 + combined_len + 1]; 
    sprintf(address, "%s1", hrp);
    for (size_t i = 0; i < combined_len; ++i) {
    address[strlen(hrp) + 1 + i] = CHARSET[combined[i]];
    }
    address[strlen(hrp) + 1 + combined_len] = '\0'; 
    debugLog(0, "", "nerveos address");
    debugLog(0, "nervos addr", address);

}


// 函数用于将单个十六进制字符转换为其对应的数值
uint8_t hexCharToValue(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + c - 'a';
    if (c >= 'A' && c <= 'F') return 10 + c - 'A';
    return 0; // 不是有效的十六进制字符
}

// 函数用于将十六进制字符串转换为字节数组
void hexStringToByteArray(const char* hexString, uint8_t* byteArray, size_t byteArrayLength) {
    size_t hexStringLength = strlen(hexString);
    for (size_t i = 0; i < hexStringLength && i / 2 < byteArrayLength; i += 2) {
        byteArray[i / 2] = hexCharToValue(hexString[i]) << 4 | hexCharToValue(hexString[i + 1]);
    }
}



void nervos_sign_sighash(HDNode *node, const uint8_t *raw_message,
                         uint32_t raw_message_len, uint8_t *signature,
                         pb_size_t *signature_len) 
{
    const char* hexString = "86970f987a4e2c282ea403b0e313e6cf84df93b5c527065bef54623aa85873e3";
    uint8_t byteArray[32]; // 因为给出的十六进制字符串长度为64，所以数组长度是32
    hexStringToByteArray(hexString, byteArray, sizeof(byteArray));


    debugLog(0, "nervos", "Final hash byteArray:");
    debug_print_hex(byteArray, 32, "data_part");

    
    debugLog(0, "nervos", "Final hash raw_message2:");
    debug_print_hex(raw_message, 32, "data_part");


    // sign tx hash
    uint8_t v;
    uint8_t sig[64]; 
    if (ecdsa_sign_digest(&secp256k1, node->private_key, byteArray,
                       sig, &v,NULL) != 0) {}
    debugLog(0, "nervos", "Final hash sig:");
    debug_print_hex(sig, 64, "data_part");

    char vs[3]; // 确保缓冲区足够大以容纳十六进制或十进制表示的uint32_t值
    snprintf(vs, sizeof(vs), "%x", v); // 十六进制格式
    debugLog(0, "nervos", vs);


    uint8_t v1;
    uint8_t sig1[64]; 
    if (ecdsa_sign_digest(&secp256k1, node->private_key, raw_message,
                       sig1, &v1,NULL) != 0) {}
    debugLog(0, "nervos", "Final hash sig2:");
    debug_print_hex(sig1, 64, "data_part");

    char vs1[3]; // 确保缓冲区足够大以容纳十六进制或十进制表示的uint32_t值
    snprintf(vs1, sizeof(vs1), "%x", v1); // 十六进制格式
    debugLog(0, "nervos", vs1);

    // 创建一个新数组来存储sig1和v1的组合
    uint8_t final_sin[65];

    // 首先复制sig1到新数组
    memcpy(final_sin, sig1, 64);

    // 然后将v1添加到数组的最后
    final_sin[64] = v1;


    memcpy(signature, sig1, 64);

    // 然后将v1添加到数组的最后
    signature[64] = v1;

    // 设置签名长度
    *signature_len = 65;


    debugLog(0, "nervos", "Final hash sigurl,,,:");
    debug_print_hex(signature, 65, "data_part");
    
    debugLog(0, "nervos", "Final hash final_sin:");
    debug_print_hex(final_sin, 64, "data_part");


    debugLog(0, "nervos", "Final hash private_key:");
    debug_print_hex(node->private_key, sizeof(node->private_key) / sizeof(node->private_key[0]), "data_part");

}
