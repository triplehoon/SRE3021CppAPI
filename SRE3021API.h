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

//Networking ฐüทร
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include "Network.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>

#include "SRE3021PacketHeader.h"
#include "SRE3021SysReg.h"
#include "SpectrumEnergy.h"


namespace hurel 
{
	namespace sre3021
	{	
		class SRE3021API
		{
		private:
			std::vector<SRE3021SysReg> SRE3021SysRegisters;								
			void ReadAllSysRegs();

			//System Register Configuration
			SRE3021SysReg DecodeReadbackSysReg(unsigned __int8 bytes[]);			
			bool ASICConfigBits[ASICBitSize];
			
			//ASIC Configuration
			void InitASICConifgBits();
			void DecodeReadbackASICReg(unsigned __int8 bytes[]);
			void ReadWriteASICReg(SRE3021ASICRegisterADDR addr, bool value);
			unsigned __int8 ConvertBoolArrayToByte(std::vector<bool> source);
			bool GetASICConfigtBitValue(SRE3021ASICRegisterADDR addr);

			std::thread udpThread;
			std::thread udpRaiserThread;
			bool isUdpServerOpen = false;

			void RunUDPServer();
			void UDPImageBufferRaiser();

			bool OpenUDPServer();
			void CloseUDPServer();

			//BaseLineCheck
			void CheckBaseline();
			size_t AnodeValueBaseline[11][11];
			size_t AnodeTimingBaseline[11][11];
			size_t CathodeValueBaseline = 0;
			size_t CathodeTimingBaseline = 0;
			double ProcessImgDataEnergyP1 = 0.321779;
			double ProcessImgDataEnergyP2 = -4.05354;

			SpectrumEnergy dataSpectrumEnergy;
			
			size_t UdpPacketCount = 0;

			std::mutex mutexUDPImageBuffer;
			std::mutex mutexUDPImageBufferRaiserFunc;
			std::queue<std::vector<unsigned __int8>> UDPImageBuffer;
			void (hurel::sre3021::SRE3021API::* UDPImageBufferRaiserFunc)(SRE3021ImageData) = nullptr;
			
			std::mutex mutexBaseLineImageEvents;
			std::vector<SRE3021ImageData> BaseLineImageEvents;
			void BaseLineEventCheck(SRE3021ImageData imageData);
		public:
			//System Register Configuration
			SRE3021SysReg ReadSysReg(SRE3021SysRegisterADDR address);
			SRE3021SysReg WriteSysReg(SRE3021SysRegisterADDR address, int value);

			void InitiateSRE3021API();
			~SRE3021API();
			
			//Aquire data
			int SetHighVoltage(int voltage, int step = 10, int sleepTimeInMillisecond = 100);
			void StartAcqusition(int HV = 1500, int VTHR = 2435, int VTHR0 = 2457, int Hold_DLY = 300, int VFP0 = 1750);
			void StopAcqusition();

			void SetImageProcessingFunc(void (hurel::sre3021::SRE3021API::*func)(SRE3021ImageData));			
			
			/// <summary>
			/// Basic ImageProcessing function. Get Image data and make spectrum (energy)
			/// </summary>
			/// <param name="imgData"></param>
			void BasicImageProcessingFunc(SRE3021ImageData imgData)
			{
				std::vector<int> interactionX;
				std::vector<int> interactionY;
				interactionX.reserve(121);
				interactionY.reserve(121);
				int interactionPotins = 0;
				int backgroundNoise = 0;

				for (int X = 0; X < 11; ++X)
				{
					for (int Y = 0; Y < 11; ++Y)
					{
						if (imgData.AnodeTiming[X][Y] > 250)
						{
							++interactionPotins;
							if (interactionPotins == 3)
							{
								return;
							}
							interactionX.push_back(X);
							interactionY.push_back(Y);
						}
						else
						{
							backgroundNoise += imgData.AnodeValue[X][Y];
						}

					}
				}
				if (interactionX.size() == 0)
				{
					return;
				}
				else if (interactionX.size() == 1)
				{
					backgroundNoise = backgroundNoise / 120;

					dataSpectrumEnergy.AddEnergy((static_cast<double>(imgData.AnodeValue[interactionX[0]][interactionY[0]]) - backgroundNoise) * ProcessImgDataEnergyP1 + ProcessImgDataEnergyP2);
				}
			};

			SpectrumEnergy GetSpectrum();
			void ResetSpectrum();

			bool CalibrateEnergySpectrumWith22Na(int minutes = 10);
			size_t GetUdpPacketCount();

		};
	};
};

 