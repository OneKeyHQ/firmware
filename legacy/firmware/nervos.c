// Version: 1.0 test code
void nervos_signing_init(const NervosSignTx *msg, const HDNode *node, const NervosNetworkInfo *network) {
  struct signing_params params = {
      .chain_id = msg->chain_id,
      .data_length = msg->data_length,
      .data_bytes = msg->data,

      .has_to = msg->has_to,
      .to = msg->to,

      .value_size = msg->value.size,
      .value_bytes = msg->value.bytes,

      .network = network,
  };

  if (!nervos_signing_init_common(&params)) {
    nervos_signing_abort();
    return;
  }

  // ... 省略详细的签名逻辑代码 ...

  nervos_signing_confirm(&params, node);
}



static void send_request_chunk(void) {
  //计算签名进度
  int progress = 1000 - (data_total > 1000000 ? data_left / (data_total / 800)
  //在UI上展示当前的进度                                            : data_left * 800 / data_total);
  layoutProgress(_("Signing"), progress);
  msg_tx_request.has_data_length = true;
  msg_tx_request.data_length = data_left <= 1024 ? data_left : 1024;
  msg_write(MessageType_MessageType_NervosTxRequest, &msg_tx_request);
}


static void send_signature(void) {
  uint8_t hash[32] = {0}, sig[64] = {0};
  uint8_t v = 0;
  layoutProgress(_("Signing"), 1000);

  if (eip1559) {
    hash_rlp_list_length(rlp_calculate_access_list_length(
        signing_access_list, signing_access_list_count));
    for (size_t i = 0; i < signing_access_list_count; i++) {
      uint8_t address[20] = {0};
      if (!nervos_parse(signing_access_list[i].address, address)) {
        fsm_sendFailure(FailureType_Failure_DataError, _("Malformed address"));
        ethereum_signing_abort();
        return;
      }

      uint32_t address_length =
          rlp_calculate_length(sizeof(address), address[0]);
      uint32_t keys_length = rlp_calculate_access_list_keys_length(
          signing_access_list[i].storage_keys,
          signing_access_list[i].storage_keys_count);

      hash_rlp_list_length(address_length +
                           rlp_calculate_length(keys_length, 0xff));
      hash_rlp_field(address, sizeof(address));
      hash_rlp_list_length(keys_length);
      for (size_t j = 0; j < signing_access_list[i].storage_keys_count; j++) {
        hash_rlp_field(signing_access_list[i].storage_keys[j].bytes,
                       signing_access_list[i].storage_keys[j].size);
      }
    }
  } else {
    /* eip-155 replay protection */
    /* hash v=chain_id, r=0, s=0 */
    hash_rlp_number(chain_id);
    hash_rlp_length(0, 0);
    hash_rlp_length(0, 0);
  }

  keccak_Final(&keccak_ctx, hash);
  if (ecdsa_sign_digest(&secp256k1, privkey, hash, sig, &v,
                        ethereum_is_canonic) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Signing failed"));
    ethereum_signing_abort();
    return;
  }

  memzero(privkey, sizeof(privkey));

  /* Send back the result */
  msg_tx_request.has_data_length = false;

  msg_tx_request.has_signature_v = true;
  if (eip1559 || chain_id > MAX_CHAIN_ID) {
    msg_tx_request.signature_v = v;
  } else {
    msg_tx_request.signature_v = v + 2 * chain_id + 35;
  }

  msg_tx_request.has_signature_r = true;
  msg_tx_request.signature_r.size = 32;
  memcpy(msg_tx_request.signature_r.bytes, sig, 32);

  msg_tx_request.has_signature_s = true;
  msg_tx_request.signature_s.size = 32;
  memcpy(msg_tx_request.signature_s.bytes, sig + 32, 32);
  // 发送含有签名的交易请求消息
  msg_write(MessageType_MessageType_Nervos_TxRequest, &msg_tx_request);
  // 终止签名
  nervos_signing_abort();
}