#include "stdafx.h"
#include "Socket.h"


	Socket::Socket() {

	}



	void Socket::setMyID(int &id) { myID = id; }
	int Socket::getMyID()const { return myID; }
	void Socket::setFD(SOCKET &fd) { m_fd = fd; }
	SOCKET Socket::getFD()const { return m_fd; }

	void Socket::setCP(HANDLE cp) { m_cp = cp; }

	void Socket::setChatManager(chatManager* mngr) {cmngr=mngr;	}
	chatManager*Socket::getChatMngr() { return cmngr; }

	void Socket::AssingCP() {
		CreateIoCompletionPort((HANDLE)m_fd, m_cp, (ULONG_PTR)this, 0);
	}
	void Socket::onRead() {
		getChatMngr()->HandlePacket(this);

	}

	void Socket::Disconnect() {
		getChatMngr()->onDisconnect(this);
		closesocket(m_fd);
	}


	void Socket::Send_(Packet& pkt) {
		pkt.setPacket(sendpkt.data);
		sendpkt.slen = PACKETSIZE;
		setSendEvent();
		sendpkt.slen = 0;
	}
	void Socket::setRecvEvent() {

		WSABUF wsa;
		DWORD len = 0, flags = 0;
		wsa.buf = recvpkt.data;
		wsa.len = sizeof(Packet);
		m_readevent.Reset(Event::RECVD);
		if (WSARecv(m_fd, &wsa, 1, &len, &flags,&m_readevent.m_ov, 0)) {
			int err = WSAGetLastError();
			if (err != WSA_IO_PENDING) {
				Disconnect();
			}
		}
	}
	void Socket::setSendEvent() {


		std::lock_guard<std::mutex> lock(mMutex);
		WSABUF wsa;
		DWORD len = 0, flags = 0;
		wsa.buf = sendpkt.data;
		wsa.len = sendpkt.slen; //this very important! if we sended data we should set this to 0 or this gonna fuck up program.
		m_sendevent.Reset(Event::SENDD);
		int result =WSASend(m_fd, &wsa, 1, &len, flags,&m_sendevent.m_ov, 0);
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING) {
			//disconnect event
		}
	}