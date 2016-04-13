#pragma once
#include "stdafx.h"

#ifndef MYHEADER
#define MYHEADER

#define PORT 5000
#define LISTENADDRESS "127.0.0.1"
#define PACKETSIZE 500

enum Event {
	RECVD = 0,
	SENDD = 1,
	NUMOFEVENTNUMBERS = 2
};

template<typename tEntity>
class Delegate {
	typedef void* InstancePtr;
	typedef void(*InternalFnc)(InstancePtr, tEntity*);
	InstancePtr iPtr;
	InternalFnc iFnc;
public:
	template<class C, void(C::*Function)(tEntity*)>
	static Delegate* create(C* obj) {
		Delegate *del = new Delegate();
		del->Bind<C, Function>(obj);
		return del;
	}
	template<class C, void(C::*Function)(tEntity*)>
	void Bind(C* obj) {
		iPtr = obj;
		iFnc = &classMethod<C, Function>;
	}
	template<class C, void(C::*FUnction)(tEntity*)>
	static void classMethod(InstancePtr *o, tEntity* s) {
		return (static_cast<C*>(o)->iFnc)(s);
	}
	void Invoke(tEntity* s) {
		iFnc(iPtr, s);
	}
};
class OverlappedStruct {
public:
	OVERLAPPED m_ov;
	Event ev;
	bool m_Inuse;
	OverlappedStruct() {
		memset(&m_ov, 0, sizeof(m_ov));
		m_Inuse = false;
	}
	OverlappedStruct(Event e):ev(e) {
		memset(&m_ov, 0, sizeof(m_ov));
	}
	void Reset(Event e) {
		memset(&m_ov, 0, sizeof(m_ov));
		ev = e;
	}

	Event getEvent()const { return ev; }
};
enum PacketType {
	NICK = 3,
	TOALL = 4,
	PM = 5,
	CLIST=6
};

struct Packet {
	unsigned int pkType = 0;
	char data[PACKETSIZE]="\0";
	void setPacket(char *buffer) {
		memcpy(buffer, this, sizeof(Packet));// pushing data after type

	}
	void undoPacket(char* buffer) {
		memcpy(this, buffer, sizeof(Packet));

	}
};

#endif