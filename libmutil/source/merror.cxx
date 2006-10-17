
#include<config.h>

#include<libmutil/merror.h>

#ifdef _MSC_VER
#include<Windows.h>
#include<iostream>
#include<libmutil/stringutils.h>
#else
#include<stdio.h>
#endif

void merror(const char* s){
#ifdef _MSC_VER
	DWORD err = GetLastError();
	LPVOID lpMsgBuf;

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf, 0, NULL );
	std::string emsg = std::string(s)+": Error "+itoa(err)+": "+std::string((char*)lpMsgBuf);
	std::cerr << emsg <<std::endl;
#else
	perror(s);
#endif
}

