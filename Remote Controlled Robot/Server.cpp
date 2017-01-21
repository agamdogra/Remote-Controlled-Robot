#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include "Pkt_Def.h"
#include <iostream>
using namespace std;


void main(int argc, char *argv[])
{
	struct sockaddr_in SvrAddr;

	PktDef Packet;
	DriveBody Drive;
	int Packet_count = 0;
	char * Generated_Packet;
	SOCKET WelcomeSocket, ConnectionSocket;
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		return;

	if ((WelcomeSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		return;

	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	SvrAddr.sin_port = htons(5000);

	if ((bind(WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
	{
		closesocket(WelcomeSocket);
		WSACleanup();
		return;
	}

	if (listen(WelcomeSocket, 1) == SOCKET_ERROR)
	{
		closesocket(WelcomeSocket);
		return;
	}

	cout << "Waiting for client connection\n" << endl;
	ConnectionSocket = SOCKET_ERROR;

	char input[15] = {};
	char direction[15] = {};
	char RxBuffer[255] = {};

	do
	{
		if ((ConnectionSocket = accept(WelcomeSocket, NULL, NULL)) == SOCKET_ERROR)
		{
			return;
		}
		else
		{
			cout << "Connection Established" << endl;

			do
			{
				int duration;

				cout << "\nEnter a command DRIVE (1), STATUS (2), SLEEP (3) : ";
				cin >> input;

				if (strcmpi(input, "DRIVE") == 0 || atoi(input) == 1)
				{
					Packet.SetCmd(DRIVE);
					bool flag = false;


					do
					{
						cout << "\nEnter a Drive direction FORWARD (1), BACKWARD (2), RIGHT (3), LEFT (4) : ";
						cin >> input;

						if (strcmpi(input, "FORWARD") == 0 || atoi(input) == 1)
						{
							Drive.Direction = FORWARD;
							strcpy(input, "FORWARD");

						}
						else if (strcmpi(input, "BACKWARD") == 0 || atoi(input) == 2)
						{
							Drive.Direction = BACKWARD;
							strcpy(input, "BACKWARD");
						}
						else if (strcmpi(input, "LEFT") == 0 || atoi(input) == 3)
						{
							Drive.Direction = LEFT;
							strcpy(input, "LEFT");
						}
						else if (strcmpi(input, "RIGHT") == 0 || atoi(input) == 4)
						{
							Drive.Direction = RIGHT;
							strcpy(input, "RIGHT");
						}
						else
						{
							cout << "\nInvalid command";
							flag = true;
						}

					} while (flag == true);

					cout << "\nEnter the duration for the " << input << " direction : ";
					cin >> duration;
					Drive.Duration = duration;

					Packet.SetBodyData((char*)&Drive, 2);

					Packet_count++;
					Packet.SetPktCount(Packet_count);
					Packet.CalcCRC();
					Generated_Packet = Packet.GenPacket();

					send(ConnectionSocket, Generated_Packet, Packet.GetLength(), 0);

					recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);

					PktDef Received_packet(RxBuffer);
					Packet_count = Received_packet.GetPktCount();

					if (Received_packet.CheckCRC(RxBuffer, Received_packet.GetLength()))
						cout << "\nCRC is correct from the Acknowledgement packet";
					else
						cout << "\nCRC is NOT correct from the Acknowledgement packet";
				}
				else if (strcmpi(input, "STATUS") == 0 || atoi(input) == 2)
				{
					Packet.SetCmd(STATUS);
					Packet_count++;
					Packet.SetPktCount(Packet_count);
					Packet.CalcCRC();
					Generated_Packet = Packet.GenPacket();

					send(ConnectionSocket, Generated_Packet, Packet.GetLength(), 0);

					recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
					cout << "\nAcknowledgment Received";

					PktDef Received_packet(RxBuffer);
					Packet_count = Received_packet.GetPktCount();

					if (Received_packet.CheckCRC(RxBuffer, Received_packet.GetLength()))
					{
						cout << "\nCRC Accepted";

						recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
						cout << "\nReceived Status Command";

						PktDef Received_packet2(RxBuffer);
						Packet_count = Received_packet2.GetPktCount();
						if (Received_packet2.CheckCRC(RxBuffer, Received_packet2.GetLength()))
						{

							cout << "\nCRC is correct from the Status packet";
							cout << "\nPacket Length = " << Received_packet2.GetLength();
							cout << "\nPkt Counter = " << Received_packet2.GetPktCount();
							char* ptr = Received_packet2.GetBodyData();
							short Light;
							memcpy(&Light, ptr, 2);
							cout << "\nLight Reading = " << Light;
							cout << "\nSwitch 1 = " << ((bool)ptr[2] ? "TRUE" : "FALSE");
							cout << "\nSwitch 2 = " << ((bool)ptr[3] ? "TRUE" : "FALSE");
							cout << "\nRAW Data Bytes: " << hex;
							for (int i = 0; i < (Received_packet2.GetLength() - HEADERSIZE) - 1; i++)
							{
								cout << (int)*ptr++ << ',';
							}
							cout << dec;
						}
						else
							cout << "\nCRC is NOT correct from the Status packet";

					}
					else
						cout << "\nCRC is NOT correct from the Acknowledgement packet";

				}
				else if (strcmpi(input, "SLEEP") == 0 || atoi(input) == 3)
				{
					Packet.SetCmd(SLEEP);
					Packet_count++;
					Packet.SetPktCount(Packet_count);

					Packet.CalcCRC();
					Generated_Packet = Packet.GenPacket();

					send(ConnectionSocket, Generated_Packet, Packet.GetLength(), 0);

					recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);

					cout << "\n Acknowledgment Received";

					PktDef Received_packet(RxBuffer);
					Packet_count = Received_packet.GetPktCount();

					if (Received_packet.CheckCRC(RxBuffer, Received_packet.GetLength()))
					{
						cout << "\nCRC Accepted";
					}
					else
					{
						cout << "\nCRC not valid";
					}

					strcpy(input, "SLEEP");

				}
				else
				{
					cout << "\nInvalid Command";
				}



			} while (strcmpi(input, "SLEEP") != 0);// while loop end

			closesocket(ConnectionSocket);

		}// else end
	} while (strcmpi(input, "SLEEP") != 0);

	WSACleanup();
}