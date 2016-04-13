#include "stdafx.h"

#include "ChatManager.h"
#include <iostream>
#include<string>

typedef void(*ophandler)(Socket* s, int len);
void recvhandle(Socket* s, int len) {

	if (len) {
		s->onRead();
		s->setRecvEvent();
	}
	else
		s->Disconnect();

}
void sendhandle(Socket *s, int len) {
	if (len) {
		if(s->getsndBuffer()->slen>0)s->getsndBuffer()->slen -=len; //remove sended data len if we didnt 
		s->setSendEvent();											// IOCP non stop sending over and over dead lock...
	}
}
void otherhandle_(Socket *s, int len) {

}

static ophandler ophandlers[] = {
	&recvhandle,
	&sendhandle,
	&otherhandle_
};

chatManager::chatManager() {


}
 void chatManager::startListen(void* p) {
	chatManager *mng = (chatManager*)p;
	mng->server = new chatServer();
	mng->server->setChatMngr(mng);
	mng->server->acceptThread();


}
void WINAPI chatManager::socketWorkerThread(void *p) {
	chatManager *mng = (chatManager*)p;
	DWORD len = 0, flags = 0;
	LPOVERLAPPED ov = NULL;
	Socket *s = nullptr;
	OverlappedStruct* o_;
	HANDLE cp = mng->getIOCP();
	while (true) {
		if (!GetQueuedCompletionStatus(cp, &len, (LPDWORD)&s/*when wakes up its returning class that we assigned*/, 
			&ov/*this important recv or send? data check*/, INFINITE)) {
			if (s != nullptr) {
				s->Disconnect(); //if GetQueuedCompletionStatus fails (causes connection lost) lets out it
			}
		}
		o_ = CONTAINING_RECORD(ov, OverlappedStruct,m_ov);// remember we created a class that wraps OVERLAPPED struct
		                                                  // then assigned with 2 obj m_readevent m_sendevent
														  // this macro returns segment of that m_readeven or m_sendevent
														  // so we figure out which even has come.

		if (o_->getEvent() < Event::NUMOFEVENTNUMBERS) {
			ophandlers[o_->getEvent()](s, len);
		}
	}
}
HANDLE chatManager::getIOCP() { return server->getIOCP(); }

void chatManager::spawnWorkerThreads() {
	threads.push_back(new std::thread(startListen, this));
	Sleep(1000);// sleep for server start
	for (int i = 0; i < 1; i++)// we can create many threads but first we should fix date race condition with creating a mutex class
		threads.push_back(new std::thread(socketWorkerThread, this));
}
void chatManager::startChat() {
	spawnWorkerThreads();
}

void chatManager::sendToAll(Packet &pkt, Socket* s) {
	std::lock_guard<std::mutex> lock(mMutex);

	for (auto itr = activeClients.begin(); itr != activeClients.end(); itr++)
		(*itr)->Send_(pkt);
}

void chatManager::sendPM(Packet &pkt, Socket* s) {
	int id = NULL;
	char cid[4];
	Packet newpkt;
	memcpy(cid, &pkt.data[4], 4); //first 4 characer is "/pm" after that id coming
	id = atoi(cid);
	if (id >= activeClients.size()) {
		std::string er = "Error: there no 1 exist in this id";
		memcpy(newpkt.data, er.c_str(), er.size());
		newpkt.pkType = PacketType::PM;
		s->Send_(newpkt);
		return;

	}
	int senderID = s->getMyID();
	std::string str;
	str = "PM Message from ";
	str += s->getMyNick();
	str += "(";
	str += _itoa(id, cid, 10);
	str += ")";
	str += ":";
	char temp[500]="\0";

	memcpy(temp,&pkt.data[6],494);
	str += temp;

	newpkt.pkType = pkt.pkType;
	memcpy(newpkt.data, str.c_str(), str.size());

	activeClients[id]->Send_(newpkt);
}

void chatManager::onDisconnect(Socket *s) {
	std::string str;
	str = s->getMyNick();
	str += " has been disconnected.";

	Packet pkt;
	pkt.pkType = PacketType::TOALL;
	memcpy(pkt.data, str.c_str(), str.size());
	sendToAll(pkt, s);
	std::cout << inet_ntoa(s->getMyAddr().sin_addr) << str << std::endl;

	std::lock_guard<std::mutex> lock(mMutex);


	for (auto itr = activeClients.begin(); itr != activeClients.end(); itr++) {
		if ((*itr)->getMyID() == s->getMyID()) {
			activeClients.erase(itr);
			
			break;
		}
	}
	delete s; //delete it! or leak!!

}

void chatManager::newConnection(Packet &pkt, Socket* s) {
	Sleep(100); //need mutex class for remove this shits, causes runtime error
	int cid = s->getMyID();
	char nick[10];
	memcpy(nick, pkt.data, sizeof(nick));
	Packet p;
	p.pkType = PacketType::NICK;
	memcpy(p.data, nick, sizeof(nick));
	memcpy(&p.data[sizeof(nick)], " connected!", sizeof(" connected"));
	s->setMyNick(nick, 10);
	sendToAll(p,s);

}

void chatManager::chatBoard(Packet& pkt, Socket *s) {
	Packet npkt;
	int id = s->getMyID();
	char cid[4];
	memcpy(cid, (void *)&id, 4);

	Packet newpkt;


	std::string str;
	str += s->getMyNick();
	str += "(";
	str += _itoa(id, cid, 10);
	str += ")";
	str += ":";
	str += pkt.data;
	npkt.pkType = pkt.pkType;
	memcpy(npkt.data, str.c_str(), str.size());
	sendToAll(npkt, s);
}

void chatManager::sendList(Socket *s){
	Packet pkt;
	std::string list;
	for (auto itr : activeClients) {
		list += itr->getMyNick();
		list += "\n";
	}
	pkt.pkType = PacketType::PM;
	memcpy(pkt.data, list.c_str(), list.size());
	s->Send_(pkt);
}


void chatManager::HandlePacket(Socket* s) {
	Packet pkt;  //every packet we should handle in here u can create ur own commands! enjoy fun
	pkt.undoPacket(s->getrcvBuffer()->data);

	switch (pkt.pkType) {
	case PacketType::NICK: newConnection(pkt,s); break;
	case PacketType::TOALL: chatBoard(pkt,s); break;
	case PacketType::PM: sendPM(pkt,s); break;
	case PacketType::CLIST: sendList(s);
	default: std::cout << "handle this error!!";
	}
}