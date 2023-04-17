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
#include "SRE3021API.h"

using namespace std;
using namespace hurel::sre3021;

void hurel::sre3021::SRE3021API::ReadAllSysRegs()
{
	SRE3021SysRegisters = vector<SRE3021SysReg>();
	for(SRE3021SysRegisterADDR reg : SysRegAddrAll)
	{
		SRE3021SysRegisters.push_back(ReadSysReg(reg));
	}

	ReadWriteASICReg(SRE3021ASICRegisterADDR::NOTSELECT, false);

/*	for (int i = 0; i < 650; ++i)
	{
		cout << ASICConfigBits[i] << endl;
	}*/	
}

void hurel::sre3021::SRE3021API::BaseLineEventCheck(SRE3021ImageData imageData)
{
	mutexBaseLineImageEvents.lock();
	BaseLineImageEvents.push_back(imageData);
	mutexBaseLineImageEvents.unlock();
}

SRE3021SysReg hurel::sre3021::SRE3021API::ReadSysReg(SRE3021SysRegisterADDR address)
{
	//open TCP Windows Only
	/*
		서버의 IP주소와 PORT번호를 저장합니다.
	*/
	const char* sre3021IPAddress = "10.10.0.50"; //Fixed
	enum ePort { PORT = 50010 };

	/*
		1. WinSock을 초기화합니다.
		: WSAStartup() 함수 사용
	*/
	WSADATA wsData;
	int wsResult = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (wsResult != 0)
	{
		std::cerr << "Can't start Winsock, Err #" << wsResult << std::endl;
		return SRE3021SysReg();
	}

	/*
		2. 소켓을 생성합니다.
		: socket() 함수 사용
	*/
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return SRE3021SysReg();
	}

	/*
		3. sockaddr_in 구조체 변수 생성

		- inet_pton(): char* 타입의 IP 주소를 바이트 타입으로 변환하여 3번째 인자에 저장합니다.
	*/
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT);
	int convResult = inet_pton(AF_INET, sre3021IPAddress, &hint.sin_addr);
	if (convResult != 1)
	{
		std::cerr << "Can't convert IP address, Err #" << convResult << std::endl;
		WSACleanup();
		return SRE3021SysReg();;
	}

	/*
		4. 서버에 연결(server의 bind와 비슷한 개념)
		: connect() 함수 사용, 소켓에 IP주소와 PORT번호와 같은 정보를 추가합니다.
	*/
	int connResult = connect(sock, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return SRE3021SysReg();
	}


	//end TCP Windows Only

	SRE3021PacketHeader pHeader = SRE3021PacketHeader(SRE3021PacketType::READ_SYS_REG, 2);
	int sendDataLength = SRE3021_PACKET_HEADER_LENGTH + 2;
	unsigned __int8 sendData[SRE3021_PACKET_HEADER_LENGTH + 2];
	for (int i = 0; i < SRE3021_PACKET_HEADER_LENGTH; ++i)
	{
		sendData[i] = pHeader.ByteData[i];
	}
	unsigned int addr = static_cast<unsigned int>(address);


#if BIG_ENDIAN
	sendData[10] = static_cast<unsigned __int8*>(static_cast<void*>(&addr))[0];
	sendData[11] = static_cast<unsigned __int8*>(static_cast<void*>(&addr))[1];
#endif // BID_ENDIAN

#if LITTLE_ENDIAN
	sendData[11] = static_cast<unsigned __int8*>(static_cast<void*>(&addr))[0];
	sendData[10] = static_cast<unsigned __int8*>(static_cast<void*>(&addr))[1];
#endif

	//Send data
	unsigned __int8 checkData[2048];
	/*
		서버에 연결 후 Do somthing
	*/
	// 입력한 메세지를 서버에 전송합니다.
	int sendResult = send(sock, static_cast<char*>(static_cast<void*>(sendData)), sendDataLength, 0);
	if (sendResult != SOCKET_ERROR)
	{
		// 서버로부터 전송된 메세지를 receive 합니다. (recv())
		int bytesReceived = recv(sock, static_cast<char*>(static_cast<void*>(checkData)), 2048, 0);
		if (bytesReceived <= 0)
		{
			return SRE3021SysReg();
			// 서버로부터 받은 메세지를 출력합니다.
			/*for (int i = 0; i < 2048; ++i)
			{
				std::cout << hex << static_cast<unsigned int>(checkData[i]) << endl;
			}*/
		}
	}

	//close TCP
	closesocket(sock);

	return DecodeReadbackSysReg(checkData);
}

SRE3021SysReg hurel::sre3021::SRE3021API::WriteSysReg(SRE3021SysRegisterADDR address, int value)
{
	///open TCP Windows Only
	/*
		서버의 IP주소와 PORT번호를 저장합니다.
	*/
	const char* sre3021IPAddress = "10.10.0.50"; //Fixed
	enum ePort { PORT = 50010 };

	/*
		1. WinSock을 초기화합니다.
		: WSAStartup() 함수 사용
	*/
	WSADATA wsData;
	int wsResult = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (wsResult != 0)
	{
		std::cerr << "Can't start Winsock, Err #" << wsResult << std::endl;
		return SRE3021SysReg();
	}

	/*
		2. 소켓을 생성합니다.
		: socket() 함수 사용
	*/
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return SRE3021SysReg();
	}

	/*
		3. sockaddr_in 구조체 변수 생성

		- inet_pton(): char* 타입의 IP 주소를 바이트 타입으로 변환하여 3번째 인자에 저장합니다.
	*/
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT);
	int convResult = inet_pton(AF_INET, sre3021IPAddress, &hint.sin_addr);
	if (convResult != 1)
	{
		std::cerr << "Can't convert IP address, Err #" << convResult << std::endl;
		WSACleanup();
		return SRE3021SysReg();;
	}

	/*
		4. 서버에 연결(server의 bind와 비슷한 개념)
		: connect() 함수 사용, 소켓에 IP주소와 PORT번호와 같은 정보를 추가합니다.
	*/
	int connResult = connect(sock, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return SRE3021SysReg();
	}


	//end TCP Windows Only

	if (SRE3021SysReg::CheckType(address) == RWType::R)
	{
		return SRE3021SysReg();
	}
	

	int registerLength = SRE3021SysReg::GetByteLength(address);

	vector<unsigned __int8> regData;
	regData.reserve(registerLength);
#if LITTLE_ENDIAN
	switch (registerLength)
	{
	case 1:
	{
		unsigned __int8 valueArray = static_cast<unsigned __int8>(value);
		regData.push_back(valueArray);
		break;
	}
	case 2:
	{
		unsigned __int16 valueArray = static_cast<unsigned __int16>(value);
		unsigned __int8* valueArrayPointer = static_cast<unsigned __int8*>(static_cast<void*>(&valueArray));
		regData.push_back(valueArrayPointer[1]);
		regData.push_back(valueArrayPointer[0]);
		break;
	}
	case 4:
	{
		unsigned __int32 valueArray = static_cast<unsigned __int16>(value);		
		unsigned __int8* valueArrayPointer = static_cast<unsigned __int8*>(static_cast<void*>(&valueArray));
		regData.push_back(valueArrayPointer[3]);
		regData.push_back(valueArrayPointer[2]);	
		regData.push_back(valueArrayPointer[1]);
		regData.push_back(valueArrayPointer[0]);
		break;
	}
	default:
		assert(0);
		return SRE3021SysReg();
	}
#endif

#if BIG_ENDIAN
	switch (registerLength)
	{
	case 1:
	{
		unsigned __int8 valueArray = static_cast<unsigned __int8>(value);
		regData.push_back(valueArray);
		break;
	}
	case 2:
	{
		unsigned __int16 valueArray = static_cast<unsigned __int16>(value);
		unsigned __int8* valueArrayPointer = static_cast<unsigned __int8*>(static_cast<void*>(&valueArray));
		regData.push_back(valueArrayPointer[0]);
		regData.push_back(valueArrayPointer[1]);
		break;
	}
	case 4:
	{
		unsigned __int32 valueArray = static_cast<unsigned __int16>(value);
		unsigned __int8* valueArrayPointer = static_cast<unsigned __int8*>(static_cast<void*>(&valueArray));
		regData.push_back(valueArrayPointer[0]);
		regData.push_back(valueArrayPointer[1]);
		regData.push_back(valueArrayPointer[2]);
		regData.push_back(valueArrayPointer[3]);
		break;
	}
	default:
		assert(0);
		return SRE3021SysReg();
	}
#endif
	unsigned __int8 sendData[2048];
	SRE3021PacketHeader pHeader = SRE3021PacketHeader(SRE3021PacketType::WRITE_SYS_REG, static_cast<unsigned __int16>(registerLength + 3));
	
	unsigned __int16 regAddress = static_cast<unsigned __int16>(address);
#if LITTLE_ENDIAN
	unsigned __int8 regAddressArray[2];
	regAddressArray[1] = static_cast<unsigned __int8*>(static_cast<void*>(&regAddress))[0];
	regAddressArray[0] = static_cast<unsigned __int8*>(static_cast<void*>(&regAddress))[1];
#endif
#if BIG_ENDIAN
	unsigned __int8 regAddressArray[2];
	regAddressArray = static_cast<unsigned __int8*>(static_cast<void*>(&regAddress));
#endif
	int sendDataLength = SRE3021_PACKET_HEADER_LENGTH + 2 + 1 + registerLength;
	int idx = 0;

	for (int i = 0; i < SRE3021_PACKET_HEADER_LENGTH; ++i)
	{
		sendData[idx++] = pHeader.ByteData[i];
		
	}
	sendData[idx++] = regAddressArray[0];	
	sendData[idx++] = regAddressArray[1];
	sendData[idx++] = static_cast<unsigned __int8>(registerLength);

	for (auto aReg : regData)
	{
		sendData[idx++] = aReg;
	}

	//Send data
	unsigned __int8 checkData[2048];
	/*
		서버에 연결 후 Do somthing
	*/
	// 입력한 메세지를 서버에 전송합니다.
	int sendResult = send(sock, static_cast<char*>(static_cast<void*>(sendData)), sendDataLength, 0);
	if (sendResult != SOCKET_ERROR)
	{
		// 서버로부터 전송된 메세지를 receive 합니다. (recv())
		int bytesReceived = recv(sock, static_cast<char*>(static_cast<void*>(checkData)), 2048, 0);
		if (bytesReceived <= 0)
		{
			return SRE3021SysReg();
			// 서버로부터 받은 메세지를 출력합니다.
			/*for (int i = 0; i < 2048; ++i)
			{
				std::cout << setw(2) << hex << static_cast<unsigned int>(checkData[i]) << endl;
			}*/
		}
	}

	//close TCP
	closesocket(sock);

	return DecodeReadbackSysReg(checkData);
}

void hurel::sre3021::SRE3021API::InitiateSRE3021API()
{
	// ping check to SRE3021
	FILE* pipe = _popen("ping 10.10.0.50 -n 2", "rt");
	if (pipe != NULL)
	{
		int rd = 0;
		size_t ret;

		char buffer[1024] = { 0 };
		while ((ret = fread(buffer + rd, 1, 1024 - rd, pipe)) > 0)
		{
			rd += static_cast<int>(ret);
		}

		if (strstr(buffer, "TTL=") != NULL)
		{
			printf("\nSRE3021 is reachable!\n");
		}
		else
		{
			printf("\nSRE3021 is NOT reachable!\n");
			return;
		}

		//printf( "%d bytes read\n\n%s\n", rd, buffer );

		_pclose(pipe);
	}
	else
	{
		printf("Error in pipe opening!\n");
		return;
	}

	cout << "Start initiating" << endl;
	OpenUDPServer();

	
	ReadWriteASICReg(SRE3021ASICRegisterADDR::Anode_Channel_3_Disable, true);

	ReadWriteASICReg(SRE3021ASICRegisterADDR::Anode_Channel_1_Disable, true);

	CheckBaseline();
	InitASICConifgBits();

	ReadAllSysRegs();
	dataSpectrumEnergy = SpectrumEnergy(5.0, 3000);
	

	ReadWriteASICReg(SRE3021ASICRegisterADDR::Anode_Channel_3_Disable, true);

	ReadWriteASICReg(SRE3021ASICRegisterADDR::Anode_Channel_1_Disable, true);
	UDPImageBufferRaiserFunc = &hurel::sre3021::SRE3021API::BasicImageProcessingFunc;
	return ;
}

SRE3021SysReg hurel::sre3021::SRE3021API::DecodeReadbackSysReg(unsigned __int8 bytes[])
{
	SRE3021PacketHeader pHeader = SRE3021PacketHeader(*bytes);
	assert(pHeader.PacketType == SRE3021PacketType::READBACK_SYS_REG);
	
	unsigned __int8 data[1024];
	if (pHeader.DataLength == 0)
	{
		return SRE3021SysReg();
	}

	for (int i = SRE3021_PACKET_HEADER_LENGTH; i < SRE3021_PACKET_HEADER_LENGTH + pHeader.DataLength; ++i)
	{
		data[i - SRE3021_PACKET_HEADER_LENGTH] = bytes[i];
	}
#if LITTLE_ENDIAN
	unsigned __int8 addrValueArray[2]{ data[1], data[0] };
#endif
#if BIG_ENDIAN
	unsigned __int8 addrValueArray[2]{ data[0], data[1] };
#endif

	SRE3021SysRegisterADDR addr = static_cast<SRE3021SysRegisterADDR>(*static_cast<unsigned __int16*>(static_cast<void*>(addrValueArray)));

	unsigned __int8 registerLength = data[2];
	unsigned __int64 regData;

#if LITTLE_ENDIAN
	switch (registerLength)
	{
	case 1:
	{		
		regData = static_cast<unsigned __int64>(data[3]);

		break;
	}
	case 2:
	{
		unsigned __int8 valueArrayPointer[2]{data[4], data[3]};
		regData = static_cast<unsigned __int64>(*static_cast<unsigned __int16*>(static_cast<void*>(valueArrayPointer)));

		break;
	}
	case 3:
	{
		unsigned __int8 valueArrayPointer[4]{ data[5], data[4], data[3], 0x00 };
		regData = static_cast<unsigned __int64>(*static_cast<unsigned __int32*>(static_cast<void*>(valueArrayPointer)));

		break;
	}
	case 4:
	{
		unsigned __int8 valueArrayPointer[4]{data[6], data[5], data[4], data[3]};
		regData = static_cast<unsigned __int64>(*static_cast<unsigned __int32*>(static_cast<void*>(valueArrayPointer)));
		
		break;
	}
	case 8:
	{
		unsigned __int8 valueArrayPointer[8]{ data[10], data[9], data[8], data[7], data[6], data[5], data[4], data[3] };
		regData = *static_cast<unsigned __int64*>(static_cast<void*>(valueArrayPointer));

		break;
	}
	default:
		assert(0);
		return SRE3021SysReg();
	}
#endif

#if BIG_ENDIAN
	switch (registerLength)
	{
	case 1:
	{
		regData = static_cast<unsigned __int64>(data[3]);

		break;
	}
	case 2:
	{
		unsigned __int16 valueArray;
		unsigned __int8 valueArrayPointer[2]{ data[3], data[4] };
		regData = static_cast<unsigned __int64>(*static_cast<unsigned __int16*>(static_cast<void*>(valueArrayPointer)));

		break;
	}
	case 3:
	{
		unsigned __int32 valueArray;
		unsigned __int8 valueArrayPointer[4]{ 0x00, data[3], data[4], data[5]};
		regData = static_cast<unsigned __int64>(*static_cast<unsigned __int32*>(static_cast<void*>(valueArrayPointer)));

		break;
	}
	case 4:
	{
		unsigned __int32 valueArray;
		unsigned __int8 valueArrayPointer[4]{ data[3], data[4], data[5], data[6] };
		regData = static_cast<unsigned __int64>(*static_cast<unsigned __int32*>(static_cast<void*>(valueArrayPointer)));

		break;
	}
	case 8:
	{
		unsigned __int64 valueArray;
		unsigned __int8 valueArrayPointer[8]{ data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10] };
		regData = static_cast<unsigned __int64>(*static_cast<unsigned __int64*>(static_cast<void*>(valueArrayPointer)));

		break;
	}
	default:
		assert(0);
		return SRE3021SysReg();
	}
#endif

	SRE3021SysReg sysReg = SRE3021SysReg(addr, registerLength * 8);
	sysReg.Value = regData;
	
	return sysReg;
}

void hurel::sre3021::SRE3021API::InitASICConifgBits()
{
	unsigned __int8 initBytes[82]{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										  0x00, 0x00, 0x1b, 0x20, 0x00 };
	DecodeReadbackASICReg(initBytes);




	for (int j = 1; j <= 650; ++j)
	{
		bool result = GetASICConfigtBitValue((SRE3021ASICRegisterADDR)j);
		//Debug.WriteLine("{0} is {1}", ((SRE3021ASICRegisterADDR)j).ToString(), result);
	}
}

void hurel::sre3021::SRE3021API::ReadWriteASICReg(SRE3021ASICRegisterADDR addr, bool value)
{

	//open TCP Windows Only
	/*
		서버의 IP주소와 PORT번호를 저장합니다.
	*/
	const char* sre3021IPAddress = "10.10.0.50"; //Fixed
	enum ePort { PORT = 50010 };

	/*
		1. WinSock을 초기화합니다.
		: WSAStartup() 함수 사용
	*/
	WSADATA wsData;
	int wsResult = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (wsResult != 0)
	{
		std::cerr << "Can't start Winsock, Err #" << wsResult << std::endl;
		return;
	}

	/*
		2. 소켓을 생성합니다.
		: socket() 함수 사용
	*/
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}

	/*
		3. sockaddr_in 구조체 변수 생성

		- inet_pton(): char* 타입의 IP 주소를 바이트 타입으로 변환하여 3번째 인자에 저장합니다.
	*/
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT);
	int convResult = inet_pton(AF_INET, sre3021IPAddress, &hint.sin_addr);
	if (convResult != 1)
	{
		std::cerr << "Can't convert IP address, Err #" << convResult << std::endl;
		WSACleanup();
		return;
	}

	/*
		4. 서버에 연결(server의 bind와 비슷한 개념)
		: connect() 함수 사용, 소켓에 IP주소와 PORT번호와 같은 정보를 추가합니다.
	*/
	int connResult = connect(sock, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}


	//end TCP Windows Only

	SRE3021PacketHeader pHeader = SRE3021PacketHeader(SRE3021PacketType::RW_ASIC_REG, 85);
	unsigned __int8 data[82];
	if (addr != SRE3021ASICRegisterADDR::NOTSELECT)
	{
		ASICConfigBits[(650 - (int)addr)] = value;
	}

	bool changedConfig[650];
	for (int i = 0; i < 650; ++i)
	{
		changedConfig[i] = ASICConfigBits[i];
	}	
	vector<bool> configBits;
	configBits.reserve(8);
	for (int i = 0; i < 81; ++i)
	{
	
		for (int j = 0; j < 8; ++j)
		{
			configBits.push_back(ASICConfigBits[8 * i + j]);
		}
		data[i] = ConvertBoolArrayToByte(configBits);
		configBits.clear();		
	}
	for (int j = 0; j < 1; ++j)
	{
		configBits.push_back(ASICConfigBits[8 * 81 + j]);
	}
	data[81] = ConvertBoolArrayToByte(configBits);

	unsigned __int8 sendData[SRE3021_PACKET_HEADER_LENGTH + 2 + 1 + 82];
	int sendDataLength = SRE3021_PACKET_HEADER_LENGTH + 2 + 1 + 82;
	int idx = 0;
	

	for (int i = 0; i < SRE3021_PACKET_HEADER_LENGTH; ++i)
	{
		sendData[idx++] = pHeader.ByteData[i];
	}
	sendData[idx++] = 0x00;
	sendData[idx++] = 0x02;
	sendData[idx++] = 0x8a;
	
	for (int i = 0; i < 82; ++i)
	{
		sendData[idx++] = data[i];
	}
	

	//Send data
	//Error fix -> readback twice to set ASIC properly
	unsigned __int8 checkBytes[2048];
	/*
		서버에 연결 후 Do somthing
	*/
	// 입력한 메세지를 서버에 전송합니다.
	int sendResult = send(sock, static_cast<char*>(static_cast<void*>(sendData)), sendDataLength, 0);
	if (sendResult != SOCKET_ERROR)
	{
		// 서버로부터 전송된 메세지를 receive 합니다. (recv())
		int bytesReceived = recv(sock, static_cast<char*>(static_cast<void*>(checkBytes)), 2048, 0);
		if (bytesReceived <= 0)
		{
			return;
			//// 서버로부터 받은 메세지를 출력합니다.
			//for (int i = 0; i < 2048; ++i)
			//{
			//	std::cout << setw(2) << hex << static_cast<unsigned int>(checkBytes[i]) << endl;
			//}
		}
	}

	// 입력한 메세지를 서버에 전송합니다.
	sendResult = send(sock, static_cast<char*>(static_cast<void*>(sendData)), sendDataLength, 0);
	if (sendResult != SOCKET_ERROR)
	{
		// 서버로부터 전송된 메세지를 receive 합니다. (recv())
		int bytesReceived = recv(sock, static_cast<char*>(static_cast<void*>(checkBytes)), 2048, 0);
		if (bytesReceived <= 0)
		{
			return;
			//// 서버로부터 받은 메세지를 출력합니다.
			//for (int i = 0; i < 2048; ++i)
			//{
			//	std::cout << setw(2) << hex << static_cast<unsigned int>(checkBytes[i]) << endl;
			//}
		}
	}

	//close TCP
	closesocket(sock);

	DecodeReadbackASICReg(&checkBytes[13]);

	for (int i = 0; i < 650; ++i)
	{
		if (changedConfig[i] != ASICConfigBits[i])
		{
			assert(0);
		}
	}

}

void hurel::sre3021::SRE3021API::DecodeReadbackASICReg(unsigned __int8 bytes[])
{
	int i = 0;
	for (int j = 0; j < 3000; ++j)
	{	
		for (int k = 0; k < 8; ++k)
		{
			if (i == 650)
			{
				break;
			}
			ASICConfigBits[i]  = bytes[j]>> (7 - k) & 0b0001;
			++i;
		}

	}
}

unsigned __int8 hurel::sre3021::SRE3021API::ConvertBoolArrayToByte(std::vector<bool> source)
{
	unsigned __int8 result = 0;
	// This assumes the array never contains more than 8 elements!
	int index = 0;

	// Loop through the array
	for(bool b : source)
	{
		// if the element is 'true' set the bit at that position
		if (b)
		{
			result |= (unsigned __int8)(1 << (7 - index));
		}

		index++;
	}

	return result;
}

bool hurel::sre3021::SRE3021API::GetASICConfigtBitValue(SRE3021ASICRegisterADDR addr)
{
	return ASICConfigBits[650 - static_cast<int>(addr)];
}

bool hurel::sre3021::SRE3021API::OpenUDPServer()
{
	isUdpServerOpen = true;
	udpThread = thread([this] {RunUDPServer(); });
	
	
	udpRaiserThread = thread([this] {UDPImageBufferRaiser(); });
	return true;
}

void hurel::sre3021::SRE3021API::RunUDPServer()
{
	char* Buffer = new char[16 * 4096];
	
	int PORT = 50011;
	
	
	WSASession Session;
	UDPSocket Socket;

	Socket.Bind(PORT);
	while (true)
	{
		int dataSize = 0;
		sockaddr_in add = Socket.RecvFrom(Buffer, 16 * 4096, 0, &dataSize);
		if (dataSize == 514)
		{
			mutexUDPImageBuffer.lock();
			//std::cout << "Data received: " << dataSize << std::endl
			UdpPacketCount++;
			if( UdpPacketCount % 100000 == 0)
			{
				printf("UDP Packet Count %d\n", UdpPacketCount);
			}
			
			vector<unsigned __int8> v(Buffer, Buffer + 514);
			UDPImageBuffer.push(v);
			mutexUDPImageBuffer.unlock();
		}
		if (!isUdpServerOpen)
		{
			break;
		}
	}

	delete[] Buffer;


	printf("udp listener end\n");
	return;
}

void hurel::sre3021::SRE3021API::UDPImageBufferRaiser()
{
	while (true)
	{
		if (!isUdpServerOpen)
		{
			break;
		}
		mutexUDPImageBufferRaiserFunc.lock();
		if (UDPImageBufferRaiserFunc == nullptr)
		{
			mutexUDPImageBufferRaiserFunc.unlock();
			continue;
		}
		mutexUDPImageBufferRaiserFunc.unlock();
		bool bIslock = false;
		mutexUDPImageBuffer.lock();
		bIslock = true;
		if (UDPImageBuffer.size() != 0)
		{
			std::vector<unsigned __int8> bytes = UDPImageBuffer.front();
			UDPImageBuffer.pop();
			mutexUDPImageBuffer.unlock();
			
			bIslock = false;
			unsigned __int8 headerbytes[10];
			for (int i = 0; i < 10; ++i)
			{
				headerbytes[i] = bytes[i];
			}
			auto pHeader = SRE3021PacketHeader(*headerbytes);
			if (pHeader.PacketType != SRE3021PacketType::IMG_DATA)
			{
				return;
			}
			SRE3021ImageData imageData;
#if LITTLE_ENDIAN
			unsigned __int8 catEByte[2]{ bytes[21], bytes[20] };
			imageData.CathodeValue = static_cast<size_t>(*static_cast<unsigned __int16*>(static_cast<void*>(catEByte))) - CathodeValueBaseline;
			unsigned __int8 catTByte[2]{ bytes[23], bytes[22] };
			imageData.CathodeTiming = static_cast<size_t>(*static_cast<unsigned __int16*>(static_cast<void*>(catTByte))) - CathodeTimingBaseline;
			int anodeE[11][11];
			int anodeT[11][11];
			int UnusableChannel1 = ASICChannelNumber[2][0];
			int UnusableChannel2 = ASICChannelNumber[0][0];
			int imgOrder = 0;
			for (int Y = 0; Y < 11; ++Y)
			{
				for (int X = 0; X < 11; ++X)
				{
					unsigned __int8 anodeEByte[2]{ bytes[31 + imgOrder], bytes[30 + imgOrder] };
					unsigned __int8 anodeTByte[2]{ bytes[273 + imgOrder], bytes[272 + imgOrder] };
					imageData.AnodeValue[X][Y] = *static_cast<unsigned __int16*>(static_cast<void*>(anodeEByte)) - AnodeValueBaseline[X][Y];
					imageData.AnodeTiming[X][Y] = *static_cast<unsigned __int16*>(static_cast<void*>(anodeTByte)) - AnodeTimingBaseline[X][Y];
					imgOrder += 2;

				}
			}
#endif
#if BIG_ENDIAN
			unsigned __int8 catEByte[2]{ bytes[20], bytes[21] };
			imageData.CathodeValue = static_cast<int>(*static_cast<unsigned __int16*>(static_cast<void*>(catEByte))) - CathodeValueBaseline;
			unsigned __int8 catTByte[2]{ bytes[22], bytes[23] };
			imageData.CathodeTiming = static_cast<int>(*static_cast<unsigned __int16*>(static_cast<void*>(catTByte))) - CathodeTimingBaseline;
			int anodeE[11][11];
			int anodeT[11][11];
			int UnusableChannel1 = ASICChannelNumber[2][0];
			int UnusableChannel2 = ASICChannelNumber[0][0];
			int imgOrder = 0;
			for (int Y = 0; Y < 11; ++Y)
			{
				for (int X = 0; X < 11; ++X)
				{
					unsigned __int8 anodeEByte[2]{ bytes[30 + imgOrder], bytes[31 + imgOrder] };
					unsigned __int8 anodeTByte[2]{ bytes[272 + imgOrder], bytes[273 + imgOrder] };
					imageData.AnodeValue[X][Y] = static_cast<int>(*static_cast<unsigned __int16*>(static_cast<void*>(anodeEByte))) - AnodeValueBaseline[X][Y];
					imageData.AnodeTiming[X][Y] = static_cast<int>(*static_cast<unsigned __int16*>(static_cast<void*>(anodeTByte))) - AnodeTimingBaseline[X][Y];
					imgOrder += 2;

				}
			}
#endif
			

			(this->*UDPImageBufferRaiserFunc)(imageData);
		}

		if (bIslock)
		{
			mutexUDPImageBuffer.unlock();
		}
		
	}
}

void hurel::sre3021::SRE3021API::CloseUDPServer()
{
	if (isUdpServerOpen)
	{
		isUdpServerOpen = false;
		if (udpThread.joinable())
		{
			udpThread.join();
		}				
		if (udpRaiserThread.joinable())
		{
			udpRaiserThread.join();
		}
	}
	
	
}

void hurel::sre3021::SRE3021API::CheckBaseline()
{
	int numPulses = 5000;
	WriteSysReg(SRE3021SysRegisterADDR::CFG_PHYSTRIG_EN, false);
	printf("SRE3021API: Set HV as 0 for checking basline\n");
	SetHighVoltage(0);

	printf("SRE3021API: Configuring ASIC.\n");
	ReadWriteASICReg(SRE3021ASICRegisterADDR::Test_on, false);
	ReadWriteASICReg(SRE3021ASICRegisterADDR::Current_compensation_enable, true);

	printf("SRE3021API: Configuring system.\n");
	WriteSysReg(SRE3021SysRegisterADDR::CFG_FIXED_CH, 128); //This vale ensures that single channel readout is not envoked during baseline readout.
	WriteSysReg(SRE3021SysRegisterADDR::CFG_HOLD_DLY, 1); //This is the minimum value for the peak-hold to pick up less noise.
	WriteSysReg(SRE3021SysRegisterADDR::CFG_CALTRIG_EN, 1);

	printf("SRE3021API: Configuring calibration pulse genenerator (CalGen).\n");
	WriteSysReg(SRE3021SysRegisterADDR::CAL_PULSE_POLAR, 0);
	WriteSysReg(SRE3021SysRegisterADDR::CAL_NUM_PULSE, numPulses);
	WriteSysReg(SRE3021SysRegisterADDR::CAL_PULSE_LENGTH, 5);
	WriteSysReg(SRE3021SysRegisterADDR::CAL_PULSE_INTERVAL, 40000);

	printf("SRE3021API: Start pulse genenerator (CalGen).\n");

	clock_t start, end;

	SetImageProcessingFunc(&hurel::sre3021::SRE3021API::BaseLineEventCheck);

	WriteSysReg(SRE3021SysRegisterADDR::CAL_EXECUTE, 1);

	start = clock();
	mutexBaseLineImageEvents.lock();
	int BaseLineEventsCount = BaseLineImageEvents.size(); 
	mutexBaseLineImageEvents.unlock();
	while (BaseLineEventsCount < numPulses)
	{
		end = clock();

		if ((static_cast<float>(end - start)) / CLOCKS_PER_SEC > 5)
		{
			mutexBaseLineImageEvents.lock();
			BaseLineEventsCount = BaseLineImageEvents.size();
			
			printf("SRE3021API: BaseLineImageEvents Count is %d\n", static_cast<int>(BaseLineImageEvents.size()));
			if (BaseLineEventsCount > numPulses * 0.999)
			{
				mutexBaseLineImageEvents.unlock();
				break;
			}
			else
			{
				mutexBaseLineImageEvents.unlock();
				printf("SRE3021API: Retart pulse genenerator (CalGen) [%d].\n", static_cast<int>(BaseLineImageEvents.size()));
				WriteSysReg(SRE3021SysRegisterADDR::CAL_EXECUTE, 1);
				start = clock();
			}			
		}
		
	}
	end = clock();
	printf("SRE3021API: Baseline Cal Execute time is %.2f [s] %d #\n", static_cast<float>(end / CLOCKS_PER_SEC), static_cast<int>(BaseLineEventsCount));
	SetImageProcessingFunc(nullptr);

	size_t baselineSum[11][11];
	size_t baselineTimingSum[11][11];

	for (int i = 0; i < 11; ++i)
	{
		for (int j = 0; j < 11; ++j)
		{
			baselineSum[i][j] = 0;
			baselineTimingSum[i][j] = 0;
		}
	}
	size_t catbaslineSum = 0;
	size_t catTimingbaselineSum = 0;
	mutexBaseLineImageEvents.lock();
	for(auto imageData : BaseLineImageEvents)
	{
		catbaslineSum += imageData.CathodeValue;
		catTimingbaselineSum += imageData.CathodeTiming;
		for (int i = 0; i < 11; ++i)
		{
			for (int j = 0; j < 11; ++j)
			{
				baselineSum[i][j] += imageData.AnodeValue[i][j];
				baselineTimingSum[i][j] += imageData.AnodeTiming[i][j];
			}
		}
	}
	CathodeValueBaseline = static_cast<size_t>(catbaslineSum / BaseLineImageEvents.size());
	CathodeTimingBaseline = static_cast<size_t>(catTimingbaselineSum / BaseLineImageEvents.size());
	for (int i = 0; i < 11; ++i)
	{
		for (int j = 0; j < 11; ++j)
		{
			AnodeValueBaseline[i][j] = static_cast<size_t>(baselineSum[i][j] / BaseLineImageEvents.size());
			AnodeTimingBaseline[i][j] = static_cast<size_t>(baselineTimingSum[i][j] / BaseLineImageEvents.size());
		}
	}

	BaseLineImageEvents.clear();
	SetImageProcessingFunc(&hurel::sre3021::SRE3021API::BasicImageProcessingFunc);	

	mutexBaseLineImageEvents.unlock();
	return;
}

int hurel::sre3021::SRE3021API::SetHighVoltage(int voltage, int step, int sleepTimeInMillisecond)
{
	int CurrentVoltage = static_cast<int>(ReadSysReg(SRE3021SysRegisterADDR::HV_DAC).Value);

	if (CurrentVoltage > voltage)
	{
		int setVoltage = CurrentVoltage;
		while (voltage < static_cast<int>(ReadSysReg(SRE3021SysRegisterADDR::HV_DAC).Value))
		{
			if (voltage + step < CurrentVoltage)
			{
				setVoltage -= step;
			}
			else
			{
				setVoltage = voltage;
			}

			CurrentVoltage = static_cast<int>(WriteSysReg(SRE3021SysRegisterADDR::HV_DAC, setVoltage).Value);
			if (CurrentVoltage % 200 == 0)
			{
				printf("SRE3021API: Voltage drop to %d V \n", CurrentVoltage);
			}
			this_thread::sleep_for(std::chrono::milliseconds(sleepTimeInMillisecond));
		}
	}
	else
	{
		int setVoltage = CurrentVoltage;
		while (voltage > static_cast<int>(ReadSysReg(SRE3021SysRegisterADDR::HV_DAC).Value))
		{
			if (voltage - step > CurrentVoltage)
			{
				setVoltage += step;
			}
			else
			{
				setVoltage = voltage;
			}

			CurrentVoltage = static_cast<int>(WriteSysReg(SRE3021SysRegisterADDR::HV_DAC, setVoltage).Value);
			if (CurrentVoltage % 100 == 0)
			{
				printf("SRE3021API: Voltage rise to %d V \n", CurrentVoltage);
			}
			this_thread::sleep_for(std::chrono::milliseconds(sleepTimeInMillisecond));

		}
	}
	return CurrentVoltage;
}

void hurel::sre3021::SRE3021API::StartAcqusition(int HV, int VTHR, int VTHR0, int Hold_DLY, int VFP0)
{
	ReadWriteASICReg(SRE3021ASICRegisterADDR::Test_on, false);
	ReadWriteASICReg(SRE3021ASICRegisterADDR::Test_on, false);
	ReadWriteASICReg(SRE3021ASICRegisterADDR::Current_compensation_enable, true);
	WriteSysReg(SRE3021SysRegisterADDR::VTHR, VTHR);
	WriteSysReg(SRE3021SysRegisterADDR::VTHR0, VTHR0);
	WriteSysReg(SRE3021SysRegisterADDR::VFP0, VFP0);
	WriteSysReg(SRE3021SysRegisterADDR::CFG_PHYSTRIG_EN, 0);
	WriteSysReg(SRE3021SysRegisterADDR::CMD_SET_CH, 0);
	WriteSysReg(SRE3021SysRegisterADDR::CFG_CALTRIG_EN, 0);
	WriteSysReg(SRE3021SysRegisterADDR::CFG_HOLD_DLY, Hold_DLY); //default 300
	SetHighVoltage(HV, 10, 50);


	WriteSysReg(SRE3021SysRegisterADDR::CFG_PHYSTRIG_EN, 1);
}

void hurel::sre3021::SRE3021API::StopAcqusition()
{
	WriteSysReg(SRE3021SysRegisterADDR::CFG_PHYSTRIG_EN, 0);
	SetHighVoltage(0, 10, 50);
}

void hurel::sre3021::SRE3021API::SetImageProcessingFunc(void (hurel::sre3021::SRE3021API::*func)(SRE3021ImageData))
{
	mutexUDPImageBufferRaiserFunc.lock();
	UDPImageBufferRaiserFunc = func;
	mutexUDPImageBufferRaiserFunc.unlock();
}

SpectrumEnergy hurel::sre3021::SRE3021API::GetSpectrum()
{
	return dataSpectrumEnergy;
}

void hurel::sre3021::SRE3021API::ResetSpectrum()
{
	dataSpectrumEnergy.Reset();
	return;
}

size_t hurel::sre3021::SRE3021API::GetUdpPacketCount()
{
	return UdpPacketCount;
}

bool hurel::sre3021::SRE3021API::CalibrateEnergySpectrumWith22Na(int minutes)
{
	ResetSpectrum();
	StartAcqusition();
	cout << "Started loop.." << endl;
	for (int i = 0; i < 60 * minutes; ++i) {
		cout << i << " seconds: packetcounts = " << GetUdpPacketCount() << endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::vector<double> peaks2 = GetSpectrum().FindPeaks();
		for (double p : peaks2)
		{
			cout << p << ", ";			
		}
		cout << endl;
	}
	GetSpectrum().PrintSpectrum();
	std::vector<double> peaks = GetSpectrum().FindPeaks();
	cout << "Find peaks: ";

	double peak511 = 0;
	double peak1275 = 0;

	for (double p : peaks)
	{
		cout << p << ", ";
		if (p > 450 && p < 560)
		{
			peak511 = p;
		}
		if (p > 1275 - 100 && p < 1275 + 100)
		{
			peak1275 = p;
		}
	}
	cout << endl;
	StopAcqusition();
	if (peak511 == 0 || peak1275 == 0)
	{
		printf("peak not found\n");
		return false;
	}
	double Channel511 = (peak511 - ProcessImgDataEnergyP2) / ProcessImgDataEnergyP1;
	double Channel1275 = (peak1275 - ProcessImgDataEnergyP2) / ProcessImgDataEnergyP1;


	ProcessImgDataEnergyP1 =  (1275 - 511) / (Channel1275 - Channel511);
	ProcessImgDataEnergyP2 = ProcessImgDataEnergyP1 * Channel511 - 511;

	printf("22Na calibration done\n");
	printf("P1: %f\n", ProcessImgDataEnergyP1);
	printf("P2: %f\n", ProcessImgDataEnergyP2);
	return true;
}

hurel::sre3021::SRE3021API::~SRE3021API()
{
	CloseUDPServer();
	WSACleanup();
}






