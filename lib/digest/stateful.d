/****************************************************************************************
 *   Copyright: Copyright (C) 2009-2010 Ulrik Mikaelsson. All rights reserved
 *
 *   License:
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ***************************************************************************************/
module lib.digest.stateful;

public import tango.util.digest.Digest;

/****************************************************************************************
 * IStatefulDigest is the interface for Digesters able to store and load their state.
 * The state should be represented by a variable sequence of bytes in machine-
 * independent form. (Network Byte Order where it applies)
 *
 * TODO: Refactor to take OutputData-instances, or at least OutputStreams directly.
 ***************************************************************************************/
interface IStatefulDigest {
    /************************************************************************************
     * Store the current state into a provided buffer, and return the used slice of the
     * buffer.
     ***********************************************************************************/
    ubyte[] save(ubyte[] buf);

    /************************************************************************************
     * Load the state provided in buf, and return the number of consumed bytes.
     ***********************************************************************************/
    size_t load(ubyte[] buf);

    /************************************************************************************
     * Return the maximum required size for preserving the state. In short, the
     * size of the buffer the caller needs to allocate before calling store().
     ***********************************************************************************/
    size_t maxStateSize();

    /// See: tango.util.digest.Digest
    Digest update(void[] data);
    /// ditto
    ubyte[] binaryDigest(ubyte[] buffer = null);
    /// ditto
    uint digestSize();
    /// ditto
    char[] hexDigest(char[] buffer = null);
}
