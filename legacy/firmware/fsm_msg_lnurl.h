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

extern int ethereum_is_canonic(uint8_t v, uint8_t signature[64]);

void fsm_msgLnurlAuth(const LnurlAuth *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(LnurlAuthResp);

  if (!layout_lnurl_auth(msg)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
  }
  uint32_t address_n[8] = {0};
  // # m/138'/0
  address_n[0] = 2147483786;
  address_n[1] = 0;
  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, address_n, 2, NULL);
  if (!node) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Failed to derive node");
    layoutHome();
    return;
  }

  uint8_t dm[32] = {0};
  hmac_sha256(msg->domain.bytes, msg->domain.size, node->private_key, 32, dm);

  // "m/138'/{long1}/{long2}/{long3}/{long4}"
  address_n[1] = (dm[0] << 24) + (dm[1] << 16) + (dm[2] << 8) + dm[3];
  address_n[2] = (dm[4] << 24) + (dm[5] << 16) + (dm[6] << 8) + dm[7];
  address_n[3] = (dm[8] << 24) + (dm[9] << 16) + (dm[10] << 8) + dm[11];
  address_n[4] = (dm[12] << 24) + (dm[13] << 16) + (dm[14] << 8) + dm[15];

  resp->has_path = true;
#if !EMULATOR
  snprintf(resp->path, 64, "m/138'/%ld/%ld/%ld/%ld", address_n[1], address_n[2],
           address_n[3], address_n[4]);
#else
  snprintf(resp->path, 64, "m/138'/%d/%d/%d/%d", address_n[1], address_n[2],
           address_n[3], address_n[4]);
#endif

  if (msg->data.size > 0) {
    if (msg->data.size != 32) {
      fsm_sendFailure(FailureType_Failure_ProcessError, "unexpected data size");
      layoutHome();
      return;
    }

    node = fsm_getDerivedNode(SECP256K1_NAME, address_n, 5, NULL);
    if (!node) {
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      "Failed to derive node");
      layoutHome();
      return;
    }

    uint8_t v;
    uint8_t sig[65] = {0};
    if (ecdsa_sign_digest(&secp256k1, node->private_key, msg->data.bytes, sig,
                          &v, ethereum_is_canonic) != 0) {
      fsm_sendFailure(FailureType_Failure_ProcessError, _("Signing failed"));
      return;
    }
    if (hdnode_fill_public_key(node) != 0) {
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      _("Failed to derive public key"));
      layoutHome();
      return;
    }

    resp->has_signature = true;
    resp->signature.size = ecdsa_sig_to_der(sig, resp->signature.bytes);
  }

  resp->has_publickey = true;
  data2hexaddr(node->public_key, 33, resp->publickey);

  msg_write(MessageType_MessageType_LnurlAuthResp, resp);

  layoutHome();
}
