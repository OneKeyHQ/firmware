/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2018 Pavol Rusnak <stick@satoshilabs.com>
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

static bool fsm_nervosCheckPath(uint32_t address_n_count,
                                        const uint32_t *address_n,
                                        bool pubkey_export) {
  if (nervos_path_check(address_n_count, address_n, pubkey_export)) {
    return true;
  }
  if (config_getSafetyCheckLevel() == SafetyCheckLevel_Strict) {
    fsm_sendFailure(FailureType_Failure_DataError, _("Forbidden key path"));
    return false;
  }
  return fsm_layoutPathWarning(address_n_count, address_n);
}


void fsm_msgNervosGetAddress(const NervosGetAddress *msg) {
  RESP_INIT(NervosAddress);

  CHECK_INITIALIZED

  CHECK_PIN

  if (!fsm_nervosCheckPath(msg->address_n_count, msg->address_n, false)) {
    layoutHome();
    return;
  }


  const HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                          msg->address_n_count, NULL);
  if (!node) return;

  uint8_t pubkeyhash[20];

  if (!hdnode_get_ethereum_pubkeyhash(node, pubkeyhash)) {
    layoutHome();
    return;
  }
  char hrp[4] = {"ckb"};
  uint32_t hashtype = 1;
  if (msg->has_is_testnet && msg->is_testnet){
    strlcpy(hrp, "ckt", sizeof(hrp));
  }

  if (msg->has_hashtype){
    hashtype = msg->hashtype;
  }
  if(!nervos_generate_ckb_address(resp->address,pubkeyhash,hrp,hashtype)){
    layoutHome();
    return;
  }

  resp->has_address = true;
  if (msg->has_show_display && msg->show_display) {
    char desc[257] = {0};
    if (msg->has_is_testnet && msg->is_testnet){
      strlcpy(desc, "Testnet Address:", sizeof(desc));
    }
    else{
      strlcpy(desc, "Mainnet Address:", sizeof(desc));
    }
    if (!fsm_layoutAddress(resp->address, NULL, desc, false, 0, msg->address_n,
                           msg->address_n_count, true, NULL, 0, 0, NULL)) {
      return;
    }
  }
  msg_write(MessageType_MessageType_NervosAddress, resp);
  layoutHome();
}
