#!/bin/bash
#
# Copyright (C) 2009-2010 Ulrik Mikaelsson. All rights reserved
#
#   License:
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

CODE_DIR=$(dirname $0)
CODE_ROOT=$(dirname $CODE_DIR)
source $CODE_ROOT/common.sh

BHFUSE_MOUNTPOINT=bhfuse
DAEMON_SOCKET=$(readlink -f bithorded.sock)

TESTHASHES="3PPIRZ7KRLJJXCZPWKXYW3C64VRK45VHNBJ7FSY 5IQSBSLIQRSV7CRA6CFODJFO3F7475TVL6VKYCY NBV6LJVUSX3J4I5N7PQAJ5V2QKVK6PTJ7HVCQ5I T4FGCXKVBTLRQ2ZQQVUIVAJ4BLGCRWNKIGS6Y7Y T4FGCXKVBTLRQ2ZQQVUIVAJ4BLGCRWNKIGS6Y8Y WBANPJDT2TUYCT6R7RPO6SVH5N7XHBIKPDIR3HQ"

function setup() {
    mkdir -p $BHFUSE_MOUNTPOINT
}

setup || exit_error "Failed setup"

echo "Starting up Nodes..."
trap stop_children EXIT
bithorded_start src --server.unixSocket "$DAEMON_SOCKET"

echo "Starting bhfuse..."
"$BHFUSE" -u$DAEMON_SOCKET "$BHFUSE_MOUNTPOINT"

echo "Testing to read available files through bhfuse mount point..."
for TESTHASH in $TESTHASHES; do
  [ -e "${BHFUSE_MOUNTPOINT}/magnet:?xt=urn:tree:tiger:${TESTHASH}" ] || exit_error "Asset $TESTHASH not available through bhfuse"
done

exit_success
