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
#include "SRE3021PacketHeader.h"

using namespace std;
using namespace hurel::sre3021;
SRE3021PacketHeader::SRE3021PacketHeader(unsigned __int8& data)
{
	for (int i = 0; i < SRE3021_PACKET_HEADER_LENGTH; ++i)
	{
		ByteData[i] = (&data)[i];
	}
	unsigned __int8 idPacket[2]{ ByteData[1], ByteData[0] };
	auto calcPacketID = CalcPacketID(*idPacket);
	Version = get<0>(calcPacketID);
	SystemNumber = get<1>(calcPacketID);
	PacketType = get<2>(calcPacketID);

	unsigned __int8 sequencePacket[2]{ ByteData[3], ByteData[2] };
	auto calcPacketSequence = CalcPacketSequence(*sequencePacket);
	SequenceFlag = get<0>(calcPacketSequence);
	PacketCount = get<1>(calcPacketSequence);
	Reserved = 0;
	unsigned __int8 lengthPacket[2]{ ByteData[9], ByteData[8] };
	DataLength = GetValueFromBytes(*static_cast<unsigned __int16*>(static_cast<void*>(lengthPacket)), 0, 15);

	IsFromPCToSys = false;
}

static unsigned __int16 TotalPacketCount = 0;

SRE3021PacketHeader::SRE3021PacketHeader(SRE3021PacketType packetType, unsigned __int16 datalength)
{
	Version = 0;
	SystemNumber = 0;
	PacketType = packetType;
	SequenceFlag = SRE3021PacketSequence::STAND_ALONE;
	PacketCount = TotalPacketCount;
	Reserved = 0;
	DataLength = datalength;
	IsFromPCToSys = true;

	unsigned __int8 resultPacketID_SysNum = 0x00;
	unsigned __int8 resultPacket_Type = static_cast<unsigned __int8>(packetType);

	unsigned __int8* resultPacketSequence = static_cast<unsigned __int8*>(static_cast<void*>(&TotalPacketCount));


	unsigned __int8 resultPacketTimestamp[4]{0x0, 0x00, 0x00, 0x00};

	unsigned __int8* resultPacketDatalength = static_cast<unsigned __int8*>(static_cast<void*>(&datalength));
	unsigned __int8 bData[10]{
		resultPacketID_SysNum, resultPacket_Type, resultPacketSequence[1], resultPacketSequence[0], resultPacketTimestamp[3], resultPacketTimestamp[2], resultPacketTimestamp[1], resultPacketTimestamp[0], resultPacketDatalength[1], resultPacketDatalength[0]
	};
	for (int i = 0; i < 10; ++i)
	{
		ByteData[i] = bData[i];
	}
	++TotalPacketCount;
	constexpr int maxTotalPacketCount = 0b0100000000000000 - 1;
	if (TotalPacketCount >= maxTotalPacketCount)
	{
		TotalPacketCount = 0;
	}
}

std::tuple<SRE3021PacketSequence, int> SRE3021PacketHeader::CalcPacketSequence(unsigned __int8& data)
{
	int seFalg = GetValueFromBytes((&data)[1], 6, 7);
	unsigned __int8 pData[2]{ (&data)[0], (&data)[1] };
	int pCount = GetValueFromBytes(*static_cast<unsigned __int16*>(static_cast<void*>(&pData)), 0, 4);
	return std::tuple<SRE3021PacketSequence, int>(static_cast<SRE3021PacketSequence>(seFalg), pCount);
}


std::tuple<int, int, SRE3021PacketType> SRE3021PacketHeader::CalcPacketID(unsigned __int8& data)
{
	int ver = GetValueFromBytes((&data)[1], 5, 7);
	int sysNum = GetValueFromBytes((&data)[1], 0, 4);
	SRE3021PacketType pType = (SRE3021PacketType)GetValueFromBytes((&data)[0], 0, 7);

	return std::tuple<int, int, SRE3021PacketType>(ver, sysNum, pType);
}


int SRE3021PacketHeader::GetValueFromBytes(unsigned __int8 data, unsigned int startIdx, unsigned int endIdx)
{
	bitset<8> bits;

	for (unsigned int i = startIdx; i <= endIdx; ++i)
	{
		bool bit = data >> i & 0b0001;
		bits[i - startIdx] = bit;
	}
	return bits.to_ulong();
}

int SRE3021PacketHeader::GetValueFromBytes(unsigned __int16 data, unsigned int startIdx, unsigned int endIdx)
{
	bitset<16> bits;
#if LITTLE_ENDIAN
	for (unsigned int i = startIdx; i <= endIdx; ++i)
	{
		bool bit = data >> i & 0b0001;
		bits[i - startIdx] = bit;
	}
#endif
#if BIG_ENDIAN
	for (unsigned int i = startIdx; i <= endIdx; ++i)
	{
		bool bit = data >> i & 0b0001;
		if (i - startIdx < 8)
		{
			bits[i - startIdx + 7] = bit;
		}
		else
		{
			bits[i - startIdx - 7] = bit;
		}
		
	}
#endif
	return bits.to_ulong();
}
