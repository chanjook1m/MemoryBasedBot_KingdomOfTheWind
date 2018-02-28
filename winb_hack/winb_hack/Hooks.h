#pragma once

#include "Properties.h"
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <WinSock2.h>
class Client;

class Hooks
{
private:
	static Client* Owner;
public:
	Hooks(Client* Owner)
	{
		this->Owner = Owner;
	}

	static unsigned char encrypted[100];
	static void Send_Packet_Hook(void);
	static void Recv_Packet_Hook(void);
	static void Con_Packet_Hook(void);

	static void LoadEncrypt(char* Source, int SourceSize, unsigned char* Dest);
	static int Outgoing_Packet_Length;
	static int Outgoing_Packet_Pointer;
	static int Outgoing_Packet_Return_Address;

	static int Packet_Encrypt_Return_Address;

	static int Packet_Recv_Return_Address;
	static int Ingoing_Packet_Pointer;
	static int Ingoing_Packet_Length;

	static int Con_Packet_Return_Address;
	static int Con_Packet_Socket;
	static SOCKET sock;
	static struct sockaddr *addr;
	static int length;
};