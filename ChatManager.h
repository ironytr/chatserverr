#pragma once

#ifndef CHATMANAGER
#define CHATMANAGER

#include "stdafx.h"
#include<mutex>
#include<condition_variable>

#include "ChatServer.h"
#include "Socket.h"
class chatServer;
class Socket;
class chatManager {


public:
	chatManager();
	static void startListen(void* p);
	static void WINAPI socketWorkerThread(void *p);
	HANDLE getIOCP();
	void spawnWorkerThreads();
	void startChat();
	void onDisconnect(Socket *s);
	void sendToAll(Packet & pkt, Socket* s);
	void HandlePacket(Socket* s);
	void chatBoard(Packet& pkt, Socket *s);
	void newConnection(Packet& pkt, Socket* s);
	void assignClient(Socket *s) {
		activeClients.push_back(s);
	}
	void sendPM(Packet &, Socket* s);
	void sendList(Socket *s);
private:

	chatServer *server;
	std::vector<std::thread *> threads;
	std::vector<Socket*> activeClients;
	std::mutex mMutex;
};
#endif