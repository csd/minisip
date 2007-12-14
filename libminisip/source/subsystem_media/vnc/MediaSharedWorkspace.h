#ifndef MEDIA_SHARED_WORKSPACE_H
#define MEDIA_SHARED_WORKSPACE_H

#include<string>
#include<libminisip/media/ReliableMedia.h>


class MediaSharedWorkspace : public ReliableMedia {
	public:
		MediaSharedWorkspace();
		
		virtual std::string getMediaFormats(){return "vnc";}

		virtual MRef<ReliableMediaStream*> createMediaStream(std::string callId);

		virtual std::string getSdpMediaType();

	private:
};


#endif
