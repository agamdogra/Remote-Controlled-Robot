#include "Pkt_Def.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

PktDef::PktDef()
{
	memset(&CMDPacket.Head, 0, HEADERSIZE);
	CMDPacket.Head.Length = HEADERSIZE + 1;

	CMDPacket.Body = nullptr;
	RawBuffer = nullptr;
	/*
	CMDPacket.Head.PktCount = 0;
	CMDPacket.Head.Drive = 0;
	CMDPacket.Head.Status = 0;
	CMDPacket.Head.Sleep = 0;
	CMDPacket.Head.Ack = 0;
	CMDPacket.Head.pad = 0;
	*/
	CMDPacket.CRC = 0;

}

PktDef::PktDef(char* Buffer)
{
	RawBuffer = nullptr;
	memset(&CMDPacket.Head, 0, HEADERSIZE);
	memcpy(&CMDPacket.Head, Buffer, HEADERSIZE);


	if (CMDPacket.Head.Length > 7)
	{

		CMDPacket.Body = new char[(CMDPacket.Head.Length - HEADERSIZE) - 1];
		for (int x = HEADERSIZE, i = 0; i < (CMDPacket.Head.Length - HEADERSIZE) - 1; x++, i++)
		{
			CMDPacket.Body[i] = Buffer[x];
		}

		CMDPacket.CRC = Buffer[CMDPacket.Head.Length - 1];
	}
	else
	{
		CMDPacket.Body = nullptr;

		CMDPacket.CRC = Buffer[HEADERSIZE];
	}

}

void PktDef::SetCmd(CmdType enumcmdtype) {
	if (enumcmdtype == DRIVE) {
		CMDPacket.Head.Drive = 1;
		CMDPacket.Head.Status = 0;
		CMDPacket.Head.Sleep = 0;
		CMDPacket.Head.Ack = 0;
	}
	else if (enumcmdtype == STATUS) {
		CMDPacket.Head.Drive = 0;
		CMDPacket.Head.Status = 1;
		CMDPacket.Head.Sleep = 0;
		CMDPacket.Head.Ack = 0;
	}
	else if (enumcmdtype == SLEEP) {
		CMDPacket.Head.Drive = 0;
		CMDPacket.Head.Status = 0;
		CMDPacket.Head.Sleep = 1;
		CMDPacket.Head.Ack = 0;
	}
	else if (enumcmdtype == ACK) {
		if (CMDPacket.Head.Drive == 1 || CMDPacket.Head.Status == 1 || CMDPacket.Head.Sleep == 1)
			CMDPacket.Head.Ack = 1;
		else
			CMDPacket.Head.Ack = 0;
	}


}

int PktDef::GetLength()
{
	return (int)CMDPacket.Head.Length;
}

bool PktDef::GetAck()
{
	if (CMDPacket.Head.Ack == (unsigned)1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

CmdType PktDef::GetCmd()
{
	if (CMDPacket.Head.Drive)
		return DRIVE;

	if (CMDPacket.Head.Status)
		return STATUS;

	if (CMDPacket.Head.Sleep)
		return SLEEP;

	if (CMDPacket.Head.Ack)
		return ACK;

}

char* PktDef::GetBodyData()
{
	return CMDPacket.Body;
}

int PktDef::GetPktCount()
{
	return (int)CMDPacket.Head.PktCount;
}

char* PktDef::GenPacket()
{
	if (RawBuffer != nullptr)
	{
		delete[] RawBuffer;
		RawBuffer = nullptr;
	}

	RawBuffer = new char[CMDPacket.Head.Length];

	memcpy(RawBuffer, &CMDPacket.Head, HEADERSIZE);
	memcpy(&RawBuffer[HEADERSIZE], CMDPacket.Body, (CMDPacket.Head.Length - HEADERSIZE) - 1);
	memcpy(&RawBuffer[CMDPacket.Head.Length - 1], &CMDPacket.CRC, 1);

	return RawBuffer;
}

void PktDef::SetBodyData(char* src, int size)
{
	CMDPacket.Body = new char[size];

	for (int i = 0; i < size; i++)
	{
		CMDPacket.Body[i] = src[i];
	}

	CMDPacket.Head.Length += size;
}

void PktDef::SetPktCount(int count)
{
	CMDPacket.Head.PktCount = count;
}

bool PktDef::CheckCRC(char* Buffer, int size)
{
	int counter = 0;
	for (int x = 0; x < size - 1; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if ((Buffer[x] & 0x01) == 0x01)
				counter++;
			Buffer[x] = Buffer[x] >> 1;
		}
	}

	int crc = CMDPacket.CRC;

	if (crc == counter)
		return true;
	else
		return false;
}

void PktDef::CalcCRC()
{
	GenPacket();

	int counter = 0;

	for (int x = 0; x < CMDPacket.Head.Length - 1; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if ((RawBuffer[x] & 0x01) == 0x01)
				counter++;
			RawBuffer[x] = RawBuffer[x] >> 1;
		}
	}
	CMDPacket.CRC = counter;
}

PktDef::~PktDef()
{
	if (CMDPacket.Body != nullptr)
		delete[] CMDPacket.Body;
	if (RawBuffer != nullptr)
		delete[] RawBuffer;
	CMDPacket.Body = nullptr;
	RawBuffer = nullptr;
}
