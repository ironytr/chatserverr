
/*
 This project example for IOCP server client sides. IOCP requiest
 OVERLAPPED struct <- this important for ussage wrapped with overllappedstruct so we can figure out IOCP senind data or REcvs
 GetQueuedCompletionStatus <-when socket fired it wakes up and returning obj
 CreateIoCompletionPort
 WSAAccept
 WSASocket
 WSASend
 WSARecv
 WSABUF <- assign buffer with this dont forget to add its lengt for first or socket cannot be assigned with IOCP

 you can found these functions explains on net. just debug program and see whats going on. you dont need to understand 
 "why this function tkaes those argumants? " just remember that this API requiest that variables even other big projects defines
 same way. 

 classes
 Socket <- when some1 connected im assigning socket with this then CreateIoCompletionPort after that register with Wsarecv

 chatServer <- listening port and accept thread

 chatManager <- socketworkerthreads and other handling stuffs exampla handing incoming packets
*/


/*
 handling packets:
 i just made a simple way created Buffer class. but if u gonna use big project u should make 
 circularbuffer ! remember when did u send ur data handle ur buffer lenghts and overlap structs


*/
#include "stdafx.h"

#include "ChatManager.h"

HANDLE chatServer::getIOCP() { return m_cp; }
chatServer::chatServer() {
	m_cp = nullptr;
	StartWsa();
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	ZeroMemory(&address.sin_zero, sizeof(address.sin_zero));

	m_fd = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
	int result = bind(m_fd, (SOCKADDR*)&address, sizeof(address));
	result = listen(m_fd, 0);
	m_cp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, (ULONG_PTR)0, 0);

}
void chatServer::acceptThread() {
	struct sockaddr_in client;
	int len = sizeof(client);
	int id = 0;
	while (true) {
		SOCKET tempSocket = WSAAccept(m_fd, (SOCKADDR*)&client, &len, 0, 0);
		Socket *s = new Socket();
		s->setCP(m_cp);
		s->setMyID(id);
		s->setFD(tempSocket);
		s->AssingCP();
		s->setRecvEvent();
		s->setChatManager(cMngr);
		s->setMyAddr(client);
		cMngr->assignClient(s);
		id++;
		std::cout << inet_ntoa(client.sin_addr) << " has been connected";
	}
}
void chatServer::StartWsa() {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 0), &wsa);
}
/*
void chatServer::setChatMngr(chatManager* mngr) { cMngr = mngr; }
chatManager* chatServer::getChatMngr() { return cMngr; }*/


int main() {



	chatManager chatserver;

	chatserver.startChat();

	while (1)
		Sleep(1000);

	return 0;
}


#pragma comment(lib,"ws2_32.lib")
