#ifndef _PRESENCEMESSAGECONTENT_H
#define _PRESENCEMESSAGECONTENT_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>
#include<libmsip/SipMessageContent.h>
#include<libmsip/SipMessageContentFactory.h>



MRef<SipMessageContent*> presenceSipMessageContentFactory(const string &, const string &ContentType);

class PresenceMessageContent : public SipMessageContent{
	public:
		PresenceMessageContent(string from, string to, string onlineStatus, string onlineStatusDesc);
		PresenceMessageContent(const string &buildFrom);
		virtual std::string getMemObjectType(){return "PresenceMessageContent";}
		virtual std::string getString();
		virtual std::string getContentType(){return "application/xpidf+xml";}


	private:
		string fromUri;
		string toUri;
		string document;
		string onlineStatus;
		string onlineStatusDesc;
		string presentity;
};


#endif
