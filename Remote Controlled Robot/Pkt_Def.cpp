#pragma once

enum CmdType
{
	DRIVE, STATUS, SLEEP, ACK
};
struct Header
{

	unsigned int PktCount;
	unsigned char Drive : 1;
	unsigned char Status : 1;
	unsigned char Sleep : 1;
	unsigned char Ack : 1;
	//unsigned char pad : 4;
	unsigned char Length;
};
struct DriveBody
{
	unsigned char Direction;
	unsigned char Duration;
};
const int FORWARD = 1, BACKWARD = 2, RIGHT = 3, LEFT = 4, HEADERSIZE = 6;
class PktDef
{
	struct
	{
		Header Head;
		char *Body;
		char CRC;
	}CMDPacket;

	char *RawBuffer;

public:
	PktDef();
	PktDef(char*);
	void SetCmd(CmdType);
	void SetBodyData(char*, int);
	void SetPktCount(int);
	CmdType GetCmd();
	bool GetAck();
	int GetLength();
	char *GetBodyData();
	int GetPktCount();
	bool CheckCRC(char*, int);
	void CalcCRC();
	char* GenPacket();
	~PktDef();

};
