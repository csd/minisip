#ifndef _SIPDialogPRESENCE_H
#define _SIPDialogPRESENCE_H

#include<libmsip/SipDialogContainer.h>

class SipDialogPresence : public SipDialog{
    public:
	SipDialogPresence(MRef<SipDialogContainer*> dContainer, SipDialogConfig &callConfig);
	void setUpStateMachine();

	virtual bool handleCommand(const SipSMCommand &cmd);

    private:

    
};


#endif
