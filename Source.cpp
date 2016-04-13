#include "stdafx.h"
#include "TestSocketMgr.h"



void main(){

	testSocketMgr<TestSocket> test;
	test.startServer();
	test.spawnthreads();
	

	while (1);
}


#pragma comment(lib,"ws2_32.lib")