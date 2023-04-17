// ----------------------------------------------------------------------------
// -                        SRE3021API C++ version                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2022-2022 Choi, Sehoon (triplehoon95@hanyang.ac.kr)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------
#pragma once

#include <cassert>
#include <tuple>
#include <bitset>

#include "SRE3021Types.h"

namespace hurel {
    namespace sre3021 {
        class SRE3021PacketHeader
        {

        public:
            /// <summary>
            /// 3 bits. default 0
            /// </summary>
            int Version;

            /// <summary>
            /// 5 bits
            /// </summary>
            int SystemNumber;

            /// <summary>
            /// 8 bits, PacketType
            /// </summary>
            SRE3021PacketType PacketType;

            /// <summary>
            /// 2 bits, PacketSeqeunce
            /// </summary>
            SRE3021PacketSequence SequenceFlag;

            /// <summary>
            /// 14 bits, Incremental counter for packet identification
            /// </summary>
            int PacketCount;

            /// <summary>
            /// 32 bits, fixed = 0 // or TimeStamp
            /// </summary>
            int Reserved;

            /// <summary>
            /// Uint16, Data length
            /// </summary>
            int DataLength;

            /// <summary>
            /// Byte array, Decode / Encode data
            /// </summary>
            unsigned __int8 ByteData[10];

            /// <summary>
            /// Type
            /// </summary>
            bool IsFromPCToSys;

            SRE3021PacketHeader(unsigned __int8& data);
            SRE3021PacketHeader(SRE3021PacketType packetType, unsigned __int16 datalength);

            std::tuple<int, int, SRE3021PacketType> CalcPacketID(unsigned __int8& data);

            std::tuple<SRE3021PacketSequence, int> CalcPacketSequence(unsigned __int8& data);

            int GetValueFromBytes(unsigned __int8 data, unsigned int startIdx, unsigned int endIdx);
            int GetValueFromBytes(unsigned __int16 data, unsigned int startIdx, unsigned int endIdx);


        };
    }
};