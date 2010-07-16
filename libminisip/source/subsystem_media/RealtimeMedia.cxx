
#include<config.h>

#include<libminisip/media/RealtimeMedia.h>

#include<string.h>

using namespace std;

RealtimeMedia::RealtimeMedia(MRef<Codec *> codec ){
	codecList.push_back(codec);
	//selectedCodec = codec;
}

// pn430 Function added for multicodec
RealtimeMedia::RealtimeMedia( const std::list<MRef<Codec *> >& codecListing ){
	codecList = codecListing;
	//selectedCodec = defaultCodec;
}

RealtimeMedia::~RealtimeMedia(){

}

// pn507 Added for being able to change the current codec
// pn507 NOTE Using this during a conference call will most likely cause complete havoc.
MRef<Codec *> RealtimeMedia::getCodec( uint8_t payloadType ){
	std::list< MRef<Codec *> >::iterator iCodec;
	
	for( iCodec = codecList.begin(); iCodec != codecList.end(); iCodec ++ ){
		if ( (*iCodec)->getSdpMediaType() == payloadType ) {
//			selectedCodec = (*iCodec);
			return *iCodec;
		}
	}
	
	return NULL;
}

list< MRef<Codec *> >  RealtimeMedia::getAvailableCodecs(){
	list<MRef<Codec *> > copy;
	codecListLock.lock();
	copy = codecList;
	codecListLock.unlock();
	return copy;
}

void RealtimeMedia::registerRealtimeMediaSender( MRef<RealtimeMediaStreamSender *> sender ){


	sendersLock.lock();
	senders.push_back( sender );
	sendersLock.unlock();
}

void RealtimeMedia::unregisterRealtimeMediaSender( MRef<RealtimeMediaStreamSender *> sender ){
	sendersLock.lock();
	senders.remove( sender );
	sendersLock.unlock();
}

void RealtimeMedia::sendData( byte_t * data, uint32_t length, int sampleRate, uint32_t ts, bool marker ){
	list< MRef<RealtimeMediaStreamSender *> >::iterator i;
	sendersLock.lock();
	for( i = senders.begin(); i != senders.end(); i++ ){
		//only send if active sender, or if muted only if keep-alive
		if( (*i)->isMuted () ) {
			if( (*i)->muteKeepAlive( 50 ) ) { //send one packet of every 50
				memset( data, 0, length );
			} else {
				continue;
			}
		}
		(*i)->send( data, length, &ts, marker );
	}
	sendersLock.unlock();
}


MRef<CodecState *> RealtimeMedia::createCodecInstance( uint8_t payloadType ){
	std::list< MRef<Codec *> >::iterator iC;

	for( iC = codecList.begin(); iC != codecList.end(); iC ++ ){
		if( (*iC)->getSdpMediaType() == payloadType ){
			MRef<CodecState *> tmp  = (*iC)->newInstance();
			return (*iC)->newInstance();
		}
	}
	return NULL;
}


