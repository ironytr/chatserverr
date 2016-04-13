#pragma once
#ifndef MYSOCKET
#define MYSOCKET

#include "stdafx.h"
#include "Header.h"
#include "ChatManager.h"
class chatManager;
struct Buffer{
	char data[PACKETSIZE]="\0";
	int slen = 0, rlen = 0;


};
class Socket {

public:

	Socket() ;

	Event getMyStatus();
	void setMyStatus(int m);

	void setMyID(int &id);
	int getMyID()const;
	void setFD(SOCKET &fd);
	SOCKET getFD()const;

	void setCP(HANDLE cp);
	void AssingCP();
	void onRead();
	void setRecvEvent();
	void Send_(Packet&);
	void setSendEvent();
	void Disconnect();

	void setChatManager(chatManager* mngr);
	chatManager* getChatMngr();
	Buffer* getsndBuffer() { return &sendpkt; }
	Buffer* getrcvBuffer() { return &recvpkt; }
	char* getMyNick() { return myNick; }
	void setMyNick(char *nick,int len) {
		memcpy(myNick, nick, len);
	}
	void setMyAddr(struct sockaddr_in &tad) {
		memcpy(&myAddres, &tad, sizeof(myAddres));
	}
	struct sockaddr_in getMyAddr()const { return myAddres; }
private:
	int myID;
	SOCKET m_fd;
	HANDLE m_cp;
	OverlappedStruct m_readevent, m_sendevent;
	Event myStatus;
	struct sockaddr_in myAddres;
	Buffer recvpkt, sendpkt;
	chatManager *cmngr;
	std::mutex mMutex;
	char myNick[10] = "\0";
};


#endif