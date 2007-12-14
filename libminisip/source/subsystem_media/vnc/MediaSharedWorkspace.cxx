
#include<config.h>

#include"MediaSharedWorkspace.h"
#include<libminisip/media/MediaStream.h>

using namespace std;

class SWSMediaStream : public ReliableMediaStream {
public:
	SWSMediaStream( std::string callId, MRef<ReliableMedia*> m );
	std::string getMediaFormats();

	void start();
	void stop();
	virtual uint16_t getPort();
	virtual uint16_t getPort(string transport);
};

SWSMediaStream::SWSMediaStream( std::string callId, MRef<ReliableMedia*> m ) : ReliableMediaStream(callId,m) {

}

string SWSMediaStream::getMediaFormats(){
	return "vnc";
}

void SWSMediaStream::start(){

}

void SWSMediaStream::stop(){
}

uint16_t SWSMediaStream::getPort(){
	return 0;
}

uint16_t SWSMediaStream::getPort(string transport){
	return 0;
}


///////
///////

MediaSharedWorkspace::MediaSharedWorkspace() : ReliableMedia("application", true,true) {
	addSdpAttribute("recvonly");
	addSdpAttribute("setup:active");
}

string MediaSharedWorkspace::getSdpMediaType(){
	return "application";
}

MRef<ReliableMediaStream*> MediaSharedWorkspace::createMediaStream(string callId){
	MRef<ReliableMediaStream*> m =  new SWSMediaStream(callId, this);
	return m;
}

