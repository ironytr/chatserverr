// ConsoleApplication3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<thread>
#include<iostream>
#include<vector>
#include<string>
#include<WinSock2.h>
#include<mutex>
#include<condition_variable>
enum PacketType {
	NICK = 3,
	TOALL = 4,
	PM = 5,
	CLIST = 6
};

struct Packet {
	unsigned int pkType = 0;
	char data[500];
	void setPacket(char *buffer) {
		memcpy(buffer, this, sizeof(Packet));// pushing data after type

	}
	void undoPacket(char* buffer) {
		memcpy(this, buffer, sizeof(Packet));

	}
};



static void startWSA(){
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 0), &wsa);
	
}
class client {
public:
	std::mutex mMutex;
	SOCKET s;
	struct sockaddr_in adres;
	client(std::string& nick) {
		s = socket(AF_INET, SOCK_STREAM, 0);
		
		adres.sin_family = AF_INET;	
		adres.sin_port = htons(5000);
		ZeroMemory(&adres.sin_zero, sizeof(adres.sin_zero));
		struct hostent* host = gethostbyname("127.0.0.1");
		adres.sin_addr = *(struct in_addr*)host->h_addr;

		int rest = connect(s, (sockaddr*)&adres, sizeof(adres));
		Packet p;
		
		p.pkType = PacketType::NICK;
		
		memcpy(&p.data, nick.c_str(), nick.size());
		memcpy(&p.data[nick.size()], "\0", (500 - nick.size()));
		p.setPacket(sendbuff);

		int result = send(s, sendbuff, sizeof(Packet), 0);


		mthread = new std::thread(recvthread, this);
		bool isPm;
		while (1) {

			Packet paket;

			std::string msg;
			int type;
		//	std::getline(std::cin, msg);
			std::getline(std::cin, msg);
	
			char checkcmnd[4];
			memcpy(checkcmnd, msg.c_str(), 4);

			isPm= false;

			char PM[4] = "/pm";
			for (int i = 0; i < 3; i++) {
				if(checkcmnd[i] == PM[i]) {
					isPm = true;
				}else {
					isPm = false;
					break;
				}
			}

			bool isRequestUserList = false;
			char checkRU[7];
			memcpy(checkRU, msg.c_str(), 7);
			
			char ReqList[7] = "/users";
			for (int i = 0; i < 7; i++) {
				if (ReqList[i] == checkRU[i]) {
					isRequestUserList = true;
				}
				else {
					isRequestUserList = false;
					break;
				}
			}


			if (isPm)
				paket.pkType = PacketType::PM;
			else if(isRequestUserList)
				paket.pkType = PacketType::CLIST;
			else
				paket.pkType = PacketType::TOALL;
			
			memcpy(&paket.data, msg.c_str(), msg.size());
			memcpy(&paket.data[msg.size()], "\0", (500 - msg.size()));
			paket.setPacket(sendbuff);

			int result = send(s, sendbuff, sizeof(Packet), 0);
		
		}

	}
	static void WINAPI recvthread(client *c) {
		Packet pkt;
		while (1) {

			int resulst = recv(c->s, c->recvbuff, 504, 0);
			pkt.undoPacket(c->recvbuff);
			switch (pkt.pkType) {
			case PacketType::NICK: std::cout << pkt.data << " say hello to him via pm! his ID: \/pm ID msg" << std::endl; break;
			case PacketType::TOALL: std::cout << pkt.data << std::endl; break;
			case PacketType::PM: std::cout << pkt.data << std::endl; break;

			default: std::cout << "handle this error";

			}
		
		}
	}

	char sendbuff[500], recvbuff[500];
	std::thread* mthread;
};

int main(){
	startWSA();
	std::string nick;
	std::cout << "enter your nick:";
	std::getline(std::cin, nick);
	client c(nick);

	
}

#pragma comment(lib,"ws2_32.lib")