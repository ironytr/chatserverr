#pragma once

#ifndef CHATSERVER
#define CHATSERVER

#include "stdafx.h"

#include "Header.h"
#include "ChatManager.h"

#include "Socket.h"

class chatManager;
class Socket;

class chatServer {
public:
	HANDLE getIOCP();
	chatServer();
	void acceptThread();
	static void StartWsa();
	void setChatMngr(chatManager* mngr) { cMngr = mngr; }
	chatManager* getChatMngr() { return cMngr; }
private:
	SOCKET m_fd;
	struct sockaddr_in address;
	HANDLE m_cp;
	chatManager* cMngr;
};
#endif