# This file is part of the OneKey project, https://onekey.so/
#
# Copyright (C) 2021 OneKey Team <core@onekey.so>
#
# This library is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library.  If not, see <http://www.gnu.org/licenses/>.


from typing import TYPE_CHECKING

from . import messages
from .tools import expect

if TYPE_CHECKING:
    from .client import TrezorClient


@expect(messages.LnurlAuthResp)
def auth(client: "TrezorClient", domain: bytes, data: bytes):
    return client.call(messages.LnurlAuth(domain=domain, data=data))
