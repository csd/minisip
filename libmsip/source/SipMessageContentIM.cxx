
#include<libmsip/SipMessageContentIM.h>

SipMessageContentIM::SipMessageContentIM(string m): msg(m){

}

string SipMessageContentIM::getString(){
	return msg;
}

string SipMessageContentIM::getContentType(){
	return "text/plain";
}
