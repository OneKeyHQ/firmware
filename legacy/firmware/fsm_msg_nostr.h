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

void fsm_msgNostrGetPublicKey(const NostrGetPublicKey *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(NostrPublicKey);

  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  if (hdnode_fill_public_key(node) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to derive public key"));
    layoutHome();
    return;
  }

  resp->has_publickey = true;
  resp->has_npub = true;
  data2hexaddr(node->public_key + 1, 32, resp->publickey);
  if (!nostr_get_pubkey(resp->npub, node->public_key + 1)) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to get pubkey"));
    layoutHome();
    return;
  }

  if (msg->has_show_display && msg->show_display) {
    char desc[32] = {0};
    strcat(desc, "Nostr ");
    strcat(desc, _("Public Key"));
    if (!fsm_layoutAddress(resp->npub, NULL, desc, false, 0, msg->address_n,
                           msg->address_n_count, true, NULL, 0, 0, NULL)) {
      return;
    }
  }

  msg_write(MessageType_MessageType_NostrPublicKey, resp);
  layoutHome();
}

void fsm_msgNostrSignEvent(const NostrSignEvent *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(NostrSignedEvent);

  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  if (hdnode_fill_public_key(node) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to derive public key"));
    layoutHome();
    return;
  }

  if (!nostr_sign_event(msg, node, resp)) {
    return;
  }
  msg_write(MessageType_MessageType_NostrSignedEvent, resp);

  layoutHome();
}

void fsm_msgNostrEncryptMessage(NostrEncryptMessage *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(NostrEncryptedMessage);

  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  if (hdnode_fill_public_key(node) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to derive public key"));
    layoutHome();
    return;
  }

  if (!nostr_encrypt_message(msg, node, resp)) {
    return;
  }
  msg_write(MessageType_MessageType_NostrEncryptedMessage, resp);

  layoutHome();
}

void fsm_msgNostrDecryptMessage(NostrDecryptMessage *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(NostrDecryptedMessage);

  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  if (hdnode_fill_public_key(node) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to derive public key"));
    layoutHome();
    return;
  }

  if (!nostr_decrypt_message(msg, node, resp)) {
    return;
  }
  msg_write(MessageType_MessageType_NostrDecryptedMessage, resp);

  layoutHome();
}

void fsm_msgNostrSignSchnorr(const NostrSignSchnorr *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(NostrSignedSchnorr);

  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  if (hdnode_fill_public_key(node) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to derive public key"));
    layoutHome();
    return;
  }

  if (!nostr_sign_schnorr(msg, node, resp)) {
    return;
  }
  msg_write(MessageType_MessageType_NostrSignedSchnorr, resp);

  layoutHome();
}
