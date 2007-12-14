
#include<config.h>

#include"MediaSharedWorkspace.h"
#include<libminisip/media/MediaStream.h>

using namespace std;

class SWSMediaStream : public ReliableMediaStream {
public:
	SWSMediaStream( std::string callId, MRef<ReliableMedia*> m );
	std::string getSdpMediaType();
};

SWSMediaStream::SWSMediaStream( std::string callId, MRef<ReliableMedia*> m ) : ReliableMediaStream(callId,m) {

}

string SWSMediaStream::getSdpMediaType(){
	return "vnc";
}

string MediaSharedWorkspace::getSdpMediaType(){
	return "application";
}

MediaSharedWorkspace::MediaSharedWorkspace() : ReliableMedia("application", true,true) {
}

MRef<ReliableMediaStream*> MediaSharedWorkspace::createMediaStream(string callId){
	MRef<ReliableMediaStream*> m =  new SWSMediaStream(callId, this);
	return m;
}

