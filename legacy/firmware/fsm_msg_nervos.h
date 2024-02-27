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

// void print_public_key_with_debugLog(const uint8_t *public_key, size_t length) {
//     // 预计每个字节需要'\xXX'格式，加上终止符'\0'
//     char str[3 * length + 1];
//     int offset = 0;

//     for (size_t i = 0; i < length; ++i) {
//         // 使用snprintf安全地格式化字符串，并处理偏移
//         offset += snprintf(str + offset, sizeof(str) - offset, "\\x%02x", public_key[i]);
//     }

//     debugLog(0, "nervos", "Public Key");
//     debugLog(0, "nervos", str);
// }


void fsm_msgNervosGetAddress(const NervosGetAddress *msg) {
  debugLog(0, "", "nerveos begin");
  printf("begin nervos get address");
  CHECK_INITIALIZED
  CHECK_PIN
  RESP_INIT(NervosAddress);
  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  hdnode_fill_public_key(node);
  //size_t public_key_length = 33; 
  
  if (!node) return;
  debugLog(0, "", "nerveos 000");
  nervos_get_address_from_public_key(node->public_key, resp->address);



  if (msg->has_show_display && msg->show_display) {
    debugLog(0, "", "nerveos 111");
    char desc[16] = {0};
    strcat(desc, "Nervos");
    strcat(desc, _("Address:"));
    debugLog(0, "", "nerveos 222");
    if (!fsm_layoutAddress(resp->address, NULL, desc, false, 0,msg->address_n,
                           msg->address_n_count, false, NULL, 0, 0, NULL)) {
        debugLog(0, "", "nerveos 1112");
      return;
    }
  }
  debugLog(0, "", "nerveos msg_write before");
  msg_write(MessageType_MessageType_NervosAddress, resp);
  debugLog(0, "", "nerveos msg_write");
  layoutHome();
  debugLog(0, "", "nerveos layoutHome");
}




void fsm_msgNervosSignTx(const NervosSignTx *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  debugLog(0, "nervos", "Final hash nervos begin signature:");

  RESP_INIT(NervosSignedTx);

  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);

  //nervos_sign_tx(msg, node, resp);

  nervos_sign_sighash(node, msg->raw_message.bytes, msg->raw_message.size, resp->signature.bytes, &resp->signature.size);   

  msg_write(MessageType_MessageType_NervosSignedTx, resp);  




  layoutHome();
}
