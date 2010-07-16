/*
 Copyright (C) 2006  Mikael Magnusson
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* Copyright (C) 2006
 *
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include<config.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include<libminisip/media/Media.h>
#include<libminisip/media/video/VideoMedia.h>
#include<libminisip/media/video/grabber/Grabber.h>
#include<libminisip/media/video/codec/AVCoder.h>
#include<libminisip/media/video/codec/VideoCodec.h>
#include<libminisip/media/video/mixer/ImageMixer.h>

#include<libminisip/media/MediaStream.h>

class VideoPlugin : public MediaPlugin{
	public:
		VideoPlugin(MRef<Library*> lib);
		virtual ~VideoPlugin();

		virtual MRef<Media*> createMedia( MRef<SipSoftPhoneConfiguration *> config );
		///////
		virtual MRef<Media*> createMedia2stream(  MRef<SipSoftPhoneConfiguration *> config );

		virtual std::string getMemObjectType() const{ return "VideoPlugin"; }
		virtual std::string getName() const;
		virtual uint32_t getVersion() const;
		virtual std::string getDescription() const;
};

using namespace std;

static std::list<std::string> pluginList;
static bool initialized;


extern "C" LIBMINISIP_API
std::list<std::string> *mvideo_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MPlugin * mvideo_LTX_getPlugin( MRef<Library*> lib ){
	return new VideoPlugin( lib );
}

VideoPlugin::VideoPlugin( MRef<Library*> lib ): MediaPlugin( lib ){
	// Start video plugin registries
	GrabberRegistry::getInstance();
	VideoDisplayRegistry::getInstance();
}

VideoPlugin::~VideoPlugin(){
}

MRef<Media*> VideoPlugin::createMedia( MRef<SipSoftPhoneConfiguration *> config ){
	

	//cout << "-------------------- VideoPlugin createGrabber :: for device " << config->videoDevice <<endl;
	MRef<Grabber *> grabber = GrabberRegistry::getInstance()->createGrabber( config->videoDevice );
	MRef<MVideoCodec *> videoCodec = new MVideoCodec();
	MRef<ImageMixer *> mixer = NULL;//new ImageMixer();
	MRef<VideoMedia *> videoMedia = new VideoMedia( *videoCodec, NULL/*display*/, mixer, grabber, config->frameWidth, config->frameHeight );
	if( mixer ){
		mixer->setMedia( videoMedia );
	}

	return *videoMedia;
}

MRef<Media*> VideoPlugin::createMedia2stream(  MRef<SipSoftPhoneConfiguration *> config ){

        //cout << "-------------------- VideoPlugin createMedia2stream  createGrabber :: for device " << config->videoDevice2 <<endl;
        MRef<Grabber *> grabber = GrabberRegistry::getInstance()->createGrabber( config->videoDevice2 );
        MRef<MVideoCodec *> videoCodec = new MVideoCodec();
        MRef<ImageMixer *> mixer = NULL;//new ImageMixer();
        MRef<VideoMedia *> videoMedia = new VideoMedia( *videoCodec, NULL/*display*/, mixer, grabber, config->frameWidth, config->frameHeight );
        if( mixer ){
                mixer->setMedia( videoMedia );
        }
	grabber->setLocalDisplay(NULL);
        return *videoMedia;
}


std::string VideoPlugin::getName() const{
	return "video";
}

uint32_t VideoPlugin::getVersion() const{
	return 0x00000001;
}

std::string VideoPlugin::getDescription() const{
	return "video media plugin";
}
