/*
 * This file is part of the OneKey project, https://onekey.so/
 *
 * Copyright (C) 2022 OneKey Team <core@onekey.so>
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

#ifndef __COSMOS_H__
#define __COSMOS_H__

#include <stdbool.h>
#include "bip32.h"
#include "cosmos_networks.h"
#include "messages-cosmos.pb.h"

int cosmos_get_address(char *address, const uint8_t *public_key,
                       const char *hrp);
bool cosmos_sign_tx(const CosmosSignTx *msg, const HDNode *node,
                    CosmosSignedTx *resp);

#endif  // __COSMOS_H__
