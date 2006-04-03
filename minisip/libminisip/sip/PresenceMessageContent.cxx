
#include"PresenceMessageContent.h"

MRef<SipMessageContent*> presenceSipMessageContentFactory(const string &buf, const string &){
	        return new PresenceMessageContent(buf);
}


PresenceMessageContent::PresenceMessageContent(string from, string to, string os, string osd) : 
		fromUri(from),
		toUri(to),
		onlineStatus(os), 
		onlineStatusDesc(osd)
{
	string status;
	if (os=="online"){
		status="open";
	}else{
		status="inuse";
	}
	document = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE presence\n"
                "PUBLIC \"-//IETF//DTD RFCxxxx XPIDF 1.0//EN\" \"xpidf.dtd\">\n"
                "<presence>\n"
                "  <presentity uri=\"" +toUri+";method=SUBSCRIBE\" />\n"
                "  <atom id=\"1000\">\n"
                "    <address uri=\""+fromUri+"\">\n"
                "      <status status=\""+status+"\" />\n" 			//"open"|("closed")|"inuse"
                "      <msnsubstatus substatus=\""+onlineStatus+"\" />\n"
                "    </address>\n"
                "  </atom>\n"
                "</presence>\n";
}

PresenceMessageContent::PresenceMessageContent(const string &buildFrom){
	document = buildFrom;
}

string PresenceMessageContent::getString(){
	return document;
}


