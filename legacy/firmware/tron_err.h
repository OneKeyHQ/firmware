/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
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

#ifndef __TRON_ERR_H__
#define __TRON_ERR_H__

enum TRON_ERROR_CODE {
  E_TRON_DecodeTriggerSmartContract = 0x00003001,
  E_TRON_EncodeTronAddress = 0x00003002,
  E_TRON_InvalidMethodSignature = 0x00003003,
  E_TRON_InvalidContractDataSize = 0x00003004,
  E_TRON_InvalidAddress = 0x00003005,
  E_TRON_InvalidCallData = 0x00003006,
  E_TRON_DecodeTransferContract = 0x00003007,
  E_TRON_UnsupportedToken = 0x00003008,
};

#endif  // __TRON_ERR_H__
