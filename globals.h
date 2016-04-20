#pragma once
#include<string>
//#ifndef DEFAULT_SENDBUFLEN
#define DEFAULT_SENDBUFLEN 2457600//480*640*4*2
//#endif
//#ifndef DEFAULT_RECVBUFLEN
#define DEFAULT_RECVBUFLEN 256 //may want to make super small, like a single binary value
//#endif

namespace Globals {
	extern char* data;
	extern bool completed;
}