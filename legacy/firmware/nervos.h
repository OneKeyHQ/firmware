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

#ifndef __NERVOS_H__
#define __NERVOS_H__

#include <stdbool.h>
#include "bip32.h"
#include "messages-nervos.pb.h"

bool nervos_generate_ckb_address(char *output,uint8_t *pubkeyhash,
                      char *hrp,uint32_t hashtype);

bool nervos_path_check(uint32_t address_n_count,
                                const uint32_t *address_n, bool pubkey_export);
#endif  // __NERVOS_H__
