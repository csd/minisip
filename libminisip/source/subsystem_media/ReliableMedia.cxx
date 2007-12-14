
#include<config.h>

#include<libminisip/media/ReliableMedia.h>

using namespace std;

ReliableMedia::ReliableMedia(string type, bool isClient_, bool isServer_)
		: /*isClient(isClient_),
		isServer(isServer_), */
		sdpType(type)
{

}

ReliableMedia::~ReliableMedia(){

}

string ReliableMedia::getSdpMediaType(){
	return sdpType;
}

