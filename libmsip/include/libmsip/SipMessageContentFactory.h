#ifndef _SIPMESSAGECONTENTFACTORY_H
#define _SIPMESSAGECONTENTFACTORY_H

#include<map>
#include<libmsip/SipMessageContent.h>
#include<string>
#include<libmutil/MemObject.h>

using namespace std;

typedef MRef<SipMessageContent*>(*SipMessageContentFactoryFuncPtr)(const string &);

class SMCFCollection{
public:
	void addFactory(string contentType, SipMessageContentFactoryFuncPtr);
	SipMessageContentFactoryFuncPtr getFactory(const string contentType);
	
private:
	map<string, SipMessageContentFactoryFuncPtr > factories; // map with content type as key
};

#endif
