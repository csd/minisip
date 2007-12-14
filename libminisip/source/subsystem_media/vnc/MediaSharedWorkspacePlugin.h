#ifndef SWSPLUGIN_H
#define SWSPLUGIN_H


#include<libmutil/Library.h>
#include<libminisip/media/Media.h>

#include"MediaSharedWorkspace.h"

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

#endif
