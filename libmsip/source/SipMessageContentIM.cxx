
#include<libmsip/SipMessageContentIM.h>



MRef<SipMessageContent*> sipIMMessageContentFactory(const string &buf){
        return new SipMessageContentIM(buf);
}

SipMessageContentIM::SipMessageContentIM(string m): msg(m){

}

string SipMessageContentIM::getString(){
	return msg;
}

string SipMessageContentIM::getContentType(){
	return "text/plain";
}
