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

#include "SRE3021SysReg.h"
using namespace hurel::sre3021;

hurel::sre3021::SRE3021SysReg::SRE3021SysReg(SRE3021SysRegisterADDR addr, unsigned int bitLength)
{
    Address = addr;
    BitLength = bitLength;
    Type = CheckType(addr);
}

hurel::sre3021::SRE3021SysReg::SRE3021SysReg()
{
    Address = SRE3021SysRegisterADDR::FAIL;
    BitLength = 0;
    Type = RWType::P;
    Value = 0;
}

RWType hurel::sre3021::SRE3021SysReg::CheckType(SRE3021SysRegisterADDR addr)
{
	unsigned __int16 __int16A = (unsigned __int16)addr;
	if (__int16A <= 0x0002)
	{
		return RWType::R;
	}
	else if (__int16A > 0x0002 && __int16A <= 0x11)
	{
		return RWType::RW;
	}
	else if (__int16A >= 0x0900 && __int16A <= 0x0A03)
	{
		return RWType::R;
	}
	else if (__int16A == 0x0C00)
	{
		return RWType::P;
	}
	else if (__int16A >= 0x0C01 && __int16A <= 0xF00F)
	{
		return RWType::RW;
	}
	else if (__int16A == 0xF010)
	{
		return RWType::P;
	}
	else if (__int16A >= 0XF011 && __int16A <= 0xf013)
	{
		return RWType::RW;
	}
	else
	{
		return RWType::R;
	}
}

int hurel::sre3021::SRE3021SysReg::GetByteLength(SRE3021SysRegisterADDR addr)
{
    unsigned __int16  __int16A = (unsigned __int16)addr;
    if (__int16A == 0x0000)
    {
        return 4;
    }
    else if (__int16A >= 0x0001 && __int16A <= 0x0002)
    {
        return 2;
    }
    else if (__int16A == 0x0010)
    {
        return 1;
    }
    else if (__int16A >= 0x0900 && __int16A <= 0x0A03)
    {
        return 2;
    }
    else if (__int16A >= 0x0C00 && __int16A <= 0x0C01)
    {
        return 1;
    }
    else if (__int16A == 0x0C02)
    {
        return 2;
    }
    else if (__int16A >= 0x0C03 && __int16A <= 0x0C04)
    {
        return 4;
    }
    else if (__int16A >= 0x0E00 && __int16A <= 0xF00F)
    {
        return 2;
    }
    else if (__int16A == 0xF010)
    {
        return 1;
    }
    else if (__int16A >= 0XF011 && __int16A <= 0xf013)
    {
        return 1;
    }
    else
    {

    }
}
