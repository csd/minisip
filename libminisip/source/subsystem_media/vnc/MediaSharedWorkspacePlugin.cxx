
#include<config.h>

#include<libmutil/Library.h>
#include<libminisip/media/Media.h>

#include"MediaSharedWorkspace.h"

#include<iostream>

using namespace std;

class SharedWorkspacePlugin : public MediaPlugin {
public:
	SharedWorkspacePlugin(MRef<Library*> lib);
	~SharedWorkspacePlugin();

	virtual MRef<Media*> createMedia( MRef<SipSoftPhoneConfiguration *> config );
	virtual std::string getMemObjectType() const{ return "SharedWorkspacePlugin"; }
	virtual std::string getName() const;
	virtual uint32_t getVersion() const;
	virtual std::string getDescription() const;

private:


};

static std::list<std::string> pluginList;

static bool initialized;



extern "C" LIBMINISIP_API
std::list<std::string> *msws_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MPlugin * msws_LTX_getPlugin( MRef<Library*> lib ){
	return new SharedWorkspacePlugin( lib );
}

SharedWorkspacePlugin::SharedWorkspacePlugin( MRef<Library*> lib ): MediaPlugin( lib ){
}

SharedWorkspacePlugin::~SharedWorkspacePlugin(){
}

MRef<MediaSharedWorkspace*> swsm;

MRef<Media*> SharedWorkspacePlugin::createMedia( MRef<SipSoftPhoneConfiguration *> config ){
	MRef<MediaSharedWorkspace*> swsMedia = new MediaSharedWorkspace();
	swsm = swsMedia;
	return *swsMedia;
}

std::string SharedWorkspacePlugin::getName() const{
	return "sharedworkspace";
}

uint32_t SharedWorkspacePlugin::getVersion() const{
	return 0x00000001;
}

std::string SharedWorkspacePlugin::getDescription() const{
	return "presentation sharing plugin";
}

