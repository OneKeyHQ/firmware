void fsm_msgNervosGetAddress(const NervosGetAddress *msg) {
  RESP_INIT(NervosAddress);

  CHECK_INITIALIZED   // 初始化检查
  CHECK_PIN   // 检查用户是否输入了 PIN 码

  // 获取 Nervos 网络相关信息，此处假设逻辑与以太坊相似
  const NervosNetworkInfo *network = get_nervos_network_definition(msg->address_n, msg->address_n_count);

  if (!network) {
    layoutHome();
    return;
  }

  const HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n, msg->address_n_count, NULL);
  if (!node) return;

  uint8_t pubkeyhash[20];
  if (!hdnode_get_nervos_pubkeyhash(node, pubkeyhash)) {
    layoutHome();
    return;
  }

  resp->has_address = true;
  nervos_address_checksum(pubkeyhash, resp->address);
  if (msg->has_show_display && msg->show_display) {
    char desc[257] = {0};
    snprintf(desc, 257, "%s Address:", network->name);
    if (!fsm_layoutAddress(resp->address, NULL, desc, false, 0, msg->address_n, msg->address_n_count, true, NULL, 0, 0, NULL)) {
      return;
    }
  }
  msg_write(MessageType_MessageType_NervosAddress, resp);
  layoutHome();
}


void fsm_msgNervosSignTx(const NervosSignTx *msg) {
  CHECK_INITIALIZED
  CHECK_PIN

  const NervosNetworkInfo *network = get_nervos_network_definition(msg->chain_id);

  if (!network) {
    layoutHome();
    return;
  }

  const HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n, msg->address_n_count, NULL);
  if (!node) return;

  nervos_signing_init(msg, node, network);
}